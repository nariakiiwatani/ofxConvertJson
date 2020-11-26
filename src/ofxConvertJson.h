#pragma once

#include "ofJson.h"
#include <initializer_list>

namespace ofx { namespace convertjson {

using ConvFunc = std::function<ofJson(const ofJson&)>;
using PickerFunc = std::function<bool(const std::string&)>;
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

static ConvFunc CherryPick(Picker pick) {
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

static ConvFunc Set(const ofJson &new_value) {
	return [new_value](const ofJson &src) {
		return new_value;
	};
}

}}
