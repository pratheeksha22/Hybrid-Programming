#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#if _POSIX_TIMERS
#include <time.h>
#ifdef CLOCK_MONOTONIC_RAW
#define SYS_RT_CLOCK_ID CLOCK_MONOTONIC_RAW
#else
#define SYS_RT_CLOCK_ID CLOCK_MONOTONIC
#endif

double
get_time(void)
{
    struct timespec ts;
    double t;
    if (clock_gettime(SYS_RT_CLOCK_ID, &ts) != 0)
    {
        perror("clock_gettime");
        abort();
    }
    t = (double)ts.tv_sec + (double)ts.tv_nsec * 1.0e-9;
    return t;
}

#else /* !_POSIX_TIMERS */
#include <sys/time.h>

double
get_time(void)
{
    struct timeval tv;
    double t;
    if (gettimeofday(&tv, NULL) != 0)
    {
        perror("gettimeofday");
        abort();
    }
    t = (double)tv.tv_sec + (double)tv.tv_usec * 1.0e-6;
    return t;
}

#endif


#define SMALL    32

extern double get_time (void);
void merge (int a[], int size, int temp[]);
void insertion_sort (int a[], int size);
void mergesort_serial (int a[], int size, int temp[]);
void mergesort_parallel_mpi (int a[], int size, int temp[],
			     int level, int my_rank, int max_rank,
			     int tag, MPI_Comm comm);
int my_topmost_level_mpi (int my_rank);
void run_root_mpi (int a[], int size, int temp[], int max_rank, int tag,
		   MPI_Comm comm);
void run_helper_mpi (int my_rank, int max_rank, int tag, MPI_Comm comm);
int main (int argc, char *argv[]);

int main (int argc, char *argv[])
{
 
  MPI_Init (&argc, &argv);
  int comm_size;
  MPI_Comm_size (MPI_COMM_WORLD, &comm_size);
  int my_rank;
  MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
  int max_rank = comm_size - 1;
  int tag = 123;
 
  if (my_rank == 0)
    {				
      puts ("-MPI Recursive Mergesort-\t");
    
      if (argc != 2)	
	{
	  printf ("Usage: %s array-size\n", argv[0]);
	  MPI_Abort (MPI_COMM_WORLD, 1);
	}
    
      int size = atoi (argv[1]);	
      printf ("Array size = %d\nProcesses = %d\n", size, comm_size);
      
      int *a = malloc (sizeof (int) * size);
      int *temp = malloc (sizeof (int) * size);
      if (a == NULL || temp == NULL)
	{
	  printf ("Error: Could not allocate array of size %d\n", size);
	  MPI_Abort (MPI_COMM_WORLD, 1);
	}
    
      srand (314159);
      int i;
      for (i = 0; i < size; i++)
	{
	  a[i] = rand () % size;
	}
    
      double start = get_time ();
      run_root_mpi (a, size, temp, max_rank, tag, MPI_COMM_WORLD);
      double end = get_time ();
      printf ("Start = %.2f\nEnd = %.2f\nElapsed = %.2f\n",
	      start, end, end - start);
     
      for (i = 1; i < size; i++)
	{
	  if (!(a[i - 1] <= a[i]))
	    {
	      printf ("Implementation error: a[%d]=%d > a[%d]=%d\n", i - 1,
		      a[i - 1], i, a[i]);
	      MPI_Abort (MPI_COMM_WORLD, 1);
	    }
	}
    }				
  else
    {				
      run_helper_mpi (my_rank, max_rank, tag, MPI_COMM_WORLD);
    }
  fflush (stdout);
  MPI_Finalize ();
  return 0;
}

// Root process code
void run_root_mpi (int a[], int size, int temp[], int max_rank, int tag,MPI_Comm comm)
{
  int my_rank;
  MPI_Comm_rank (comm, &my_rank);
  if (my_rank != 0)
    {
      printf("Error: run_root_mpi called from process %d; must be called from process 0 only\n",my_rank);
      MPI_Abort (MPI_COMM_WORLD, 1);
    }
  mergesort_parallel_mpi (a, size, temp, 0, my_rank, max_rank, tag, comm);
 
  return;
}


void run_helper_mpi (int my_rank, int max_rank, int tag, MPI_Comm comm)
{
  int level = my_topmost_level_mpi (my_rank);
  
  MPI_Status status;
  int size;
  MPI_Probe (MPI_ANY_SOURCE, tag, comm, &status);
  MPI_Get_count (&status, MPI_INT, &size);
  int parent_rank = status.MPI_SOURCE;
  
  int *a = malloc (sizeof (int) * size);
  int *temp = malloc (sizeof (int) * size);
  MPI_Recv (a, size, MPI_INT, parent_rank, tag, comm, &status);
  mergesort_parallel_mpi (a, size, temp, level, my_rank, max_rank, tag, comm);
  // Send sorted array to parent process
  MPI_Send (a, size, MPI_INT, parent_rank, tag, comm);
  return;
}

// Given a process rank, calculate the top level of the process tree in which the process participates
// Root assumed to always have rank 0 and to participate at level 0 of the process tree
int my_topmost_level_mpi (int my_rank)
{
  int level = 0;
  while (pow (2, level) <= my_rank)
    level++;
  return level;
}

void mergesort_parallel_mpi (int a[], int size, int temp[],	int level, int my_rank, int max_rank,int tag, MPI_Comm comm)
{
  int helper_rank = my_rank + pow (2, level);
  if (helper_rank > max_rank)
    {				// no more processes available
      mergesort_serial (a, size, temp);
    }
  else
    {

      MPI_Request request;
      MPI_Status status;
      // Send second half, asynchronous
      MPI_Isend (a + size / 2, size - size / 2, MPI_INT, helper_rank, tag,
		 comm, &request);
      // Sort first half
      mergesort_parallel_mpi (a, size / 2, temp, level + 1, my_rank, max_rank,
			      tag, comm);
      // Free the async request (matching receive will complete the transfer).
      MPI_Request_free (&request);
      // Receive second half sorted
      MPI_Recv (a + size / 2, size - size / 2, MPI_INT, helper_rank, tag,
		comm, &status);
      merge (a, size, temp);
    }
  return;
}

void mergesort_serial (int a[], int size, int temp[])
{
  // Switch to insertion sort for small arrays
  if (size <= SMALL)
    {
      insertion_sort (a, size);
      return;
    }
  mergesort_serial (a, size / 2, temp);
  mergesort_serial (a + size / 2, size - size / 2, temp);
  // Merge the two sorted subarrays into a temp array
  merge (a, size, temp);
}

void merge (int a[], int size, int temp[])
{
  int i1 = 0;
  int i2 = size / 2;
  int tempi = 0;
  while (i1 < size / 2 && i2 < size)
    {
      if (a[i1] < a[i2])
	{
	  temp[tempi] = a[i1];
	  i1++;
	}
      else
	{
	  temp[tempi] = a[i2];
	  i2++;
	}
      tempi++;
    }
  while (i1 < size / 2)
    {
      temp[tempi] = a[i1];
      i1++;
      tempi++;
    }
  while (i2 < size)
    {
      temp[tempi] = a[i2];
      i2++;
      tempi++;
    }
  // Copy sorted temp array into main array, a
  memcpy (a, temp, size * sizeof (int));
}

void
insertion_sort (int a[], int size)
{
  int i;
  for (i = 0; i < size; i++)
    {
      int j, v = a[i];
      for (j = i - 1; j >= 0; j--)
	{
	  if (a[j] <= v)
	    break;
	  a[j + 1] = a[j];
	}
      a[j + 1] = v;
    }
}
