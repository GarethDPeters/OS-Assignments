Gareth Peters
260678626
README.txt

OS used: linux.cs.mcgill.ca

1. PARTITION gets deleted when the program first runs, and is created when mount is first called.
2. testfile.txt contains exec calls to three other testfiles: testfile1.txt, testfile2.txt, testfile3.txt
3. exec and run will print out the commands as they are processed. So for example, if you call exec testfile.txt that contains the set x 1 command, set x 1 will be printed in the terminal.
4. mount can also be ran outside of an exec call since it does not require a PCB to run
5. Files that are created have the default location of the project folder. If a name was added to the partition struct and saved when it was unmounted, then separate folders could have been created for their respective partitions. 

