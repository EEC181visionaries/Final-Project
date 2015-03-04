#include <stdio.h>

int recognizer(float finalB1L1[200][1], float finalB1L2[200][1], float finalW1L1[200][784], float finalW1L2[200][200], float finalSoftmaxTheta[10][200], int data)
{
  Vdat = data;
  Vb1 = finalW1L1*Vdat;
  Vb1 = Vb1 + finalB1L1;
  Vb1 = sigmf(Vb1,[1 0]);
  Vb1 = finalW1L2*Vb1;
  Vb1 = Vb1 + finalB1L2;



 Vb1 = sigmf(Vb1,[1 0]);
  Vb1 = finalSoftmaxTheta*Vb1;

  M = find(Vb1==max(Vb1));
  if(M == 10)
      M = 0;
  end

  output = M;
  
  
    printf("Hello, World!\n");

    return 0;
}

