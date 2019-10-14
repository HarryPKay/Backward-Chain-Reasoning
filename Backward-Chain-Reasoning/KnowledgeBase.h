#pragma once
#include <string>
#include <vector>
#include "Parser.h"
#include <sstream>
#include <map>

namespace backward_chain_reasoning
{
	using namespace std;

	class KnowledgeBase
	{
	public:

		//TODO: make private and implement tell
		vector<string> sentences;
		//ask

		// validate
		// validation rules:
		// left parenth must be closed at some point
		// all ops must be preceded by a negation or a variable (except closing parenthesis).
		// negation must be preceded with negation or variable
		// variables must have op other than parenth and negation to join eachother.

		// tell
		//query

		void tell(string sentence)
		{
			sentences.push_back(sentence);
			// for every predicate, assign to vartoboolmapping and assume false
			// if predicate by it self,  vartoboolmapping true
		}

		/*
		 * Determines if the given hypothesis can be inferred from the knowledge base.
		 * 
		 * Algorithm description:
		 * The function will find all inference rules that infers the given hypothesis, for each of these
		 * it will than recursively query each predicate of the rule until it can find an assertion
		 * or nothing. It stores those predicates that can be inferred back to an assertion as true in it's
		 * predicateToBoolMapping map, which is then used to determine if the rule is true. If the rule is true
		 * for all queries that lead up to the original hypothesis, then the hypothesis can be inferred from this
		 * knowledge base.
		 */
		bool ask(const string& hypothesis)
		{
			unordered_map<string, bool> predicateToBoolMapping;
			return ask(hypothesis, predicateToBoolMapping);
		}

	private:
		bool ask(const string& hypothesis, unordered_map<string, bool>& predicateToBoolMapping)
		{
			// Base case:
			// If the hypothesis itself exists in the sentences, it then has been asserted as a fact.

			if (hypothesis.size() == 1 
				&& find(sentences.begin(), sentences.end(), hypothesis) != sentences.end())
			{
				std::cout << hypothesis << " = T\n";
				return true;
			}

			// General case:
			// The hypothesis does not exists as an assertion in the data base, so find an inference rule that
			// implies that the hypothesis is true.
			
			// Get all rules that infer the hypothesis
			for (const string& rule : fetchInferenceRules(hypothesis))
			{
				std::cout << "inference match: " << rule << "=>" << hypothesis << "\n";

				// For every predicate of the inference rule, determine if it is in the sentences or it can be inferred
				// by another rule or has been asserted as a fact by recursively calling this function.
				for (char c : rule)
				{
					if (isOperator(c))
					{
						continue;
					}
					string predicate;
					predicate.push_back(c);
					
					// Optimization: skip if predicate has already been evaluated.
					auto it = predicateToBoolMapping.find(predicate);
					if (it == predicateToBoolMapping.end())
					{
						std::cout << "asking: " << predicate << "\n";
						predicateToBoolMapping.insert({ predicate, ask(predicate, predicateToBoolMapping) });
					}

					if (evaluate(rule, predicateToBoolMapping))
					{
						std::cout << rule << "=>" << hypothesis << " can be inferred\n";
						return true;
					}
				}
			}

			// If we couldn't infer anything, than return false.
			std::cout << hypothesis << " = F\n";
			return false;
		}

		vector<string> fetchInferenceRules(const string& hypothesis)
		{
			vector<string> rules;
			for (const auto& sentence : sentences)
			{
				vector <string> tokens;
				stringstream ss(sentence);
				string intermediate;
				while (getline(ss, intermediate, implication))
				{
					tokens.push_back(intermediate);
				}

				if (tokens.size() < 2)
				{
					continue;
				}

				if (tokens[1] == hypothesis)
				{
					rules.push_back(tokens[0]);
				}
			}

			return rules;
		}
	};
}
