// color2gray3.c
// takes rgb image and returns gray image

//#include <stdio.h>
//#include <string.h>

#define START_CAM_ADDRESS 0xFF200040 // 0x00000040
#define iD1 0xFF200010 // 0x00000010
#define iD2 0xFF200000 // 0x00000000
#define oD1 0xFF200030 // 0x00000030
#define oD2 0xFF200020 // 0x00000020

int main(void)
{
  volatile int * start_cam = (int*) START_CAM_ADDRESS;
  volatile int * iRead_Data1 = (int *) iD1; // input from read_data1 from Sdram_Control
  volatile int * iRead_Data2 = (int *) iD2; // input from read_data2 from Sdram_Control
  volatile int * oRead_Data1 = (int *) oD1; // output to read_data1 into VGA_Controller
  volatile int * oRead_Data2 = (int *) oD2; // output to read_data2 into VGA_Controller
  
  int red = 0, green, green_h, green_l, blue, gray;
  
  // Camera on
  *start_cam = 1;
  
  while(1)
  {
    //*(oRead_Data1) = *(iRead_Data1);
    //*(oRead_Data2) = *(iRead_Data2);


    
    // Separate inputs into rgb
    red = *(iRead_Data2) & 1023; // red = iRead_Data2[9:0] ?1023?
    green = (*(iRead_Data1) & 31744)/32 + (*(iRead_Data2) & 31744)/1024; // green = {iRead_Data1[14:10], iRead_Data2[14:10]}
    blue = *(iRead_Data1) & 1023;// blue = iRead_Data1[9:0]
  
    // Do the multiplication
    gray = .21 * red + .72 * green + .07 * blue;


    // Store into the output
    *(oRead_Data1) = ((gray & 992) * 32) + gray;  // {green[9:5], blue[9:0]}
    *(oRead_Data2) = ((gray & 31) * 1024) + gray; // {green[4:0], red[9:0]}
  
  }
  return 0;
}
