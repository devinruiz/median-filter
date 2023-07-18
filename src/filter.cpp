#include "filter.h"
#include <iostream>
using namespace std;

static PIXEL find_med(PIXEL WB[3][3]) {
	short int values[9];
#pragma HLS array_partition variable=values complete dim=0

	ap_uint<4> k = 0;
	ap_uint<2> i;
	ap_uint<2> j;

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			values[k++] = (short int)WB[i][j];
		}
	}

	for (int i = 0; i < 8; i++) {
#pragma HLS unroll
	    for (int j = 0; j < 8 - i; j++) {
#pragma HLS unroll
	        if (values[j] > values[j + 1]) {
	            PIXEL temp = values[j];
	            values[j] = values[j + 1];
	            values[j + 1] = temp;
	        }
	    }
	}

	return (PIXEL) values[4];
}

void filter(hls::stream<trans_pkt>& src, hls::stream<trans_pkt>& dst, int rows, int cols)
{
#pragma HLS INTERFACE axis port=src
#pragma HLS INTERFACE axis port=dst
#pragma HLS INTERFACE s_axilite port=rows  bundle=CTRL
#pragma HLS INTERFACE s_axilite port=cols  bundle=CTRL
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL

	trans_pkt data_p;

	PIXEL median;

	PIXEL LineBuffer[3][WIDTH];
#pragma HLS ARRAY_PARTITION variable=LineBuffer complete dim=1

	PIXEL WindowBuffer[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
#pragma HLS ARRAY_PARTITION variable=WindowBuffer complete dim=0

	ap_uint<13> row, col;
	ap_uint<2> lb_r_i;
	ap_uint<2> top, mid, btm;//line buffer row index

	for(col = 0; col < cols; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS pipeline
		LineBuffer[0][col] = 0;
		data_p = src.read();
		LineBuffer[1][col] = (PIXEL) data_p.data;
	}

	lb_r_i = 2;
	for(row = 1; row < rows + 1; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		if(lb_r_i == 2)
		{
			top = 0; mid = 1; btm = 2;
		}
		else if(lb_r_i == 0)
		{
			top = 1; mid = 2; btm = 0;
		}
		else if(lb_r_i == 1)
		{
			top = 2; mid = 0; btm = 1;
		}

		WindowBuffer[top][0] = WindowBuffer[top][1] = 0;
		WindowBuffer[mid][0] = WindowBuffer[mid][1] = 0;
		WindowBuffer[btm][0] = WindowBuffer[btm][1] = 0;

		for(col = 0; col < cols; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
#pragma HLS pipeline
			if(row < rows)
			{
				data_p = src.read();
				LineBuffer[btm][col] = (PIXEL) data_p.data;
			}
			else
				LineBuffer[btm][col] = 0;

			WindowBuffer[0][2] = LineBuffer[top][col];
			WindowBuffer[1][2] = LineBuffer[mid][col];
			WindowBuffer[2][2] = LineBuffer[btm][col];
			median = find_med(WindowBuffer);
			WindowBuffer[0][0] = WindowBuffer[0][1];
			WindowBuffer[1][0] = WindowBuffer[1][1];
			WindowBuffer[2][0] = WindowBuffer[2][1];
			WindowBuffer[0][1] = WindowBuffer[0][2];
			WindowBuffer[1][1] = WindowBuffer[1][2];
			WindowBuffer[2][1] = WindowBuffer[2][2];

			if ((row == rows ) && (col == cols - 1))
				data_p.last = 1;
			else
				data_p.last= 0;
			data_p.data = median;
			dst.write(data_p);
		}
		lb_r_i++;
		if(lb_r_i == 3) lb_r_i = 0;
	}
}
