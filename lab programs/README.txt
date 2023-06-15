1a. gcc Program1.c -fopenmp
    ./a.out
1b. mpicc Program1b.c
    mpirun -np 4 ./a.out
2. gcc Program2.c -fopenmp
   ./a.out
3. g++ Program3.c -fopenmp -lm
   ./a.out
4&4b. gcc Program4.c -fopenmp -lgd
   ./a.out
Note:Four input png files must be present in the working directory with names in1.png, in2.png, in3.png, in4.png. Else segmentation fault will occur.
5. gcc Program5.c -fopenmp -lm
Note: If reduction error is raised need to update gcc and openmp. Ask lab assistant
6. gcc Program6.c -fopenmp
Note: Four input text files must be present in the working directory with names file1.txt, file2.txt, file3.txt, file4.txt. Else segmentation fault will occur.
7. mpicc Program7.c 
   mpirun -np 3 ./a.out <input>
Note: Give only odd number of processes like 3, 5, 7, etc. p2_compute()-Count odd numbers p1_compute()-Count even numbers
8. mpicc Program8.c -lm
   mpirun ./a.out <a-value> <b-value> <c-value>
Note: The value of exact has to be computed if value of a and b changes. Use calculator to calculate the definite integral.
9. mpicc Program8.c
   mirun -np 4 ./a.out   mirun -np 8 ./a.out
10. pgcc -acc -ta=multicore -Minfo=accel Program10.c
    ./a.out <size>
11. pgcc -acc -ta=multicore -Minfo=accel Program11.c -lm
    ./a.out