#pragma once
#include <gdiplus.h>
using namespace Gdiplus;

struct SGrayBuff  //一块灰度图数据区的描述，便于参数传递
{
	unsigned char *  pData;         //灰度数据首地址
	int             width;         //像素宽度
	int             height;        //像素高度
};

class CMyGdiPlus
{
public:
	CMyGdiPlus(void);
	~CMyGdiPlus(void);

private:
	ULONG_PTR           m_gdiplusToken;
	void SBmpBuffFree(SGrayBuff &bmpBuff);
	void SBmpBuffAlloc(SGrayBuff &bmpBuff,int w,int h);

public:
	bool DrawLine(Color color,float width,Point pStart,Point pEnd);
	bool InitGdiPlus(HDC hdc);

	Graphics  * m_pGraphics;
	Bitmap * m_pBitmap;
	Bitmap * m_pBitmapOpen;
	Graphics  * m_pGra;
	SGrayBuff m_SGrayBuff;

	CSize CreateImageToBuff(WCHAR * wc, BYTE ** pBmpData);
	void DrawBmp(BYTE *pBmpData, Point pTopLeft, int w, int h);
	bool Buff2Bitmap(const SGrayBuff grayBuff, Bitmap * bmp);
};
