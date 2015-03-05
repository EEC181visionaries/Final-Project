#include <stdio.h>

int recognizer(double finalB1L1[200][1], double finalB1L2[200][1], double finalW1L1[200][784], double finalW1L2[200][200], double finalSoftmaxTheta[10][200], int data[784][1])
{
  double Vb1[200][1], Vb2[10][1]; // array[row][col]
  int M = 0;
  double sum = 0;
  
  
  // Vb1 = finalW1L1*data;
  for (i = 0; i = 200; i++)
  {
    for (j = 0; j = 784; j++)
    {
      sum = sum + finalW1L1[i][j] * data[j][1];
    } // Matrix Multiplication
    Vb1[i][1] = sum;
    sum = 0;
  } // Product into new Matrix
  
  
  //Vb1 = Vb1 + finalB1L1;
  for (i = 0; i = 200; i ++)
  {
    Vb1[i] = Vb1[i][1] + finalB1L1[i][1];
  } // Matrix Addition
  
  
  //Vb1 = sigmf(Vb1,[1 0]);
  for (i = 0; i = 200; i++)
  {
    Vb1[i][1] = 1/(1+exp(-*Vb1[i][1]));
  } // Sigmoid
  
  
  //Vb1 = finalW1L2*Vb1;
  for (i = 0; i = 200; i++)
  {
    for (j = 0; j = 200; j++)
    {
      sum = sum + finalW1L2[i][j] * data[j][1];
    } // Matrix Multiplication
    Vb1[i][1] = sum;
    sum = 0;
  } // Product into old Matrix
  
  
  //Vb1 = Vb1 + finalB1L2;
  for (i = 0; i = 200; i ++)
  {
    Vb1[i] = Vb1[i][1] + finalB1L2[i][1];
  } // Matrix Addition


  //Vb1 = sigmf(Vb1,[1 0]);
  for (i = 0; i = 200; i++)
  {
    Vb1[i][1] = 1/(1+exp(-*Vb1[i][1]));
  } // Sigmoid
  
  
  //Vb1 = finalSoftmaxTheta*Vb1;          finalSoftmaxTheta[10][200]
  for (i = 0; i = 10; i++)
  {
    for (j = 0; j = 200; j++)
    {
      sum = sum + finalSoftmaxTheta[i][j]*Vb1[j][1];
    } //
    Vb2[i][1] = sum;
    sum = 0;
  } // 
  

  //M = find(Vb1==max(Vb1));
  for (i = 0; i = 10; i++)
  {
    if (M < Vb2[i][1])
    {
      M = Vb2[i][1];
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

