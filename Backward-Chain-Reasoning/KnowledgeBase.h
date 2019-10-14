#pragma once
#include <string>
#include <vector>
#include "Parser.h"
#include <sstream>
#include <map>
#include <algorithm>

namespace backward_chain_reasoning
{
	using namespace std;

	unordered_map<char, string> alternativeSymbols
	{
		{'^', "&"},
		{'v', "|"}
	};

	class KnowledgeBase
	{
	public:
		//TODO: make private and implement tell
		vector<string> sentences;

		static void cleanInput(string& input)
		{
			for (auto it = input.begin(); it != input.end(); ++it)
			{
				if (*it == ' ')
				{
					input.erase(it, it + 1);
					--it;
				}
				else if (*it == '=' && *(it + 1) == '>')
				{
					input.erase(it, it + 1);
					--it;
				}
				else
				{
					auto newSymbol = alternativeSymbols.find(*it);
					if (newSymbol != alternativeSymbols.end())
					{
						input.replace(it, it + 1, newSymbol->second);
					}
				}
			}
		}

		static bool validateTellInput(const string& input)
		{
			// left parenth must be closed at some point

			bool containsImplication = false;

			for (size_t i = 0; i < input.size(); ++i)
			{
				if (input[i] == implication)
				{
					containsImplication = true;
				}

				if (isOperator(input[i]))
				{
					
					if (input[i] == negation)
					{
						if (i + 1 >= input.size() 
							|| (input[i + 1] != negation && isOperator(input[i + 1])))
						{
							cerr << "negation (~) must be post fixed by a variable.\n";
							return false;
						}
					}
					else
					{
						if (i == 0 
							|| i + 1 >= input.size() 
							|| isOperator(input[i - 1]) 
							|| isOperator(input[i + 1]))
						{
							cerr << input[i] << " must be post fixed and prefixed by a variable.\n";
							return false;
						}
					}	
				}

				if (isPredicate(input[i]) && i + 1 < input.size())
				{
					if (!isOperator(input[i + 1]))
					{
						cerr << "Predicates must be joined by an operator.\n";
						return false;
					}
				}
			}

			if (input.size() > 1 && !containsImplication)
			{
				cerr << "Non assertions must contain an implication.\n";
				return false;
			}

			return true;
		}

		static bool validateAsk(string input)
		{
			if (input.size() == 2)
			{
				if (input[1] == '?')
				{
					input.erase(input.begin() + 1, input.begin() + 2);
				}
			}

			return input.size() == 1 && isPredicate(input[0]);
		}

		void run()
		{
			cout << "\n\nThis is an extended propositional backward chaining.\n"
				<< "Your knowledge base can only accept facts like P1 ^ P2 ^ ... ^ Pk => P,\n"
				<< "P1 v P2 v...v Pk => P,\n or P.\n";
			cout << "Now input your knowledge.\nType nil when done.\n";

			string input;
			do
			{
				cout << "> ";
				getline(cin, input);
				cleanInput(input);

				if (input == "nil")
				{
					break;
				}

				if (validateTellInput(input))
				{
					tell(input);
				}
		
			} while (input != "nil");

			cout << "You can now query the knowledge base.\nType quit to exist\n";

			do
			{
				cout << "> ";
				getline(cin, input);

				if (input == "quit")
				{
					break;
				}

				if (!validateAsk(input))
				{
					cerr << "Invalid statement, ask only takes a single letter predicate.\n";
				}
				else
				{
					if (ask(input))
					{
						cout << "yes\n";
					}
					else
					{
						cout << "no\n";
					}
				}

			} while (input != "quit");

			cout << "Program terminated.\n";
		}

		// tell
		void tell(string sentence)
		{
			assert(validateTellInput(sentence));
			sentences.push_back(sentence);
		}

		/*
		 * Determines if the given primitive can be inferred from the knowledge base.
		 * 
		 * Algorithm description:
		 * The function will find all inference rules that infers the given primitive, for each of these rules
		 * it will than recursively query each predicate of the rule until it can find an assertion, another inference rule
		 * or nothing. It stores those predicates that can be inferred back to an assertion as true in it's
		 * predicateToBoolMapping map, which is then used to determine if the rule is true.
		 * 
		 * The algorithm is depth first search since it does not stop expanding nodes until it meets a base case.
		 * It works with both conjunct and disjunct prepositions as it uses a parser understands these operations.
		 */
		bool ask(const string& primitive)
		{

			unordered_map<string, bool> predicateToBoolMapping;
			return ask(primitive, predicateToBoolMapping);
		}

	private:
		bool ask(const string& primitive, unordered_map<string, bool>& predicateToBoolMapping)
		{
			// Base case:
			// If the primitive itself exists in the sentences of KB, it then has been asserted as a fact.

			if (primitive.size() == 1 
				&& find(sentences.begin(), sentences.end(), primitive) != sentences.end())
			{
				//std::cout << primitive << " = T\n";
				return true;
			}


			// General case:
			// The primitive does not exists as an assertion in the data base, so find an inference rule that
			// implies that the primitive is true.
			
			// Get all rules that infer the primitive
			for (const string& rule : fetchInferenceRules(primitive))
			{
				//std::cout << "inference match: " << rule << "=>" << primitive << "\n";

				// For every predicate that belongs to the inference rule, recursively prove it.
				for (char c : rule)
				{
					if (isOperator(c))
					{
						continue;
					}
					string predicate;
					predicate.push_back(c);
					
					// Optimization: skip proving the predicate if it has already been proved.
					auto it = predicateToBoolMapping.find(predicate);
					if (it == predicateToBoolMapping.end())
					{
						//std::cout << "asking: " << predicate << "\n";
						predicateToBoolMapping.insert({ predicate, ask(predicate, predicateToBoolMapping) });
					}

					// Return when ever our inference rule is proven to be true.
					if (evaluate(rule, predicateToBoolMapping))
					{
						//std::cout << rule << "=>" << primitive << " can be inferred\n";
						return true;
					}
				}
			}

			// If we couldn't infer anything, than return false.
			//std::cout << primitive << " = F\n";
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
