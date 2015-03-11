// read_nn.c - Reads in the files that were
// included in NN.mat. Files are used for
// the image processing neural network.

//#include<stdio.h>

//Declare global variables
double finalB1L1[200];
double finalB1L2[200];
double finalW1L1[200][784];
double finalW1L2[200][200];
double finalSoftmaxTheta[10][200];
int data[784];

/*
void read_nn(void);
// Test if it works...
int main(void)
{
  int i; 
  read_nn();
  for (i = 0; i < 10; i ++)
  {
    printf("%le\n", finalB1L1[i]);
  }
  return 0;
}
*/

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
      fscanf(fp, "%le", &(finalW1L1[i][j]));
    }
  }
  fclose(fp);

  // Open finalW1L2.csv	
  fp = fopen("finalW1L2.csv","r");
  for(i = 0; i < 200; i++)
  {
    for(j = 0; j < 200; j++)
    {
      fscanf(fp, "%le", &(finalW1L2[i][j]));
    }
  }
  fclose(fp);

  // Open finalSoftmaxTheta.csv	
  fp = fopen("finalSoftmaxTheta.csv","r");
  for(i = 0; i < 10; i++)
  {
    for(j = 0; j < 200; j++)
    {
      fscanf(fp, "%le", &(finalSoftmaxTheta[i][j]));
    }
  }
  fclose(fp);

  // Open finalB1L1.csv
  fp = fopen("finalB1L1.csv","r");
  for(i = 0; i < 200; i++)
  {
    fscanf(fp, "%le", &(finalB1L1[i]));
  }
  fclose(fp);
	
  // Open finalB1L2.csv
  fp = fopen("finalB1L2.csv","r");
  for(i = 0; i < 200; i++)
  {
    fscanf(fp, "%le", &(finalB1L2[i]));
  }
  fclose(fp);

  // Open data.csv	
  fp = fopen("data.csv","r");
  for(i = 0; i < 784; i++)
  {
    fscanf(fp, "%d", &(data[i]));
  }
  fclose(fp);	

}// read_nn(void)
