#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#define SEED 50123

int main()
{
	long n = 0, i, count = 0;
	double x,y,z;
	srand(SEED);
	printf("Size\t\tT1\t\t\tT2\t\t\tT4\t\t\tT8");
	for(n=100; n<=1000000; n*=10){
		printf("\n%ld\t",n);
		for(int t=1; t<=8; t*=2){
			count = 0;
			double start = omp_get_wtime();
			#pragma omp parallel for private(x, y, z) reduction(+:count) num_threads(t)
			for ( i=0; i<n; i++){
				x = (double)rand()/RAND_MAX;
				y = (double)rand()/RAND_MAX;
				z = x*x+y*y;
				if (z<=1) count++;
			}
			double pi=(double)count/n * 4;
			double stop = omp_get_wtime();
			printf("%lf %lfs\t",pi,stop-start);
		}
	}
	printf("\n");
	return 0;
}

