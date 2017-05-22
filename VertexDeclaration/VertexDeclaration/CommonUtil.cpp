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
	bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmp_size;    // BMPͼ���ļ��Ĵ�С
	bmpHeader.bfType = 0x4D42;    // λͼ��𣬸��ݲ�ͬ�Ĳ���ϵͳ����ͬ����Windows�У����ֶε�ֵ��Ϊ��BM��
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);            // BMPͼ�����ݵ�ƫ��λ��
	bmpHeader.bfReserved1 = 0;    // ��Ϊ0
	bmpHeader.bfReserved2 = 0;    // ��Ϊ0
	fwrite(&bmpHeader, sizeof(bmpHeader), 1, pFile);

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(bmiHeader);                // ���ṹ��ռ���ֽ�������sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = width;                            // λͼ��ȣ���λ�����أ�
	bmiHeader.biHeight = height;                        // λͼ�߶ȣ���λ�����أ�
	bmiHeader.biPlanes = 1;                                // Ŀ���豸�ļ��𣬱���Ϊ1
	bmiHeader.biBitCount = bitCount;                    // ���ص�λ����ÿ�����������λ������Χ��1��4��8��24��32��
	bmiHeader.biCompression = 0;                        // ѹ�����ͣ�0����ѹ�� 1��BI_RLE8ѹ������ 2��BI_RLE4ѹ�����ͣ�
	bmiHeader.biSizeImage = bmp_size;                    // λͼ��С����λ���ֽڣ�
	bmiHeader.biXPelsPerMeter = 0;                        // ˮƽ�ֱ���(����/��)
	bmiHeader.biYPelsPerMeter = 0;                        // ��ֱ�ֱ���(����/��)
	bmiHeader.biClrUsed = 0;                            // λͼʵ��ʹ�õĲ�ɫ���е���ɫ������
	bmiHeader.biClrImportant = 0;                        // ��ͼ����ʾ����ҪӰ�����ɫ��������Ŀ

	// ��дλͼ��Ϣͷ��BITMAPINFO��bmiHeader��Ա����
	fwrite(&bmiHeader, sizeof(bmiHeader), 1, pFile);

	// ��д�������ݡ�
	fwrite(pBmp, bmp_size, 1, pFile);
	fclose(pFile);

	return true;
}