#ifndef _IMAGEUTILS_H_
#define _IMAGEUTILS_H_

void flipChannels(unsigned char *pixels, int nPixels, const int channels){
	unsigned char tmp;
	do {
		tmp = pixels[2];
		pixels[2] = pixels[0];
		pixels[0] = tmp;
		pixels += channels;
	} while (--nPixels);
}

int icerp(int a, int b, int c, int d, int x){
	int p = (d - c) - (a - b);
	int q = (a - b) - p;
	int r = c - a;
	return (x * (x * (x * p + (q << 7)) + (r << 14)) + (b << 21)) >> 21;
}

#endif // _IMAGEUTILS_H_
