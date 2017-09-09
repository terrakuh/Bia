#pragma once

#include "biaNativeVariable.h"
#include "biaNumberBase.h"
#include "biaDouble.h"


namespace bia
{
namespace api
{
namespace framework
{
namespace native
{

template<>
class BiaNativeVariable<float> final : public BiaNumberBase
{
public:
	inline BiaNativeVariable(float p_data) : m_rData(p_data) {}
	template<typename T>
	inline BiaNativeVariable(T&&)
	{
		throw exception::UnknownException("Unknown float type.");
	}
	inline virtual void Print() const override
	{
		printf("%f", m_rData);
	}
	inline virtual bool Clone(void * p_pDestination) override
	{
		new(p_pDestination) BiaNativeVariable<float>(m_rData);

		return true;
	}
	inline virtual bool Store(BiaMember * p_pRight) override
	{
		return NativeHelper<bool, true, false>([this](auto p_right) { m_rData = static_cast<float>(p_right); }, p_pRight);
	}
	inline virtual bool Inc() override
	{
		++m_rData;

		return true;
	}
	inline virtual bool Dec() override
	{
		--m_rData;

		return true;
	}
	inline virtual int Conditional() const override
	{
		return m_rData != 0.0 ? 1 : 0;
	}
	inline virtual int Operator(unsigned int p_uiOperator, void * p_pDestination, BiaMember * p_pRight) override
	{
		return NativeOperatorHelperFloat<float>(p_uiOperator, m_rData, p_pDestination, p_pRight);
	}
	inline virtual unsigned int GetFlags() const override
	{
		return F_VARIABLE | F_NATIVE_FLOAT;
	}

protected:
	inline virtual void * LoadNative() override
	{
		return &m_rData;
	}
	inline virtual void * LoadPtr(PTR_TYPE) override
	{
		m_prData = &m_rData;

		return &m_prData;
	}
	inline virtual bool IsType(NATIVE_TYPE p_type, bool) const override
	{
		return p_type == NATIVE_TYPE::FLOAT;
	}

private:
	float m_rData;
	float * m_prData;
};

}
}
}
}