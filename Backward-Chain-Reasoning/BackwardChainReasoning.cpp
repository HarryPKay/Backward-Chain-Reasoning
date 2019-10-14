#include "Parser.h"
#include "KnowledgeBase.h"
#include "tests.h"

namespace backward_chain_reasoning
{


}

int main()
{
	using namespace std;
	using namespace backward_chain_reasoning;
	allTests();

	//KnowledgeBase kb;
	////kb.sentences.push_back("A&B>C");
	////kb.sentences.push_back("A");
	////kb.sentences.push_back("B");
	////kb.sentences.push_back("(A|B)&F>J");
	//kb.sentences.push_back("~Z>J");
	////kb.sentences.push_back("A|B|C|F>J");
	//kb.sentences.push_back("A|B>E");
	//kb.sentences.push_back("A&B>D");
	//kb.sentences.push_back("D&E>F");
	//kb.sentences.push_back("B&E>F");
	//kb.sentences.push_back("A");
	//kb.sentences.push_back("B");
	//kb.sentences.push_back("C");
	//if (kb.ask("J"))
	//{
	//	cout << "yes.\n";
	//}
	//else
	//{
	//	cout << "no.\n";
	//}

	return 0;
}