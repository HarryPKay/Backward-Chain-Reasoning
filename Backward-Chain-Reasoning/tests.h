#pragma once
#include <cassert>
#include "Parser.h"
#include "KnowledgeBase.h"

namespace backward_chain_reasoning
{
	inline void testShuntingYard()
	{
		vector<char> correctResult = { 'A', 'B', '|' };
		vector<char> tokens = { 'A', '|', 'B' };
		tokens = convertToPrefix(tokens);
		assert(correctResult == tokens);

		correctResult = { 'A', 'B', '&', 'C','|', 'D', 'E','|','F', '&', '|' };
		tokens = { '(','(','A','&','B',')','|','C',')','|','(','(','D' ,'|', 'E',')','&' ,'F',')' };
		tokens = convertToPrefix(tokens);
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

	inline void testKnowledgeBaseAsk()
	{
		/* Test sample 1 */
		KnowledgeBase kb1;
		kb1.tell("A|B>E");
		kb1.tell("A&B>D");
		kb1.tell("D&E>F");
		kb1.tell("B&E>F");
		kb1.tell("A");
		kb1.tell("B");
		kb1.tell("C");
		assert(kb1.ask("F"));
		assert(kb1.ask("E"));
		assert(kb1.ask("D"));
		assert(kb1.ask("P") == false);

		/* Test sample 2 */
		KnowledgeBase kb2;
		kb2.tell("A&B&C&D>F");
		kb2.tell("B|E|G>F");
		kb2.tell("H&I>A");
		kb2.tell("J&K>B");
		kb2.tell("U|T|W>J");
		kb2.tell("I&J&T>P");
		kb2.tell("I&J&F>Q");
		kb2.tell("H");
		kb2.tell("I");
		kb2.tell("K");
		kb2.tell("D");
		kb2.tell("W");
		kb2.tell("C");
		assert(kb2.ask("F"));
		assert(kb2.ask("A"));
		assert(kb2.ask("B"));
		assert(kb2.ask("J"));
		assert(kb2.ask("P") == false);
		assert(kb2.ask("Q"));

		/* Test sample 3 */
		KnowledgeBase kb3;
		kb3.tell("A&B>C");
		kb3.tell("C|D>F");
		kb3.tell("F|G|H|P>X");
		kb3.tell("X&Y>Z");
		kb3.tell("A");
		kb3.tell("B");
		kb3.tell("Y");
		assert(kb3.ask("Z"));

		/* Test sample 4, infinite recursion termination */
		KnowledgeBase kb4;
		kb4.tell("A&B>C");
		kb4.tell("C>A");
		kb4.tell("C>B");
		assert(kb4.ask("C") == false);

		/* Test sample 5, same rule expanded more than once 
		 * but not ruled out as infinite recursion.
		 */
		KnowledgeBase kb5;
		kb5.tell("A");
		kb5.tell("A>D");
		kb5.tell("B>C");
		kb5.tell("A>C");
		kb5.tell("D&C>E");
		assert(kb5.ask("E"));

		cout << "testKnowledgeBaseAsk() successful.\n";
	}

	inline void allTests()
	{
		testShuntingYard();
		testEvaluate();
		testKnowledgeBaseAsk();
	}
}
