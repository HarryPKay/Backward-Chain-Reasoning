#pragma once
#include <unordered_map>
#include <stack>
#include <iostream>
#include <cassert>

/* This parser is used to evaluate boolean expressions by the knowledge base. */
namespace backward_chain_reasoning
{
	using namespace std;

	unordered_map<char, string> alternativeSymbols
	{
		{'^', "&"},
		{'v', "|"}
	};

	const char openParenthesis = '(';
	const char closeParenthesis = ')';

	const char negation = '~';
	const char conjunction = '&';
	const char disjunction = '|';
	const char implication = '>';
	const char iff = '-';

	/* Mappings for the shunting yard algorithm.
	 * Higher precedence will be evaluated first. */
	unordered_map<char, int> precedence
		= {
			{openParenthesis, 5},
			{closeParenthesis, 5},
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
			{openParenthesis, true},
			{closeParenthesis, false},
			{negation, false},
			{conjunction, true},
			{disjunction, true},
			{implication, true},
			{iff, true}
	};

	inline bool isPredicate(char c)
	{
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
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
	 * Edsger Dijkstra Shunting Yard algorithm, but adapted to be used for boolean expressions
	 * rather than normal arithmetic.
	 * 
	 * Precondition: A boolean expression in in Infix notation.
	 * Returns a boolean expression in Polish Prefix notation.
	 */
	inline vector<char> convertToPrefix(std::vector<char> tokens)
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
					&& ((operatorStack.top() != openParenthesis
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

			if (openParenthesis == token)
			{
				operatorStack.push(token);
			}

			if (closeParenthesis == token)
			{
				while (!operatorStack.empty() && operatorStack.top() != openParenthesis)
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
		for (size_t i = 0; i < postFixOutput.size(); ++i)
		{
			if (postFixOutput[i] == openParenthesis || postFixOutput[i] == closeParenthesis)
			{

				postFixOutput.erase(postFixOutput.begin() + i, postFixOutput.begin() + i + 1);
				--i;
			}
		}

		return postFixOutput;
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
	inline bool evaluate(string expression, unordered_map<string, bool> variableToBoolMapping)
	{
		if (expression.size() == 1)
		{
			return variableToBoolMapping[expression];
		}

		// Convert to Polish Postfix for easier parsing.
		vector<char> tokens;
		for (auto& e : expression)
		{
			tokens.push_back(e);
		}
		tokens = convertToPrefix(tokens);

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
				bool intermediateEvaluation;
				const bool rightOperand = evaluation.top();
				evaluation.pop();

				switch (token)
				{
				case negation:
					intermediateEvaluation = !rightOperand;
					break;
				case conjunction:
					intermediateEvaluation = evaluation.top() && rightOperand;
					evaluation.pop();
					break;
				case disjunction:
					intermediateEvaluation = evaluation.top() || rightOperand;
					evaluation.pop();
					break;
				case implication:
					intermediateEvaluation = !evaluation.top() || rightOperand;
					evaluation.pop();
					break;
				case iff:
					intermediateEvaluation = (!evaluation.top() || rightOperand) && (evaluation.top() || !rightOperand);
					evaluation.pop();
					break;
				default:
					;
				}

				evaluation.push(intermediateEvaluation);
			}
		}

		/* Evaluation should end up having size of 1. */
		assert(evaluation.size() == 1);
		return  evaluation.top();
	}
}