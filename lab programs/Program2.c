#include<stdio.h>
#include<omp.h>
#include<stdlib.h>

int main(int argc, char* argv[]){
	printf("Size\t1\t2\t4\t8");
	for(int n=500; n<=2000; n+=500){
		printf("\n%d\t",n);
		int **a,**b,**c;
		int i,j,k;

		a=(int**)malloc(n* sizeof (int*));
		b=(int**)malloc(n* sizeof (int*));
		c=(int**)malloc(n* sizeof (int*));
		for(i=0; i<n; i++){
			a[i] = (int*) malloc(n * sizeof (int));
			b[i] = (int*) malloc(n * sizeof (int));
			c[i] = (int*) malloc(n * sizeof (int));
			for(j=0; j<n; j++){
				a[i][j] = rand()%n;
				b[i][j] = rand()%n;
			}
		}

		for(int t=1; t<=8; t*=2){
			double x = omp_get_wtime();
			#pragma omp parallel for private(j,k) num_threads(t)
			for(i=0; i<n; i++){
				for(j=0; j<n; j++){
					c[i][j]=0;
					for(k=0; k<n; k++)
						c[i][j] += a[i][k]*b[k][j];
				}
			}
			double y = omp_get_wtime();
			printf("%lf\t", y-x);
		}
	}
	printf("\n");
	return 0;
}
