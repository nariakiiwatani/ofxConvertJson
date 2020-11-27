#pragma once

#include "ofJson.h"
#include <initializer_list>

namespace ofx { namespace convertjson {
namespace conv {

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

static ofJson ObjToArray(const ofJson &src, const std::string &name_of_key) {
	using namespace std;
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

using ObjNamer = std::function<std::string(const std::string &key, const ofJson &value, const ofJson &src)>;

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

using ArrayNamer = std::function<std::string(std::size_t index, const ofJson &value, const ofJson &src)>;
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
		Save(src, namer(i, src[i], src), indent);
	}
}
static void SaveArrayEach(const ofJson &src, const std::string &basename="", int indent=-1) {
	int digit = getDigit(src.size());
	return SaveArrayEach(src, [digit, basename, indent](std::size_t index, const ofJson&, const ofJson&) {
		return basename+ofToString(index, digit, indent);
	}, indent);
}
}
}}
