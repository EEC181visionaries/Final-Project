#include <stdio.h>






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
	volatile int * (vga_ctrl_clk) = (int *) VGA_CLK_IN;	//Input
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
	int snapshot = 0;
	int test[10000] = {0};
	
	*clock_select = 0;
/*
	printf("Press enter to start\n");
	fflush(stdin);
	getchar();
*/
	*cam_start = 1;

	printf("starting...\n");
	/*
	for (i = 0; i < 9999; i++)
	{
	}
	*cam_start = 0;
	for (i = 0; i < 640*480; i++)
	{
		red = *(iRead_Data2) & 1023; // red = iRead_Data2[9:0] ?1023?
		green = (*(iRead_Data1) & 31744)/32 + (*(iRead_Data2) & 31744)/1024; // green = {iRead_Data1[14:10], iRead_Data2[14:10]}
		blue = *(iRead_Data1) & 1023;// blue = iRead_Data1[9:0]
		gray = .21 * red + .72 * green + .07 * blue;
	}
	*/

	while(1)
	{
		*(sdram_read) = *(vga_read);
		if (i == 10000)
			snapshot = 1;
		if (snapshot)
		{
			*clock_select = 1;
			while (*vga_read)
			{
				*clock_gen = 1;
				*clock_gen = 0;
			}
			while(!(*vga_read))
			{
				*clock_gen = 1;
				*clock_gen = 0;
			}
			while (*vga_read)	// this only generates 48 writes for 1 row, therefore only 24 pixels of data per row
			{
				*clock_gen = 1;
				*(write_block) = *(sdram_data1);
				write_block++;
				*(write_block) = *(sdram_data2);
				write_block++;
				*clock_gen = 0;
			}
			*clock_select = 0;
			snapshot = 0;
			printf("Done\n");
		}
		i++;
	}

/*
	while(!(*vga_read)){}
	while(*vga_read){
		*(sdram_read) = 1;
		*(write_block) = *(sdram_data1);
		*(vga_data1) = *(sdram_data1);
		write_block++;
		*(write_block) = *(sdram_data2);
		*(vga_data2) = *(sdram_data2);
		write_block++;
		while((*(vga_ctrl_clk))){}
		*(sdram_read) = *(vga_read);
		while(!(*(vga_ctrl_clk))){}			
	}//while
*/
//	*(sdram_read) = *(vga_read);
//	write_block = DDR3_ADDR;
//	if (write_block == DDR3_ADDR+1000)
//		break;

	*(sdram_read) = 0;
	printf("Done\n");
	return 0;
}

