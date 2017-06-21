#include "StdAfx.h"
#include "mygdiplus.h"
#include <math.h>

CMyGdiPlus::CMyGdiPlus(void)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	m_pGraphics = NULL;
	m_pBitmap = NULL;
	m_pBitmapOpen = NULL;
	m_pGra = NULL;

	m_SGrayBuff.width = 0;
	m_SGrayBuff.height = 0;
	m_SGrayBuff.pData = NULL;
}

CMyGdiPlus::~CMyGdiPlus(void)
{
	if(m_pBitmapOpen)
		delete m_pBitmapOpen;
	if(m_pBitmap)
		delete m_pBitmap;
	if(m_pGra)
		delete m_pGra;
	GdiplusShutdown(m_gdiplusToken);
	SBmpBuffFree(m_SGrayBuff);
}

bool CMyGdiPlus::InitGdiPlus(HDC hdc)
{
	if(m_pGraphics != NULL)
		delete m_pGraphics;
	m_pGraphics = new Graphics(hdc);
	return true;
}

bool CMyGdiPlus::DrawLine(Color color,float width,Point pStart,Point pEnd)
{
	if(NULL==m_pGraphics)
		return false;
	Status s;
	Pen      pen(color,width);
	s = m_pGraphics->DrawLine(&pen,pStart,pEnd);

	if(!s)
		return true;
	else
		return false;
}


void CMyGdiPlus::DrawBmp(BYTE *pBmpData, Point pTopLeft, int w, int h)
{
	if(NULL==m_pGraphics)
		return;

	Rect r(pTopLeft.X, pTopLeft.Y, w, h);

	SGrayBuff bmpBuff;
	bmpBuff.pData = pBmpData;
	bmpBuff.width = w;
	bmpBuff.height = h;
	Bitmap* bmp = new Bitmap(w, h,PixelFormat32bppARGB);

	Buff2Bitmap(bmpBuff, bmp);

	m_pGraphics->DrawImage(bmp,r);

	delete bmp;
}


void CMyGdiPlus::SBmpBuffAlloc(SGrayBuff &bmpBuff,int w,int h)
{
	bmpBuff.height = h;
	bmpBuff.width = w;
	bmpBuff.pData = (BYTE *)GlobalLock(GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, h*w));
}
void CMyGdiPlus::SBmpBuffFree(SGrayBuff &bmpBuff)
{
	bmpBuff.height = bmpBuff.width = 0;
	if(bmpBuff.pData)
	{
		GlobalUnlock(bmpBuff.pData);
		GlobalFree(bmpBuff.pData);
	}
}


CSize CMyGdiPlus::CreateImageToBuff(WCHAR * wc, BYTE ** pBmpData)
{
	CSize imgSize;
	imgSize.SetSize(0, 0);

	//if(NULL==m_pGraphics)
	//	return NULL;

	if(m_pBitmapOpen)
		delete m_pBitmapOpen;
	m_pBitmapOpen = new Bitmap(wc);
	imgSize.cx = m_pBitmapOpen->GetWidth();
	imgSize.cy = m_pBitmapOpen->GetHeight();

	//因为不用改变图像尺寸，所以注释掉以下8行代码
	//if(m_pBitmap)
	//	delete m_pBitmap;
	//m_pBitmap = new Bitmap(imgSize.cx, imgSize.cy);
	//if(m_pGra)
	//	delete m_pGra;
	//m_pGra = Graphics::FromImage(m_pBitmap);
	//m_pGra->SetInterpolationMode(InterpolationModeHighQualityBicubic);//InterpolationModeBicubic//InterpolationModeHighQualityBilinear//InterpolationModeBilinear//InterpolationModeHighQualityBicubic
	//m_pGra->DrawImage(m_pBitmapOpen, 0,0, imgSize.cx, imgSize.cy);

	SBmpBuffFree(m_SGrayBuff);
	SBmpBuffAlloc(m_SGrayBuff, imgSize.cx, imgSize.cy);
	
	BitmapData bmpData;
	Rect rect(0, 0, m_SGrayBuff.width, m_SGrayBuff.height);
	Status  stat = m_pBitmapOpen->LockBits(&rect,ImageLockModeRead,PixelFormat32bppRGB,&bmpData);
	UINT* pixels = (UINT*)bmpData.Scan0;
	UINT pixel;
	int i,j,offset = 0;	
	for (i=0;i<m_SGrayBuff.height;i++)
	{
		for (j=0;j<m_SGrayBuff.width;j++) 
		{
			pixel = pixels[offset + j];
			m_SGrayBuff.pData[offset + j] = (BYTE) ((pixel & 0x000000ff));
		}
		offset += m_SGrayBuff.width;
	}
	stat = m_pBitmapOpen->UnlockBits(&bmpData);
	*pBmpData = m_SGrayBuff.pData;

	return imgSize;
}

bool CMyGdiPlus::Buff2Bitmap(const SGrayBuff grayBuff, Bitmap * bmp)
{
	int size = grayBuff.width * grayBuff.height;
	if(0==size || !(grayBuff.pData))
		return false;

	UINT pixelA,pixel;
	UINT* pixels = (UINT*)GlobalLock(GlobalAlloc(GMEM_FIXED, size*4));
	if(!pixels)
		return false;

	int i,j,pos,offset = 0;
	for (i=0;i<grayBuff.height;i++)
	{
		for (j=0;j<grayBuff.width;j++) 
		{
			pos = offset + j;
			pixelA = (UINT)255;
			pixel = (UINT)(grayBuff.pData[pos]);
			pixels[pos] = (pixelA<<24) + (pixel<<16) + (pixel<<8) + pixel;
		}
		offset += grayBuff.width;
	}

	BitmapData bmpData;
	Rect rect(0, 0, grayBuff.width, grayBuff.height);
	bmpData.Width = grayBuff.width;
	bmpData.Height = grayBuff.height;
	bmpData.Stride = 4*bmpData.Width;
	bmpData.PixelFormat = PixelFormat32bppARGB;//bmp->GetPixelFormat(); 
	bmpData.Scan0 = (VOID*)pixels;
	bmpData.Reserved = NULL;
	if(bmp->GetWidth() != bmpData.Width || bmp->GetHeight() != bmpData.Height)
	{
		delete bmp;
		bmp = new Bitmap(bmpData.Width,bmpData.Height,bmpData.PixelFormat);
		Status  stat = bmp->LockBits(&rect,ImageLockModeWrite|ImageLockModeUserInputBuf,PixelFormat32bppARGB,&bmpData);
		stat = bmp->UnlockBits(&bmpData);
	}
	else
	{
		Status  stat = bmp->LockBits(&rect,ImageLockModeWrite|ImageLockModeUserInputBuf,PixelFormat32bppARGB,&bmpData);
		stat = bmp->UnlockBits(&bmpData);
	}

	GlobalUnlock(pixels);
	GlobalFree(pixels);
	return true;
}