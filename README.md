# Hybrid-Programming
The basic aims of parallel programming are to decrease the runtime for the solution to a problem and increase the size of the problem that can be solved.The conventional parallel programming practices involve a a pure OpenMP implementation on a shared memory architecture or a pure MPI implementation on distributed memory computer architectures.The largest and fastest computers today employ both shared and distributed memory architecture. This gives a flexibility in tuning the parallelism in the programs to generate maximum efficiency and balance the computational and communication loads in the program. A wise implementation of hybrid parallel programs utilizing the modern hybrid computer hardware can generate massive speedups in the otherwise pure MPI and pure OpenMP implementations.
Hybrid application programs using MPI + OpenMP are now commonplace on large HPC systems. There are essentially two main motivations for this combination of programming models:
  1. Reduction in memory footprint, both in the application and in the MPI library (e.g. communication buffers).
  2. Improved performance, especially at high core counts where the pure MPI scalability is running out.

In this project, I have implemented the mergesort algorithm using OpenMP and MPI constructs and compared the execution times between sequential, OpenMP, MPI and Hybrid programming. 

<h3> Results </h3>
The hybrid model starts to perform better as the input size and the no of core counts increases.

 <img width="899" alt="Screen Shot 2023-06-15 at 4 06 55 PM" src="https://github.com/pratheeksha22/Hybrid-Programming/assets/51379668/75257440-cdb0-4006-b882-38b7ae966fbc">
