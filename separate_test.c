// test.c - Reads in a photo and prints out data

// Libraries
#include<stdlib.h>
#include<stdio.h>
#include<math.h>

// Definitions
#define WIDTH 2448
#define HEIGHT 3264
#define DDR3_ADDR 0x00000000	// Address of SDRAM

// Declare global variables
// Reading in files
long double finalB1L1[200];
long double finalB1L2[200];
long double finalW1L1[200][784];
long double finalW1L2[200][200];
long double finalSoftmaxTheta[10][200];
//long double data[784];
int read_data1[2448][3264];
int read_data2[2448][3264];

// Computing ROI and Separate Images
double image[WIDTH][HEIGHT];
int w, x, y, v, lt, lb, rt, rb;
int black_white[WIDTH][HEIGHT];
int roi[WIDTH][HEIGHT];
int digit[WIDTH][HEIGHT];
int digits[4][WIDTH][HEIGHT];
int size_x = 0, size_y = 0;

// List of functions
void read_pic(void);
void read_nn(void);
void gray_bw(void);
void gray_bw2(void);
void region(void);
void separate(void);

int main(void)
{
  read_pic();	// Reads files correctly
  //printf("%d\n",read_data1[0][0]);
  //printf("%d\n",read_data2[0][0]);

  gray_bw2();	// 
  //printf("%f\n",image[0][0]);
  //printf("%d\n",black_white[0][0]);

  region();
  separate();

  FILE *f = fopen("image.csv", "w");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  int i, j;
  for (i = 0; i < WIDTH; i++)
  {
    for (j = 0; j < HEIGHT; j++)
    {
      fprintf(f, "%d", black_white[i][j]);
      if ((j + 1) != HEIGHT)
      {
        fprintf(f, ", ");
      }//if 
      else
        fprintf(f, "\n");
    }//for j
  }//for i
  fclose(f);
  
  return 0;
}


//
// read_pic - Reads in photo data
//
void read_pic(void)
{
  FILE* fp;
  int i,j;

  // Open pic1_1.csv
  fp = fopen("pic1_1.csv","r");
  for(i = 0; i < 2448; i++)
  {
    for(j = 0; j < 3264; j++)
    {
      fscanf(fp, "%d", &(read_data1[i][j]));
    }//for j
  }//for i
  fclose(fp);

  // Open pic1_2.csv
  fp = fopen("pic1_2.csv","r");
  for(i = 0; i < 2448; i++)
  {
    for(j = 0; j < 3264; j++)
    {
      fscanf(fp, "%d", &(read_data2[i][j]));
    }//for j
  }//for i
  fclose(fp);
}


//
// Reads in files for the Neural Netword
//
void read_nn(void)
{
    FILE* fp;
  int i,j;

  // Open finalW1L1.csv
  fp = fopen("finalW1L1.csv","r");
  for(i = 0; i < 200; i++)
  {
    for(j = 0; j < 784; j++)
    {
      fscanf(fp, "%Le", &(finalW1L1[i][j]));
    }//for j
  }//for i
  fclose(fp);

  // Open finalW1L2.csv	
  fp = fopen("finalW1L2.csv","r");
  for(i = 0; i < 200; i++)
  {
    for(j = 0; j < 200; j++)
    {
      fscanf(fp, "%Le", &(finalW1L2[i][j]));
    }//for j
  }//for i
  fclose(fp);

  // Open finalSoftmaxTheta.csv	
  fp = fopen("finalSoftmaxTheta.csv","r");
  for(i = 0; i < 10; i++)
  {
    for(j = 0; j < 200; j++)
    {
      fscanf(fp, "%Le", &(finalSoftmaxTheta[i][j]));
    }//for j
  }//for i
  fclose(fp);

  // Open finalB1L1.csv
  fp = fopen("finalB1L1.csv","r");
  for(i = 0; i < 200; i++)
  {
    fscanf(fp, "%Le", &(finalB1L1[i]));
  }//for i
  fclose(fp);
	
  // Open finalB1L2.csv
  fp = fopen("finalB1L2.csv","r");
  for(i = 0; i < 200; i++)
  {
    fscanf(fp, "%Le", &(finalB1L2[i]));
  }//for i
  fclose(fp);

/*
  // Open data.csv	
  fp = fopen("data.csv","r");
  for(i = 0; i < 784; i++)
  {
    fscanf(fp, "%Le", &(data[i]));
  }//for i
  fclose(fp);	
*/
}// read_nn(void)


//
// gray_bw - takes in color data from the SDRAM
//           changes color to gray scale
//           changes gray scale to black and white
//
void gray_bw(void)
{
  int i,j, data1,data2, red, green, blue, gray;
  volatile char * write_block = (int *) DDR3_ADDR;

  for(i = 0; i < WIDTH; i++)
  {
    for(j = 0; j < HEIGHT; j++)
    {
      image[i][j] = 0;
      black_white[i][j] = 0;
      data1 = *(write_block);
      write_block++;
      data2 = *(write_block);
      write_block++;

      red = data1 & 1023;
      blue = data2 & 1023;
      green = ((data1 & 31744)>>5) | ((data2 &31744) >> 10);
      gray = (red *.21) + (.72 * green) + (.07*blue);
      image[i][j] = gray;
      if(gray > 179)		// .7 * 255
        black_white[i][j] = 1;
      else
        black_white[i][j] = 0;				
    }// for j
  }// for i
}// gray_bw()


//
// gray_bw2 - 
//
void gray_bw2(void)
{
  int i, j, red, green, blue, gray;
//  FILE *fp = fopen("pic.csv", "r");
	
  for(i = 0; i < WIDTH; i++)
  {
    for(j = 0; j < HEIGHT; j++)
    {
      image[i][j] = 0;
      black_white[i][j] = 0;
//      fscanf(fp,"%lf",&data1);
//      fscanf(fp,"%lf",&data2);

      red = read_data1[i][j] & 1023;
      blue = read_data2[i][j] & 1023;
      green = ((read_data1[i][j] & 31744)>>5) | ((read_data2[i][j] &31744) >> 10);
      gray = (red *.21) + (.72 * green) + (.07*blue);
      image[i][j] = gray;
      if(gray > 179)
        black_white[i][j] = 1;
      else
        black_white[i][j] = 0;			
    }//for j
  }//for i
}//gray_bw()


void region(void)
{
  int prev = 0, val = 0, i, j, prev_val;
  i = HEIGHT/2;
 
  // Left Edge = x
  for(j = 0; j < WIDTH; j +=50)
  {
    prev_val = val;
    if(black_white[i][j] == 1)
    {
      x = j;
      val = val + 1;
    }
    if(prev_val == val && val)
      val = 0;
    if(val == 3)
    {
      val = 0;
      prev_val = 0;
      break;
    }
  }

  // Right Edge = y
  for(j = WIDTH; j > 0; j -=50)
  {
    prev_val = val;
    if(black_white[i][j] == 1)
    {
      y = j;
      val = val + 1;
    }
    if(prev_val == val && val)
      val = 0;
    if(val == 3)
    {
      val = 0;
      prev_val = 0;
      break;
    }
  }

  // Top Edge = v
  j = WIDTH/2;
  for(i = 0; i < HEIGHT; i +=50)
  {
    prev_val = val;
    if(black_white[i][j] == 1)
    {
      v = i;
      val = val + 1;
    }
    if(prev_val == val && val)
      val = 0;
    if(val == 3)
    {
      val = 0;
      prev_val = 0;
      break;
    }
  }

  // Bottom Edge = w
  for(i = HEIGHT; i > 0; i -=50)
  {
    prev_val = val;
    if(black_white[i][j] == 1)
    {
      w = i;
      val = val + 1;
    }//if bw = 1
    if(prev_val == val && val)
      val = 0;
    if(val == 3)
    {
      val = 0;
      prev_val = 0;
      break;
    }
  }//for i

// Print LROI
  printf("%d, %d, %d, %d\n", x, y, v, w);
  FILE *fp = fopen("lroi.csv", "w");

  if (fp == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  for(i = v; i < w; i++)
  {
    for(j = x; j < y; j++)
    {
      fprintf(fp, "%d",black_white[i][j]);
      if ((j + 1) != y)
      {
        fprintf(fp, ", ");
      }//if 
      else
        fprintf(fp, "\n");
    }
  }
  fclose(fp);
// end print LROI


// Get new x
  i = ((w-v)/2 + v);
  int temp = (x + ((y-x)/2));
  for(j = x; j < y; j+=5)
  {
    prev_val = val;
    if(black_white[i][j] == 0)
    {
      if(val == 0)
        x = j;
      val = val + 1;
    }
    if(prev_val == val && val)
      val = 0;
    if(val == 2)
    {
      val = 0;
      prev_val = 0;
      break;
    }
  }

// Get new y
  for(j = y; j > x; j-=8)
  {
    prev_val = val;
    if(black_white[i][j] == 0)
    {
      if(val == 0)
        y = j;
      val = val + 1;
    }
    if(prev_val == val && val)
      val = 0;
    if(val == 5)
    {
      val = 0;
      prev_val = 0;
      break;
    }
  }

// Get new v
  j = temp;
  for(i = v; i < w; i+=5)
  {
    prev_val = val;
    if(black_white[i][j] == 0)
    {
      if(val == 0)
        v = i;
      val = val + 1;
    }
    if(prev_val == val && val)
      val = 0;
    if(val == 2)
    {
      val = 0;
      prev_val = 0;
      break;
    }
  }

// Get new w
  for(i = w; i > v; i-=5)
  {
    prev_val = val;
    if(black_white[i][j] == 0)
    {
      if(val == 0)
        w = i;
      val = val + 1;
    }
    if(prev_val == val && val)
      val = 0;
    if(val == 2)
    {
      val = 0;
      prev_val = 0;
      break;
    }
  }
/*	
	x += 3;
	y -= 3;

	for(i = v; i < w; i++){
		if(lt == 0){
			if(black_white[i][v]==0)
				lt = i;
		}
		if(lb == 0){
			if(black_white[v-i][v]==0)
				lb = w-i;
		}
		if(rt == 0){
			if(black_white[i][y]==0)
				rt = i;
		}
		if(rb == 0){
			if(black_white[v-i][y]==0)
				rb = v-i;
		}
		if(lt && lb && rt && rb)
			break;
		
	}

	if(lt >= rt)
		w = lt;
	else
		w = rt;

	if(lb <= rb)
		v = lb;
	else
		v = rb;

*/
// Move region of interest into new array
  int tempx = 0, tempy = 0;
  for(i = v + 10; i < w - 5; i++)
  {
    tempy = 0;
    for(j = x + 15; j < y; j++)
    {
      roi[tempx][tempy] = black_white[i][j];
      tempy++;
      //printf("%d\n",roi[tempx][tempy]); 
    }
    tempx++;
  }
  size_x = tempx;
  size_y = tempy;
  printf("%d, %d\n", size_x, size_y);

// Prints out region of interest
  printf("%d, %d, %d, %d\n", x, y, v, w);
  fp = fopen("roi.csv", "w");

  if (fp == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  for(i = 0; i < size_x; i++)
  {
    for(j = 0; j < size_y; j++)
    {
      fprintf(fp, "%d",roi[i][j]);
      if ((j + 1) != size_y)
      {
        fprintf(fp, ", ");
      }//if 
      else
        fprintf(fp, "\n");
    }
  }
  fclose(fp);
	printf("size x %d\n", size_x);

}//region()



void separate(){


	//NOTE: EDGE in this case means edge of the number, not the border of the image. From now on edge refers to the touching point of a number and border is the position of where the final image is cropped.

	int e_left = 0, e_right, i_left, i_right; //markers for edge of number, border of image
	int edge_select = 0; //flag. 0 is look for left, 1 is look for right.
	int right_select = 0; //flag
	int i = 0, j = 0;
	FILE *fp;
	int start = 0; //starting index position, changes with iteration
	double sum = 0; //sum of pixel column
	int place = 0; //place of digit we are examining, changes with iteration
	int done = 0;
	while(!done){

	i_left = start; //set the left border, it never changes
	edge_select = 0;
	right_select = 0;

	for(i = start; i < size_y; i++){
		sum = 0;
		for(j = 0; j < size_x; j++){
			sum += roi[j][i];	

		}//calculate the sum of the pixel column


		if(sum > 5 && !right_select){
			if(edge_select){

				e_right = i;
			}//if we are looking for the right edge
			else{

				e_left = i;
				edge_select = 1;
			}

		}//if we first hit an edge, set the left edge and begin advancing the right edge. Currently it is useless to set the right edge.
		if(sum > 5 && right_select){

			i_right = e_right + ((i - 1) - (e_right))/2;
			start = i_right;
			break;
		}//looking for the start of next digit. once found, set the center to be halfway between edges of two number.
		if(sum < 5){

			i_right = i;
			if(edge_select){
				right_select = 1;
			}//if we had hit a number have since exited, begin looking for start of next number
		}//if we hit a black column, set the new right.

	}
	printf("ix%d iy%d\n", i_left, i_right);

	if(i == size_y){
		i_right = size_y;
		done = 1;
	}

	int width = i_right - i_left; //width of digit
	int height = size_x;  //height of digit
	printf("height %d\n", height);
	int tempx = 0, tempy = 0;
	for(i = i_left; i < i_right; i++){
		tempx = 0;
		for(j = 0; j < height; j++){
			digit[tempy][tempx] = roi[j][i];
			tempx++;
		}
		tempy++;
	}//fill empty matrix
	printf("tx%d,ty %d\n", tempx, tempy);
	switch(place){
		case 0:	fp = fopen("digit0.csv", "w"); break;
		case 1: fp = fopen("digit1.csv", "w"); break;
		case 2: fp = fopen("digit2.csv", "w"); break;
		case 3: fp = fopen("digit3.csv", "w"); break;
		default: fp = fopen("junk.csv", "w"); break;

	}
  if (fp == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }
	printf("%d by %d\n", tempy, tempx);

  for(i = 0; i < tempy; i++)
  {

    for(j = 0; j < tempx; j++)
    {
      fprintf(fp, "%d",digit[j][i]);
      if ((j + 1) != size_x)
      {
        fprintf(fp, ", ");
      }//if 
      else
        fprintf(fp, "\n");
    }
  }
  fclose(fp);
/*
	//pixels are pixel count increments scaled down by 28
	//start and ends are boxes that should fit 28 by 28, trimmed down edges by remainder/2 each side
	int x_pixels = width/28 - (width%28);
	int y_pixels = height/28 - (height%28); 
	int x_start = (width%28)/2;
	int x_end = width - (width%28)/2;
	int y_start = (height%28)/2;
	int y_end = height - (height%28)/2;
	int k, l;
	double average;
	double square = x_pixels * y_pixels;
	int digitsx = 0, digitsy = 0;//These are the pixels of the scaled down digit		

	for(i = x_start; i < x_end; i += x_pixels){
		for(j = y_start; j < y_end; j += y_pixels){
			for(k = 0; k < x_pixels; k++){
				for(l = 0; l < y_pixels; l++){
					average += digit[k][l];
				}//Calculate the sum of the square
			}
			average = average / square; //Get the average density
			if(sum >= 0.5)
				digits[place][digitsx][digitsy] = 1;
			else
				digits[place][digitsx][digitsy] = 0;
			digitsy++;
		}
		digitsx++;
	}
*/
	place++;//Increase to next digit.
	}

}



