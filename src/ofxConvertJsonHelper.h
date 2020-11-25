#pragma once

#include "ofJson.h"

namespace ofx { namespace convertjson {
template<typename Parent>
class ContainerHelper;

class Helper
{
public:
	Helper(ofJson &ref)
	:ref_(ref)
	{}

	operator ofJson&() { return get(); }
	ofJson& get() { return ref_; }
	
	template<typename ConvType, typename ...Args>
	Helper& apply(Args &&...args) {
		ref_ = ConvType(std::forward<Args...>(args...)).convert(get());
		return *this;
	}
	Helper& copy(ofJson &dst) {
		dst = get();
		return *this;
	}
	ContainerHelper<Helper> foreach();
	
protected:
	ofJson &ref_;
};

template<typename Parent>
class ContainerHelper
{
public:
	ContainerHelper(Parent &parent)
	:parent_(parent)
	{}
	
	template<typename ConvType, typename ...Args>
	ContainerHelper<Parent>& apply(Args &&...args) {
		parent_.template apply<ForEach<ConvType>>(std::forward<Args...>(args...));
		return *this;
	}

	ofJson& get() { return parent_.get(); }
	Parent& join() { return parent_; }
	ContainerHelper<Parent>& copy(ofJson &dst) {
		parent_.copy(dst);
		return *this;
	}

	ContainerHelper<ContainerHelper<Parent>> foreach() {
		return ContainerHelper<ContainerHelper<Parent>>(*this);
	}
protected:
	Parent &parent_;
};

inline ContainerHelper<Helper> Helper::foreach() {
	return ContainerHelper<Helper>(*this);
}

}}
