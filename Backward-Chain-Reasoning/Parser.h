#pragma once
#include <unordered_map>
#include <stack>
#include <iostream>
#include <cassert>

namespace backward_chain_reasoning
{
	using namespace std;

	const char leftParenthesis = '(';
	const char rightParenthesis = ')';

	const char negation = '~';
	const char conjunction = '&';
	const char disjunction = '|';
	const char implication = '>';
	const char iff = '-';

	/* Mappings for the shunting yard algorithm. */
	unordered_map<char, int> precedence
		= {
			{leftParenthesis, 5},
			{rightParenthesis, 5},
			{negation, 4},
			{conjunction, 3},
			{disjunction, 2},
			{implication, 1},
			{iff, 0}
	};

	unordered_map<char, int> argumentCount
		= {
			{negation, 1},
			{conjunction, 2},
			{disjunction, 2},
			{implication, 2},
			{iff, 2}
	};

	unordered_map<char, bool> leftAssociativity
		= {
			{leftParenthesis, true},
			{rightParenthesis, false},
			{negation, false},
			{conjunction, true},
			{disjunction, true},
			{implication, true},
			{iff, true}
	};

	inline bool isPredicate(char c)
	{
		return c >= 'A' && c <= 'Z';
	}

	inline bool isOperator(char c)
	{
		for (auto& it : precedence)
		{
			if (it.first == c)
			{
				return true;
			}
		}
		return false;
	}

	/* 
	 * Converts a boolean expression into Polish Prefix notation for easier computation. Using
	 * Edsger Dijkstra algorithm adapted to be used for boolean expressions rather than normal arithmetic
	 * Based off the psuedo code from https://en.wikipedia.org/wiki/Shunting-yard_algorithm, section 
	 * "The algorithm in detail"  
	 * 
	 * Precondition: A boolean expression in Compounding Normal Form and in Infix notation is passed
	 * Returns a boolean expression in Polish Prefix notation.
	 */
	inline vector<char> shuntingYard(std::vector<char> tokens)
	{
		stack<char> operatorStack;
		vector<char> postFixOutput;

		while (!tokens.empty())
		{
			char token = *tokens.begin();
			tokens.erase(tokens.begin(), tokens.begin() + 1);

			if (isPredicate(token))
			{

				postFixOutput.push_back(token);
			}

			if (isOperator(token))
			{
				while (!operatorStack.empty()
					&& ((operatorStack.top() != leftParenthesis
						&& precedence[operatorStack.top()] > precedence[token])
						||
						(isOperator(operatorStack.top())
							&& precedence[operatorStack.top()] == precedence[token]
							&& leftAssociativity[operatorStack.top()])))
				{
					postFixOutput.push_back(operatorStack.top());
					operatorStack.pop();
				}
				operatorStack.push(token);
			}

			if (leftParenthesis == token)
			{
				operatorStack.push(token);
			}

			if (rightParenthesis == token)
			{
				while (!operatorStack.empty() && operatorStack.top() != leftParenthesis)
				{
					postFixOutput.push_back(operatorStack.top());
					operatorStack.pop();
				}
				operatorStack.pop();
			}
		}

		while (!operatorStack.empty())
		{
			postFixOutput.push_back(operatorStack.top());

			operatorStack.pop();
		}

		// Remove any parenthesis.
		for (int i = 0; i < postFixOutput.size(); ++i)
		{
			if (postFixOutput[i] == leftParenthesis || postFixOutput[i] == rightParenthesis)
			{

				postFixOutput.erase(postFixOutput.begin() + i, postFixOutput.begin() + i + 1);
				--i;
			}
		}

		return postFixOutput;
	}

	/*
	 * Replaces all occurrences of P->Q for any P and Q to the 
	 * equivalent form of ~P OR Q.
	 */
	inline void replaceImplications(string& expression)
	{
		for (size_t i = 0; i < expression.size(); ++i)
		{
			if (expression[i] == implication)
			{
				string newSubExpression;
				auto startIt = expression.begin() + i - 1;
				auto endIt = expression.begin() + i + 2;

				newSubExpression.push_back(negation);
				if (i - 2 >= 0 && expression[i - 2] == negation)
				{
					newSubExpression.push_back(negation);
					--startIt;
				}
				newSubExpression.push_back(expression[i - 1]);
				newSubExpression.push_back(disjunction);

				newSubExpression.push_back(expression[i + 1]);
				if (expression[i + 1] == negation)
				{
					newSubExpression.push_back(expression[i + 2]);
					++endIt;
				}
				expression.replace(startIt, endIt, newSubExpression);
			}
		}
	}

	/*
	 * Replaces all occurrences of ~~P for any P to the
	 * equivalent form of P.
	 */
	inline void replaceDoubleNegations(string& expression)
	{
		for (size_t i = 0; i < expression.size(); ++i)
		{
			if (expression[i] == negation && expression[i + 1] == negation)
			{
				expression.replace(expression.begin() + i, expression.begin() + i + 2, "");
			}
		}
	}

	/* Converts boolean expression to conjunctive normal form.
	 * TODO: Complete implementation, partially complete. */
	inline string convertToCNF(string expression)
	{
		replaceImplications(expression);
		replaceDoubleNegations(expression);
		return expression;
	}

	/* 
	 * Evaluates a boolean expression.
	 * 
	 * Precondition: A string representing a boolean expression.
	 * Post condition: Evaluation has a size of 1, as all sub expressions should
	 * have been evaluated.
	 * returns the boolean evaluation of the given expression.
	 * 
	 */
	inline bool evaluate(string expression, unordered_map<string, bool>& variableToBoolMapping)
	{
		if (expression.size() == 1)
		{
			return variableToBoolMapping[expression];
		}

		// Evaluate() only understands OR, AND and negation, so convert to CFN.
		expression = convertToCNF(expression);

		// Convert to Polish Postfix for easier parsing.
		vector<char> tokens;
		for (auto& e : expression)
		{
			tokens.push_back(e);
		}
		tokens = shuntingYard(tokens);

		stack<bool> evaluation;
		while (!tokens.empty())
		{
			const char token = *tokens.begin();
			tokens.erase(tokens.begin(), tokens.begin() + 1);

			if (isPredicate(token))
			{
				string temp;
				temp.push_back(token);
				evaluation.push(variableToBoolMapping[temp]);
			}
			else
			{
				bool subEvaluation;
				const bool rightOperand = evaluation.top();
				evaluation.pop();

				switch (token)
				{
				case negation:
					subEvaluation = !rightOperand;
					break;
				case conjunction:
					subEvaluation = evaluation.top() && rightOperand;
					evaluation.pop();
					break;
				case disjunction:
					subEvaluation = evaluation.top() || rightOperand;
					evaluation.pop();
					break;
				}

				evaluation.push(subEvaluation);
			}
		}

		/* Evaluation should end up having size of 1. */
		assert(evaluation.size() == 1);
		return  evaluation.top();
	}
}
