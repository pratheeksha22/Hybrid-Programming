#include <stdio.h>
#include <error.h>
#include <gd.h>
#include<string.h>
#include<omp.h>

int main(int argc, char **argv) 
{
	FILE *fp,*fp1 = {0};
	gdImagePtr img;
	char iname[15], oname[15];
	int color, x, y, i=0, red, green, blue,tmp;
	long w,h;
	omp_sched_t def_sched; int def_chunk_size;
	omp_get_schedule(&def_sched,&def_chunk_size);
	printf("Deafault %d %d \n",def_sched,def_chunk_size);
	printf("Size\t\tDefault\t\tStatic\t\tDynamic\t\tGuided\n");
	for(int i=0;i<4;i++){
		sprintf(iname,"in%d.png",i+1);
		for(int sched=0x0; sched<=0x3; sched++){
			fp = fopen(iname,"r");
			sprintf(oname,"Output%d%d.png",i+1,sched);
			img = gdImageCreateFromPng(fp);
			w = gdImageSX(img);
			h = gdImageSY(img);

			if(sched == 0x0){
				printf("%4ldx%4ld\t",w,h);
				//if(i<=1) printf("\t");
				omp_set_schedule(def_sched, def_chunk_size);
			}
			else
				omp_set_schedule(sched, 0);
			
			double t = omp_get_wtime();
			#pragma omp parallel for private(y,color,red,green,blue,tmp)
			for(x = 0; x < w; x++){
				for(y = 0; y < h; y++){
					color=gdImageGetPixel(img, x, y);
					red   = gdImageRed(img, color);
					green = gdImageGreen(img, color);
					blue  = gdImageBlue(img, color);
					tmp = (red+green+blue)/ 3;
					red = green = blue = tmp;
					color = gdImageColorAllocate(img, red, green, blue);
					gdImageSetPixel(img, x, y, color);
				}
			}
			t = omp_get_wtime() - t;
			fp1 = fopen(oname,"w");
			gdImagePng(img, fp1);
			fclose(fp1);

			gdImageDestroy(img);
			printf("%.4fms\t",t*1000);
		}
		printf("\n");
	}
	return 0;
}
