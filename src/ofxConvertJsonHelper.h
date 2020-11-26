#pragma once

#include "ofJson.h"
#include "ofxConvertJson.h"

namespace ofx { namespace convertjson {

namespace helpers {

using ConvFunc = ofx::convertjson::ConvFunc;
using Picker = ofx::convertjson::Picker;

template<typename T>
class ValueOrRef
{
public:
	ValueOrRef(): ref_(value_) {}
//	ValueOrRef(T value): value_(value), ref_(value_) {}
	ValueOrRef(const T &value): value_(value), ref_(value_) {}
	ValueOrRef(T &&value): value_(value), ref_(value_) {}
	ValueOrRef(T &ref): ref_(ref) {}
	
	const T& value() const { return ref_; }
	T& ref() { return ref_; }
	operator T&() { return ref(); }
	operator const T&() const { return value(); }
protected:
	T value_, &ref_;
};

class Array;

template<typename ConcreteHelper>
class Helper : public ValueOrRef<ofJson>
{
public:
	using ValueOrRef::ValueOrRef;
	
	ConcreteHelper& apply(ConvFunc proc) {
		ref() = proc(value());
		return casted();
	}
	ConcreteHelper& copy(ofJson &dst) {
		dst = value();
		return casted();
	}
	ConcreteHelper& save(const std::string &basename, int indent=-1) {
		ofFile(basename+".json", ofFile::WriteOnly) << ref().dump(indent);
		return casted();
	}
	ConcreteHelper& print(int indent=-1, std::ostream &os=std::cout) {
		os << ref().dump(indent);
		return casted();
	}
	ConcreteHelper& println(int indent=-1, std::ostream &os=std::cout) {
		print(indent, os);
		os << std::endl;
		return casted();
	}
	Array dispatch(std::initializer_list<ConvFunc> proc) const;
	
	template<typename T>
	T castTo() { return T(ref()); }
private:
	ConcreteHelper& casted() { return static_cast<ConcreteHelper&>(*this); }
};

class Value : public Helper<Value>
{
public:
	using Helper::Helper;
};

class Object : public Helper<Object>
{
public:
	using Helper::Helper;
	
	using NamerFunction = std::function<std::string(const std::string &key, const ofJson &value, const ofJson &src)>;
	
	template<typename T>
	Object& pick(T picker_info, ConvFunc proc) {
		return pick(Picker(picker_info), proc);
	}

	Object& pick(Picker picker, ConvFunc proc);

	Array toArray(const std::string &name_of_key) const;

	Object& saveEach(NamerFunction namer, int indent=-1);
	Object& saveEach(const std::string &basename="", int indent=-1) {
		return saveEach([basename](const std::string &key, const ofJson&, const ofJson&) {
			return basename + key;
		}, indent);
	}
};

class Array : public Helper<Array>
{
public:
	using Helper::Helper;
	
	std::size_t size() const { return value().size(); }
	
	using NamerFunction = std::function<std::string(std::size_t index, const ofJson &value, const ofJson &src)>;
	
	Object toObject(NamerFunction namer) const;
	Object toObject(const std::string &basename="item_") const {
		int digit = getDigit();
		return toObject([basename, digit](std::size_t index, const ofJson&, const ofJson&) {
			return basename + ofToString(index, digit, '0');
		});
	}
	enum MergeStrategy {
		KEEP_FIRST,
		KEEP_LAST,
	};
	Object mergeAsObject(MergeStrategy strategy, NamerFunction not_obj_namer) const;
	Object mergeAsObject(MergeStrategy strategy, const std::string &not_obj_basename="item_") const {
	   int digit = getDigit();
	   return mergeAsObject(strategy, [not_obj_basename, digit](std::size_t index, const ofJson&, const ofJson&) {
		   return not_obj_basename + ofToString(index, digit, '0');
	   });
	}
	
	Array& saveEach(NamerFunction namer, int indent=-1);
	Array& saveEach(const std::string &basename="", int indent=-1) {
		int digit = getDigit();
		return saveEach([digit, basename, indent](std::size_t index, const ofJson&, const ofJson&) {
			return basename+ofToString(index, digit, indent);
		}, indent);
	}

private:
	std::size_t getDigit() const {
		int digit = 0;
		while(pow(10, ++digit) <= size()) {}
		return digit;
	}
};
}
}}
