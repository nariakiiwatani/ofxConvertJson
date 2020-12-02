#pragma once

#include <json.hpp>
#include <initializer_list>
#include <iomanip>
#include <sstream>

namespace convjson {

using Json = nlohmann::json;

extern bool SaveJson(const std::string &path, const Json &json);
extern void PrintFunc(const Json &json);

using PickerFunc = std::function<bool(const std::string&)>;

using ValueDispatchFunc = std::function<Json(std::size_t index, const Json &src)>;
template<typename Ret>
using ObjItemFunc = std::function<Ret(const std::string &key, const Json &item, const Json &obj)>;
template<typename Ret>
using ArrayItemFunc = std::function<Ret(std::size_t index, const Json &item, const Json &obj)>;

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
	
	operator PickerFunc() const { return func_; }
	bool operator()(const std::string &key) const { return func_(key); }
private:
	PickerFunc func_;
};

static void CherryPick(Json &srcdst, Picker pick) {
	using namespace std;
	Json ret;
	for(auto it = begin(srcdst); it != end(srcdst); ++it) {
		if(pick(it.key())) {
			ret[it.key()] = it.value();
		}
	}
	srcdst = ret;
}

static Json Dispatch(const Json &src, std::size_t count, ValueDispatchFunc func) {
	Json ret;
	for(std::size_t i = 0; i < count; ++i) {
		ret.push_back(func(i, src));
	}
	return ret;
}

static Json Set(const Json &src, const Json &new_value) {
	return new_value;
}

static void Print(const Json &src) {
	PrintFunc(src);
}

static void Copy(const Json &src, Json &dst) {
	dst = src;
}

static void Save(const Json &src, const std::string &basename) {
	SaveJson(basename+".json", src);
}

using ObjNamer = ObjItemFunc<std::string>;
static void SaveObjEach(const Json &src, ObjNamer namer) {
	using namespace std;
	for(auto it = begin(src); it != end(src); ++it) {
		Save(it.value(), namer(it.key(), it.value(), src));
	}
}
static void SaveObjEach(const Json &src, const std::string &basename="") {
	return SaveObjEach(src, [basename](const std::string &key, const Json&, const Json&) {
		return basename + key;
	});
}
static Json ObjToArray(const Json &src, ObjItemFunc<Json> proc) {
	using namespace std;
	Json ret;
	for(auto it = begin(src); it != end(src); ++it) {
		ret.push_back(proc(it.key(), it.value(), src));
	}
	return ret;
}
static void ObjForEach(Json &srcdst, ObjItemFunc<Json> proc, Picker pick=Picker(true)) {
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
template<typename T>
std::string toString(T value, int precision, char padding='0') {
	std::ostringstream oss;
	oss << std::setw(precision) << std::setfill(padding) << value;
	return oss.str();
}
}
static void SaveArrayEach(const Json &src, ArrayNamer namer) {
	using namespace std;
	for(size_t i = 0; i < src.size(); ++i) {
		Save(src[i], namer(i, src[i], src));
	}
}
static void SaveArrayEach(const Json &src, const std::string &basename="") {
	int digit = getDigit(src.size());
	return SaveArrayEach(src, [digit, basename](std::size_t index, const Json&, const Json&) {
		return basename+toString(index, digit);
	});
}
static Json ArrayToObj(const Json &src, ArrayItemFunc<std::pair<std::string, Json>> proc) {
	Json ret;
	for(size_t i = 0; i < src.size(); ++i) {
		auto kv = proc(i, src[i], src);
		ret[kv.first] = kv.second;
	}
	return ret;
}

namespace helpers {

// A Converter can change both data inside and Helper's type in method chain.
template<typename Input, typename Output>
using Converter = std::function<Output(const Json&)>;
// A Modifier modifies data but doesn't change Helper's type.
template<typename Input>
using Modifier = std::function<void(Json&)>;
// An Effector provides various effects outside JSON. Doesn't change data at all.
template<typename Input>
using Effector = std::function<void(const Json&)>;

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

class Any : public ValueOrRef<Json>
{
public:
	using ValueOrRef::ValueOrRef;
};

template<typename ConcreteType>
class Wrapper : public Any
{
public:
	using Any::Any;
	
	template<typename Output>
	Output convert(Converter<ConcreteType, Output> converter) {
		return converter(ref());
	}
	
	ConcreteType modify(Modifier<ConcreteType> modifier) {
		modifier(ref());
		return static_cast<const ConcreteType&>(*this);
	}
	
	ConcreteType effect(Effector<ConcreteType> effector) const {
 		effector(value());
		return static_cast<const ConcreteType&>(*this);
	}
	
	template<typename T>
	T castTo() { return T(ref()); }
};

class Value : public Wrapper<Value>
{
public:
	using Wrapper::Wrapper;
};

class Object : public Wrapper<Object>
{
public:
	using Wrapper::Wrapper;
};

class Array : public Wrapper<Array>
{
public:
	using Wrapper::Wrapper;
	std::size_t size() const { return value().size(); }
};

template<typename Input, typename Output, typename ...Args>
static std::function<Converter<Input, Output>(Args...)> ConvCast(Json(*proc)(const Json&, Args...)) {
	return [proc](Args &&...args) -> Converter<Input, Output> {
		return std::bind(proc, std::placeholders::_1, std::forward<Args>(args)...);
	};
}

template<typename Input, typename ...Args>
static std::function<Modifier<Input>(Args...)> ModCast(void(*proc)(Json&, Args...)) {
	return [proc](Args &&...args) -> Modifier<Input> {
		return std::bind(proc, std::placeholders::_1, std::forward<Args>(args)...);
	};
}

template<typename Input, typename ...Args>
static std::function<Effector<Input>(Args...)> EffectCast(void(*proc)(const Json&, Args...)) {
	return [proc](Args &&...args) {
		return std::bind(proc, std::placeholders::_1, std::forward<Args>(args)...);
	};
}

static auto Dispatch = ConvCast<Any, Array>(::convjson::Dispatch);
static auto CherryPick = ModCast<Object>(::convjson::CherryPick);

static auto ToArray = ConvCast<Object, Array>(::convjson::ObjToArray);

static auto ObjForEachFunc = ModCast<Object>(::convjson::ObjForEach);
static auto ObjForEach(::convjson::ObjItemFunc<Any> proc, ::convjson::Picker pick=::convjson::Picker(true))
-> decltype(ObjForEachFunc(proc, pick)) {
	return ObjForEachFunc(proc, pick);
}

static auto ToObj = ConvCast<Array, Object>(::convjson::ArrayToObj);

static auto Print = EffectCast<Any>(::convjson::Print);

static auto SetValue = ConvCast<Any, Value>(::convjson::Set);
static auto SetObj = ConvCast<Any, Object>(::convjson::Set);
static auto SetArray = ConvCast<Any, Array>(::convjson::Set);

static auto Copy = EffectCast<Any>(::convjson::Copy);

static auto Save = EffectCast<Any>(::convjson::Save);


static auto SaveObjEach = EffectCast<Object>(static_cast<void (*)(const Json&, const std::string&)>(::convjson::SaveObjEach));
static auto SaveObjEachWithNamer = EffectCast<Object>(static_cast<void (*)(const Json&, ::convjson::ObjNamer)>(::convjson::SaveObjEach));

static auto SaveArrayEach = EffectCast<Array>(static_cast<void (*)(const Json&, const std::string&)>(::convjson::SaveArrayEach));
static auto SaveArrayEachWithNamer = EffectCast<Array>(static_cast<void (*)(const Json&, ::convjson::ArrayNamer)>(::convjson::SaveArrayEach));
}
}
