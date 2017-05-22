#include "CommonUtil.h"
#include <windows.h>
#include <stdio.h>
#include <assert.h>

bool BMP_SaveFile(const char* szFile, const void* pBmp, int width, int height, int bitCount)
{
	FILE* pFile = fopen(szFile, "wb");
	assert(pFile != NULL);

	int bmp_size = width*height*(bitCount / 8);

	BITMAPFILEHEADER bmpHeader;
	bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmp_size;    // BMP图像文件的大小
	bmpHeader.bfType = 0x4D42;    // 位图类别，根据不同的操作系统而不同，在Windows中，此字段的值总为‘BM’
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);            // BMP图像数据的偏移位置
	bmpHeader.bfReserved1 = 0;    // 总为0
	bmpHeader.bfReserved2 = 0;    // 总为0
	fwrite(&bmpHeader, sizeof(bmpHeader), 1, pFile);

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(bmiHeader);                // 本结构所占用字节数，即sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = width;                            // 位图宽度（单位：像素）
	bmiHeader.biHeight = height;                        // 位图高度（单位：像素）
	bmiHeader.biPlanes = 1;                                // 目标设备的级别，必须为1
	bmiHeader.biBitCount = bitCount;                    // 像素的位数（每个像素所需的位数，范围：1、4、8、24、32）
	bmiHeader.biCompression = 0;                        // 压缩类型（0：不压缩 1：BI_RLE8压缩类型 2：BI_RLE4压缩类型）
	bmiHeader.biSizeImage = bmp_size;                    // 位图大小（单位：字节）
	bmiHeader.biXPelsPerMeter = 0;                        // 水平分辨率(像素/米)
	bmiHeader.biYPelsPerMeter = 0;                        // 垂直分辨率(像素/米)
	bmiHeader.biClrUsed = 0;                            // 位图实际使用的彩色表中的颜色索引数
	bmiHeader.biClrImportant = 0;                        // 对图象显示有重要影响的颜色索引的数目

	// 【写位图信息头（BITMAPINFO的bmiHeader成员）】
	fwrite(&bmiHeader, sizeof(bmiHeader), 1, pFile);

	// 【写像素内容】
	fwrite(pBmp, bmp_size, 1, pFile);
	fclose(pFile);

	return true;
}