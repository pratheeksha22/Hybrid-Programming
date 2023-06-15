#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<math.h>
#define CLUSTER_SIZE 4

int cluster[CLUSTER_SIZE][2] = {{75, 25}, {25, 25}, {25, 75}, {75, 75}};
long long cluster_count[CLUSTER_SIZE];
unsigned long points_sizes[6]={100000,500000,1000000,5000000,10000000};
short points[10000000][2];
char output[10000]="";

void populate_points(unsigned long long size) {
	long long i;
	for(i = 0; i < size; i++) {
		srand(i);
		points[i][0] = rand() % 100;
		points[i][1] = rand() % 100;
	}
}

double get_distance(int x1,  int y1,  int x2,  int y2) {
	int x = x2-x1, y = y2-y1;
	return (double)sqrt((x * x) + (y * y));
}

int main() {
	double t;
	sprintf(output,"Size\t\tT1\tT2\tT4\tT8\n");
	for(int index=0; index<5; index++){
		printf("Size: %ld",points_sizes[index]);
		sprintf(output,"%s%8ld\t",output,points_sizes[index]);
		populate_points(points_sizes[index]);
		unsigned long long i;
		for(int nt=1;nt<9;nt*=2){
			for(i = 0; i < CLUSTER_SIZE; i++)
				cluster_count[i] = 0;
			t = omp_get_wtime();
			#pragma omp parallel for reduction(+:cluster_count) num_threads(nt)
			for(i = 0; i < points_sizes[index]; i++) {
				double min_dist = 100, cur_dist = -1;
				int j, cluster_index = -1;
				for(j = 0; j < CLUSTER_SIZE; j++) {
					cur_dist = get_distance(points[i][0], points[i][1], cluster[j][0], cluster[j][1]);
					if(cur_dist < min_dist) {
						min_dist = cur_dist;
						cluster_index = j;
					}
				}
				cluster_count[cluster_index]++;
			}
			t = omp_get_wtime() - t;
			sprintf(output,"%s%.4lfs\t",output,t);
			if (nt==8)
			for(i = 0; i < CLUSTER_SIZE; i++)
				printf("\nCluster (%d, %d): %lld", cluster[i][0], cluster[i][1], cluster_count[i]);
		}
		printf("\n");
		sprintf(output,"%s\n",output);
	}
	puts(output);
	return 0;
}
