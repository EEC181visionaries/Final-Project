

//Declare global variables
double finalB1L1[200];
double finalB1L2[200];
double finalW1L1[200][784];
double finalW1L2[200][200];
double finalSoftmaxTheta[10][200];
int data[784];

void read_nn(void){
	
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
	
	
	
}
