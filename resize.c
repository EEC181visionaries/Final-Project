//need global variables size_x size_y roi[WIDTH][HEIGHT] digit[WIDTH][HEIGHT] scaled[WIDTH][HEIGHT]

void resize(){
	int width = size_x; //width of digit
	int height = size_y;  //height of digit
	int tempx = 0, tempy = 0;
	for(int i = i_left; i < i_right; i++){
		for(j = 0; j < height; j++){
			digit[tempx][tempy] = roi[i][j];
			tempy++;
		}
		tempx++;
	}//fill empty matrix

	//pixels are pixel count increments scaled down by 28
	//start and ends are boxes that should fit 28 by 28, trimmed down edges by remainder/2 each side
	int x_pixels = width/28 - (width%28);
	int y_pixels = height/28 - (height%28); 
	int x_start = (width%28)/2;
	int x_end = width - (width%28)/2;
	int y_start = (height%28)/2;
	int y_end = height - (height%28)/2;
	double average;
	double square = x_pixels * y_pixels;
	int digitsx = 0, digitsy = 0;//These are the pixels of the scaled down digit		

	for(int i = x_start; i < x_end; i += x_pixels){
		for(int j = y_start; j < y_end; j += y_pixels){
			for(int k = 0; k < x_pixels; k++){
				for(int l = 0; l < y_pixels; l++){
					average += digit[k][l]
				}//Calculate the sum of the square
			}
			average = average / square; //Get the average density
			if(sum >= 0.5)
				scaled[digitsx][digitsy] = 1;
			else
				scaled[place][digitsx][digitsy] = 0;
			digitsy++;
		}
		digitsx++;
	}
}
