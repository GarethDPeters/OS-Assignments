Gareth Peters
260678626
README.txt

OS used: linux.cs.mcgill.ca

1. The run(quanta) method does not take as input a quanta. Instead it takes in an error pointer to return an error code for quitting the program. If the quanta length needs to be changed, it can be changed by calling CPU_Init(quanta). For this assignment, I don't think think it necessary to change quanta lengths seeing as the the PDF states the quanta length will always be 2.
2. testfile.txt contains exec calls to three other testfiles: testfile1.txt, testfile2.txt, testfile3.txt
3. exec and run will print out the commands as they are processed. So for example, if you call exec testfile.txt that contains the set x 1 command, set x 1 will be printed in the terminal.
4. BackingStore entries are deleted after every exec, but can be looked at as the program runs. The BackingStore will be refreshed for the next exec call. 
