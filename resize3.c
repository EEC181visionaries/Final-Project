void resize(int height, int width, int** digit){
	FILE *fp;
	int digit_final[28][28];
	int i,j;
	int x_pixels = (width + 27)/28;
	int y_pixels = (height + 27)/28;
	int k, l;
	double average;
	double square = x_pixels * y_pixels;
	int digit_x = 0, digit_y = 0;//These are the pixels of the scaled down digit		

	printf("%d by %d and %d by %d\n", height, width, y_pixels, x_pixels);
	for(i = 0; i < 28; i++){
		for(j = 0; j < 28; j++)
			digit_final[i][j] = 0;
	}
/*
	for(i = 0; i <= height - (height%y_pixels); i++)
	{
		for(j = 0; j <= width - (width%x_pixels); j++)
			printf("(%d,%d) %d\n",i,j, digit[i][j]);
	}
*/
	for(i = 0; i < (height - (height%28)); i = i + y_pixels){
		digit_x = 0;
		for(j = 0; j < (width - (width%28)); j = j + x_pixels){
			average = 0;

			for(k = 0; k < y_pixels && ((i+k) < height); k++){
				for(l = 0; l < x_pixels && ((j+l) < width); l++){
					average += digit[i+k][j+l];
//					printf("accessing (%d, %d), j %d\n", i+k, j+l, j);
					
				}
				//printf("Exit for(l...)\n");
			}
			//printf("Exit for(k...)\n");
			average = average / square;

			if(average >= 0.5){
				digit_final[digit_y][digit_x] = 1;
				//printf("if (white)\n");
			}
			else{
				digit_final[digit_y][digit_x] = 0;
				//printf("else //black\n");
			}
			digit_x++;
//			printf("digit j %d\n", j);
//			printf("%d by %d and %d by %d\n", y_pixels, x_pixels, (height-height%y_pixels), (width - width%x_pixels));
		}
		//printf("Exit for(j...)\n");
		digit_y++;
	}

/*
  fp = fopen("resize.csv", "w");

  if (fp == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  for(i = 0; i < 28; i++)
  {
    for(j = 0; j < 28; j++)
    {
      fprintf(fp, "%d",digit_final[i][j]);
      if ((j + 1) != 28)
      {
        fprintf(fp, ", ");
      }//if 
      else
        fprintf(fp, "\n");
    }
  }
  fclose(fp);
*/
}
