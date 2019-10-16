#pragma once
#include <string>
#include <vector>
#include "Parser.h"
#include <sstream>
#include <map>
#include <algorithm>
#include "ModelChecker.h"

namespace backward_chain_reasoning
{
	using namespace std;

	class KnowledgeBase
	{
	public:

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
			bool conjunctionFound = false;
			bool disjunctionFound = false;
			bool implicationFound = false;

			for (size_t i = 0; i < input.size(); ++i)
			{
				if (!isPredicate(input[i]) && !isOperator(input[i]))
				{
					cerr << "'" << input[i] << "' " << "unrecognizable symbol.\n";
					return false;
				}

				if (input[i] == implication)
				{
					implicationFound = true;
					if (i + 2 != input.size())
					{
						cerr << "Conclusion clause (after implication) can only be predicate.\n";
						return false;
					}
				}

				if (isOperator(input[i]))
				{
					if (input[i] == conjunction)
					{
						conjunctionFound = true;
					}
					if (input[i] == disjunction)
					{
						disjunctionFound = true;
					}

					if (input[i] == closeParenthesis 
						|| input[i] == openParenthesis
						|| input[i] == iff)
					{
						cerr << "'" << input[i] << "' " << "Not supported.\n";
						return false;
					}

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
							cerr << "'" << input[i] << "' " << " must be post fixed and prefixed by a variable.\n";
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

			if (input.size() > 1 && !implicationFound)
			{
				cerr << "Non assertions must contain an implication.\n";
				return false;
			}

			if (conjunctionFound && disjunctionFound)
			{
				cerr << "Preposition must either be all conjunctions or all disjunctions.\n";
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
				<< "P1 v P2 v...v Pk => P, or P.\n";
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

			cout << "Would you like to print out the search trace for the reasoning? (y/n)\n";
			do
			{
				cout << "> ";
				getline(cin, input);
				if (input == "y")
				{
					shouldPrintTrace = true;
				}
				else if (input != "n")
				{
					shouldPrintTrace = false;
					cout << "Invalid input.\n";
				}
			} while (input != "y" && input != "n");

			cout << "You can now query the knowledge base. Type quit to exit\n";

			do
			{
				cout << "> ";
				getline(cin, input);
				cleanInput(input);

				if (input.size() == 2 && input[1] == '?')
				{
					input.erase(input.begin() + 1, input.begin() + 2);
				}

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
						cout << "\nCan " << input << " be inferred? Yes.\n";
					}
					else
					{
						cout << "\nCan " << input << " be inferred? No.\n";
					}
				}

			} while (input != "quit");

			cout << "Program terminated.\n";
		}

		/* Inserts a horn clause into the knowledge base.
		 * Precondition: is either a conjunction clause (p1 & p2 & ... & pn)
		 * or a injunction clause (p1 | p2 | ... | pn) or a fact (p).
		 */
		void tell(string hornClause)
		{
			cleanInput(hornClause);
			assert(validateTellInput(hornClause));
			hornClauses.push_back(hornClause);
		}

		/*
		 * Depth first search that determines if the given query can be inferred from the knowledge base.
		 * returns true if so.
		 * 
		 * Precondition: 
		 * - A single letter predicate (primitive) query.
		 * 
		 * Base case:
		 *  - The query exists in the form of P in the knowledge base.
		 *  - The query does not exist in the form of P and no rule infers P.
		 *  - The rule is in path.
		 * 
		 * General case:
		 *  - There is a rule that infers P.		
		 */
		bool ask(const string& query)
		{
			if (shouldPrintTrace)
			{
				cout << "\n\nTrace:\n" << query << "?\n";
			}
			
			unordered_map<string, bool> literalTruthTable;
			const vector<string> path;
			return ask(query, literalTruthTable, path);
		}

	private:
		bool ask(const string& query, unordered_map<string, bool>& literalTruthTable, vector<string> path)
		{
			// If the query is in the knowledge base as a fact, then the fact is asserted.
			if (query.size() == 1 
				&& find(hornClauses.begin(), hornClauses.end(), query) != hornClauses.end())
			{
				if (shouldPrintTrace)
				{
					cout << query << " = True\n";
				}
				
				return true;
			}

			// Get all caluses that infer the query
			for (const string& clause : fetchInferenceRules(query))
			{
				// If we have evaluated this path before then it may not complete
				// return to prevent infinite recursion.
				if (find(path.begin(), path.end(), clause) != path.end())
				{
					if (shouldPrintTrace)
					{
						cout << "Path is not complete: ";
						for (const auto& i : path)
						{
							cout << i << "->";
						}
						cout << clause << "\n";
					}

					return false;
				}

				const bool clauseOfConjunctions = find(clause.begin(), clause.end(), conjunction) != clause.end();

				if (shouldPrintTrace)
				{
					cout << clause << "=>" << query << "?\n";
				}

				// For every predicate that belongs to the inference rule, recursively prove it.
				for (const string& literal : getLiterals(clause))
				{
					// Optimization: 
					// skip proving the predicate if it has already been proved.
					auto it = literalTruthTable.find(literal);
					if (it == literalTruthTable.end())
					{
						if (shouldPrintTrace)
						{
							cout << literal << "?\n";
						}

						path.push_back(clause);
						literalTruthTable.insert({ literal, ask(literal, literalTruthTable, path) });
						path.pop_back();
					}

					// Optimization: 
					// Return when ever our inference rule is proven to be true if it is made of disjunctions.
					// If it is made of conjunctions, we can return false when any occurence is known to be false.
					if (clauseOfConjunctions && !literalTruthTable[literal])
					{
						if (shouldPrintTrace)
						{
							cout << clause << "=>" << query << " = False" << "\n";
						}
						
						return false;
					}
					if (evaluate(clause, literalTruthTable))
					{
						if (shouldPrintTrace)
						{
							cout <<  clause << "=>" << query << " = True" << "\n";
						}
						
						return true;
					}
				}
			}

			// If we couldn't infer anything and couldn't find any assertions, than return false.
			if (shouldPrintTrace)
			{
				std::cout << query << " = False\n";
			}
			
			return false;
		}

		/* Returns a vector of size 1 atomic variables for the given horn clause. */
		static vector<string> getLiterals(const string& hornClauses)
		{
			vector<string> literals;
			for (auto atomic : hornClauses)

			if (isPredicate(atomic))
			{
				string temp;
				temp.push_back(atomic);
				literals.push_back(temp);
			}
			return literals;
		}

		/* Returns a vector of hornClause that infers the hypothesis.
		 * Precondition: hypothesis is an atomic variable of size 1.
		 */
		vector<string> fetchInferenceRules(const string& hypothesis)
		{
			vector<string> antecedents;

			if (hypothesis.size() != 1)
			{
				return antecedents;
			}

			for (const auto& clause : hornClauses)
			{
				vector <string> tokens;
				stringstream ss(clause);
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
					antecedents.push_back(tokens[0]);
				}
			}

			return antecedents;
		}

		vector<string> hornClauses;
		bool shouldPrintTrace = true;
	};
}
