#pragma once
#include <cassert>
#include "Parser.h"

namespace backward_chain_reasoning
{

	inline void testReplaceImplications()
	{
		string expression = "A|B";
		replaceImplications(expression);
		assert("A|B" == expression);
		expression = "A>B";
		replaceImplications(expression);
		assert("~A|B" == expression);
		expression = "A>B>C>D";
		replaceImplications(expression);
		assert("~A|~B|~C|D" == expression);
		expression = "~A>~B>C>~D";
		replaceImplications(expression);
		assert("~~A|~~B|~C|~D" == expression);
		cout << "testReplaceImplications() successful.\n";
		
	}

	inline void testReplaceDoubleNegations()
	{
		string expression = "A";
		replaceDoubleNegations(expression);
		assert("A" == expression);
		expression = "~A";
		replaceDoubleNegations(expression);
		assert("~A" == expression);
		expression = "~~A";
		replaceDoubleNegations(expression);
		assert("A" == expression);
		expression = "~~A|~~B|~C|~D";
		replaceDoubleNegations(expression);
		assert("A|B|~C|~D" == expression);
		cout << "testReplaceDoubleNegations() successful.\n";
	}

	inline void testShuntingYard()
	{
		vector<char> correctResult = { 'A', 'B', '|' };
		vector<char> tokens = { 'A', '|', 'B' };
		tokens = shuntingYard(tokens);
		assert(correctResult == tokens);

		correctResult = { 'A', 'B', '&', 'C','|', 'D', 'E','|','F', '&', '|' };
		tokens = { '(','(','A','&','B',')','|','C',')','|','(','(','D' ,'|', 'E',')','&' ,'F',')' };
		tokens = shuntingYard(tokens);
		assert(correctResult == tokens);

		cout << "testShuntingYard() successful.\n";
	}

	inline void testEvaluate()
	{
		unordered_map<string, bool> variables = { 
			{ "A", true }, { "B", true },
			{ "C", false }, { "D", false } 
		};
		
		/* AND */
		string expression = "A&B";
		assert(evaluate(expression, variables));
		expression = "A&C";
		assert(evaluate(expression, variables) == false);
		expression = "D&C";
		assert(evaluate(expression, variables) == false);
		expression = "A&B&C&D";
		assert(evaluate(expression, variables) == false);

		/* Negation */
		expression = "A&B&~C&~D";
		assert(evaluate(expression, variables));
		expression = "~A|~B";
		assert(evaluate(expression, variables) == false);
		
		/* OR*/
		expression = "A|B";
		assert(evaluate(expression, variables));
		expression = "A|C";
		assert(evaluate(expression, variables));
		expression = "D|C";
		assert(evaluate(expression, variables) == false);

		/* Implication */
		expression = "D>C";
		assert(evaluate(expression, variables));
		expression = "D>A";
		assert(evaluate(expression, variables));
		expression = "A>A";
		assert(evaluate(expression, variables));
		expression = "A>D";
		assert(evaluate(expression, variables) == false);

		/* Precedence */
		expression = "(A|C)&(A|D)";
		assert(evaluate(expression, variables));
		expression = "(C|D)|(A&B)";
		assert(evaluate(expression, variables));
		expression = "(C|D)|(A&D)";
		assert(evaluate(expression, variables) == false);
		expression = "((A|D)|C)&((A|C)|D)";
		assert(evaluate(expression, variables));
		expression = "((A&D)|A)&((A|C)&D)";
		assert(evaluate(expression, variables) == false);
		cout << "testEvaluate() successful.\n";

	}

	inline void allTests()
	{
		cout << "CNF tests begin:\n";
		testReplaceImplications();
		testReplaceDoubleNegations();
		cout << "CNF tests end\n\n";

		cout << "Calculation tests begin:\n";
		testShuntingYard();
		testEvaluate();
		cout << "Calculation tests end\n\n";

		cout << "Backward Chain Reasoning tests begin:\n";

		cout << "Backward Chain Reasoning  tests end\n\n";
	}
}
