#pragma once

#include "ofJson.h"
#include <initializer_list>

namespace ofx { namespace convertjson {

using PickerFunc = std::function<bool(const std::string&)>;

using Mod = std::function<ofJson(const ofJson&)>;
using NoMod = std::function<void(const ofJson&)>;

using ConvFunc = Mod; // deprecated

class Picker
{
public:
	Picker(const std::string &key) {
		func_ = [key](const std::string &k){return k==key;};
	}
	Picker(const std::vector<std::string> &keys) {
		func_ = [keys](const std::string &key) {
			using namespace std;
			return find(begin(keys),end(keys),key)!=end(keys);
		};
	}
	Picker(std::initializer_list<std::string> keys)
	:Picker(std::vector<std::string>(keys))
	{}
	
	operator PickerFunc() const { return func_; }
	bool operator()(const std::string &key) const { return func_(key); }
private:
	PickerFunc func_;
}; 

static Mod CherryPick(Picker pick) {
	return [pick](const ofJson &src) {
		assert(src.is_object());
		using namespace std;
		ofJson ret;
		for(auto it = begin(src); it != end(src); ++it) {
			if(pick(it.key())) {
				ret[it.key()] = it.value();
			}
		}
		return ret;
	};
}

static Mod Set(const ofJson &new_value) {
	return [new_value](const ofJson &src) {
		return new_value;
	};
}

static Mod ToArray(const std::string &name_of_key) {
	using namespace std;
	return [name_of_key](const ofJson &src) -> ofJson {
		vector<ofJson> ret;
		ret.reserve(src.size());
		for(auto it = begin(src); it != end(src); ++it) {
			ofJson elem = it.value();
			if(!name_of_key.empty()) {
				elem[name_of_key] = it.key();
			}
			ret.push_back(elem);
		}
		return std::move(ret);
	};
}

static NoMod Print(int indent=-1, std::ostream &os=std::cout) {
	return [indent, &os](const ofJson &src) {
		os << src.dump(indent);
	};
}

}}
