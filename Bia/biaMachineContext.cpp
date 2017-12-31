#include "biaMachineContext.hpp"
#include "biaOutputStreamBuffer.hpp"
#include "biaUnknown.hpp"

#include <memory>
#include <cstdint>


namespace bia
{
namespace machine
{

void BiaMachineContext::Run(stream::BiaInputStream & p_input)
{
	uint8_t aucSpace[sizeof(BiaMachineCode)];
	std::unique_ptr<BiaMachineCode, void(*)(BiaMachineCode*)> pMachineCode(nullptr, [](BiaMachineCode * p_pCode) {
		p_pCode->~BiaMachineCode();
	});

	{
		//Compile
		stream::BiaOutputStreamBuffer compiled;

		//Make the compiled buffer executable
		pMachineCode.reset(new(aucSpace) BiaMachineCode(compiled.GetBuffer()));
	}

	//Execute script
	if (pMachineCode->IsValid())
		pMachineCode->Execute();
}

void BiaMachineContext::DeleteTemporaryObjectTo(uint32_t p_unLowerIndex)
{
}

framework::BiaMember * BiaMachineContext::AddressOf(StringKey p_name)
{
	auto pResult = m_index.find(p_name);

	//Address found
	if (pResult != m_index.end())
		return pResult->second;
	//Create address
	else
	{
		auto pAddress = m_storage.CreateElement<framework::BiaUnknown>();

		m_index.insert({ p_name, pAddress });

		return pAddress;
	}
}

framework::BiaMember * BiaMachineContext::TemporaryAddress(uint32_t p_unIndex)
{
	return nullptr;
	//return m_temporaryStorage.GetSpace(p_unIndex);
}

}
}