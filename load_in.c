#include <stdio.h>




// used to determine what is white and what is black
#define BW_LEVEL	350

#define READ_IN_ADDR	0xFF200090
#define READ_OUT_ADDR 	0xFF200080
#define VGA_DATA1	0xFF200070
#define VGA_DATA2	0xFF200060
#define SDRAM_DATA1	0xFF200050
#define SDRAM_DATA2	0xFF200040
#define VGA_CLK_IN	0xFF200030
#define CAM_START	0xFF200020
#define SOURCE_SELECT	0xFF200010
#define CONTROLLING_CLK 0xFF200000
#define TEST 0xFF200100
#define DDR3_ADDR	0x00100000



int main(void){
	volatile int * (cam_start) = (int *) CAM_START;	// Output
	volatile int * (sdram_data1) = (int *) SDRAM_DATA1;	//Input
	volatile int * (sdram_data2) = (int *) SDRAM_DATA2;	//Input
	volatile int * (sdram_read) = (int *) READ_OUT_ADDR;	//Output
	volatile int * (read_good) = (int *) VGA_CLK_IN;	//Input
	volatile int * (vga_read) = (int *) READ_IN_ADDR; //Input
	volatile int * (vga_data1) = (int *) VGA_DATA1;	//Output
	volatile int * (vga_data2) = (int *) VGA_DATA2;	//Output
	volatile int *(clock_test) = (int *) TEST;
	volatile int * (ddr3) = (int *) DDR3_ADDR;
	volatile int * write_block;
	volatile int *(clock_select) = (int *) SOURCE_SELECT;
	volatile int *(clock_gen) = (int *) CONTROLLING_CLK;
	//int start = 0;
	write_block = DDR3_ADDR;
	int red = 0;
	int green = 0;
	int green_h = 0;
	int green_l = 0;
	int blue = 0;
	int gray = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	int L = 0;
	int snapshot = 0;
	int test[10000] = {0};
	*vga_data1 = 1;
	*clock_select = 0;
	int write_data = 0;
	int written = 0;
	int image[480][640];



	*cam_start = 1;

	printf("Press enter to start\n");

	// fill blocks with known value for debugging. Not needed in the end
	 for (j = 0; j < (307200); j = j+1)	// 640x480
	 {
		 *(write_block) = 0x7FFF7FFF;//*(sdram_data1);
				write_block++;
	 }
	 write_block = DDR3_ADDR;

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
			*cam_start = 0;	// pause camera
			*clock_select = 1;	// choose custom clock from hps
			*vga_data1 = 0;	// reset sdram
			*vga_data1 = 1;	
			*sdram_read = 1;	// set read request to high

			// clear out first horizontal row, it is all black
			for (k = 0; k < 643; k = k+1)
			{
				*clock_gen = 1;	// generate 4 clock cycles to move slower clock 1 cycle
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
					*clock_gen = 1;	// generate 4 clock cycles, checking each cycle
					if (!written)
					{
						if (*read_good)	// take in data from verilog to read block (not sure if needed)
						{
							write_data = *(sdram_data1);
							if (write_data < BW_LEVEL)	// write black or white
							{
								*write_block = 0;
								image[j][k] = 0;
							}
							else
							{
								*write_block = 1;
								image[j][k] = 1;
							}
							write_block++;
							written = 1;
						}
					}
					*clock_gen = 0;
				}
				written = 0;
			}
			// write blocks to separate each row to debug in memory
			*(write_block) = 0x7FFF7FFF;
			write_block++;
			*(write_block) = 0x7FFF7FFF;
			write_block++;
			*(write_block) = 0x7FFF7FFF;
			write_block++;
			*(write_block) = 0x7FFF7FFF;
			write_block++;
			*sdram_read = 0;
			*sdram_read = 1;
			}
			*sdram_read = 0;
			//*sdram_read = *vga_read;
			*cam_start = 1;
			*clock_select = 0;
			snapshot = 0;
			printf("Done\n");
		}

		// used to output memory values to text to copy onto text file
		
		for (j = 0; j < 480; j = j+1)
		{
			for (k = 0; k < 640; k=k+1)
			{
				printf("%d\t",image[j][k]);
			}
			printf("\n");
		}

		


	*(sdram_read) = 0;
	printf("Final Done\n");
	return 0;
}

