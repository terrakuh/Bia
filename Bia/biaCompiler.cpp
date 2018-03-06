#include "biaCompiler.hpp"
#include "biaException.hpp"
#include "biaOutputStreamBuffer.hpp"
#include "biaFloat.hpp"
#include "biaInterpreterStringSet.hpp"

#include <vector>
#include <algorithm>


namespace bia
{
namespace compiler
{

BiaCompiler::BiaCompiler(stream::BiaOutputStream & p_output, machine::BiaMachineContext & p_context, machine::BiaAllocator * p_pAllocator) : m_toolset(p_output), m_context(p_context), m_machineSchein(p_pAllocator)
{
	//Reserve temporary members
	m_parameter = m_toolset.ReserveTemporyMembers();
}

BiaCompiler::~BiaCompiler()
{
	//Clean up
	auto max = m_counter.Max();

	if (max == 0)
		m_toolset.DiscardTemporaryMembers(m_parameter);
	else
		m_toolset.CommitTemporaryMembers(m_context, m_parameter, max);
}

void BiaCompiler::Report(const grammar::Report * p_pBegin, const grammar::Report * p_pEnd)
{/*
	for (auto i = p_pBegin; i < p_pEnd; ++i)
	{
		printf("ID: %ui TK: %ui\n", i->unRuleId, i->unTokenId);
	}
	*/
	HandleRoot(p_pBegin);
}

machine::BiaMachineSchein BiaCompiler::GetMachineSchein()
{
	return std::move(m_machineSchein);
}

void BiaCompiler::HandleConstantOperation(VALUE_TYPE p_leftType, Value p_leftValue, uint32_t p_unOperator)
{
	using namespace framework;

	switch (p_leftType)
	{
	case VALUE_TYPE::INT_32:
		return HandleConstantOperation(p_leftValue.nInt, p_unOperator);
	case VALUE_TYPE::INT_64:
		return HandleConstantOperation(p_leftValue.llInt, p_unOperator);
	case VALUE_TYPE::FLOAT:
		return HandleConstantOperation(p_leftValue.rFloat, p_unOperator);
	case VALUE_TYPE::DOUBLE:
		return HandleConstantOperation(p_leftValue.rDouble, p_unOperator);
	case VALUE_TYPE::STRING:
	default:
		BIA_COMPILER_DEV_INVALID
	}
}

void BiaCompiler::HandleNumber(const grammar::Report * p_pReport)
{
	switch (p_pReport->unCustomParameter)
	{
	case grammar::NI_INTEGER:
	{
		if (p_pReport->content.token.iSize < 70)
		{
			char acTmp[70];

			memcpy(acTmp, p_pReport->content.token.pcString, p_pReport->content.token.iSize);
			acTmp[p_pReport->content.token.iSize] = 0;

			//Interpret
			char * pcEnd = nullptr;
			m_value.llInt = std::strtoll(acTmp, &pcEnd, 10);

			if (acTmp == pcEnd)
				throw exception::NumberException("Invalid number.");
		}
		else
			throw exception::NumberException("Too long number.");

		//64-Bit number
		if(m_value.llInt < 0xffffffff80000000ll || m_value.llInt > 0x7fffffffll)
			m_valueType = VALUE_TYPE::INT_64;
		//32-Bit number
		else
		{
			m_value.nInt = static_cast<int32_t>(m_value.llInt);
			m_valueType = VALUE_TYPE::INT_32;
		}

		break;
	}
	case grammar::NI_FLOAT:
	{
		if (p_pReport->content.token.iSize < 128)
		{
			char acTmp[129];

			memcpy(acTmp, p_pReport->content.token.pcString, p_pReport->content.token.iSize);
			acTmp[p_pReport->content.token.iSize] = 0;

			//Interpret
			char * pcEnd = nullptr;
			m_value.rFloat = strtof(acTmp, &pcEnd);

			if (acTmp == pcEnd)
				throw exception::NumberException("Invalid number.");
		}
		else
			throw exception::NumberException("Too long number.");

		m_valueType = VALUE_TYPE::FLOAT;

		break;
	}
	case grammar::NI_DOUBLE:
	{
		if (p_pReport->content.token.iSize < 128)
		{
			char acTmp[129];

			memcpy(acTmp, p_pReport->content.token.pcString, p_pReport->content.token.iSize);
			acTmp[p_pReport->content.token.iSize] = 0;

			//Interpret
			char * pcEnd = nullptr;
			m_value.rDouble = strtod(acTmp, &pcEnd);

			if (acTmp == pcEnd)
				throw exception::NumberException("Invalid number.");
		}
		else
			throw exception::NumberException("Too long number.");

		m_valueType = VALUE_TYPE::DOUBLE;

		break;
	}
	default:
		BIA_COMPILER_DEV_INVALID
	}
}

void BiaCompiler::HandleString(const grammar::Report * p_pReport)
{
	m_valueType = VALUE_TYPE::STRING;
	m_value.string.pcString = p_pReport->content.token.pcString;
	m_value.string.iSize = p_pReport->content.token.iSize;
}

void BiaCompiler::HandleIdentifier(const grammar::Report * p_pReport)
{
	m_valueType = VALUE_TYPE::MEMBER;
	m_value.pMember = m_context.AddressOf(machine::StringKey(p_pReport->content.token.pcString, p_pReport->content.token.iSize));
}

void BiaCompiler::HandleOperator(VALUE_TYPE p_leftType, Value p_leftValue, uint32_t p_unOperator, BiaTempCounter::counter_type p_destinationIndex)
{
	using namespace machine::architecture;

	//Mark current
	//m_counter.Current();

	switch (p_leftType)
	{
	case VALUE_TYPE::INT_32:
	{
		switch (m_valueType)
		{
		case VALUE_TYPE::MEMBER:
			m_toolset.Call(&machine::link::OperatorCallInt_32, p_leftValue.nInt, p_unOperator, m_value.pMember, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		case VALUE_TYPE::TEMPORARY_MEMBER:
			m_toolset.Call(&machine::link::OperatorCallInt_32, p_leftValue.nInt, p_unOperator, m_value.pMember, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		}

		break;
	}
	case VALUE_TYPE::INT_64:
	{
		switch (m_valueType)
		{
		case VALUE_TYPE::MEMBER:
			m_toolset.Call(&machine::link::OperatorCallInt_64, p_leftValue.llInt, p_unOperator, m_value.pMember, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		case VALUE_TYPE::TEMPORARY_MEMBER:
			m_toolset.Call(&machine::link::OperatorCallInt_64, p_leftValue.llInt, p_unOperator, m_value.pMember, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		}

		break;
	}
	case VALUE_TYPE::FLOAT:
	{
		switch (m_valueType)
		{
		case VALUE_TYPE::MEMBER:
			m_toolset.Call(&machine::link::OperatorCallFloat, p_leftValue.rFloat, p_unOperator, m_value.pMember, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		case VALUE_TYPE::TEMPORARY_MEMBER:
			m_toolset.Call(&machine::link::OperatorCallFloat, p_leftValue.rFloat, p_unOperator, m_value.pMember, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		}

		break;
	}
	case VALUE_TYPE::DOUBLE:
	{
		switch (m_valueType)
		{
		case VALUE_TYPE::MEMBER:
			m_toolset.Call(&machine::link::OperatorCallDouble, p_leftValue.rDouble, p_unOperator, m_value.pMember, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		case VALUE_TYPE::TEMPORARY_MEMBER:
			m_toolset.Call(&machine::link::OperatorCallDouble, p_leftValue.rDouble, p_unOperator, m_value.pMember, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		}

		break;
	}
	case VALUE_TYPE::MEMBER:
	{
		//Right value
		switch (m_valueType)
		{
		case VALUE_TYPE::INT_32:
			if (p_destinationIndex != -1)
				m_toolset.Call(&framework::BiaMember::OperatorCallInt_32, p_leftValue.pMember, p_unOperator, m_value.nInt, BiaToolset::TemporaryMember(p_destinationIndex));
			else
				m_toolset.Call(&framework::BiaMember::OperatorAssignCallInt_32, p_leftValue.pMember, p_unOperator, m_value.nInt);

			break;
		case VALUE_TYPE::INT_64:
			if (p_destinationIndex != -1)
				m_toolset.Call(&framework::BiaMember::OperatorCallInt_64, p_leftValue.pMember, p_unOperator, m_value.llInt, BiaToolset::TemporaryMember(p_destinationIndex));
			else
				m_toolset.Call(&framework::BiaMember::OperatorAssignCallInt_64, p_leftValue.pMember, p_unOperator, m_value.llInt);

			break;
		case VALUE_TYPE::FLOAT:
			if (p_destinationIndex != -1)
				m_toolset.Call(&framework::BiaMember::OperatorCallFloat, p_leftValue.pMember, p_unOperator, m_value.rFloat, BiaToolset::TemporaryMember(p_destinationIndex));
			else
				m_toolset.Call(&framework::BiaMember::OperatorAssignCallFloat, p_leftValue.pMember, p_unOperator, m_value.rFloat);

			break;
		case VALUE_TYPE::DOUBLE:
			if (p_destinationIndex != -1)
				m_toolset.Call(&framework::BiaMember::OperatorCallDouble, p_leftValue.pMember, p_unOperator, m_value.rDouble, BiaToolset::TemporaryMember(p_destinationIndex));
			else
				m_toolset.Call(&framework::BiaMember::OperatorAssignCallDouble, p_leftValue.pMember, p_unOperator, m_value.rDouble);

			break;
		case VALUE_TYPE::MEMBER:
			if (p_destinationIndex != -1)
				m_toolset.Call(&framework::BiaMember::OperatorCall, p_leftValue.pMember, p_unOperator, m_value.pMember, BiaToolset::TemporaryMember(p_destinationIndex));
			else
				m_toolset.Call(&framework::BiaMember::OperatorAssignCall, p_leftValue.pMember, p_unOperator, m_value.pMember);

			break;
		case VALUE_TYPE::TEMPORARY_MEMBER:
			if (p_destinationIndex != -1)
				m_toolset.Call(&framework::BiaMember::OperatorCall, p_leftValue.pMember, p_unOperator, BiaToolset::TemporaryMember(m_value.temporaryResultIndex), BiaToolset::TemporaryMember(p_destinationIndex));
			else
				m_toolset.Call(&framework::BiaMember::OperatorAssignCall, p_leftValue.pMember, p_unOperator, BiaToolset::TemporaryMember(m_value.temporaryResultIndex));

			break;
		default:
			BIA_COMPILER_DEV_INVALID
		}

		break;
	}
	case VALUE_TYPE::TEMPORARY_MEMBER:
	{
		switch (m_valueType)
		{
		case VALUE_TYPE::INT_32:
			//Assign call if destination is the left hand side
			if (p_leftValue.temporaryResultIndex == p_destinationIndex)
				m_toolset.Call(&framework::BiaMember::OperatorAssignCallInt_32, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, m_value.nInt);
			//Normal operator call with new destination
			else
				m_toolset.Call(&framework::BiaMember::OperatorCallInt_32, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, m_value.nInt, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		case VALUE_TYPE::INT_64:
			//Assign call if destination is the left hand side
			if (p_leftValue.temporaryResultIndex == p_destinationIndex)
				m_toolset.Call(&framework::BiaMember::OperatorAssignCallInt_64, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, m_value.llInt);
			//Normal operator call with new destination
			else
				m_toolset.Call(&framework::BiaMember::OperatorCallInt_64, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, m_value.llInt, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		case VALUE_TYPE::FLOAT:
			//Assign call if destination is the left hand side
			if (p_leftValue.temporaryResultIndex == p_destinationIndex)
				m_toolset.Call(&framework::BiaMember::OperatorAssignCallFloat, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, m_value.rFloat);
			//Normal operator call with new destination
			else
				m_toolset.Call(&framework::BiaMember::OperatorCallFloat, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, m_value.rFloat, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		case VALUE_TYPE::DOUBLE:
			//Assign call if destination is the left hand side
			if (p_leftValue.temporaryResultIndex == p_destinationIndex)
				m_toolset.Call(&framework::BiaMember::OperatorAssignCallDouble, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, m_value.rDouble);
			//Normal operator call with new destination
			else
				m_toolset.Call(&framework::BiaMember::OperatorCallDouble, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, m_value.rDouble, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		case VALUE_TYPE::MEMBER:
			//Assign call if destination is the left hand side
			if (p_leftValue.temporaryResultIndex == p_destinationIndex)
				m_toolset.Call(&framework::BiaMember::OperatorAssignCall, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, m_value.pMember);
			//Normal operator call with new destination
			else
				m_toolset.Call(&framework::BiaMember::OperatorCall, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, m_value.pMember, BiaToolset::TemporaryMember(p_destinationIndex));

			break;
		case VALUE_TYPE::TEMPORARY_MEMBER:
			//Assign call if destination is the left hand side
			if (p_leftValue.temporaryResultIndex == p_destinationIndex)
				m_toolset.Call(&framework::BiaMember::OperatorAssignCall, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, BiaToolset::TemporaryMember(m_value.temporaryResultIndex));
			//Normal operator call with new destination
			else
				m_toolset.Call(&framework::BiaMember::OperatorCall, BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator, BiaToolset::TemporaryMember(m_value.temporaryResultIndex), BiaToolset::TemporaryMember(p_destinationIndex));
			
			break;
		default:
			BIA_COMPILER_DEV_INVALID
		}

		break;
	}
	default:
		BIA_COMPILER_DEV_INVALID
	}

	m_valueType = VALUE_TYPE::TEMPORARY_MEMBER;
	m_value.temporaryResultIndex = p_destinationIndex;
}

void BiaCompiler::HandleCompareOperator(VALUE_TYPE p_leftType, Value p_leftValue, uint32_t p_unOperator)
{
gt_redo:;

	switch (p_leftType)
	{
	case VALUE_TYPE::INT_32:
	case VALUE_TYPE::INT_64:
	case VALUE_TYPE::FLOAT:
	case VALUE_TYPE::DOUBLE:
	case VALUE_TYPE::STRING:
	{
		switch (m_valueType)
		{
		case VALUE_TYPE::INT_32:
			return HandleConstantCompareOperation(p_leftType, p_leftValue, m_value.nInt, p_unOperator);
		case VALUE_TYPE::INT_64:
			return HandleConstantCompareOperation(p_leftType, p_leftValue, m_value.llInt, p_unOperator);
		case VALUE_TYPE::FLOAT:
			return HandleConstantCompareOperation(p_leftType, p_leftValue, m_value.rFloat, p_unOperator);
		case VALUE_TYPE::DOUBLE:
			return HandleConstantCompareOperation(p_leftType, p_leftValue, m_value.rDouble, p_unOperator);
		//case VALUE_TYPE::STRING:
		case VALUE_TYPE::MEMBER:
		case VALUE_TYPE::TEMPORARY_MEMBER:
		case VALUE_TYPE::RESULT_REGISTER:
			std::swap(m_valueType, p_leftType);
			std::swap(m_value, p_leftValue);

			goto gt_redo;
		default:
			BIA_COMPILER_DEV_INVALID
		}
	}
	case VALUE_TYPE::MEMBER:
		HandleCompareOperatorCall(p_leftValue.pMember, p_unOperator);

		break;
	case VALUE_TYPE::TEMPORARY_MEMBER:
		HandleCompareOperatorCall(machine::architecture::BiaToolset::TemporaryMember(p_leftValue.temporaryResultIndex), p_unOperator);

		break;
	case VALUE_TYPE::RESULT_REGISTER:
		HandleCompareOperatorCall(machine::architecture::BiaToolset::ResultValue(), p_unOperator);

		break;
	default:
		BIA_COMPILER_DEV_INVALID
	}

	m_valueType = VALUE_TYPE::TEST_VALUE_REGISTER;
}

const char * BiaCompiler::GetNameAddress(const grammar::Report * p_pReport)
{
	return m_context.NameAddressOf(p_pReport->content.token.pcString, p_pReport->content.token.iSize);
}

const char * BiaCompiler::GetStringLocation(Value::String p_string)
{
	if (p_string.iSize)
	{
		auto allocation = m_machineSchein.GetAllocator()->Allocate(p_string.iSize + 1, machine::BiaAllocator::MEMORY_TYPE::NORMAL);

		m_machineSchein.RegisterAllocation(allocation);

		//Copy string
		memcpy(allocation.pAddress, p_string.pcString, p_string.iSize);
		static_cast<char*>(allocation.pAddress)[p_string.iSize] = 0;

		return static_cast<const char*>(allocation.pAddress);
	}
	//Empty string
	else
		return "";
}

const char * BiaCompiler::GetParameterFormat(Value::String p_string)
{
	return m_context.FormatAddressOf(p_string.pcString, p_string.iSize);
}

const grammar::Report * BiaCompiler::HandleVariableDeclaration(grammar::report_range p_reports)
{
	//Handle value and prepare the result for a function call
	HandleValue<false>(FindNextChild<grammar::BGR_VALUE, 0, true>(p_reports.pBegin + 2, p_reports.pEnd)->content.children, [&] {
		//Get address of variable
		auto pVariable = m_context.AddressOf(machine::StringKey(p_reports.pBegin[1].content.token.pcString, p_reports.pBegin[1].content.token.iSize));
	
		//Make call
		switch (m_valueType)
		{
		case VALUE_TYPE::INT_32:
		{
			//Optimize common used constant values
			switch (m_value.nInt)
			{
			case 0:
				m_toolset.SafeCall(&machine::link::InstantiateInt0, pVariable);
	
				break;
			case 1:
				m_toolset.SafeCall(&machine::link::InstantiateIntP1, pVariable);
	
				break;
			case -1:
				m_toolset.SafeCall(&machine::link::InstantiateIntN1, pVariable);
	
				break;
			default:
				m_toolset.SafeCall(&machine::link::InstantiateInt_32, pVariable, m_value.nInt);
	
				break;
			}
	
			break;
		}
		case VALUE_TYPE::INT_64:
			m_toolset.SafeCall(&machine::link::InstantiateInt_64, pVariable, m_value.llInt);
	
			break;
		case VALUE_TYPE::FLOAT:
			m_toolset.SafeCall(&machine::link::InstantiateFloat, pVariable, m_value.rFloat);

			break;
		case VALUE_TYPE::DOUBLE:
			m_toolset.SafeCall(&machine::link::InstantiateDouble, pVariable, m_value.rDouble);
	
			break;
		case VALUE_TYPE::STRING:
			m_toolset.SafeCall(&machine::link::InstantiateCString, pVariable, GetStringLocation(m_value.string));

			break;
		case VALUE_TYPE::MEMBER:
			m_toolset.SafeCall(&framework::BiaMember::Clone, m_value.pMember, pVariable);
	
			break;
		case VALUE_TYPE::TEMPORARY_MEMBER:
			m_toolset.Call(&framework::BiaMember::Clone, machine::architecture::BiaToolset::TemporaryMember(m_value.temporaryResultIndex), pVariable);

			break;
		case VALUE_TYPE::TEST_VALUE_REGISTER:
			m_toolset.Call(&machine::link::InstantiateInt_32, pVariable, machine::architecture::BiaToolset::TestValueResult());

			break;
		case VALUE_TYPE::TEST_VALUE_CONSTANT:
			if(m_value.bTestValue)
				m_toolset.SafeCall(&machine::link::InstantiateIntP1, pVariable);
			else
				m_toolset.SafeCall(&machine::link::InstantiateInt0, pVariable);

			break;
		case VALUE_TYPE::RESULT_REGISTER:
			m_toolset.Call(&framework::BiaMember::Clone, machine::architecture::BiaToolset::ResultValue(), pVariable);

			break;
		default:
			BIA_COMPILER_DEV_INVALID
		}
	});

	return p_reports.pEnd + 1;
}

const grammar::Report * BiaCompiler::HandleConditionExpression(grammar::report_range p_reports)
{
	//Handle left value
	p_reports.pBegin = HandleMathExpressionTerm(p_reports.pBegin[1].content.children);

	//Condition expression
	if (p_reports.pBegin < p_reports.pEnd)
	{
		auto leftType = m_valueType;
		auto leftValue = m_value;

		//Handle right value
		HandleMathExpressionTerm(p_reports.pBegin[1].content.children);

		//Call compare operator
		uint32_t unOperator = 0;

		memcpy(&unOperator, p_reports.pBegin->content.token.pcString, p_reports.pBegin->content.token.iSize);

		HandleCompareOperator(leftType, leftValue, unOperator);
	}

	return p_reports.pEnd + 1;
}

const grammar::Report * BiaCompiler::HandleValueRaw(grammar::report_range p_reports)
{
	switch (p_reports.pBegin[1].unTokenId)
	{
	case grammar::BV_NUMBER:
		HandleNumber(p_reports.pBegin + 1);

		break;
	case grammar::BV_TRUE:
		m_valueType = VALUE_TYPE::TEST_VALUE_CONSTANT;
		m_value.bTestValue = true;

		break;
	case grammar::BV_FALSE:
		m_valueType = VALUE_TYPE::TEST_VALUE_CONSTANT;
		m_value.bTestValue = false;

		break;
	case grammar::BV_MEMBER:
		HandleMember(p_reports.pBegin[1].content.children);

		break;
	default:
		BIA_COMPILER_DEV_INVALID
	}

	return p_reports.pEnd + 1;
}

const grammar::Report * BiaCompiler::HandleMathFactor(grammar::report_range p_reports)
{
	switch (p_reports.pBegin[1].unRuleId)
	{
	case grammar::BGR_VALUE_RAW:
		HandleValueRaw(p_reports.pBegin[1].content.children);

		break;
	case grammar::BGR_VALUE:
		HandleValue<false>(p_reports.pBegin[1].content.children);

		break;
	default:
		BIA_COMPILER_DEV_INVALID
	}

	return p_reports.pEnd + 1;
}

const grammar::Report * BiaCompiler::HandleIf(grammar::report_range p_reports)
{
	std::vector<machine::architecture::BiaToolset::position> vEndJumps;

	++p_reports.pBegin;

	while (true)
	{
		//Else statement
		if (p_reports.pBegin->unRuleId == grammar::BGR_IF_HELPER_1)
		{
			//Set the results for compiling
			m_valueType = VALUE_TYPE::TEST_VALUE_CONSTANT;
			m_value.bTestValue = true;

			//Move begin since the else statement is wrapped up
			++p_reports.pBegin;
		}
		//Handle condition
		else
			p_reports.pBegin = HandleValue<true>(p_reports.pBegin->content.children, [] {});

		//Compile statement
		auto bCompile = m_valueType != VALUE_TYPE::TEST_VALUE_CONSTANT || m_value.bTestValue;

		if (bCompile)
		{
			//Write jump
			auto bConstant = m_valueType == VALUE_TYPE::TEST_VALUE_CONSTANT;
			auto jump = bConstant ? 0 : m_toolset.WriteJump(machine::architecture::BiaToolset::JUMP::JUMP_IF_FALSE);

			//Compile statement
			p_reports.pBegin = HandleRoot(p_reports.pBegin);

			//Write end jump but only if the end has not been reached
			if (p_reports.pBegin < p_reports.pEnd && !bConstant)
			{
				vEndJumps.push_back(m_toolset.WriteJump(machine::architecture::BiaToolset::JUMP::JUMP));

				//Update test jump to next statement
				m_toolset.WriteJump(machine::architecture::BiaToolset::JUMP::JUMP_IF_FALSE, m_toolset.GetBuffer().GetPosition(), jump);
			}
			//End of ifs or condition is at compile time true
			else
			{
				//Update test jump to ent
				if (!bConstant)
					m_toolset.WriteJump(machine::architecture::BiaToolset::JUMP::JUMP_IF_FALSE, m_toolset.GetBuffer().GetPosition(), jump);

				break;
			}
		}
		//Skip statements. These are conditions that are at compile time false
		else
		{
			p_reports.pBegin = HandleRoot<true>(p_reports.pBegin);

			if (p_reports.pBegin >= p_reports.pEnd)
				break;
		}
	}

	//Update end jump locations
	auto endPos = m_toolset.GetBuffer().GetPosition();

	for (auto & jumpPos : vEndJumps)
		m_toolset.WriteJump(machine::architecture::BiaToolset::JUMP::JUMP, endPos, jumpPos);

	return p_reports.pEnd + 1;
}

const grammar::Report * bia::compiler::BiaCompiler::HandlePrint(grammar::report_range p_reports)
{
	//Handle value to print
	HandleValue<false>(p_reports.pBegin[1].content.children, [this] {
		//Call print
		switch (m_valueType)
		{
		case VALUE_TYPE::INT_32:
			m_toolset.SafeCall(&machine::link::Print_i, m_value.nInt);
	
			break;
		case VALUE_TYPE::INT_64:
			m_toolset.SafeCall(&machine::link::Print_I, m_value.llInt);
	
			break;
		case VALUE_TYPE::FLOAT:
			m_toolset.SafeCall(&machine::link::Print_f, m_value.rFloat);
	
			break;
		case VALUE_TYPE::DOUBLE:
			m_toolset.SafeCall(&machine::link::Print_d, m_value.rDouble);
	
			break;
		case VALUE_TYPE::STRING:
			m_toolset.SafeCall(&machine::link::Print_s, GetStringLocation(m_value.string));

			break;
		case VALUE_TYPE::MEMBER:
			m_toolset.SafeCall(&framework::BiaMember::Print, m_value.pMember);
	
			break;
		case VALUE_TYPE::TEMPORARY_MEMBER:
			m_toolset.Call(&framework::BiaMember::Print, machine::architecture::BiaToolset::TemporaryMember(m_value.temporaryResultIndex));
	
			break;
		case VALUE_TYPE::TEST_VALUE_REGISTER:
			m_toolset.Call(&machine::link::Print_b, machine::architecture::BiaToolset::TestValueResult());

			break;
		case VALUE_TYPE::TEST_VALUE_CONSTANT:
			if (m_value.bTestValue)
				m_toolset.SafeCall(&machine::link::Print_true);
			else
				m_toolset.SafeCall(&machine::link::Print_false);

			break;
		case VALUE_TYPE::RESULT_REGISTER:
			m_toolset.Call(&framework::BiaMember::Print, machine::architecture::BiaToolset::ResultValue());

			break;
		default:
			BIA_COMPILER_DEV_INVALID
		}
	});

	return p_reports.pEnd + 1;
}

const grammar::Report * BiaCompiler::HandleMember(grammar::report_range p_reports)
{
	const grammar::Report * pNext = nullptr;
	auto bFirst = true;
	VALUE_TYPE parameterType = VALUE_TYPE::NONE;
	VALUE_TYPE biaInstanceType = VALUE_TYPE::NONE;
	Value parameterValue;
	Value biaInstanceValue;

	++p_reports.pBegin;
	m_counter.Next();

	BiaTempCounter::counter_type destination = -1;

	while(p_reports.pBegin < p_reports.pEnd)
	{
		//Function call
		if (p_reports.pBegin->unRuleId == grammar::BGR_PARAMETER)
		{
			auto tmpType = m_valueType;
			auto tmpValue = m_value;

			//Push parameters
			pNext = HandleParameters(p_reports.pBegin->content.children);

			parameterType = m_valueType;
			parameterValue = m_value;
			m_valueType = tmpType;
			m_value = tmpValue;
		}
		//Handle first value
		else if (bFirst)
		{
			//Function call
			if (p_reports.pBegin + 1 < p_reports.pEnd)
			{
				//Function call
				if (p_reports.pBegin[1].unRuleId == grammar::BGR_PARAMETER)
				{
					//Push parameters
					pNext = HandleParameters(p_reports.pBegin[1].content.children);

					parameterType = m_valueType;
					parameterValue = m_value;
				}
			}

			switch (p_reports.pBegin->unTokenId)
			{
			case grammar::BM_INSTANTIATION:
				p_reports.pBegin = HandleInstantiation(p_reports.pBegin->content.children);

				break;
			case grammar::BM_STRING:
				HandleString(p_reports.pBegin++);

				break;
			case grammar::BM_IDENTIFIER:
				HandleIdentifier(p_reports.pBegin++);

				break;
			default:
				BIA_COMPILER_DEV_INVALID
			}
			
			bFirst = false;
		}

		//Function call
		if (pNext)
		{
			switch (parameterType)
			{
			case VALUE_TYPE::PARAMETER:
			{
				if (destination == -1)
					destination = m_counter.Current();

				switch (m_valueType)
				{
				case VALUE_TYPE::MEMBER:
				{
					HandleFunctionCall(parameterValue.parameter, m_value.pMember, static_cast<framework::BiaMember*>(0), destination);

					break;
				}
				case VALUE_TYPE::TEMPORARY_MEMBER:
				{
					HandleFunctionCall(parameterValue.parameter, machine::architecture::BiaToolset::TemporaryMember(m_value.temporaryResultIndex), static_cast<framework::BiaMember*>(0), destination);

					break;
				}
				case VALUE_TYPE::RESULT_REGISTER:
				{
					HandleFunctionCall(parameterValue.parameter, machine::architecture::BiaToolset::ResultValue(), static_cast<framework::BiaMember*>(0), destination);

					break;
				}
				default:
					BIA_COMPILER_DEV_INVALID
				}

				break;
			}
			default:
				BIA_COMPILER_DEV_INVALID
			}
			
			p_reports.pBegin = pNext;
			pNext = nullptr;

			continue;
		}
		//Get member
		else if (p_reports.pBegin < p_reports.pEnd)
		{
			biaInstanceType = m_valueType;
			biaInstanceValue = m_value;

			//Arrow access
			if (p_reports.pBegin->unTokenId == grammar::BAO_ARROW_ACCESS)
				HandleIdentifier(++p_reports.pBegin);
			//Dot access
			else
			{
				switch (m_valueType)
				{
				case VALUE_TYPE::MEMBER:
					m_toolset.Call(&framework::BiaMember::GetMember, m_value.pMember, m_context.NameAddressOf(p_reports.pBegin->content.token.pcString, p_reports.pBegin->content.token.iSize));

					break;
				case VALUE_TYPE::TEMPORARY_MEMBER:
					m_toolset.Call(&framework::BiaMember::GetMember, machine::architecture::BiaToolset::TemporaryMember(m_value.temporaryResultIndex), m_context.NameAddressOf(p_reports.pBegin->content.token.pcString, p_reports.pBegin->content.token.iSize));

					break;
				case VALUE_TYPE::RESULT_REGISTER:
					m_toolset.Call(&framework::BiaMember::GetMember, machine::architecture::BiaToolset::ResultValue(), m_context.NameAddressOf(p_reports.pBegin->content.token.pcString, p_reports.pBegin->content.token.iSize));

					break;
				default:
					BIA_COMPILER_DEV_INVALID
				}

				m_valueType = VALUE_TYPE::RESULT_REGISTER;
			}
		}
		
		++p_reports.pBegin;
	}

	return p_reports.pEnd + 1;
}

const grammar::Report * BiaCompiler::HandleParameters(grammar::report_range p_reports)
{
	++p_reports.pBegin;

	auto bUseCounter = true;
	machine::architecture::BiaToolset::pass_count quartetsPassed = 0;
	framework::BiaMember::parameter_count parameterCounter = 0;
	std::string stFormat;

	//Handle value
	while (p_reports.pBegin < p_reports.pEnd)
	{
		p_reports.pBegin = HandleValue<false>(p_reports.pBegin->content.children, [&] {
			++parameterCounter;

			switch (m_valueType)
			{
			case VALUE_TYPE::INT_32:
				quartetsPassed += m_toolset.Pass(m_value.nInt);
				stFormat += 'i';
				bUseCounter = false;

				break;
			case VALUE_TYPE::INT_64:
				quartetsPassed += m_toolset.Pass(m_value.llInt);
				stFormat += 'I';
				bUseCounter = false;

				break;
			case VALUE_TYPE::FLOAT:
				quartetsPassed += m_toolset.Pass(m_value.rFloat);
				stFormat += 'f';
				bUseCounter = false;

				break;
			case VALUE_TYPE::DOUBLE:
				quartetsPassed += m_toolset.Pass(m_value.rDouble);
				stFormat += 'd';
				bUseCounter = false;

				break;
			case VALUE_TYPE::STRING:
				quartetsPassed += m_toolset.Pass(GetStringLocation(m_value.string));
				stFormat += 's';
				bUseCounter = false;

				break;
			case VALUE_TYPE::MEMBER:
				quartetsPassed += m_toolset.Pass(m_value.pMember);
				stFormat += 'M';

				break;
			case VALUE_TYPE::TEMPORARY_MEMBER:
				quartetsPassed += m_toolset.Pass(machine::architecture::BiaToolset::TemporaryMember(m_value.temporaryResultIndex));
				stFormat += 'M';

				break;
			default:
				BIA_COMPILER_DEV_INVALID
			}
		});
	}

	//Only members or nothing was passed
	m_valueType = VALUE_TYPE::PARAMETER;

	//Pure
	if (bUseCounter)
	{
		m_value.parameter.pcFormat = nullptr;
		m_value.parameter.parameterCount = parameterCounter;
		m_value.parameter.quartetsPassed = quartetsPassed;
	}
	//Formatted
	else
	{
		m_value.parameter.pcFormat = GetParameterFormat({ stFormat.c_str(), stFormat.length() });
		m_value.parameter.parameterCount = parameterCounter;
		m_value.parameter.quartetsPassed = quartetsPassed;
	}

	return p_reports.pEnd + 1;
}

const grammar::Report * BiaCompiler::HandleTestLoop(grammar::report_range p_reports)
{
	using namespace machine::architecture;

	++p_reports.pBegin;

	//Pre test or post test loop and loop type
	BiaToolset::position preTestJumpPosition;

	if (p_reports.pBegin->content.token.iSize == grammar::Keyword_do::Size())
	{
		preTestJumpPosition = -1;
		++p_reports.pBegin;
	}
	//Write pre test jump
	else
		preTestJumpPosition = m_toolset.WriteJump(BiaToolset::JUMP::JUMP);


	//Loop jump type
	auto jumpType = p_reports.pBegin++->unTokenId == grammar::LT_WHILE ? machine::architecture::BiaToolset::JUMP::JUMP_IF_TRUE : machine::architecture::BiaToolset::JUMP::JUMP_IF_FALSE;

	//Handle loop condition
	BiaToolset::position conditionJumpPosition = -1;
	stream::BiaOutputStreamBuffer conditionBuffer;
	auto & originalBuffer = m_toolset.GetBuffer();
	auto bCompile = true;

	//Redirect conditional code to a temporary buffer
	m_toolset.SetOutput(conditionBuffer);

	//Loop condition
	p_reports.pBegin = HandleValue<true>(p_reports.pBegin->content.children, [&] {
		//Constant condition
		if (m_valueType == VALUE_TYPE::TEST_VALUE_CONSTANT)
		{
			//Don't compile this loop
			if (!m_value.bTestValue)
			{
				bCompile = false;

				return;
			}

			//Change to unconditional jump
			jumpType = machine::architecture::BiaToolset::JUMP::JUMP;
		}
		//Not a test register
		else if (m_valueType != VALUE_TYPE::TEST_VALUE_REGISTER)
			BIA_COMPILER_DEV_INVALID;

		conditionJumpPosition = m_toolset.WriteJump(jumpType);
	});

	//Reset output buffer
	m_toolset.SetOutput(originalBuffer);

	//Compile loop
	if (bCompile)
	{
		auto loopStartPosition = originalBuffer.GetPosition();

		HandleRoot(p_reports.pBegin);

		//Update jump positions
		auto loopEndPosition = originalBuffer.GetPosition();

		conditionJumpPosition += loopEndPosition;

		//Write loop condition
		originalBuffer.WriteStream(conditionBuffer);

		//Upate jump offset for loop jump
		m_toolset.WriteJump(jumpType, loopStartPosition, conditionJumpPosition);

		//Upate jump offset for pre test
		if (preTestJumpPosition != -1)
			m_toolset.WriteJump(BiaToolset::JUMP::JUMP, loopEndPosition, preTestJumpPosition);
	}
	//Discard pre test jump
	else if(preTestJumpPosition != -1)
		m_toolset.GetBuffer().SetPosition(preTestJumpPosition);

	return p_reports.pEnd + 1;
}

const grammar::Report * BiaCompiler::HandleImport(grammar::report_range p_reports)
{
	//TODO
	return p_reports.pEnd + 1;
}

const grammar::Report * BiaCompiler::HandleInstantiation(grammar::report_range p_reports)
{
	//Handle identifier
	HandleIdentifier(p_reports.pBegin + 1);
	
	auto pIdentifier = m_value.pMember;
	
	//Handle parameters
	HandleParameters(p_reports.pBegin[2].content.children);

	if (m_valueType != VALUE_TYPE::PARAMETER)
		BIA_COMPILER_DEV_INVALID;

	auto parameter = m_value.parameter;

	//Call instantiation function
	m_counter.Next();

	m_valueType = VALUE_TYPE::TEMPORARY_MEMBER;
	m_value.temporaryResultIndex = m_counter.Current();

	HandleInstantiationCall(parameter, pIdentifier, m_value.temporaryResultIndex);

	return p_reports.pEnd + 1;
}

}
}