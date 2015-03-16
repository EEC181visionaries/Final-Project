// combo.c - Reads in a photo and prints out data
//           using header files

// Libraries
#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#include"finalB1L1.c"
#include"finalB1L2.c"
#include"finalSoftmaxTheta.c"
#include"finalW1L1.c"
#include"finalW1L2.c"

// Definitions
#define WIDTH 640
#define HEIGHT 480
#define DDR3_ADDR 0x00000000	// Address of SDRAM

// Declare global variables
// Reading in files
//long double finalB1L1[200];
//long double finalB1L2[200];
//long double finalW1L1[200][784];
//long double finalW1L2[200][200];
//long double finalSoftmaxTheta[10][200];

// Computing ROI and Separate Images
double image[HEIGHT][WIDTH];
int w, x, y, v, lt, lb, rt, rb;
int black_white[HEIGHT][WIDTH];
int roi[HEIGHT][WIDTH];
int digit[HEIGHT][WIDTH];
int data[784];
int size_x = 0, size_y = 0;

// List of functions
void read_pic(void);
//void read_nn(void);
void region(void);
void resize(void);
int recognizer(void);

int main(void)
{
  int M;
  //read_nn();  
  read_pic();

  region();
  resize();
  M = recognizer();
  printf("Guessed %d\n",M);

  FILE *f = fopen("image.csv", "w");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  int i, j;
  for (i = 0; i < HEIGHT; i++)
  {
    for (j = 0; j < WIDTH; j++)
    {
      fprintf(f, "%d", black_white[i][j]);
      if ((j + 1) != WIDTH)
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

  // Open 7.csv (Already in bw.)
  fp = fopen("5.txt","r");
  for(i = 0; i < HEIGHT; i++)
  {
    for(j = 0; j < WIDTH; j++)
    {
      fscanf(fp, "%d", &(black_white[i][j]));
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



void region(void)
{
  int prev = 0, val = 0, i, j, prev_val;
  i = HEIGHT/2;
 
  // Left Edge = x
  for(j = 0; j < WIDTH; j +=25)
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
  for(j = WIDTH; j > 0; j -=25)
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
  for(i = 0; i < HEIGHT; i +=25)
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
  for(i = HEIGHT; i > 0; i -=25)
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
    prev_val = val;//printf("run\n");
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

	for(i = w; i < v; i++){
		if(lt == 0){
			if(black_white[i][w]==0)
				lt = i;
		}
		if(lb == 0){
			if(black_white[v-i][w]==0)
				lb = v-i;
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
		v = lt;
	else
		v = rt;

	if(lb <= rb)
		w = lb;
	else
		w = rb;
*/

// Move region of interest into new array
  int tempx = 0, tempy = 0;
  for(i = v; i < w; i++)
  {
    tempy = 0;
    for(j = x; j < y; j++)
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
}//region()



void resize(void){
	FILE *fp;
	int digits[size_y][size_x];
	int i,j;
	int x_pixels = size_x/28;
	int y_pixels = size_y/28; 
	int x_start = (size_x%28)/2;
	int x_end = size_x - (size_x%28)/2;
	int y_start = (size_y%28)/2;
	int y_end = size_y - (size_y%28)/2;
	int k, l;
	double average;
	double square = x_pixels * y_pixels;
	int digitsx = 0, digitsy = 0;//These are the pixels of the scaled down digit		

	//printf("%d %d %d %d %d %d\n", x_pixels, y_pixels, x_start, x_end, y_start, y_end);
	for(i = x_start; i < x_end; i += x_pixels){
		digitsy = 0;
		for(j = y_start; j < y_end; j += y_pixels){
			average = 0;
			for(k = 0; k < x_pixels; k++){
				for(l = 0; l < y_pixels; l++){
					average += roi[i + k][j + l];
				}//Calculate the sum of the square
			}
			average = average / square; //Get the average density
			if(average >= 0.5)
				digits[digitsx][digitsy] = 1;
			else
				digits[digitsx][digitsy] = 0;
			digitsy++;
		}
		digitsx++;
		
	}

	  fp = fopen("resize.csv", "w");

  if (fp == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  k = 0;
  for(i = 0; i < 28; i++)
  {
    for(j = 0; j < 28; j++)
    {
      fprintf(fp, "%d",digits[i][j]);
      data[k] = digits[j][i];
      k++;
      if ((j + 1) != 28)
      {
        fprintf(fp, ", ");
      }//if 
      else
        fprintf(fp, "\n");
    }
  }
  fclose(fp);

}

int recognizer(void)
{
  long double Vb1[200], Vb2[200], Vb3[10]; // array[row][col]
  int M = 0;
  int i,j;
  long double sum = 0;
  
  
  // Vb1 = finalW1L1*data;
  for (i = 0; i < 200; i++)
  {
    for (j = 0; j < 784; j++)
    {
      sum = sum + finalW1L1[i][j] * data[j];
    } // Matrix Multiplication
    Vb1[i] = sum;
    sum = 0;
  } // Product into new Matrix
  
  
  //Vb1 = Vb1 + finalB1L1;
  for (i = 0; i < 200; i ++)
  {
    Vb1[i] = Vb1[i] + finalB1L1[i];
  } // Matrix Addition
  
  
  //Vb1 = sigmf(Vb1,[1 0]);
  for (i = 0; i < 200; i++)
  {
    Vb1[i] = 1/(1+exp(-Vb1[i]));
  } // Sigmoid
  
  
  //Vb1 = finalW1L2*Vb1;
  for (i = 0; i < 200; i++)
  {
    for (j = 0; j < 200; j++)
    {
      sum = sum + finalW1L2[i][j] * Vb1[j];
    } // Matrix Multiplication
    Vb2[i] = sum;
    sum = 0;
  } // Product into old Matrix
  
  
  //Vb1 = Vb1 + finalB1L2;
  for (i = 0; i < 200; i ++)
  {
    Vb2[i] = Vb2[i] + finalB1L2[i];
  } // Matrix Addition


  //Vb1 = sigmf(Vb1,[1 0]);
  for (i = 0; i < 200; i++)
  {
    Vb2[i] = 1/(1+exp(-Vb2[i]));
  } // Sigmoid
  
  
  //Vb1 = finalSoftmaxTheta*Vb1;          finalSoftmaxTheta[10][200]
  for (i = 0; i < 10; i++)
  {
    for (j = 0; j < 200; j++)
    {
      sum = sum + finalSoftmaxTheta[i][j]*Vb2[j];
    } //
    Vb3[i] = sum;
    sum = 0;
  } // 
  
 
  //M = find(Vb1==max(Vb1));
  double max = 0;
  for (i = 0; i < 10; i++)
  {
    if (max < Vb3[i])
    {
      max = Vb3[i];
      M = i + 1;
    }
  } // Finding Max Value
  
  
  // Check for Zero
  //if(M == 10)
  //  M = 0;
  //end
  if (M == 10)
  {
    M = 0;
  } // Check for zero


  //output = M;
    return M;
}










