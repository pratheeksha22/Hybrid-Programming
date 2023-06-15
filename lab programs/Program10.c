#include <sys/time.h>
#include<stdlib.h>
#include <stdio.h>
#define MAX 1000
int SIZE;
double **a,**b,**c,**d;
int main(int argc, char* argv[]){
  SIZE=atoi(argv[1]);
  a = (double**)malloc(SIZE*sizeof (double*));
  b = (double**)malloc(SIZE*sizeof (double*));
  c = (double**)malloc(SIZE*sizeof (double*));
  d = (double**)malloc(SIZE*sizeof (double*));
  int i,j,k;
  struct timeval tim;
  double t1, t2, tmp;

  /* Initialize matrices.*/
  
  for (i = 0; i < SIZE; ++i) {
  	a[i] = (double*)malloc(SIZE*sizeof(double));  b[i] = (double*)malloc(SIZE*sizeof(double));
  	c[i] = (double*)malloc(SIZE*sizeof(double));  d[i] = (double*)malloc(SIZE*sizeof(double));
    for (j = 0; j < SIZE; ++j) {
      a[i][j] = (double)(i + j);
      b[i][j] = (double)(i - j);
      c[i][j] = 0.0f;
      d[i][j] = 0.0f;
    }
  }
  for (i = 0; i < SIZE; ++i) {
    for (j = 0; j < SIZE; ++j) {
      tmp=0.0f;
      for (k = 0; k < SIZE; ++k) {
        tmp += a[i][k] * b[k][j];
      }
      d[i][j] = tmp;
    }
  }
  /*Time stamp t1*/
  
  gettimeofday(&tim, NULL);
  t1=tim.tv_sec+(tim.tv_usec/1000000.0);
  /* Compute matrix multiplication.*/
  
#pragma acc data copyin(a[0:SIZE][0:SIZE],b[0:SIZE][0:SIZE]) copy(c[0:SIZE][0:SIZE])
#pragma acc kernels
#pragma acc loop tile(32,32)
  for (i = 0; i < SIZE; ++i) {
    for (j = 0; j < SIZE; ++j) {
      tmp=0.0f;
#pragma acc loop reduction(+:tmp)
      for (k = 0; k < SIZE; ++k) {
        tmp += a[i][k] * b[k][j];
      }
      c[i][j] = tmp;
    }
  }
  /* Time stamp t2, elapsed time OpenACC*/
  
  gettimeofday(&tim, NULL);
  t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("%.6lf seconds with OpenACC \n", t2-t1);
  /* Check the OpenACC result matrix*/
  
  for (i = 0; i < SIZE; ++i)
    for (j = 0; j < SIZE; ++j)
      if(c[i][j] != d[i][j]) {
	printf("Error %d %d %f %f \n", i,j, c[i][j], d[i][j]);
	exit(1);
      }
  printf("OpenACC matrix multiplication test was successful!\n");
  return 0;
}
