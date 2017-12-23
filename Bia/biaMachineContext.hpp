#pragma once

#include <unordered_map>
#include <map>
#include <string>

#include "biaConfig.hpp"
#include "biaMember.hpp"
#include "biaStorage.hpp"
#include "biaTemporaryStorage.hpp"
#include "biaMachineCode.hpp"
#include "biaInputStream.hpp"


namespace bia
{
namespace machine
{

class BiaMachineContext final
{
public:
BiaMachineContext() : m_storage(34)
{}
	//void AddScript(std::string p_stScriptName, script);
	//void RemoveScript(std::string p_stScriptName);
	//void Run(std::string p_stScriptName);
	void Run(stream::BiaInputStream & p_script);

//private:
	//friend BiaCompiler;

	typedef std::string StringKey;
	/*struct StringKey
	{
		inline StringKey(std::string p_stString) : stKey(std::move(p_stString))
		{

		}
		inline StringKey(const void * p_pBuffer, size_t p_iSize) : stKey(static_cast<const char*>(p_pBuffer), p_iSize)
		{

		}

		std::string stKey;
	};*/

	std::unordered_map<StringKey, api::framework::BiaMember*> m_index;	/**	Stores all pointers to the known variables.	*/
	std::map<StringKey, BiaMachineCode> m_scripts;	/**	Stores all scripts associated with this context.	*/

	BiaStorage<16> m_storage;
	BiaTemporaryStorage<api::framework::BiaMember, 16> m_temporaryStorage;


	void DeleteTemporaryObjectTo(uint32_t p_unLowerIndex);
	api::framework::BiaMember * AddressOf(StringKey p_name);
	api::framework::BiaMember * TemporaryAddress(uint32_t p_unIndex);
};

}
}