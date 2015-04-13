// single_digit_reader_v2.c - Reads in a photo and prints out data

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "finalB1L1.c"
#include "finalB1L2.c"
#include "finalSoftmaxTheta.c"
#include "finalW1L1.c"
#include "finalW1L2.c"

// Definitions
#define WIDTH 640
#define HEIGHT 480
#define BW_LEVEL  130
#define BLACK 0
#define WHITE 1

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

// List of functions
void region(void);
void region2(int cols,int mat[][cols]);
int resize(int height, int width, int** digit);
int resize2(int height, int width, int** img);
int recognizer(int data[784]);
void digit_separate2(int num_row, int num_col, int roi[num_row][num_col]);

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

	for(i = 0; i < 28; i++){
		for(j = 0; j < 28; j++)
			digit_final[i][j] = 0;
	}
	
	for(i = 0; i < (height - (height%28)); i = i + y_pixels){
		digit_x = 0;
		for(j = 0; j < (width - (width%28)); j = j + x_pixels){
			average = 0;
			for(k = 0; k < y_pixels && ((i+k) < height); k++){
				for(l = 0; l < x_pixels && ((j+l) < width); l++){
					average += digit[i+k][j+l];
				}
			}
			average = average / square;

			if(average >= 0.5){
				digit_final[digit_y][digit_x] = 1;
			}
			else{
				digit_final[digit_y][digit_x] = 0;
			}
			digit_x++;
}

		digit_y++;
	}
  k = 0;
  for(i = 0; i < 28; i++)
  {
    for(j = 0; j < 28; j++)
    {
      digit_vector[k] = digit_final[j][i];
      k++;
    }
  }

  l = recognizer(digit_vector);
  return l;
}

int resize2(int height, int width, int** img){
	int scaled_img[28][28];
	int vector[784];

	//
	//col and row specify squares. p examines pixels. s is used for the scaled img. v is vector index.
	//	

	int col,row, p_col, p_row, s_col = 0, s_row = 0, v_index;	

	//Scales divide by 28 rounded up.	
	int col_scale = (width + 27)/28;	
	int row_scale = (height + 27)/28;

	//avg is used to calculate the average white density of a square, given by size square.
	double avg;
	double square = col_scale * row_scale;


	//
	// Initialize the image to be all black
	//
	for(int i = 0; i < 28; i++){

		for(int j = 0; j < 28; j++)
			scaled_img[i][j] = BLACK;

	}

	//
	//Examine the image, magnifying rectangle by rectangle down to 28 = (size - size%28)/scale
	//where size is the row or column and the scale is the calculated scale rounded up.
	//	
	
	for(row = 0; row < (height - (height%28)); row += row_scale){

		s_col = 0;

		for(col = 0; col < (width - (width%28)); col += col_scale){

			avg = 0;
			
			//
			//Calculate the average of a square given starting coordinates
			//

			for(p_row = 0; p_row < row_scale && ((row+p_row) < height); p_row++){
				for(p_col = 0; p_col < col_scale && (col+p_col) < width); p_col++){
					avg += img[row+p_row][col+p_col];
				}
			}

			avg = avg / square;

			if(average >= 0.5){
				scaled_img[s_row][s_col] = WHITE;
			}
			else{
				scaled_img[s_row][s_col] = BLACK;
			}
			s_col++;
	}

		s_row++;
	}

	//
	//Convert the scaled image to a vector for recognizer to use
	//

  	v_index = 0;
  	for(s_col = 0; s_col < 28; i++){
    		for(s_row = 0; s_row < 28; j++){
      		vector[index] = scaled_img[s_row][s_col];
      		v_index++;
    	}
  }

  return recognizer(vector);
}


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

void digit_separate2(int num_row, int num_col, int roi[num_row][num_col])
{
	int r = 0;
	int mid_row = num_row/2;	
	int c = 0;
	int i = 0;
	int j = 0;
	int ***digit;
	int last_col = 0;
	int right_edge = 0;
	int bad = 0;
	int col_checker = 0;
	int first = 1;
	int digit_num = 0;
	int row = 0;
	int col = 0;

	int last_pixel = 0;
	int pixel = 0;
	int mid_col = 0;
	int last_mid = 0;
	int mid = 0;

	int digit_size[5] = {0};
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
							for (i = 0; i < digit_size[digit_num]; i++)
							{
								for (j = 0; j < digit_size[digit_num]; j++)
								{
									digit[digit_num][i][j] = BLACK;
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
							
							
							
							// print digit if checking
							printf("digit number: %d\n", digit_num);


							for (i = 0; i < digit_size[digit_num]; i++)
							{
								for (j = 0; j < digit_size[digit_num]; j++)
								{
									printf("%d\t", digit[digit_num][i][j]);
								}
								printf("\n");
							}
							printf("\n\n\n");
							
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



