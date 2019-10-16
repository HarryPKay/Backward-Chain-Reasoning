#include "KnowledgeBase.h"
#include "tests.h"

int main()
{
	using namespace std;
	using namespace backward_chain_reasoning;
	allTests();
	KnowledgeBase kb;
	kb.run();

	return 0;
}