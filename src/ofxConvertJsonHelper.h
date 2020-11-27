#pragma once

#include "ofJson.h"
#include "ofxConvertJson.h"

namespace ofx { namespace convertjson {

namespace helpers {
template<typename Input, typename Output=Input>
using Modifier = std::function<Output(const Input&)>;
template<typename Input>
using Viewer = std::function<void(const Input&)>;
template<typename Input>
using Effect = std::function<void(const Input&)>;

using ConvFunc = ofx::convertjson::conv::ConvFunc;
using NoModFunc = ofx::convertjson::conv::NoMod;
using Picker = ofx::convertjson::conv::Picker;

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

class Json : public ValueOrRef<ofJson>
{
public:
	using ValueOrRef::ValueOrRef;
};


class Array;

template<typename ConcreteType>
class Any : public Json
{
public:
	using Json::Json;
	
	template<typename Output>
	Output mod(Modifier<ConcreteType, Output> modifier) {
		return modifier(casted());
	}
	
	template<typename Input>
	ConcreteType view(Viewer<Input> viewer) const {
 		viewer(casted());
		return casted();
	}
	
	template<typename Input>
	ConcreteType effect(Effect<Input> effect) const {
 		effect(casted());
		return casted();
	}
	
	Array dispatch(std::initializer_list<ConvFunc> proc) const;
	
	template<typename T>
	T castTo() { return T(ref()); }
private:
	ConcreteType& casted() { return static_cast<ConcreteType&>(*this); }
	const ConcreteType& casted() const { return static_cast<const ConcreteType&>(*this); }
};

class Value : public Any<Value>
{
public:
	using Any::Any;
	
	
};

class Object : public Any<Object>
{
public:
	using Any::Any;
	
	using NamerFunction = std::function<std::string(const std::string &key, const ofJson &value, const ofJson &src)>;
	
	template<typename T>
	Object& pick(T picker_info, ConvFunc proc) {
		return pick(Picker(picker_info), proc);
	}

	Object& pick(Picker picker, ConvFunc proc);
};

class Array : public Any<Array>
{
public:
	using Any::Any;
	
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
	
private:
	std::size_t getDigit() const {
		int digit = 0;
		while(pow(10, ++digit) <= size()) {}
		return digit;
	}
};

}
}}


namespace ofx { namespace convertjson {
namespace helpers {

template<typename Input, typename Output, typename Ret, typename ...Args>
static std::function<Modifier<Input, Output>(Args...)> ModCast(Ret (*proc)(const ofJson&, Args...)) {
	return [proc](Args &&...args) {
		return [=](const Input &input) -> Output {
			return proc(input.value(), std::move(args)...);
		};
	};
}

template<typename Input, typename Ret, typename ...Args>
static std::function<Viewer<Input>(Args...)> ViewCast(Ret (*proc)(const ofJson&, Args...)) {
	return [proc](Args &&...args) {
		return [=](const Input &input) {
			proc(input.value(), std::move(args)...);
		};
	};
}

template<typename Input, typename Ret, typename ...Args>
static std::function<Viewer<Input>(Args...)> EffectCast(Ret (*proc)(const ofJson&, Args...)) {
	return [proc](Args &&...args) {
		return [&](const Input &input) {
			proc(input.value(), std::forward<Args>(args)...);
		};
	};
}

static auto ToArray = ModCast<Object, Array>(::ofx::convertjson::conv::ObjToArray);

static auto PrintFunc = ViewCast<Json>(::ofx::convertjson::conv::Print);
static auto Print(int indent=-1)
-> decltype(PrintFunc(indent)) {
	return PrintFunc(indent);
}

static auto PrintlnFunc = ViewCast<Json>(::ofx::convertjson::conv::Println);
static auto Println(int indent=-1)
-> decltype(PrintlnFunc(indent)) {
	return PrintlnFunc(indent);
}

static auto Set = EffectCast<Json>(::ofx::convertjson::conv::Set);

static auto Copy = EffectCast<Json>(::ofx::convertjson::conv::Copy);

static auto SaveFunc = ViewCast<Json>(::ofx::convertjson::conv::Save);
static auto Save(const std::string &filename, int indent=-1)
-> decltype(SaveFunc(filename, indent)) {
	return SaveFunc(filename, indent);
}


static auto SaveObjEachFunc = ViewCast<Object>(static_cast<void (*)(const ofJson&, const std::string&, int)>(::ofx::convertjson::conv::SaveObjEach));
static auto SaveObjEach(const std::string &basename, int indent=-1)
-> decltype(SaveObjEachFunc(basename, indent)) {
	return SaveObjEachFunc(basename, indent);
}
static auto SaveObjEachWithNamerFunc = ViewCast<Object>(static_cast<void (*)(const ofJson&, conv::ObjNamer, int)>(::ofx::convertjson::conv::SaveObjEach));
static auto SaveObjEach(conv::ObjNamer namer, int indent=-1)
-> decltype(SaveObjEachWithNamerFunc(namer, indent)) {
	return SaveObjEachWithNamerFunc(namer, indent);
}

static auto SaveArrayEachFunc = ViewCast<Array>(static_cast<void (*)(const ofJson&, const std::string&, int)>(::ofx::convertjson::conv::SaveArrayEach));
static auto SaveArrayEach(const std::string &basename, int indent=-1)
-> decltype(SaveArrayEachFunc(basename, indent)) {
	return SaveArrayEachFunc(basename, indent);
}
static auto SaveArrayEachWithNamerFunc = ViewCast<Array>(static_cast<void (*)(const ofJson&, conv::ArrayNamer, int)>(::ofx::convertjson::conv::SaveArrayEach));
static auto SaveArrayEach(conv::ArrayNamer namer, int indent=-1)
-> decltype(SaveArrayEachWithNamerFunc(namer, indent)) {
	return SaveArrayEachWithNamerFunc(namer, indent);
}
}
}}
