#pragma once

#include "ofJson.h"
#include <initializer_list>

namespace ofx { namespace convertjson {
namespace conv {

using PickerFunc = std::function<bool(const std::string&)>;

using ValueDispatchFunc = std::function<ofJson(std::size_t index, const ofJson &src)>;
template<typename Ret>
using ObjItemFunc = std::function<Ret(const std::string &key, const ofJson &item, const ofJson &obj)>;
template<typename Ret>
using ArrayItemFunc = std::function<Ret(std::size_t index, const ofJson &item, const ofJson &obj)>;

class Picker
{
public:
	Picker(const std::vector<std::string> &keys) {
		func_ = [keys](const std::string &key) {
			using namespace std;
			return find(begin(keys),end(keys),key)!=end(keys);
		};
	}
	Picker(std::initializer_list<std::string> keys)
	:Picker(std::vector<std::string>(keys))
	{}
	Picker(bool always) {
		func_ = [always](const std::string&) { return always; };
	}
//	use these when I separate this file into .h and .cpp
//	static const Picker all, none;
	
	operator PickerFunc() const { return func_; }
	bool operator()(const std::string &key) const { return func_(key); }
private:
	PickerFunc func_;
};

static void CherryPick(ofJson &srcdst, Picker pick) {
	using namespace std;
	ofJson ret;
	for(auto it = begin(srcdst); it != end(srcdst); ++it) {
		if(pick(it.key())) {
			ret[it.key()] = it.value();
		}
	}
	srcdst = ret;
}

static ofJson Dispatch(const ofJson &src, std::size_t count, ValueDispatchFunc func) {
	ofJson ret;
	for(std::size_t i = 0; i < count; ++i) {
		ret.push_back(func(i, src));
	}
	return ret;
}

static ofJson Set(const ofJson &src, const ofJson &new_value) {
	return new_value;
}

static void Print(const ofJson &src, int indent=-1) {
	std::cout << src.dump(indent);
}
static void Println(const ofJson &src, int indent=-1) {
	std::cout << src.dump(indent) << std::endl;
}

static void Copy(const ofJson &src, ofJson &dst) {
	dst = src;
}

static void Save(const ofJson &src, const std::string &basename, int indent=-1) {
	ofFile(basename+".json", ofFile::WriteOnly) << src.dump(indent);
}

using ObjNamer = ObjItemFunc<std::string>;
static void SaveObjEach(const ofJson &src, ObjNamer namer, int indent=-1) {
	using namespace std;
	for(auto it = begin(src); it != end(src); ++it) {
		Save(it.value(), namer(it.key(), it.value(), src), indent);
	}
}
static void SaveObjEach(const ofJson &src, const std::string &basename="", int indent=-1) {
	return SaveObjEach(src, [basename](const std::string &key, const ofJson&, const ofJson&) {
		return basename + key;
	}, indent);
}
static ofJson ObjToArray(const ofJson &src, ObjItemFunc<ofJson> proc) {
	using namespace std;
	ofJson ret;
	for(auto it = begin(src); it != end(src); ++it) {
		ret.push_back(proc(it.key(), it.value(), src));
	}
	return ret;
}
static void ObjForEach(ofJson &srcdst, ObjItemFunc<ofJson> proc, Picker pick=Picker(true)) {
	using namespace std;
	for(auto it = begin(srcdst); it != end(srcdst); ++it) {
		if(pick(it.key())) {
			it.value() = proc(it.key(), it.value(), srcdst);
		}
	}
}

using ArrayNamer = ArrayItemFunc<std::string>;
namespace {
std::size_t getDigit(std::size_t size) {
	int digit = 0;
	while(pow(10, ++digit) <= size) {}
	return digit;
}
}
static void SaveArrayEach(const ofJson &src, ArrayNamer namer, int indent=-1) {
	using namespace std;
	for(size_t i = 0; i < src.size(); ++i) {
		Save(src[i], namer(i, src[i], src), indent);
	}
}
static void SaveArrayEach(const ofJson &src, const std::string &basename="", int indent=-1) {
	int digit = getDigit(src.size());
	return SaveArrayEach(src, [digit, basename, indent](std::size_t index, const ofJson&, const ofJson&) {
		return basename+ofToString(index, digit, indent);
	}, indent);
}
static ofJson ArrayToObj(const ofJson &src, ArrayItemFunc<std::pair<std::string, ofJson>> proc) {
	ofJson ret;
	for(size_t i = 0; i < src.size(); ++i) {
		auto kv = proc(i, src[i], src);
		ret[kv.first] = kv.second;
	}
	return ret;
}
}
}}
