Build Instructions.
========

Run the following commands in the terminal:

cd ./build
cmake ..
make

An executable binary called Backward-Chain-Reasoning will be built.

Usage:
========

The program is interactive so you only need to run the program name from the command line:

./Backward-Chain-Reasoning

I didn't create a visual tree for my search. Instead I had it print out the current query as it searched recursively.
the program will ask you if you would like to see this printout before you query the knowledge base.

Structure
========

KnowledgeBase.h:
Holds Tell() and Ask() for inserting and querying the database.
Ask is the depth first search algorithm that determines if the query can be inferred.

Parse.h:
Ask() uses the evaluate() function in Parse.h to determine if a clause is true given
what it knows right now (what it knows to be true or false is stored in the map). 


Tests.h:
Holds a series of assertions. They assert whether a given query should be true or 
false. The assertion's test data is using the test samples given and asserts the 
answer that the test samples give.

Test samples:
========

The following is the output from tests.h. The first 3 knowledge bases is from the
given sample data.

Test sample 1
--------

Trace:
F?
D&E=>F?
D?
A&B=>D?
A?
A = True
B?
B = True
A&B=>D = True
E?
A|B=>E?
A|B=>E = True
D&E=>F = True


Trace:
E?
A|B=>E?
A?
A = True
A|B=>E = True


Trace:
D?
A&B=>D?
A?
A = True
B?
B = True
A&B=>D = True


Trace:
P?
P = False

Test sample 2
--------

Trace:
F?
A&B&C&D=>F?
A?
H&I=>A?
H?
H = True
I?
I = True
H&I=>A = True
B?
J&K=>B?
J?
U|T|W=>J?
U?
U = False
T?
T = False
W?
W = True
U|T|W=>J = True
K?
K = True
J&K=>B = True
C?
C = True
D?
D = True
A&B&C&D=>F = True


Trace:
A?
H&I=>A?
H?
H = True
I?
I = True
H&I=>A = True


Trace:
B?
J&K=>B?
J?
U|T|W=>J?
U?
U = False
T?
T = False
W?
W = True
U|T|W=>J = True
K?
K = True
J&K=>B = True


Trace:
J?
U|T|W=>J?
U?
U = False
T?
T = False
W?
W = True
U|T|W=>J = True


Trace:
P?
I&J&T=>P?
I?
I = True
J?
U|T|W=>J?
U?
U = False
T?
T = False
W?
W = True
U|T|W=>J = True
I&J&T=>P = False


Trace:
Q?
I&J&F=>Q?
I?
I = True
J?
U|T|W=>J?
U?
U = False
T?
T = False
W?
W = True
U|T|W=>J = True
F?
A&B&C&D=>F?
A?
H&I=>A?
H?
H = True
H&I=>A = True
B?
J&K=>B?
K?
K = True
J&K=>B = True
C?
C = True
D?
D = True
A&B&C&D=>F = True
I&J&F=>Q = True

Test sample 3
--------

Trace:
Z?
X&Y=>Z?
X?
F|G|H|P=>X?
F?
C|D=>F?
C?
A&B=>C?
A?
A = True
B?
B = True
A&B=>C = True
C|D=>F = True
F|G|H|P=>X = True
Y?
Y = True
X&Y=>Z = True

Test sample 4
--------

Trace:
C?
A&B=>C?
A?
C=>A?
C?
Path is not complete: A&B->C->A&B
A = False
A&B=>C = False

Test sample 5
--------

Trace:
E?
D&C=>E?
D?
A=>D?
A?
A = True
A=>D = True
C?
B=>C?
B?
B = False
A=>C?
A=>C = True
D&C=>E = True