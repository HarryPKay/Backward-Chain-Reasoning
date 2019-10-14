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
		kb1.sentences.push_back("A|B>E");
		kb1.sentences.push_back("A&B>D");
		kb1.sentences.push_back("D&E>F");
		kb1.sentences.push_back("B&E>F");
		kb1.sentences.push_back("A");
		kb1.sentences.push_back("B");
		kb1.sentences.push_back("C");
		assert(kb1.ask("F"));
		assert(kb1.ask("E"));
		assert(kb1.ask("D"));
		assert(kb1.ask("P") == false);

		/* Test sample 2 */
		KnowledgeBase kb2;
		kb2.sentences.push_back("A&B&C&D>F");
		kb2.sentences.push_back("B|E|G>F");
		kb2.sentences.push_back("H&I>A");
		kb2.sentences.push_back("J&K>B");
		kb2.sentences.push_back("U|T|W>J");
		kb2.sentences.push_back("I&J&T>P");
		kb2.sentences.push_back("I&J&F>Q");
		kb2.sentences.push_back("H");
		kb2.sentences.push_back("I");
		kb2.sentences.push_back("K");
		kb2.sentences.push_back("D");
		kb2.sentences.push_back("W");
		kb2.sentences.push_back("C");
		assert(kb2.ask("F"));
		assert(kb2.ask("A"));
		assert(kb2.ask("B"));
		assert(kb2.ask("J"));
		assert(kb2.ask("P") == false);
		assert(kb2.ask("Q"));

		/* Test sample 3 */
		KnowledgeBase kb3;
		kb3.sentences.push_back("A&B>C");
		kb3.sentences.push_back("C|D>F");
		kb3.sentences.push_back("F|G|H|P>X");
		kb3.sentences.push_back("X&Y>Z");
		kb3.sentences.push_back("A");
		kb3.sentences.push_back("B");
		kb3.sentences.push_back("Y");
		assert(kb3.ask("Z"));

		cout << "testKnowledgeBaseAsk() successful.\n";
	}

	inline void allTests()
	{
		testShuntingYard();
		testEvaluate();
		testKnowledgeBaseAsk();
	}
}
