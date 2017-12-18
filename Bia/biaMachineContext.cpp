#include "biaMachineContext.hpp"
#include "biaOutputStreamBuffer.hpp"
#include "biaInt.hpp"

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

api::framework::BiaMember * BiaMachineContext::AddressOf(StringKey p_name)
{
	auto pResult = m_index.find(p_name);

	//Address found
	if (pResult != m_index.end())
		return pResult->second;
	//Create address
	else
	{
		auto pAddress = m_storage.CreateElement<api::framework::BiaMember>();

		m_index.insert({ p_name, pAddress });

		return pAddress;
	}
}

void BiaMachineContext::InstantiateInt_32(int32_t p_unValue, api::framework::BiaMember * p_pMember)
{
	printf("int instantiated %i\n", p_unValue);
	printf("member: %p\n", p_pMember);

	//Destruct old object
	p_pMember->~BiaMember();

	//Create new int object
	new(p_pMember) api::framework::BiaInt(p_unValue);
}

}
}