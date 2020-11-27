#include "ofxConvertJsonHelper.h"


using namespace ofx::convertjson;
using namespace ofx::convertjson::helpers;
using namespace std;

template<typename ConcreteType>
Array Any<ConcreteType>::dispatch(initializer_list<ConvFunc> proc) const
{
	vector<ofJson> ret;
	ret.reserve(proc.size());
	for(auto &&conv : proc) {
		ret.push_back(conv(value()));
	}
	return Array(std::move(ret));
}
Object& Object::pick(Picker picker, ConvFunc proc)
{
	auto &&src = ref();
	for(auto it = begin(src); it != end(src); ++it) {
		if(picker(it.key())) {
			it.value() = proc(it.value());
		}
	}
	return *this;
}

Object Array::toObject(NamerFunction namer) const
{
	ofJson ret;
	const auto &src = value();
	for(size_t i = 0; i < src.size(); ++i) {
		string name = namer(i, src[i], src);
		if(!name.empty()) {
			ret[name] = src[i];
		}
	}
	return Object(std::move(ret));
}

Object Array::mergeAsObject(MergeStrategy strategy, NamerFunction not_obj_namer) const
{
	std::map<std::string, ofJson> ret;
	const auto &src = value();
	for(size_t i = 0; i < src.size(); ++i) {
		const auto &item = strategy == KEEP_FIRST ? src[i] : src[src.size()-1-i];
		if(item.is_object()) {
			for(auto it = begin(item); it != end(item); ++i) {
				ret.insert(make_pair(it.key(), it.value()));
			}
		}
		else {
			ret.insert(make_pair(not_obj_namer(i, item, src), item));
		}
	}
	return Object(std::move(ret));
}

