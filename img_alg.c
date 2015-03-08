// Author: Chan

#define WIDTH
#define HEIGHT
#define DDR3_ADDR

int image[WIDTH][HEIGHT];
int w, x, y, v;
char black_white[WIDTH][HEIGHT];

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
	
	
}//region()


