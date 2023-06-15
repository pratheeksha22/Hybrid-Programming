#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#if _POSIX_TIMERS
#include <time.h>
#ifdef CLOCK_MONOTONIC_RAW
#define SYS_RT_CLOCK_ID CLOCK_MONOTONIC_RAW
#else
#define SYS_RT_CLOCK_ID CLOCK_MONOTONIC
#endif
double get_time(void)
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

double get_time(void)
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

void merge (int a[], int size, int temp[]);
void insertion_sort (int a[], int size);
void mergesort_serial (int a[], int size, int temp[]);
extern double get_time (void);
int main (int argc, char *argv[]);

int main (int argc, char *argv[])
{
  puts ("-Serial Recursive Mergesort-\t");

  if (argc != 2)
    {
      printf ("Usage: %s array-size\n", argv[0]);
      return 1;
    }
  int size = atoi (argv[1]);
  printf ("Array size = %d\n", size);
  int *a = (int*)malloc (sizeof (int) * size);
  int *temp = (int*)malloc (sizeof (int) * size);
  if (a == NULL || temp == NULL)
    {
      printf ("Error: Could not allocate array of size %d\n", size);
      return 1;
    }
   int i;
  srand (314159);
  for (i = 0; i < size; i++)
    {
      a[i] = rand () % size;
    }
  double start = get_time ();
  mergesort_serial (a, size, temp);
  double end = get_time ();
  printf ("Start = %.2f\nEnd = %.2f\nElapsed = %.2f\n",
	  start, end, end - start);
  for (i = 1; i < size; i++)
    {
      if (!(a[i - 1] <= a[i]))
	{
	  printf ("Implementation error: a[%d]=%d > a[%d]=%d\n", i - 1,a[i - 1], i, a[i]);
	  return 1;
	}
    }
  puts ("-Success-");
  return 0;
}

void
mergesort_serial (int a[], int size, int temp[])
{
  if (size <= SMALL)
    {
      insertion_sort (a, size);
      return;
    }
  mergesort_serial (a, size / 2, temp);
  mergesort_serial (a + size / 2, size - size / 2, temp);
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


