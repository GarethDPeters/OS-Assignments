Gareth Peters
260678626
README.txt

OS used: mimi.cs.mcgill.ca

1. PARTITION gets deleted when the program first runs, and is created when Mount is first called.
2. testfile.txt contains exec calls to three other testfiles: testfile1.txt, testfile2.txt, testfile3.txt
3. exec and run will print out the commands as they are processed. So for example, if you call exec testfile.txt that contains the set x 1 command, set x 1 will be printed in the terminal.
4. Mount can also be ran outside of an exec call since it does not require a PCB to run
5. When a partition is created, it also creates a folder in which all files in that partition will exist. These folders will continue to exist until manually deleted.

