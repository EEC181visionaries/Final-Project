
void separate(){


	//NOTE: EDGE in this case means edge of the number, not the border of the image. From now on edge refers to the touching point of a number and border is the position of where the final image is cropped.

	int e_left = 0, e_right, i_left, i_right; //markers for edge of number, border of image
	int edge_select = 0; //flag. 0 is look for left, 1 is look for right.
	int right_search = 0; //flag. 0 is do nothing, 1 is look for next number.
	int start = 0; //starting index position, changes with iteration
	double sum = 0; //sum of pixel column
	int place = 0; //place of digit we are examining, changes with iteration
	while(i < size_x){

	i_left = start; //set the left border, it never changes
	edge_select = 0;
	right_search = 0;
	for(int i = start; i < size_x; i++){
	
		sum = 0;
		for(int j = 0; j < size_y; j++){
			sum = sum + roi[i][j];	
		}//calculate the sum of the pixel column

		if(sum > 5 && !right_search){
			if(edge_select){
				e_right = i;
			}//if we are looking for the right edge
			else{
				e_left = i;
				edge_select = 1;
			}

		}//if we first hit an edge, set the left edge and begin advancing the right edge. Currently it is useless to set the right edge.
		if(sum > 5 && right_search){
			i_right = ((i - 1) - (e_right))/2;
			start = i_right;
			break;
		}//looking for the start of next digit. once found, set the center to be halfway between edges of two number.
		if(sum < 5){
			i_right = i;
			if(edge_select){
				right_search = 1;
			}//if we had hit a number have since exited, begin looking for start of next number
		}//if we hit a black column, set the new right.

	}
	if(i == size_x)
		i_right = ((size_x - 1) - (e_right))/2;

	int width = i_left - i_right; //width of digit
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
				digits[place][digitsx][digitsy] = 1;
			else
				digits[place][digitsx][digitsy] = 0;
			digitsy++;
		}
		digitsx++;
	}
	place++;//Increase to next digit.
	}

}
