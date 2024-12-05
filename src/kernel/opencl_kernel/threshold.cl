__kernel void threshold(__global uchar *output, __global uchar *input, int width, int height, int itemWidth, int itemHeight, uchar thresh, uchar maxval) {
    int gx = get_global_id(0);
    int gy = get_global_id(1);
	
    int xStride = gx * itemWidth ;
    int yStride = gy * itemHeight;

    for (int i = 0; i < itemHeight; i++) {
        for (int j = 0; j < itemWidth; j++) {
            int inx = xStride + j;
            int iny = yStride + i;
            if (inx < width && iny < height) {
                int index = iny * width + inx;
				output[index] = input[index] > (thresh) ? maxval : 0;
            }
        }
    }
}