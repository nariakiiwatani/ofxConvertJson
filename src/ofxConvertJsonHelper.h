#pragma once

#include "ofJson.h"
#include "ofxConvertJson.h"

namespace ofx { namespace convertjson {

namespace helpers {

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
class Helper : public ValueOrRef<ofJson>
{
public:
	using ValueOrRef::ValueOrRef;
	
	template<typename ConvType, typename ...Args>
	Helper& apply(Args &&...args) {
		ref() = ConvType(std::forward<Args...>(args...)).convert(value());
		return *this;
	}
	Helper& copy(ofJson &dst) {
		dst = value();
		return *this;
	}
	Array dispatch(std::initializer_list<std::function<ofJson(const ofJson&)>> proc) const;
};

class Array : public Helper
{
public:
	using Helper::Helper;
};

Array Helper::dispatch(std::initializer_list<std::function<ofJson(const ofJson&)>> proc) const {
	std::vector<ofJson> ret;
	ret.reserve(proc.size());
	for(auto &&conv : proc) {
		ret.push_back(conv(value()));
	}
	return Array(ret);
}

}
}}
