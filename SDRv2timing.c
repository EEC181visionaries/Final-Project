// SingleDigitReader_v2.c - Reads in a photo and prints out data
//                          Added region2 NOT TESTED!

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

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

//timing variables
unsigned int region_1, resize_1, rec_1, rec_2, main_1, main_2, main_3, main_4, main_5, main_6, main_7, final; 
unsigned int LROIstart, LROIend, SROIstart, SROIend, ROImovStart, ROImovEnd;
unsigned int resizeStart, resizeEnd, resizeMovStart, resizeMovEnd;
unsigned int MMstart1, MMend1, vstart1, vend1, sigStart1, sigEnd1, MMstart2, MMend2, vstart2, vend2, sigStart2, sigEnd2, MMstart3, MMend3, maxStart, maxEnd;




unsigned int cycle[100];


// Computing ROI and Separate Images
int w, x, y, v, lt, lb, rt, rb;
int black_white[HEIGHT][WIDTH];
int roi[HEIGHT][WIDTH];
int digit[HEIGHT][WIDTH];
int data[784];
int size_x = 0, size_y = 0;

// List of functions
void region(void);
void region2(int cols,int rows, int mat[rows][cols]);
void resize(void);
int recognizer(void);
static inline unsigned int getCycles ();
static inline void initCounters ();


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
//
	int cycleCounter = 0;
	int cycleIndex = 0;
      initCounters();

	  main_1 = getCycles();

      *cam_start = 0; // pause camera
      *clock_select = 1;  // choose custom clock from hps
      *vga_data1 = 0; // reset sdram
      *vga_data1 = 1; 
      *sdram_read = 1;  // set read request to high
//
      main_2 = getCycles();

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
//      
      main_3 = getCycles();

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
//          
//          main_4 = clock();
           
          written = 0;
        }
//        
//        main_5 = clock();
        
        *sdram_read = 0;
        *sdram_read = 1;
		cycleCounter++;
		if (cycleCounter == 48)
		{
			cycleCounter = 0;
			cycle[cycleIndex] = getCycles();
			cycleIndex++;
		}
      }
// 
      main_6 = getCycles();

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
//
    main_7 = getCycles();

	region_1 = getCycles();
    region();
	resize_1 = getCycles();
    resize();
	rec_1 = getCycles();
    M = recognizer();
	rec_2 = getCycles();
    printf("Guessed %d\n\n",M);

	for (i = 0; i < 10; i++)
		printf("%d:\t %u\n",i+1,cycle[i]);
    
    printf("\n1: \t%d\n2: \t%d\n3: \t%d\n4: \n5: \n6: \t%d\n7: \t%d\n", main_1, main_2, main_3, main_6, main_7);

    final = (main_7 - main_1);
    printf("\nTimes:\nMain: \t%d\n\n", final);

	final = (resize_1 - region_1);
    printf("Region: \t%d\n", final);
	printf("\tBreakdown:\n");
	printf("\t Large ROI:\t%d\n", LROIend - LROIstart);
	printf("\t Small ROI:\t%d\n", SROIend - SROIstart);
	printf("\t Array assignmet:\t%d\n\n", ROImovEnd - ROImovStart);

    final = (rec_1 - resize_1);
    printf("Resize: \t%d\n", final);
	printf("\tBreakdown:\n");
	printf("\t resizing:\t%d\n", resizeEnd - resizeStart);
	printf("\t Array assignment:\t%d\n\n", resizeMovEnd - resizeMovStart);


    final = (rec_2 - rec_1);
    printf("Recognize: \t%d\n", final);
	printf("\tBreakdown:\n");
	printf("\t First matrix mult:\t%d\n", MMend1 - MMstart1);
	printf("\t First vector addition:\t%d\n", vend1 - vstart1);
	printf("\t First sigmoid:\t%d\n", sigEnd1 -sigStart1);
	printf("\t 2nd matrix mult:\t%d\n", MMend2 - MMstart2);
	printf("\t 2nd vector addition:\t%d\n", vend2 - vstart2);
	printf("\t 2nd sigmoid:\t%d\n", sigEnd2 - sigStart2);
	printf("\t 3rd matrix mult:\t%d\n", MMend3 - MMstart3);
	printf("\t Determining digit:\t%d\n\n", maxEnd - maxStart);


// =========================

/*
    // Prints ROI
    for (i = 0; i < size_x; i++)
    {
      for (j = 0; j < size_y; j++)
      {
        printf("%d\t", roi[i][j]);
      }
      printf("\n");
    }
*/

// =======================

  }
  return 0;
}

void region(void)
{
//  region_1 = clock();
  int val = 0, i, j, prev_val;
  i = HEIGHT/2;
 
  LROIstart = getCycles();

  // Left Edge = x
  for(j = 0; j < WIDTH; j +=8)
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
  for(j = WIDTH; j > 0; j -=8)
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
  for(i = 0; i < HEIGHT; i +=8)
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
  for(i = HEIGHT; i > 0; i -=8)
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


  LROIend = getCycles();

// Print LROI
  //printf("%d, %d, %d, %d\n", x, y, v, w);

// Get new x
  i = ((w-v)/2 + v);
  int temp = (x + ((y-x)/2));

  SROIstart = getCycles();

  for(j = x; j < y; j+=4)
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
  for(j = y; j > x; j-=4)
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
  for(i = v; i < w; i+=4)
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
  for(i = w; i > v; i-=4)
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

  SROIend = getCycles();
	
  ROImovStart = getCycles();

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

  ROImovEnd = getCycles();
//  region_2 = clock();
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
      if (hits == 0)
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
      if (hits == 0)
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
      if (hits == 0)
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
      if (hits == 0)
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



void resize(void){

//	resize_1 = clock();
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

	resizeStart = getCycles();
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
	resizeEnd = getCycles();

	resizeMovStart = getCycles();
  k = 0;
  for(i = 0; i < 28; i++)
  {
    for(j = 0; j < 28; j++)
    {
      data[k] = digits[j][i];
      k++;
    }
  }
  resizeMovEnd = getCycles();
	
//  resize_2 = clock();
}

int recognizer(void)
{

//  rec_1 = clock();
  long double Vb1[200], Vb2[200], Vb3[10]; // array[row][col]
  int M = 0;
  int i,j;
  long double sum = 0;
  
  MMstart1 = getCycles();
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
  MMend1 = getCycles();
  vstart1 = getCycles();
  //Vb1 = Vb1 + finalB1L1;
  for (i = 0; i < 200; i ++)
  {
    Vb1[i] = Vb1[i] + finalB1L1[i];
  } // Matrix Addition
  vend1 = getCycles();
  

  sigStart1 = getCycles();
  //Vb1 = sigmf(Vb1,[1 0]);
  for (i = 0; i < 200; i++)
  {
    Vb1[i] = 1/(1+exp(-Vb1[i]));
  } // Sigmoid
  sigEnd1 = getCycles();
  
  MMstart2 = getCycles();
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
  MMend2 = getCycles();
  
  vstart2 = getCycles();
  //Vb1 = Vb1 + finalB1L2;
  for (i = 0; i < 200; i ++)
  {
    Vb2[i] = Vb2[i] + finalB1L2[i];
  } // Matrix Addition
  vend2 = getCycles();

  sigStart2 = getCycles();
  //Vb1 = sigmf(Vb1,[1 0]);
  for (i = 0; i < 200; i++)
  {
    Vb2[i] = 1/(1+exp(-Vb2[i]));
  } // Sigmoid
  sigEnd2 = getCycles();
  
  MMstart3 = getCycles();
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
  MMend3 = getCycles();
 
  maxStart = getCycles();
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
  maxEnd = getCycles();
  
  // Check for Zero
  //if(M == 10)
  //  M = 0;
  //end
  if (M == 10)
  {
    M = 0;
  } // Check for zero


  //output = M;
//  rec_2 = clock();
    return M;
}


static inline unsigned int getCycles ()
{
  unsigned int cycleCount;
  // Read CCNT register
  asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(cycleCount));  
  return cycleCount;
}

static inline void initCounters ()
{
  // Enable user access to performance counter
  asm volatile ("MCR p15, 0, %0, C9, C14, 0\t\n" :: "r"(1));
  // Reset all counters to zero
  int MCRP15ResetAll = 23; 
  asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(MCRP15ResetAll));  
  // Enable all counters:  
  asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));  
  // Disable counter interrupts
  asm volatile ("MCR p15, 0, %0, C9, C14, 2\t\n" :: "r"(0x8000000f));
  // Clear overflows:
  asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
}

