#include <biaMachineContext.hpp>

int main()
{
	bia::machine::BiaMachineContext m_context(std::shared_ptr<bia::machine::BiaAllocator>(new bia::machine::BiaAllocator()));
			auto szScript = "global test_dot_first_constant = 652 - 2 + 6956 * 998 / 53;";
			
			m_context.Run(szScript, std::char_traits<char>::length(szScript));

			//Get variable
			auto pVariable = m_context.GetGlobal("test_dot_first_constant");

			//Assert::IsNotNull(pVariable, L"Variable not found.");
			if(!pVariable)
				return 3;

			//Get value
			auto pValue = pVariable->Cast<int>();

			//Assert::IsNotNull(pValue, L"Invalid type.");
			if(!pValue)
				return 2;

			//Test value
			if(*pValue != 652 - 2 + 6956 * 998 / 53)
				return 1;
		/*}

		TEST_METHOD(TestDotFirstMixed)
		{
			auto szScript = R"(
global a = 652;
global b = 6956;
global c = 998;
global d = 53;
global test_dot_first_mixed = a - 2 + b * c / d;)";

			m_context.Run(szScript, std::char_traits<char>::length(szScript));

			//Get variable
			auto pVariable = m_context.GetGlobal("test_dot_first_mixed");

			Assert::IsNotNull(pVariable, L"Variable not found.");

			//Get value
			auto pValue = pVariable->Cast<int>();

			Assert::IsNotNull(pValue, L"Invalid type.");

			//Test value
			Assert::AreEqual(*pValue, 652 - 2 + 6956 * 998 / 53);
		}


	private:
		bia::machine::BiaMachineContext m_context;
	};
}*/
	return 0;
}