#pragma once

#include "ofJson.h"
#include <initializer_list>

namespace ofx { namespace convertjson {

using ConvFunc = std::function<ofJson(const ofJson&)>;
using ConvFuncKV = std::function<ofJson(const std::string&, const ofJson&)>;
using Picker = std::function<bool(const std::string&)>; 

class Converter
{
public:
	Converter() {}
	Converter(ConvFunc conv): conv_(conv) {}
	ofJson convert(const ofJson &src) const { return conv_(src); }
	operator ConvFunc() const { return conv_; }
protected:
	ConvFunc conv_ = [](const ofJson &src){ return src; };
};

class CherryPick : public Converter
{
public:
	CherryPick(Picker pick) {
		conv_ = [pick](const ofJson &src) {
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
	CherryPick(const std::string &key)
	:CherryPick([key](const std::string &k) {
		return k==key;
	}) {}
	
	CherryPick(const std::vector<std::string> &keys)
	:CherryPick([keys](const std::string &key) {
		using namespace std;
		return find(begin(keys), end(keys), key) != end(keys);
	}) {}
	CherryPick(std::initializer_list<std::string> keys)
	:CherryPick([keys](const std::string &key) {
		using namespace std;
		return find(begin(keys), end(keys), key) != end(keys);
	}) {}
};

class Print : public Converter
{
public:
	Print(int indent=4) {
		conv_ = [indent](const ofJson &src) {
			std::cout << src.dump(indent) << std::endl;
			return src;
		};
	}
};

/*
 * old things....

class ModValueOfKey : public ofxConvertJson
{
public:
	ModValueOfKey(const std::string &key, const std::function<ofJson(const ofJson&)> &mod)
	:mod_(mod)
	{}

	ModValueOfKey(const std::string &key, const ofJson &value)
	:ModValueOfKey(key, [value](const ofJson&){return value;})
	{}
	
		
	ofJson convert(const ofJson &src) const override {
		assert(src.is_object());
		auto it = src.find(key_);
		if(it == end(src)) {
			return src;
		}
		auto ret = src;
		ret[key_] = mod_(it.value());
		return ret;
	}
private:
	std::string key_;
	std::function<ofJson(const ofJson&)> mod_ = [](const ofJson &src){ return src; };
};

class ArrayToObj : public ofxConvertJson
{
public:
	ArrayToObj(const std::string &key)
	:key_(key)
	{}
	
	ofJson convert(const ofJson &src) const override {
		assert(src.is_array());
		using namespace std;
		ofJson ret;
		for(auto &&s : src) {
			auto it = s.find(key_);
			if(it == end(s)) {
				continue;
			}
			ret[it.value().get<string>()] = s;
		}
		return ret;
	}
protected:
	std::string key_;
};
class ObjToArray : public ofxConvertJson
{
public:
	ObjToArray(const std::string &key)
	:key_(key)
	{}
	
	ofJson convert(const ofJson &src) const override {
		assert(src.is_object());
		using namespace std;
		ofJson ret;
		for(auto it = begin(src); it != end(src); ++it) {
			auto value = it.value();
			value[key_] = it.key();
			ret.push_back(value);
		}
		return ret;
	}
protected:
	std::string key_;
};



class SaveFile : public ofxConvertJson
{
public:
	SaveFile(const std::string &filepath)
	:filepath_(filepath)
	{}
	
	ofJson convert(const ofJson &src) const override {
		ofSaveJson(filepath_, src);
		return src;
	}
protected:
	std::string filepath_;
};
*/
}}
