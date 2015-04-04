// zzAlphaImageAlgorithm.c - original code translated from .m to .c
//                           not tested
// Author: Chan

#define WIDTH
#define HEIGHT
#define DDR3_ADDR

double image[WIDTH][HEIGHT];
int w, x, y, v, lt, lb, rt, rb;
int black_white[WIDTH][HEIGHT];
int roi[WIDTH][HEIGHT]
int digit[WIDTH][HEIGHT];
int size_x = 0, size_y = 0;

void gray_bw(){


	int i,j, data1,data2, red, green, blue, gray;


	write_block = DDR3_ADDR;

	for(i = 0; i < WIDTH; i++){
	
		for(j = 0; j < HEIGHT; j++){
			image[i][j] = 0;
			black_white[i][j] = 0;
			data1 = *(write_block);
			write_block++;
			data2 = *(write_block);
			write_block++;

			red = data1 & 1023;
			blue = data2 & 1023;
			green = ((data1 & 31744)>>5) | ((data2 &31744) >> 10);
			gray = (red *.21) + (.72 * green) + (.07*blue);
			image[i][j] = gray;
			if(gray > .7)
				black_white[i][j] = 1;
			else
				black_white[i][j] = 0;			
	
		}//for j
	}//for i

}//gray_bw()



void gray_bw2(){


	int i,j, data1,data2, red, green, blue, gray;


	FILE *fp = fopen("pic.csv", "r");
	

	for(i = 0; i < WIDTH; i++){
	
		for(j = 0; j < HEIGHT; j++){
			image[i][j] = 0;
			black_white[i][j] = 0;
			fscanf(fp,"%lf",&data1);
			fscanf(fp,"%lf",&data2)

			red = data1 & 1023;
			blue = data2 & 1023;
			green = ((data1 & 31744)>>5) | ((data2 &31744) >> 10);
			gray = (red *.21) + (.72 * green) + (.07*blue);
			image[i][j] = gray;
			if(gray > .7)
				black_white[i][j] = 1;
			else
				black_white[i][j] = 0;			
	
		}//for j
	}//for i

}//gray_bw()


void region(){
	int prev = 0, val = 0, i, j;
	
	i = HEIGHT/2;
	for(j = 0; j < WIDTH; j +=50){
		prev_val = val;
		if(black_white[i][j] == 1){
			x = j;
			val = val + 1;
		}
		if(prev_val == val && val)
			val = 0;
		if(val == 3){
			val = 0;
			prev_val = 0;
			break;
		}
	}

	for(j = HEIGHT; j > 0; j -=50){
		prev_val = val;
		if(black_white[i][j] == 1){
			y = j;
			val = val + 1;
		}
		if(prev_val == val && val)
			val = 0;
		if(val == 3){
			val = 0;
			prev_val = 0;
			break;
		}
	}
	
	j = (y-x)/2;
	for(i = 0; i < HEIGHT; i +=50){
		prev_val = val;
		if(black_white[i][j] == 1){
			v = i;
			val = val + 1;
		}
		if(prev_val == val && val)
			val = 0;
		if(val == 3){
			val = 0;
			prev_val = 0;
			break;
		}
	}
	for(i = HEIGHT; i > 0; i -=50){
		prev_val = val;
		if(black_white[i][j] == 1){
			w = i;
			val = val + 1;
		}
		if(prev_val == val && val)
			val = 0;
		if(val == 3){
			val = 0;
			prev_val = 0;
			break;
		}
	}

	i = HEIGHT/2;
	for(j = x; j < y; j+=5){
		prev_val = val;
		if(black_white[i][j] == 0){
			if(val == 0)
				x = j;
			val = val + 1;
		}
		if(prev_val == val && val)
			val = 0;
		if(val == 2){
			val = 0;
			prev_val = 0;
			break;
		}
	}
	for(j = y; j > x; j-=5){
		prev_val = val;
		if(black_white[i][j] == 0){
			if(val == 0)
				y = j;
			val = val + 1;
		}
		if(prev_val == val && val)
			val = 0;
		if(val == 2){
			val = 0;
			prev_val = 0;
			break;
		}
	}

	j = 2*(v-w)/5;
	for(i = v; i < v; i+=5){
		prev_val = val;
		if(black_white[i][j] == 0){
			if(val == 0)
				v = i;
			val = val + 1;
		}
		if(prev_val == val && val)
			val = 0;
		if(val == 2){
			val = 0;
			prev_val = 0;
			break;
		}
	}
	for(i = w; i > v; i-=5){
		prev_val = val;
		if(black_white[i][j] == 0){
			if(val == 0)
				w = i;
			val = val + 1;
		}
		if(prev_val == val && val)
			val = 0;
		if(val == 2){
			val = 0;
			prev_val = 0;
			break;
		}
	}
	
	x += 3;
	y -= 3;

	for(i = w; i < v; i++){
		if(lt == 0){
			if(black_white[i][w]==0)
				lt = i;
		}
		if(lb == 0){
			if(black_white[v-i][w]==0)
				lb = v-i;
		}
		if(rt == 0){
			if(black_white[i][y]==0)
				rt = i;
		}
		if(rb == 0){
			if(black_white[v-i][y]==0)
				rb = v-i;
		}
		if(lt && lb && rt && rb)
			break;
		
	}

	if(lt >= rt)
		v = lt;
	else
		v = rt;

	if(lb <= rb)
		w = lb;
	else
		w = rb;

	int tempx = 0, tempy = 0;
	for(i = w; i < v; i++){
		for(j = x; j < y; j++){
			roi[tempx][tempy] = black_white[i][j];
			tempy++;
		}
		tempx++;
	}
	size_x = tempx;
	size_y = tempy;


	FILE *fp = fopen("roi.csv", "w");

	for(i = 0; i < size_x; i++){
		for(j = 0; j < size_y; j++){
			fprintf(fp, "%d",roi[i][j]);
			if(j < size_y - 1)
				fprintf(fp, ",");
		}
		fprintf(fp, "\n");

	}
	fclose(fp);

}//region()




void separate(){

	int i = size_y/2;
	int j = 0;
	int hit = 0;
	int right_edge = 0;
	int bad = 0;
	int last = 0;
	int last_mid = 1;
	int first = 1;
	int mid;


	for(j = 0; j < size_x; j = j + 4){
	
		if((roi[i][j] == 1) || (j + 4 > size_x)){
			if(first == 1){
				first = 0;
				hit = 1;
			}
			else if(hit == 0){
				hit = 1;
				mid = ((j - right_edge)/2) + right_edge;
				if(roi[i][j] == 1){
					for(int mid_row = 0; mid_row < size_y; mid_row++){
						if(roi[mid_row][mid] == 1){
							bad = 1;
							break;
						}
					}
				}
				int tempx = 0, tempy = 0, break_flag = 0;
				if(bad != 1){
					for(int k = 0; k < size_y; k++){
						for(int l = last_mid; l < mid){
							digit[tempx][tempy] = roi[k][l];
`							tempy++;
						} 
						tempx++;
					} //Create digit matrix
					int digit_left = 1;
					int digit_right = mid-last_mid;
					for(int k = 0; k < mid-last_mid; k++){
						for(int l = 0; int < tempy; l++){
							if(digit[k][l] == 0){
								digit_left = k;
								break_flag = 1;
								break;
							}
						}
						if(break_flag)
							break;
					}
					for(int k = mid - last_mid; k > 0; k--){
						for(int l = 0; int < tempy; l++){
							if(digit[k][l] == 0){
								digit_right = k;
								break_flag = 1;
								break;
							}
						}
						if(break_flag)
							break;
					}					
					int digit_width = digit_right - digit_left;
					int right_add = (size_y-digit_width)/2;
					int left_add = right_add + 1;


				}		

			}

		}



	}

}

