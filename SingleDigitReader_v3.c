// single_digit_reader_v2.c - Reads in a photo and prints out data

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
int size_x = 0, size_y = 0;

// List of functions
void region(void);
void region2(int cols,int mat[][cols]);
int resize(int height, int width, int** digit);
int recognizer(int data[784]);

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

void region2(int cols,int rows,int mat[rows][cols])
{
	int xLeft, xRight, yTop, yBot;
	int r = HEIGHT/2;
	int c = 0;
	int prev_hits = 0;
	int hits = 0;

  // LROI Left Edge = xLeft
	for (c = 0; c < cols; c = c + 25)
	{
		prev_hits = hits;
		if (mat[r][c] == 1)
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
	} // for (c = 0;...)

  // LROI Right Edge = xRight
  for (c = cols; c > 0; c = c - 25)
  {
    prev_hits = hits;
    if (mat[r][c] == 1)
    {
      xRight = c;
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
  } // for (c = cols;...)

  // LROI Top Edge = yTop
  c = WIDTH/2;
  for (r = 0; r < rows; c = c + 25)
  {
    prev_hits = hits;
    if (mat[r][c] == 1)
    {
      yTop = r;
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
  } // for (r = 0;...)

  // LROI Bottom Edge = yBot
  for (r = rows; r > 0; c = c - 25)
  {
    prev_hits = hits;
    if (mat[r][c] == 1)
    {
      yBot = r;
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
  } // for (r = rows;...)

  // ROI Left Edge = xLeft
  r = (yBot+yTop)/2;
  int tempxEdge = (xLeft + xRight)/2;
  for (c = xLeft; c < xRight; c = c + 5)
  {
    prev_hits = hits;
    if (mat[r][c] == 0)
    {
      if (hit == 0)
        xLeft = c;
      hits++;
    }
    if (prev_hits == hits && hits) // if no new hits, and 
      hits = 0;
    if (hits == 2)
    {
      hits = 0;
      prev_hits = 0;
      break;
    }
  } // for (c = xLeft;...)

  // ROI Right Edge = xRight
  for (c = xRight; c > xLeft; c = c - 5)
  {
    prev_hits = hits;
    if (mat[r][c] == 0)
    {
      if (hit == 0)
        xRight = c;
      hits++;
    }
    if (prev_hits == hits && hits) // if no new hits, and 
      hits = 0;
    if (hits == 2)
    {
      hits = 0;
      prev_hits = 0;
      break;
    }
  } // for (c = xRight;...)

  // ROI Top Edge = yTop
  c = tempxEdge;
  for (r = yTop; r < yBot; c = c + 5)
  {
    prev_hits = hits;
    if (mat[r][c] == 0)
    {
      if (hit == 0)
        yTop = r;
      hits++;
    }
    if (prev_hits == hits && hits) // if no new hits, and 
      hits = 0;
    if (hits == 2)
    {
      hits = 0;
      prev_hits = 0;
      break;
    }
  } // for (r = yTop;...)

  // ROI Bottom Edge = yBot
  for (r = yBot; r > yTop; c = c - 5)
  {
    prev_hits = hits;
    if (mat[r][c] == 0)
    {
      if (hit == 0)
        yBot = r;
      hits++;
    }
    if (prev_hits == hits && hits) // if no new hits, and 
      hits = 0;
    if (hits == 2)
    {
      hits = 0;
      prev_hits = 0;
      break;
    }
  } // for (r = yBot;...)

  // Move region of interest to (0,0) of existing array mat[r][c]
  size_x = xRight - xLeft;
  size_y = yBot - yTop;
  for (r = 0; r < size_y; r = r + 1)
  {
    for (c = 0; c < size_x; c = c +1)
    {
      mat[r][c] = mat[yTop + r][xLeft + c];
    } // for (c = xLeft;...)
  } // for (r = yTop;...)
  
} // region



int resize(int height, int width, int** digit){
  int digit_final[28][28];
  int digit_vector[784];
  int i,j;
  int x_pixels = (width + 27)/28;
  int y_pixels = (height + 27)/28;
  int k, l;
  double average;
  double square = x_pixels * y_pixels;
  int digit_x = 0, digit_y = 0;//These are the pixels of the scaled down digit		

  for(i = 0; i < 28; i++)
  {
    for(j = 0; j < 28; j++)
    {
      digit_final[i][j] = 0;
    }
  }

  for(i = 0; i < (height - (height%28)); i = i + y_pixels)
  {
    digit_x = 0;
    for(j = 0; j < (width - (width%28)); j = j + x_pixels)
    {
      average = 0;
      for(k = 0; k < y_pixels && ((i+k) < height); k++)
      {
        for(l = 0; l < x_pixels && ((j+l) < width); l++)
        {
          average += digit[i+k][j+l];
        }
      }
      average = average / square;

      if(average >= 0.5)
      {
        digit_final[digit_y][digit_x] = 1;
      }
      else
      {
        digit_final[digit_y][digit_x] = 0;
      }
      digit_x++;
    } //for (j = 0;...)
    digit_y++;
  } //for (i = 0;...)

  k = 0;
  for(i = 0; i < 28; i++)
  {
    for(j = 0; j < 28; j++)
    {
      digit_vector[k] = digit_final[i][j];
      k++;
    }
  }

  l = recognizer(digit_vector);
  return l;
} //void resize(...)


int recognizer(int data[784])
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





