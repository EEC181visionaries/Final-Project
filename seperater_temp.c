// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

//#include "finalB1L1.c"
//#include "finalB1L2.c"
//#include "finalSoftmaxTheta.c"
//#include "finalW1L1.c"
//#include "finalW1L2.c"
//#include "image_2.c"

// Definitions
#define WIDTH 640
#define HEIGHT 480
#define BW_LEVEL  130
#define MAX_DIGITS 5
#define WHITE 1
#define BLACK 0

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
//int digit[HEIGHT][WIDTH];
int data[784];
int size_x = 0, size_y = 0;

// List of functions
void region(void);
void region2(int cols,int rows,int mat[rows][cols]);
void resize(void);
int recognizer(void);
void digit_separate();

// Timing Variables
// Resources:
//   http://www.tutorialspoint.com/c_standard_library/c_function_clock.htm
clock_t regionStart, regionEnd, regionClocks, resizeStart, resizeEnd, resizeClocks;

/*
	FOR TIMING
	==================
	regionStart = clock()
	regionEnd = clock()
	regionClocks = regionEnd - regionStart // Returns clock ticks
	regionClocks = (double)(regionEnd - regionStart)/CLOCKS_PER_SEC; // Returns time in sec?
	resizeStart = clock()
	resizeEnd = clock()
	resizeClocks = resizeEnd - resizeStart
	
	Do this for every function and main
	
	

*/



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
  int bright_levels[48][64];
  int bright_level_row_counter = 0;
  int bright_level_col_counter = 0;
  int bi = 0;
  int bj = 0;
  int bright_write=0;


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
			  if (k%10 == 0 && j%10 == 0)
				  bright_write = 1;
			  if (bright_write == 1)
			  {
				  bright_levels[j/10][k/10] = write_data;
				  bright_write = 0;
			  }
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
/*
	for (i = 0; i< 48; i++)
	{
		for (j = 0; j < 64; j++)
		{
			printf("%d ", bright_levels[i][j]);
		}
		printf("\n");
	}
	printf("\n");
*/	

    *(sdram_read) = 0;
	//region2(WIDTH,HEIGHT,black_white);
    region();
	//size_x = 155;
	//size_y = 160;
	printf("region found\n");
	for (i= 0; i < size_y; i++)
	{
		for (j = 0; j < size_x; j++)
		{
			printf("%d\t",roi[i][j]);
		}
		printf("\n");
	}
	digit_separate();
	printf("separate done\n");
	/*
    resize();
    M = recognizer();
    printf("Guessed %d\n\n",M);
	*/

	
	

	/*
		for (i = 0; i < size_y; i++)
		{
			for (j = 0; j < size_x; j++)
			{
				printf("%d\t", roi[i][j]);
			}
			printf("\n");
		}
		*/
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
    tempx = 0;
    for(j = x; j < y; j++)
    {
      roi[tempy][tempx] = black_white[i][j];
      tempx++;
      //printf("%d\n",roi[tempx][tempy]); 
    }
    tempy++;
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
/*
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
*/


void digit_separate()
{
	int r = size_y/2;
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
	int digit_width = 0;
	int digit_top = 0;
	int digit_bot = 0;
	int digit_height = 0;
	int padding = 0;
	int horz_padding = 0;
	int last = 0;
	int ***digit;
	int digit_size[MAX_DIGITS] = {0}; // base and height of the digit image before resize
	int digit_num = 0;
	r = size_y/2;
	printf("size_x = %d\n", size_x);
	printf("size_y = %d\n", size_y);
	digit = (int ***) malloc(MAX_DIGITS*sizeof(int **));
	/*
		digit = (int **)malloc(size_y * sizeof(int *));
		for (r = 0; r < size_y; r++)
			digit[r] = (int *)malloc(size_x * sizeof(int));
	*/

	for (c = 0; c < size_x; c = c+4)
	{
		if ( (roi[r][c] == 1) || (c+4 >= size_x) )
		{
			printf("hit white\n");
			if (first)
			{
				first = 0;
				hit = 1;
			}
			else
			{
				if(hit == 0)
				{
					hit = 1;
					mid = c + right_edge/2;
					if (c +4 < size_x)
					{
						if( roi[r][c] == 1)
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
					}
					if (bad != 1)
					{
						printf("if (bad != 1\n");

						/*
						digit = (int **)malloc(size_y * sizeof(int *));
						for (r = 0; r < size_y; r++)
							digit[r] = (int *)malloc(size_x * sizeof(int));
						*/

						for ( i = 5; i < size_y-5; i++) // rows
						{
							for (j = last_mid; j < mid; j++) // cols
							{
								if (roi[i][j] == 1)
								{
									digit_top = i;
									i = size_y;
									j = size_x;
									break;
								}
							}
						}

						for ( i = size_y-6; i >= 5; i--) // rows
						{
							for (j = last_mid; j < mid; j++) // cols
							{
								if (roi[i][j] == 1)
								{
									digit_bot = i;
									i = -1;
									j = size_x;
									break;
								}
							}
						}

						printf("digit_bot = %d\n",digit_bot);
						printf("digit_top = %d\n",digit_top);

						digit_height = digit_bot - digit_top;
						printf("digit_height = %d\n",digit_height);
						padding = digit_height/5;
						printf("padding = %d\n", padding);


						if (digit_num < MAX_DIGITS)
							digit_size[digit_num] = digit_height + padding + padding;
						

						digit_left = last_mid;
						digit_right = mid;
						for (j = last_mid; j < mid; j++) // cols 
						{
							for (i = 5; i < size_y-5; i++) // rows, check each row in cloumn before next column
							{
								if (roi[i][j] == 1)
								{
									digit_left = j;
									j = mid;
									i = size_y;
									break;
								}
							}
						}

						for (j = mid-1; j >= last_mid; j--)
						{
							for (i = 5; i < size_y-5; i++)
							{
								if (roi[i][j] == 1)
								{
									digit_right = i;
									j = last_mid-1;
									i = size_y;
									break;
								}
							}
						}

						printf("digit_right = %d\n",digit_right);
						printf("digit_left = %d\n",digit_left);

						digit_width = digit_right - digit_left;
						horz_padding = (digit_size[digit_num] - digit_width)/2;
						printf("digit_width = %d\n",digit_width);


						printf("digit_num = %d\n", digit_num);
						printf("digit_size[digit_num] = %d\n",digit_size[digit_num]);
						// allocate space for digit, add padding to it
						if (digit_num < MAX_DIGITS)
						{
							printf("digit_num < MAX_DIGITS\n");
							
							digit[digit_num] = (int **) malloc(digit_size[digit_num] * sizeof(int*));
							for (i = 0; i < digit_size[digit_num]; i++)
								digit[digit_num][i] = (int *)malloc(digit_size[digit_num] * sizeof(int));

							printf("done malloc\n");

							for (i = 0; i < digit_size[digit_num]; i++) // write black to every pixel
							{
								for (j = 0; j < digit_size[digit_num]; j++) // write to every pixel in the row
								{
									digit[digit_num][i][j] = 0;
								}
							}

							printf("done creating black box\n");
							
							// write digit to middle of black box
							for (i = padding; i < padding + digit_height; i++)
							{
								for (j = horz_padding; j < horz_padding + digit_width; j++)
								{
									digit[digit_num][i][j] = roi[digit_top + i - padding][digit_left + j - horz_padding];
								}
							}

							printf("done placing digit\n");
							

							printf("number: %d\n", digit_num);

/*
							for (i = 0; i < digit_size[digit_num]; i++)
							{
								for (j = 0; j < digit_size[digit_num]; j++)
								{
									printf("%d\t", digit[digit_num][i][j]);
								}
								printf("\n");
							}
*/


							printf("\n\n\n");

							digit_num++;
						} // if(digit_num < MAX_DIGITS)

						

						// NOTES:
						// image = 28mm
						// digit size = 20mm
						// black space bot & top = 4mm each

						last_mid = mid;

					} // if (bad != 1)



				} // if ( hit == 0)
			}
			last = 1;
		} // if ( (roi[r][c] == 1) ||...
		else 
		{
			printf("hit black\n");
			if (last == 1)
			{
				right_edge = c;
			}
			last = 0;
			hit = 0;
			//printf("hit = 0;\n");
		}

		bad = 0;

	} // for(c = 0; i < size_x; c = c+4)


/*
	digit[digit_num] = (int **) malloc(digit_size[digit_num] * sizeof(int*));
	for (i = 0; i < digit_size[digit_num]; i++)
	{
		for (j = 0; j < digit_size[digit_num]; j++)
			free(digit[digit_num][i][j]);
	}
	for (i = 0; i < digit_size[digit_num]; i++)
	{
		free(digit[digit_num][i]);
	}
	for (i = 0; i < digit_num; i++)
	{
		free(digit[digit_num]);
	}
*/


} // digit_separate

void digit_separate2(int num_row, int num_col, int roi[num_row][num_col])
{
	int r = 0;
	int mid_row = num_row/2;	
	int c = 0;
	int digit***;
	int last_col = 0;
	int right_edge = 0;
	int bad = 0;
	int col_checker = 0;
	int first = 1;
	int digit_num = 0;
	int row = 0;
	int col = 0;
	int digit_top = 0;
	int digit_bot = 0;
	int digit_left = 0;
	int digit_right = 0;
	int digit_height = 0;
	int digit_width = 0;
	int padding = 0;
	int horz_padding = 0;

	digit = (int ***) malloc(MAX_DIGITS*sizeof(int **));
	
	for (c = 0; c < num_col; c = c+4)
	{
		if ( (roi[mid_row][c] == WHITE) || (c+4 >= num_col) ) // hit white or end of roi
		{
			if (first) // skip the first hit of white
			{
				first = 0;
				pixel = WHITE;
			}
			else
			{
				if(last_pixel == BLACK) // check if start of digit found
				{
					mid_col = (c - right_edge)/2 + right_edge;
					if (roi[mid_row][c] == WHITE) // skip this section if we've hit the end of the ROI
					{
						// start at 4 to size_y-4 to give some buffer for tilted ROI's
						for (col_checker = 4; col_checker < num_col-4; col_checker++)
						{
							if ( roi[col_checker][mid_col] == WHITE)
							{
								bad = 1;
								break;
							}
						}
					} // if (roi[r][c] == WHITE)
					if (!bad)
					{
						if (digit_num < MAX_DIGITS)
						{
							// check for top
							for (row = 4; row < num_row-4; row++)
							{
								for (col = last_mid; col < mid; col++)
								{
									if (roi[row][col] == WHITE)
									{
										digit_top = row;
										// break out of for loops
										col = mid;
										row = num_row;
									}
								}
							}
							// check for bot
							for (row = num_row-4 - 1; row >= 4; row--)
							{
								for (col = last_mid; col <  mid; col++)
								{
									digit_bot = row;
									
									col = mid;
									row = -1;
								}
							}
							digit_height = digit_bot - digit_top;
							padding = digit_height/5;
							digit_size[digit_num] = digit_height + padding + padding;


							// check for left
							for (col = last_mid; col < mid; col++)
							{
								for (row = 4; row < num_row-4; row++)
								{
									if (roi[row][col] == WHITE)
									{
										digit_left = col;
										row = num_row;
										col = mid;
									}
								}
							}
							// check for right
							for (col = mid-1; col >= last_mid; col--)
							{
								for (row = 4; row < num_row-4; row++)
								{
									if (roi[row][col] == WHITE)
									{
										digit_right = col;
										row = num_row;
										col = last_mid-1;
									}
								}
							}
							digit_width = digit_right - digit_left;
							horz_padding = (digit_size[digit_num] - digit_width)/2;

							// allocate space for digits
							digit[digit_num] = (int **) malloc(digit_size[digit_num] * sizeof(int*));
							for (i = 0; i < digit_size[digit_num]; i++)
								digit[digit_num][i] = (int *)malloc(digit_size[digit_num] * sizeof(int));

							// create black box
							
							
							// place digit in center of black box
							
							
							
							// print digit if checking
							
							
							// resize digit
							
							// pass digit through NN and receive integer
							// integer = function()
							
							// print single digit







							
						} // if (digit_num < MAX_DIGITS
						
						digit_num++;
					} // if (!bad)
				}
			} // else (first)
			last_pixel = WHITE;
		}
		else	// hit black
		{
			if (last_pixel == WHITE)
			{
				right_edge = c;
			}
			last_pixel = BLACK;
			pixel = BLACK;
		}
		bad = 0;













	}
} // digit_separate2
