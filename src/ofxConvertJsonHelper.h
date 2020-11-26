#pragma once

#include "ofJson.h"
#include "ofxConvertJson.h"

namespace ofx { namespace convertjson {

namespace helpers {

using ConvFunc = ofx::convertjson::ConvFunc;

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
		return static_cast<ConcreteHelper&>(*this);
	}
	ConcreteHelper& copy(ofJson &dst) {
		dst = value();
		return static_cast<ConcreteHelper&>(*this);
	}
	Array dispatch(std::initializer_list<ConvFunc> proc) const;
	
	template<typename T>
	T castTo() { return T(ref()); }
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
	
	Object& pick(const std::string &key, ConvFunc proc);
	
	Array toArray(const std::string &name_of_key) const;
};

class Array : public Helper<Array>
{
public:
	using Helper::Helper;
	
	std::size_t size() const { return value().size(); }
	
	using NamerFunction = std::function<std::string(std::size_t index)>;
	
	Object toObject(NamerFunction namer) const;
	Object toObject(const std::string &basename="item_") const {
		int digit = getDigit();
		return toObject([basename, digit](std::size_t index) {
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
	   return mergeAsObject(strategy, [not_obj_basename, digit](std::size_t index) {
		   return not_obj_basename + ofToString(index, digit, '0');
	   });
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
