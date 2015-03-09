#include <stdio.h>


//Declare global variables
double finalB1L1[200];
double finalB1L2[200];
double finalW1L1[200][784];
double finalW1L2[200][200];
double finalSoftmaxTheta[10][200];
int data[784];

void reader(void){
	
	FILE* fp;
	int i,j;
	
	fp = fopen("finalW1L1.csv",r);
	for(i = 0; i < 200; i++){
		for(j = 0; j < 784; j++){
			fscanf(fp, "%d", &(finalW1L1[i][j]))
		}
	}
	fclose(fp);
	
	fp = fopen("finalW1L2.csv",r);
	for(i = 0; i < 200; i++){
		for(j = 0; j < 200; j++){
			fscanf(fp, "%d", &(finalW1L2[i][j]))
		}
	}
	fclose(fp);
	
	fp = fopen("finalSoftmaxTheta.csv",r);
	for(i = 0; i < 10; i++){
		for(j = 0; j < 200; j++){
			fscanf(fp, "%d", &(finalSoftmaxTheta[i][j]))
		}
	}
	fclose(fp);
	
	fp = fopen("finalB1L1.csv",r);
	for(i = 0; i < 10; i++){
			fscanf(fp, "%d", &(finalB1L1[i]))
	}
	fclose(fp);
	fp = fopen("finalB1L2.csv",r);
	for(i = 0; i < 10; i++){
			fscanf(fp, "%d", &(finalB1L2[i]))
	}
	fclose(fp);
	
	fp = fopen("data.csv",r);
	for(i = 0; i < 784; i++){
			fscanf(fp, "%d", &(data[i]))
	}
	fclose(fp);
	
}
int recognizer(void)
{
  double Vb1[200], Vb2[10]; // array[row][col]
  int M = 0;
  double sum = 0;
  
  
  // Vb1 = finalW1L1*data;
  for (i = 0; i < 200; i++)
  {
    for (j = 0; j < 784; j++)
    {
      sum = sum + finalW1L1[i][j] * data[j][1];
    } // Matrix Multiplication
    Vb1[i][1] = sum;
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
    Vb1[i] = 1/(1+exp(-*Vb1[i]));
  } // Sigmoid
  
  
  //Vb1 = finalW1L2*Vb1;
  for (i = 0; i < 200; i++)
  {
    for (j = 0; j < 200; j++)
    {
      sum = sum + finalW1L2[i][j] * data[j];
    } // Matrix Multiplication
    Vb1[i] = sum;
    sum = 0;
  } // Product into old Matrix
  
  
  //Vb1 = Vb1 + finalB1L2;
  for (i = 0; i < 200; i ++)
  {
    Vb1[i] = Vb1[i] + finalB1L2[i];
  } // Matrix Addition


  //Vb1 = sigmf(Vb1,[1 0]);
  for (i = 0; i < 200; i++)
  {
    Vb1[i] = 1/(1+exp(-*Vb1[i]));
  } // Sigmoid
  
  
  //Vb1 = finalSoftmaxTheta*Vb1;          finalSoftmaxTheta[10][200]
  for (i = 0; i < 10; i++)
  {
    for (j = 0; j < 200; j++)
    {
      sum = sum + finalSoftmaxTheta[i][j]*Vb1[j];
    } //
    Vb2[i] = sum;
    sum = 0;
  } // 
  

  //M = find(Vb1==max(Vb1));
  for (i = 0; i < 10; i++)
  {
    if (M < Vb2[i])
    {
      M = Vb2[i];
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


int main(){
  read();
  int guess = recognizer();
  
  printf("Guessed %d\n", guess);
  return 0;
}
