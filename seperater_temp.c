// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "finalB1L1.c"
#include "finalB1L2.c"
#include "finalSoftmaxTheta.c"
#include "finalW1L1.c"
#include "finalW1L2.c"

// Definitions
#define WIDTH 640
#define HEIGHT 480
#define BW_LEVEL  130

// Registers
#define READ_IN_ADDR  0xFF200090
#define READ_OUT_ADDR   0xFF200080
#define VGA_DATA1 0xFF200070
#define VGA_DATA2 0xFF200060
#define SDRAM_DATA1 0xFF200050
#define SDRAM_DATA2 0xFF200040
#define VGA_CLK_IN  0xFF200030
#define CAM_START 0xFF200020
#define SOURCE_SELECT 0xFF200010
#define CONTROLLING_CLK 0xFF200000
#define TEST 0xFF200100
#define DDR3_ADDR 0x00100000

// Computing ROI and Separate Images
int w, x, y, v, lt, lb, rt, rb;
int black_white[HEIGHT][WIDTH];
int roi[HEIGHT][WIDTH];
int digit[HEIGHT][WIDTH];
int data[784];
int size_x = 0, size_y = 0;

// List of functions
void region(void);
void region2(int cols,int mat[][cols]);
void resize(void);
int recognizer(void);

int main(void)
{
  volatile int * (cam_start) = (int *) CAM_START; // Output
  volatile int * (sdram_data1) = (int *) SDRAM_DATA1; //Input
  volatile int * (sdram_read) = (int *) READ_OUT_ADDR;  //Output
  volatile int * (read_good) = (int *) VGA_CLK_IN;  //Input
  volatile int * (vga_data1) = (int *) VGA_DATA1; //Output
  volatile int * (vga_data2) = (int *) VGA_DATA2; //Output
  volatile int *(clock_select) = (int *) SOURCE_SELECT;
  volatile int *(clock_gen) = (int *) CONTROLLING_CLK;
  int M;
  int i = 0;
  int j = 0;
  int k = 0;
  int L = 0;
  int snapshot = 0;
  *vga_data1 = 1;
  *vga_data2 = 1;
  *clock_select = 0;
  int write_data = 0;
  int written = 0;


while(1){
  *cam_start = 1;
  printf("Press enter to start\n");

   fflush(stdin);
   getchar();
   fflush(stdin);



    // delay before capture
    for (i = 0; i < 30000; i++)
    {
    }
      snapshot = 1;
    if (snapshot)
    {
      *cam_start = 0; // pause camera
      *clock_select = 1;  // choose custom clock from hps
      *vga_data1 = 0; // reset sdram
      *vga_data1 = 1; 
      *sdram_read = 1;  // set read request to high

      // clear out first horizontal row, it is all black
      for (k = 0; k < 643; k = k+1)
      {
        *clock_gen = 1; // generate 4 clock cycles to move slower clock 1 cycle
        *clock_gen = 0;
        *clock_gen = 1;
        *clock_gen = 0;
        *clock_gen = 1;
        *clock_gen = 0;
        *clock_gen = 1;
        *clock_gen = 0;
      }

      // begin reading in data
      for (j = 0; j < 480; j = j+1)
      {
      for (k = 0; k < 640; k = k+1)
      {
        for (L = 0; L < 4; L = L+1)
        {
          *clock_gen = 1; // generate 4 clock cycles, checking each cycle
          if (!written)
          {
            if (*read_good) // take in data from verilog to read block (not sure if needed)
            {
              write_data = *(sdram_data1);
              if (write_data < BW_LEVEL)  // write black or white
              {
                //*write_block = 0;
                black_white[j][k] = 0;
              }
              else
              {
                //*write_block = 1;
                black_white[j][k] = 1;
              }
              //write_block++;
              written = 1;
            }
          }
          *clock_gen = 0;
        }
        written = 0;
      }
      *sdram_read = 0;
      *sdram_read = 1;
      }
      *sdram_read = 0;

      *vga_data1 = 0;
      *vga_data2 = 0;
      *vga_data1 = 1;
      *vga_data2 = 1;

      *cam_start = 1;
      *clock_select = 0;
      snapshot = 0;
      //printf("Done\n");
    }

    *(sdram_read) = 0;

    region();
    resize();
    M = recognizer();
    printf("Guessed %d\n\n",M);
  }

  return 0;
}

void region(void)
{
  int val = 0, i, j, prev_val;
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
  //printf("%d, %d, %d, %d\n", x, y, v, w);

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

  //printf("%d, %d, %d, %d\n", x, y, v, w);

}//region()

void region2(int cols,int mat[][cols])
{
	int xLeft, xRight, yTop, yBot;
	int r = ROWS/2;
	int c = 0;
	int prev_hits = 0;
	int hits = 0;

	for (c = 0; c < cols; c = c + 25)
	{
		prev_hits = hits;
		if ( mat[r][c] == 1)
		{
			xLeft = c;
			hits++;
		}
		if (prev_hits == hits && hits) // if no new hits, and 
			hits = 0;
		if (hits == 3)
		{
			hits = 0;
			prev_hits = 0;
			break;
		}
	} // for (col = 0;...)







} // region



void resize(void){
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
  k = 0;
  for(i = 0; i < 28; i++)
  {
    for(j = 0; j < 28; j++)
    {
      data[k] = digits[j][i];
      k++;
    }
  }
	
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



void digit_separate()
{
	int r = 0;
	int c = 0;
	int i = 0;
	int j = 0;
	int first = 1;
	int hit = 0;
	int mid = 0;
	int last_mid = 0;
	int right_edge = 0;
	int column_checker = 0;
	int bad = 0;
	int digit_left = 0;
	int digit_right = 0;
	int **digit;

	for (c = 0; i < size_x; c = c+4)
	{
		if ( (roi[r][c] == 1) || (c+4 > size_x) )
		{
			if (first)
			{
				first = 0;
				hit = 1;
			}
			else
			{
				if ( hit == 0)
				{
					hit = 1;
					mid = c + right_edge/2;
					if ( roi[r][c] == 1)
					{
						for (column_checker = 0; column_checker < size_y; column_checker++)
						{
							if (roi[column_checker][mid] == 1)
							{
								bad = 1;
								break;
							}
						}

					} // if ( roi[r][c] == 1)
					if (bad != 1)
					{
						/*
						digit = (int **)malloc(size_y * sizeof(int *));
						for (r = 0; r < size_y; r++)
							digit[r] = (int *)malloc(size_x * sizeof(int));
						*/
						digit_left = last_mid;
						digit_right = mid;
						for (i = last_mid; i < mid; i++) // rows
						{
							for (j = 0; j < size_y; j++) // cols
							{
								if (roi[i][j] == 1)
									digit_left = i;
							}
						}

						for (i = mid; i > last_mid; i--)
						{
							for (j = 0; j < size_y; j++)
							{
								if (roi[i][j] == 1)
									digit_right = i;
							}
						}


					} // if (bad != 1)



				} // if ( hit == 0)
			}
		} // if ( (roi[r][c] == 1) ||...




	} // for(c = 0; i < size_x; c = c+4)
} // digit_separate

void digit_separate2()
{

} // digit_separate2
