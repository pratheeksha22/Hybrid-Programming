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
	int color, x, y, i=0, red, green, blue,tmp, tid;
	long w,h;
	omp_sched_t def_sched; int def_chunk_size;
	omp_get_schedule(&def_sched,&def_chunk_size);
	printf("Deafault %d %d \n",def_sched,def_chunk_size);
	printf("Size\t\tStatic\t\tDynamic\t\tGuided\n");
	for(int i=0;i<4;i++){
		sprintf(iname,"in%d.png",i+1);
		for(int sched=0x01; sched<=0x03; sched++){
			fp = fopen(iname,"r");
			sprintf(oname,"Output%d%d.png",i+1,sched);
			img = gdImageCreateFromPng(fp);
			w = gdImageSX(img);
			h = gdImageSY(img);

			if(sched == 0x1){
				printf("%4ldx%4ld\t",w,h);
			}
            omp_set_schedule(sched, 100);
			
			double t = omp_get_wtime();
			#pragma omp parallel for private(x,y,color,red,blue,green,tid) schedule(runtime) num_threads(4)
			for(x = 0; x < w; x++){
				for(y = 0; y < h; y++){
					tid = omp_get_thread_num();
					color = gdImageGetPixel(img, x, y);
			        red = gdImageRed(img, color);
			        green = gdImageGreen(img, color);
			        blue = gdImageBlue(img, color);
			        tmp = (red + green + blue) / 3;
			        red = green = blue = tmp;
					if (tid == 0){
			            color = gdImageColorAllocate(img, red, 0, 0);
			            gdImageSetPixel(img, x, y, color);
			        }
			        if (tid == 1){
			            color = gdImageColorAllocate(img, 0, green, 0);
			            gdImageSetPixel(img, x, y, color);
			        }
			        if (tid == 2){
			            color = gdImageColorAllocate(img, 0, 0, blue);
			            gdImageSetPixel(img, x, y, color);
			        }
			        if (tid == 3){
			            color = gdImageColorAllocate(img, red, green, blue);
			            gdImageSetPixel(img, x, y, color);
			        }
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
