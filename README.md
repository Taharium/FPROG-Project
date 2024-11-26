### Why two Folders

We were unsure, if using a std::unordered_set is allowed, so made one with and one without.
The problem is that without using a set, inserting into the tree recursivly is impossible, 
because were are inserting about 590 000 words and wsl or linux in general can only tolerate about 260 000
recursions, because stack size is limited to 8 MB. Therefore, we had to insert iterativly.
However, we added futures to make it faster.

Using the set reduces the words to about 18 000 words which is easy for linux.

### How to compile and run

You need wsl to compile the program using g++.
Enter the desired folder and run wslBuild.bat to build the program and wslRun.bat to run it.

If none of it works, we provided the binaries, so you may run it.

### How to run the tests

Enter the desired folder and run testRun.bat to build and run it.



