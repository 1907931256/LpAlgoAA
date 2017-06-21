#include "stdafx.h"
#include "AaAlgorithm.h"


CAaAlgorithm::CAaAlgorithm()
{
	m_sChartArea.TopLeft = { 0,0 };
	m_sChartArea.TopRight = { 0,0 };
	m_sChartArea.BottomLeft = { 0,0 };
	m_sChartArea.BottomRight = { 0,0 };

	m_fDistScaleX1 = 0.42f;
	m_fDistScaleX2 = 0.32f;
	m_fDistScaleY1 = 0.37f;
	m_fDistScaleY2 = 0.4f;
	m_shift_factor = 5;
	m_nBlobRangeWidth = 20;//12~20

	m_sMTF.nMTFzpos = 0;

	m_nBlobWidth = 0;

	m_nLightRangeD = 1;
}


CAaAlgorithm::~CAaAlgorithm()
{
	//_CrtDumpMemoryLeaks();
}

int CAaAlgorithm::AaChartArea(const IMG_UBBUF src, IMG_UBBUF dst, AaStructChartArea & sChartArea)
{
	int status = 0;
	m_sChartArea.TopLeft = { 0,0 };
	m_sChartArea.TopRight = { 0,0 };
	m_sChartArea.BottomLeft = { 0,0 };
	m_sChartArea.BottomRight = { 0,0 };
	sChartArea = m_sChartArea;
	m_nBlobNum = 0;
	m_nMaxBlobArea = 0;

	//memcpy(dst.ptr, src.ptr, src.size.width * src.size.height);

	status = m_Alg2DBase.VisSegmentOtsu(src.ptr, src.size.width, src.size.height, dst.ptr);
	//status = m_Alg2DBase.VisSegmentCntDots(src.ptr, src.size.width, src.size.height, dst.ptr, 0.5);
	if (status != 0)
		return status;
	status = m_Alg2DBase.VisMorphologyOpen(dst, dst);
	if (status != 0)
		return status;

	status = m_AlgBlob.BlobLabelMarker(dst, m_nBlobNum);
	if (status != 0)
		return status;
	status = m_AlgBlob.BlobAreas(dst);
	if (status != 0)
		return status;
	status = m_AlgBlob.GetMaxAreaBlob(dst, m_nMaxBlobArea);
	if (status != 0)
		return status;

	// get 4 corner points to m_sChartArea
	double dist = 0;
	double distTopLeft = 99999;
	double distTopRight = 99999;
	double distBottomLeft = 99999;
	double distBottomRight = 99999;
	CPoint TopLeft = { 0,0 };
	CPoint TopRight = { src.size.width, 0 };
	CPoint BottomLeft = { 0, src.size.height };
	CPoint BottomRight = { src.size.width , src.size.height };
	int i2, ii2;
	int i, j, pos = src.size.width;
	int posU = 0, posD = pos + src.size.width;
	for (i = 1; i <  src.size.height-1; i++)
	{
		i2 = i * i;
		ii2 = (src.size.height - i) * (src.size.height - i);
		for (j = 1; j < src.size.width-1; j++)
		{
			if (dst.ptr[pos + j] && (0 == dst.ptr[pos + j - 1] || 0 == dst.ptr[pos + j + 1] || 0 == dst.ptr[posU + j] || 0 == dst.ptr[posD + j]))
			{
				//dist = sqrt(i2 + j * j); //TopLeft
				dist = i + j;
				if (dist < distTopLeft)
				{
					distTopLeft = dist;
					TopLeft = { j, i };
				}
				//dist = sqrt(i2+(src.size.width - j)*(src.size.width - j)); //TopRight
				dist = i + src.size.width - j;
				if (dist < distTopRight)
				{
					distTopRight = dist;
					TopRight = { j, i };
				}
				//dist = sqrt(ii2 + j * j); //BottomLeft
				dist = src.size.height - i + j;
				if (dist < distBottomLeft)
				{
					distBottomLeft = dist;
					BottomLeft = { j, i };
				}
				//dist = sqrt(ii2 + (src.size.width - j)*(src.size.width - j)); //BottomRight
				dist = src.size.height - i + src.size.width - j;
				if (dist < distBottomRight)
				{
					distBottomRight = dist;
					BottomRight = { j, i };
				}
			}
		}
		pos += src.size.width;
		posU += src.size.width;
		posD += src.size.width;
	}

	if (distTopLeft < 9999 && distTopRight < 9999 && distBottomLeft < 9999 && distBottomRight < 9999)
	{
		m_sChartArea.TopLeft = TopLeft;
		m_sChartArea.TopRight = TopRight;
		m_sChartArea.BottomLeft = BottomLeft;
		m_sChartArea.BottomRight = BottomRight;
		sChartArea = m_sChartArea;
		status = 0;
	}
	else
	{
		status = 1;
	}

	return status;
}

int CAaAlgorithm::AaChartBlobs(const IMG_UBBUF src, IMG_UBBUF dst, AaStructChartBlobs & sChartBlobs)
{
	int status = 0;
	int n = 0;
	for (n = 0; n < CHART_BLOB_NUM; n++)
	{
		m_sChartBlobs.Blobs[n].TopLeft = { 0,0 };
		m_sChartBlobs.Blobs[n].TopRight = { 0,0 };
		m_sChartBlobs.Blobs[n].BottomLeft = { 0,0 };
		m_sChartBlobs.Blobs[n].BottomRight = { 0,0 };
		m_sChartBlobs.Centers[n] = { 0,0 };
		m_sChartBlobs.EdgeLeft[n][0] = { 0,0 };
		m_sChartBlobs.EdgeLeft[n][1] = { 0,0 };
		m_sChartBlobs.EdgeRight[n][0] = { 0,0 };
		m_sChartBlobs.EdgeRight[n][1] = { 0,0 };
		m_sChartBlobs.EdgeUp[n][0] = { 0,0 };
		m_sChartBlobs.EdgeUp[n][1] = { 0,0 };
		m_sChartBlobs.EdgeDown[n][0] = { 0,0 };
		m_sChartBlobs.EdgeDown[n][1] = { 0,0 };
	}
	sChartBlobs = m_sChartBlobs;
	
	int extend = 3;
	int ys, ye, xs, xe;
	ys = min(m_sChartArea.TopLeft.y, m_sChartArea.TopRight.y)  - extend;
	if (ys < 0)
		ys = 0;
	ye = max(m_sChartArea.BottomLeft.y, m_sChartArea.BottomRight.y) + extend;
	if (ye >= src.size.height)
		ye = src.size.height - 1;
	xs = min(m_sChartArea.TopLeft.x, m_sChartArea.BottomLeft.x);
	xe = max(m_sChartArea.TopRight.x, m_sChartArea.BottomRight.x);
	if (xe < xs + 10 || ye < ys + 10 || xs < 0 || ys > src.size.height || xe > src.size.width || ye < 0)
	{
		status = 1;
		return status;
	}

	//plan 9 rectangles
	int xw = ((m_sChartArea.TopRight.x - m_sChartArea.TopLeft.x) + (m_sChartArea.BottomRight.x - m_sChartArea.BottomLeft.x)) / 2;
	if (xw < 5)
	{
		status = 1;
		return status;
	}
	int xshift = (m_sChartArea.TopRight.x - m_sChartArea.TopLeft.x + abs(m_sChartArea.TopRight.y - m_sChartArea.TopLeft.y)) - (m_sChartArea.BottomRight.x - m_sChartArea.BottomLeft.x + abs(m_sChartArea.BottomRight.y - m_sChartArea.BottomLeft.y));
	xshift /= m_shift_factor;
	int yh = ((m_sChartArea.BottomLeft.y - m_sChartArea.TopLeft.y) + (m_sChartArea.BottomRight.y - m_sChartArea.TopRight.y)) / 2;
	if (yh < 5)
	{
		status = 1;
		return status;
	}
	int yshift = m_shift_factor + (m_sChartArea.BottomLeft.y - m_sChartArea.TopLeft.y + abs(m_sChartArea.BottomLeft.x - m_sChartArea.TopLeft.x)) - (m_sChartArea.BottomRight.y - m_sChartArea.TopRight.y + abs(m_sChartArea.BottomRight.x - m_sChartArea.TopRight.x));
	yshift /= m_shift_factor;
	int yL = (m_sChartArea.BottomLeft.y + m_sChartArea.TopLeft.y) / 2;
	int yR = (m_sChartArea.BottomRight.y + m_sChartArea.TopRight.y) / 2;
	int xU = (m_sChartArea.TopLeft.x + m_sChartArea.TopRight.x) / 2;
	int xD = (m_sChartArea.BottomLeft.x + m_sChartArea.BottomRight.x) / 2;
	m_nBlobWidth = (xw + yh) / m_nBlobRangeWidth;
	if(m_nBlobWidth < 5)
	{
		status = 1;
		return status;
	}
	m_sChartBlobs.Centers[CHART_BLOB_NUM / 2].x = (m_sChartArea.TopLeft.x + m_sChartArea.TopRight.x + m_sChartArea.BottomLeft.x + m_sChartArea.BottomRight.x) / 4 - yshift;
	m_sChartBlobs.Centers[CHART_BLOB_NUM / 2].y = (m_sChartArea.TopLeft.y + m_sChartArea.TopRight.y + m_sChartArea.BottomLeft.y + m_sChartArea.BottomRight.y) / 4 - xshift;

	//top-left
	m_sChartBlobs.Centers[0].x = xU - int(xw * m_fDistScaleX1 + 0.5) - yshift;
	m_sChartBlobs.Centers[0].y = yL - int(yh * m_fDistScaleY1 + 0.5) - xshift;
	//top
	m_sChartBlobs.Centers[1].x = xU - yshift;
	m_sChartBlobs.Centers[1].y = m_sChartBlobs.Centers[CHART_BLOB_NUM / 2].y - int(yh * m_fDistScaleY2 + 0.5) - xshift;
	//top-right
	m_sChartBlobs.Centers[2].x = xU + int(xw * m_fDistScaleX1 + 0.5) - yshift;
	m_sChartBlobs.Centers[2].y = yR - int(yh * m_fDistScaleY1 + 0.5) - xshift;
	//left
	m_sChartBlobs.Centers[3].x = m_sChartBlobs.Centers[CHART_BLOB_NUM / 2].x - int(xw * m_fDistScaleX2 + 0.5) - yshift;
	m_sChartBlobs.Centers[3].y = yL - xshift;
	//right
	m_sChartBlobs.Centers[5].x = m_sChartBlobs.Centers[CHART_BLOB_NUM / 2].x + int(xw * m_fDistScaleX2 + 0.5) - yshift;
	m_sChartBlobs.Centers[5].y = yR - xshift;
	//bottom-left
	m_sChartBlobs.Centers[6].x = xD - int(xw * m_fDistScaleX1 + 0.5) - yshift;
	m_sChartBlobs.Centers[6].y = yL + int(yh * m_fDistScaleY1 + 0.5) - xshift;
	//bottom
	m_sChartBlobs.Centers[7].x = xD - yshift;
	m_sChartBlobs.Centers[7].y = m_sChartBlobs.Centers[CHART_BLOB_NUM / 2].y + int(yh * m_fDistScaleY2 + 0.5 - xshift);
	//bottom-right
	m_sChartBlobs.Centers[8].x = xD + int(xw * m_fDistScaleX1 + 0.5) - yshift;
	m_sChartBlobs.Centers[8].y = yR + int(yh * m_fDistScaleY1 + 0.5) - xshift;

	//plan rectangles around centers
	for (n = 0; n < CHART_BLOB_NUM; n++)
	{
		m_sChartBlobs.Blobs[n].TopLeft = { m_sChartBlobs.Centers[n].x - m_nBlobWidth, m_sChartBlobs.Centers[n].y - m_nBlobWidth };
		m_sChartBlobs.Blobs[n].TopRight = { m_sChartBlobs.Centers[n].x + m_nBlobWidth, m_sChartBlobs.Centers[n].y - m_nBlobWidth };
		m_sChartBlobs.Blobs[n].BottomLeft = { m_sChartBlobs.Centers[n].x - m_nBlobWidth, m_sChartBlobs.Centers[n].y + m_nBlobWidth };
		m_sChartBlobs.Blobs[n].BottomRight = { m_sChartBlobs.Centers[n].x + m_nBlobWidth, m_sChartBlobs.Centers[n].y + m_nBlobWidth };
		if (m_sChartBlobs.Blobs[n].TopLeft.x < xs)
			m_sChartBlobs.Blobs[n].TopLeft.x = xs;
		if (m_sChartBlobs.Blobs[n].BottomLeft.x < xs)
			m_sChartBlobs.Blobs[n].BottomLeft.x = xs;
		if (m_sChartBlobs.Blobs[n].TopRight.x > xe)
			m_sChartBlobs.Blobs[n].TopRight.x = xe;
		if (m_sChartBlobs.Blobs[n].BottomRight.x > xe)
			m_sChartBlobs.Blobs[n].BottomRight.x = xe;
		if (m_sChartBlobs.Blobs[n].TopLeft.y < ys)
			m_sChartBlobs.Blobs[n].TopLeft.y = ys;
		if (m_sChartBlobs.Blobs[n].TopRight.y < ys)
			m_sChartBlobs.Blobs[n].TopRight.y = ys;
		if (m_sChartBlobs.Blobs[n].BottomLeft.y > ye)
			m_sChartBlobs.Blobs[n].BottomLeft.y = ye;
		if (m_sChartBlobs.Blobs[n].BottomRight.y > ye)
			m_sChartBlobs.Blobs[n].BottomRight.y = ye;
	}

	int ww = xe - xs + 1;
	int hh = ye - ys + 1;
	BYTE * pSub = new BYTE[ww * hh];
	BYTE * pSubFilter = new BYTE[ww * hh];
	BYTE nThres = 0;

	//get chart areas to one sub image
	int i, j, pos = 0, pos1 = 0;
	int nC = 0;
	for (n = 0; n < CHART_BLOB_NUM; n++)
	{
		pos1 = m_sChartBlobs.Blobs[n].TopLeft.y * src.size.width;
		for (i = m_sChartBlobs.Blobs[n].TopLeft.y; i <= m_sChartBlobs.Blobs[n].BottomLeft.y; i++)
		{
			for (j = m_sChartBlobs.Blobs[n].TopLeft.x; j <= m_sChartBlobs.Blobs[n].TopRight.x; j++)
			{
				pSub[nC++] = src.ptr[pos1 + j];
			}
			pos1 += src.size.width;
		}
	}

	status = m_Alg2DBase.VisCalcOtsu(pSub, nC, 1, nThres);
	//status = m_Alg2DBase.VisSegmentOtsu(pSub, ww, hh, pSub);
	//status = m_Alg2DBase.VisSegmentCntDots(pSub, ww, hh, pSub, 0.8);
	if (status != 0)
		goto freeExit;
	if (nThres < 5 || nThres > 250)
	{
		status = 2;
		goto freeExit;
	}

	//segment chart area
	memset(pSub, 255, ww * hh);
	pos = 0, pos1 = ys * src.size.width;
	for (i = ys; i <= ye; i++)
	{
		for (j = xs; j <= xe; j++)
		{
			if(src.ptr[pos1 + j] < nThres)
				pSub[pos + j - xs] = 0;
		}
		pos += ww;
		pos1 += src.size.width;
	}

	//filter noise
	memset(pSubFilter, 0, ww * hh);
	int posU = 0, posUJ = 0;
	pos = ww;
	int posJ = 0;
	int posD = pos + ww, posDJ = 0;
	for (i = 1; i < hh-1; i++)
	{
		for (j = 1; j < ww-1; j++)
		{
			posJ = pos + j;
			posUJ = posU + j;
			posDJ = posD + j;
			if (pSub[posJ - 1] + pSub[posJ] + pSub[posJ + 1] + pSub[posUJ - 1] + pSub[posUJ] + pSub[posUJ + 1] + pSub[posDJ - 1] + pSub[posDJ] + pSub[posDJ + 1] > 1020)
				pSubFilter[posJ] = 255;
			else
				pSubFilter[posJ] = 0;
		}
		pos += ww;
		posU += ww;
		posD += ww;
	}
	//filter noise again
	memset(pSub, 0, ww * hh);
	posU = posUJ = 0;
	pos = ww;
	posJ = 0;
	posD = pos + ww, posDJ = 0;
	for (i = 1; i < hh - 1; i++)
	{
		for (j = 1; j < ww - 1; j++)
		{
			posJ = pos + j;
			posUJ = posU + j;
			posDJ = posD + j;
			if (pSubFilter[posJ - 1] + pSubFilter[posJ] + pSubFilter[posJ + 1] + pSubFilter[posUJ - 1] + pSubFilter[posUJ] + pSubFilter[posUJ + 1] + pSubFilter[posDJ - 1] + pSubFilter[posDJ] + pSubFilter[posDJ + 1] > 1020)
				pSub[posJ] = 255;
			else
				pSub[posJ] = 0;
		}
		pos += ww;
		posU += ww;
		posD += ww;
	}
	
	//copy segmented sub image to dst image
	memset(dst.ptr, 0, src.size.width * src.size.height);
	pos = 0;
	pos1 = ys * src.size.width;
	for (i = ys; i <= ye; i++)
	{
		for (j = xs; j <= xe; j++)
		{
			dst.ptr[pos1 + j] = pSub[pos + j - xs];
		}
		pos += ww;
		pos1 += src.size.width;
	}

	//check legal status for blobs
	int nSearchLen = 19;
	for (n = 0; n < CHART_BLOB_NUM; n++)
	{
		pos1 = m_sChartBlobs.Centers[n].y * src.size.width;
		if (dst.ptr[pos1 + m_sChartBlobs.Centers[n].x]) //if not find black dot, search in nearby area
		{
			status = 3;
			if(m_sChartBlobs.Centers[n].x < nSearchLen || m_sChartBlobs.Centers[n].x >= src.size.width - nSearchLen || m_sChartBlobs.Centers[n].y < nSearchLen || m_sChartBlobs.Centers[n].y >= src.size.height - nSearchLen)
				goto freeExit;
			for (i = 1; i < nSearchLen; i++)
			{
				if (0 == dst.ptr[pos1 + m_sChartBlobs.Centers[n].x + i])
				{
					m_sChartBlobs.Centers[n].x += i;
					status = 0;
					break;
				}
				else if (0 == dst.ptr[pos1 + m_sChartBlobs.Centers[n].x - i])
				{
					m_sChartBlobs.Centers[n].x -= i;
					status = 0;
					break;
				}
				else if (0 == dst.ptr[pos1 + m_sChartBlobs.Centers[n].x + i * src.size.width])
				{
					m_sChartBlobs.Centers[n].y += i;
					status = 0;
					break;
				}
				else if (0 == dst.ptr[pos1 + m_sChartBlobs.Centers[n].x - i * src.size.width])
				{
					m_sChartBlobs.Centers[n].y -= i;
					status = 0;
					break;
				}
			}
			if(status)
				goto freeExit;
		}
		//if legal, adjust blob center
		{
			pos1 = m_sChartBlobs.Centers[n].y * src.size.width;
			xs = xe = 0;
			for (i = 1; i < m_nBlobWidth * 2; i++)
			{
				if (0 == xs && m_sChartBlobs.Centers[n].x - i >= 0)
					if(dst.ptr[pos1 + m_sChartBlobs.Centers[n].x - i])
						xs = m_sChartBlobs.Centers[n].x - i;
				if (0 == xe && m_sChartBlobs.Centers[n].x + i < src.size.width)
					if(dst.ptr[pos1 + m_sChartBlobs.Centers[n].x + i])
						xe = m_sChartBlobs.Centers[n].x + i;
				if (xs && xe)
					break;
			}
			ys = ye = 0;
			pos = pos1 + src.size.width;
			pos1 -= src.size.width;
			for (i = 1; i < m_nBlobWidth * 2; i++)
			{
				if (0 == ys && m_sChartBlobs.Centers[n].y - i >= 0)
					if(dst.ptr[pos1 + m_sChartBlobs.Centers[n].x])
						ys = m_sChartBlobs.Centers[n].y - i;
				if (0 == ye && m_sChartBlobs.Centers[n].y + i < src.size.height)
					if(dst.ptr[pos + m_sChartBlobs.Centers[n].x])
						ye = m_sChartBlobs.Centers[n].y + i;
				if (ys && ye)
					break;
				pos1 -= src.size.width;
				pos += src.size.width;
			}
			m_sChartBlobs.Centers[n].x = (xs + xe) / 2;
			m_sChartBlobs.Centers[n].y = (ys + ye) / 2;
		}
		//adjust blob center again
		{
			pos1 = m_sChartBlobs.Centers[n].y * src.size.width;
			xs = xe = 0;
			for (i = 1; i < m_nBlobWidth * 2; i++)
			{
				if (0 == xs && m_sChartBlobs.Centers[n].x - i >= 0)
					if (dst.ptr[pos1 + m_sChartBlobs.Centers[n].x - i])
						xs = m_sChartBlobs.Centers[n].x - i;
				if (0 == xe && m_sChartBlobs.Centers[n].x + i < src.size.width)
					if (dst.ptr[pos1 + m_sChartBlobs.Centers[n].x + i])
						xe = m_sChartBlobs.Centers[n].x + i;
				if (xs && xe)
					break;
			}
			ys = ye = 0;
			pos = pos1 + src.size.width;
			pos1 -= src.size.width;
			for (i = 1; i < m_nBlobWidth * 2; i++)
			{
				if (0 == ys && m_sChartBlobs.Centers[n].y - i >= 0)
					if (dst.ptr[pos1 + m_sChartBlobs.Centers[n].x])
						ys = m_sChartBlobs.Centers[n].y - i;
				if (0 == ye && m_sChartBlobs.Centers[n].y + i < src.size.height)
					if (dst.ptr[pos + m_sChartBlobs.Centers[n].x])
						ye = m_sChartBlobs.Centers[n].y + i;
				if (ys && ye)
					break;
				pos1 -= src.size.width;
				pos += src.size.width;
			}
			m_sChartBlobs.Centers[n].x = (xs + xe) / 2;
			m_sChartBlobs.Centers[n].y = (ys + ye) / 2;
		}
	}

	//get all single blob's left/right edge
	for (n = 0; n < CHART_BLOB_NUM; n++)
	{
		if (detectBlobEdgeLeft(dst.ptr, src.size.width, src.size.height, m_sChartBlobs.Centers[n], m_nBlobWidth * 2, m_sChartBlobs.EdgeLeft[n]))
			status = 4;
		if (detectBlobEdgeRight(dst.ptr, src.size.width, src.size.height, m_sChartBlobs.Centers[n], m_nBlobWidth * 2, m_sChartBlobs.EdgeRight[n]))
			status = 5;
		if (detectBlobEdgeUp(dst.ptr, src.size.width, src.size.height, m_sChartBlobs.Centers[n], m_nBlobWidth * 2, m_sChartBlobs.EdgeUp[n]))
			status = 6;
		if (detectBlobEdgeDown(dst.ptr, src.size.width, src.size.height, m_sChartBlobs.Centers[n], m_nBlobWidth * 2, m_sChartBlobs.EdgeDown[n]))
			status = 7;
		if (status != 0)
			goto freeExit;
	}

	//adjust all blob's 4 edges in gray image;
	int nEdgeAdjustR = 3;
	int nGradWidth = 3;
	int w2 = src.size.width * 2;
	int nGrad = 0, nGradMax = 0;
	int nMax1 = 0, nMax2 = 0;
	int x = 0, x1 = 0, x2 = 0, dx = 0, y = 0, y1 = 0, y2 = 0, dy = 0;
	int x1s, x1e, x2s, x2e, y1s, y1e, y2s, y2e;
	int m = 0;
	for (n = 0; n < CHART_BLOB_NUM; n++)
	{
		//left
		y1 = m_sChartBlobs.EdgeLeft[n][0].y;
		y2 = m_sChartBlobs.EdgeLeft[n][1].y;
		dy = y2 - y1;
		if (dy > 0)
		{
			x1s = m_sChartBlobs.EdgeLeft[n][0].x - nEdgeAdjustR;
			if (x1s < nGradWidth)
				x1s = nGradWidth;
			x1e = m_sChartBlobs.EdgeLeft[n][0].x + nEdgeAdjustR;
			if (x1e >= src.size.width - nGradWidth)
				x1e = src.size.width - nGradWidth - 1;
			x2s = m_sChartBlobs.EdgeLeft[n][1].x - nEdgeAdjustR;
			if (x2s < nGradWidth)
				x2s = nGradWidth;
			x2e = m_sChartBlobs.EdgeLeft[n][1].x + nEdgeAdjustR;
			if (x2e >= src.size.width - nGradWidth)
				x2e = src.size.width - nGradWidth - 1;
			nMax1 = nMax2 = 0;
			nGradMax = -9999999;
			for (x1 = x1s; x1 <= x1e; x1++)
			{
				for (x2 = x2s; x2 <= x2e; x2++)
				{
					nGrad = 0;
					pos = y1 * src.size.width;
					for (y = y1; y <= y2; y += 2)
					{
						x = x1 + (y - y1) * (x2 - x1) / dy;
						pos1 = pos + x;
						for (m = 1; m <= nGradWidth; m++)
							nGrad += src.ptr[pos1 - m];
						for (m = 1; m <= nGradWidth; m++)
							nGrad -= src.ptr[pos1 + m];
						pos += w2;
					}
					if (nGradMax < nGrad)
					{
						nGradMax = nGrad;
						nMax1 = x1;
						nMax2 = x2;
					}
				}
			}
			m_sChartBlobs.EdgeLeft[n][0].x = nMax1;
			m_sChartBlobs.EdgeLeft[n][1].x = nMax2;
		}
		//right
		y1 = m_sChartBlobs.EdgeRight[n][0].y;
		y2 = m_sChartBlobs.EdgeRight[n][1].y;
		dy = y2 - y1;
		if (dy > 0)
		{
			x1s = m_sChartBlobs.EdgeRight[n][0].x - nEdgeAdjustR;
			if (x1s < nGradWidth)
				x1s = nGradWidth;
			x1e = m_sChartBlobs.EdgeRight[n][0].x + nEdgeAdjustR;
			if (x1e >= src.size.width - nGradWidth)
				x1e = src.size.width - nGradWidth - 1;
			x2s = m_sChartBlobs.EdgeRight[n][1].x - nEdgeAdjustR;
			if (x2s < nGradWidth)
				x2s = nGradWidth;
			x2e = m_sChartBlobs.EdgeRight[n][1].x + nEdgeAdjustR;
			if (x2e >= src.size.width - nGradWidth)
				x2e = src.size.width - nGradWidth - 1;
			nMax1 = nMax2 = 0;
			nGradMax = -9999999;
			for (x1 = x1s; x1 <= x1e; x1++)
			{
				for (x2 = x2s; x2 <= x2e; x2++)
				{
					nGrad = 0;
					pos = y1 * src.size.width;
					for (y = y1; y <= y2; y += 2)
					{
						x = x1 + (y - y1) * (x2 - x1) / dy;
						pos1 = pos + x;
						for (m = 1; m <= nGradWidth; m++)
							nGrad += src.ptr[pos1 + m];
						for (m = 1; m <= nGradWidth; m++)
							nGrad -= src.ptr[pos1 - m];
						pos += w2;
					}
					if (nGradMax < nGrad)
					{
						nGradMax = nGrad;
						nMax1 = x1;
						nMax2 = x2;
					}
				}
			}
			m_sChartBlobs.EdgeRight[n][0].x = nMax1;
			m_sChartBlobs.EdgeRight[n][1].x = nMax2;
		}

		//up
		x1 = m_sChartBlobs.EdgeUp[n][0].x;
		x2 = m_sChartBlobs.EdgeUp[n][1].x;
		dx = x2 - x1;
		if (dx > 0)
		{
			y1s = m_sChartBlobs.EdgeUp[n][0].y - nEdgeAdjustR;
			if (y1s < nGradWidth)
				y1s = nGradWidth;
			y1e = m_sChartBlobs.EdgeUp[n][0].y + nEdgeAdjustR;
			if (y1e >= src.size.height - nGradWidth)
				y1e = src.size.height - nGradWidth - 1;
			y2s = m_sChartBlobs.EdgeUp[n][1].y - nEdgeAdjustR;
			if (y2s < nGradWidth)
				y2s = nGradWidth;
			y2e = m_sChartBlobs.EdgeUp[n][1].y + nEdgeAdjustR;
			if (y2e >= src.size.height - nGradWidth)
				y2e = src.size.height - nGradWidth - 1;
			nMax1 = nMax2 = 0;
			nGradMax = -9999999;
			for (y1 = y1s; y1 <= y1e; y1++)
			{
				for (y2 = y2s; y2 <= y2e; y2++)
				{
					nGrad = 0;
					for (x = x1; x <= x2; x += 2)
					{
						y = y1 + (x - x1) * (y2 - y1) / dx;
						pos1 = y*src.size.width + x;
						for (m = 1; m <= nGradWidth; m++)
							nGrad += src.ptr[pos1 - m*src.size.width];
						for (m = 0; m < nGradWidth; m++)
							nGrad -= src.ptr[pos1 + m*src.size.width];
					}
					if (nGradMax < nGrad)
					{
						nGradMax = nGrad;
						nMax1 = y1;
						nMax2 = y2;
					}
				}
			}
			m_sChartBlobs.EdgeUp[n][0].y = nMax1;
			m_sChartBlobs.EdgeUp[n][1].y = nMax2;
		}

		//down
		x1 = m_sChartBlobs.EdgeDown[n][0].x;
		x2 = m_sChartBlobs.EdgeDown[n][1].x;
		dx = x2 - x1;
		if (dx > 0)
		{
			y1s = m_sChartBlobs.EdgeDown[n][0].y - nEdgeAdjustR;
			if (y1s < nGradWidth)
				y1s = nGradWidth;
			y1e = m_sChartBlobs.EdgeDown[n][0].y + nEdgeAdjustR;
			if (y1e >= src.size.height - nGradWidth)
				y1e = src.size.height - nGradWidth - 1;
			y2s = m_sChartBlobs.EdgeDown[n][1].y - nEdgeAdjustR;
			if (y2s < nGradWidth)
				y2s = nGradWidth;
			y2e = m_sChartBlobs.EdgeDown[n][1].y + nEdgeAdjustR;
			if (y2e >= src.size.height - nGradWidth)
				y2e = src.size.height - nGradWidth - 1;
			nMax1 = nMax2 = 0;
			nGradMax = -9999999;
			for (y1 = y1s; y1 <= y1e; y1++)
			{
				for (y2 = y2s; y2 <= y2e; y2++)
				{
					nGrad = 0;
					for (x = x1; x <= x2; x += 2)
					{
						y = y1 + (x - x1) * (y2 - y1) / dx;
						pos1 = y*src.size.width + x;
						for (m = 1; m <= nGradWidth; m++)
							nGrad += src.ptr[pos1 + m*src.size.width];
						for (m = 0; m < nGradWidth; m++)
							nGrad -= src.ptr[pos1 - m*src.size.width];
					}
					if (nGradMax < nGrad)
					{
						nGradMax = nGrad;
						nMax1 = y1;
						nMax2 = y2;
					}
				}
			}
			m_sChartBlobs.EdgeDown[n][0].y = nMax1;
			m_sChartBlobs.EdgeDown[n][1].y = nMax2;
		}
	}

freeExit:
	if (0 == status)
	{
		sChartBlobs = m_sChartBlobs;
	}
	delete[] pSub;
	delete[] pSubFilter;
	return status;
}

int CAaAlgorithm::detectBlobEdgeLeft(const unsigned char * pImg, const int width, const int height, const CPoint Center, int blobWidth, CPoint EdgeLeft[2])
{
	CPoint pntUp[CHART_BLOB_EDGE_MAX_PNTS];
	int nUp = 0;
	CPoint pntDown[CHART_BLOB_EDGE_MAX_PNTS];
	int nDown = 0;
	if (blobWidth > CHART_BLOB_EDGE_MAX_PNTS)
		blobWidth = CHART_BLOB_EDGE_MAX_PNTS;

	int nMinPnts = 4;
	int nMaxBreak = nMinPnts / 2 + 1;
	int nCenterShiftFactorY = 5;

	int seedX = 0, seedY = 0;
	int xs = 0, xe = 0, pos = 0;
	int x = 0, y = 0;

	//seed
	CPoint left, right;
	left.x = (m_sChartArea.TopLeft.x + m_sChartArea.BottomLeft.x) / 2;
	left.y = (m_sChartArea.TopLeft.y + m_sChartArea.BottomLeft.y) / 2;
	right.x = (m_sChartArea.TopRight.x + m_sChartArea.BottomRight.x) / 2;
	right.y = (m_sChartArea.TopRight.y + m_sChartArea.BottomRight.y) / 2;
	CPoint cn;
	cn.x = Center.x;
	cn.y = Center.y + blobWidth * (left.y - right.y) / (right.x - left.x) / nCenterShiftFactorY;
	xs = cn.x - 1;
	xe = cn.x - blobWidth;
	if (xe < 0)
		xe = 0;
	pos = cn.y * width;
	for (x = xs; x > xe; x--)
	{
		if (255 == pImg[pos + x] && 0 == pImg[pos + x + 1])
		{
			seedX = x + 1;
			seedY = cn.y;
			break;
		}
	}
	if (0 == seedY)
		return 1;

	//up
	int tmpX = 0, tmpY = 0;
	int searchX = seedX;
	int ys = seedY;
	int ye = seedY - blobWidth;
	if (ye < 0)
		ye = 0;
	pos = ys * width;
	for (y = ys; y > ye; y--)
	{
		tmpX = tmpY = 0;
		if (255 == pImg[pos + searchX] && 0 == pImg[pos + searchX + 1])
		{
			tmpX = searchX + 1;
			tmpY = y;
		}
		else
		{
			searchX--;
			if (255 == pImg[pos + searchX] && 0 == pImg[pos + searchX + 1])
			{
				tmpX = searchX + 1;
				tmpY = y;
			}
			else
			{
				searchX += 2;
				if (255 == pImg[pos + searchX] && 0 == pImg[pos + searchX + 1])
				{
					tmpX = searchX + 1;
					tmpY = y;
				}
				/*else
				{
					searchX -= 3;
					if (255 == pImg[pos + searchX] && 0 == pImg[pos + searchX + 1])
					{
						tmpX = searchX + 1;
						tmpY = y;
					}
					else
					{
						searchX += 4;
						if (255 == pImg[pos + searchX] && 0 == pImg[pos + searchX + 1])
						{
							tmpX = searchX + 1;
							tmpY = y;
						}
					}
				}*/
			}
		}
		if (tmpX > 0 || tmpY > 0)
		{
			pntUp[nUp].x = tmpX;
			pntUp[nUp].y = tmpY;
			nUp++;
		}
		else
		{
			if (nUp < 1)
				break;
			if (pntUp[nUp - 1].y - y > nMaxBreak)
				break;
		}
		pos -= width;
	}
	if (nUp < nMinPnts)
		return 2;

	//down
	searchX = seedX;
	ys = seedY;
	ye = seedY + blobWidth;
	if (ye > height - 1)
		ye = height - 1;
	pos = ys * width;
	for (y = ys; y < ye; y++)
	{
		tmpX = tmpY = 0;
		if (255 == pImg[pos + searchX] && 0 == pImg[pos + searchX + 1])
		{
			tmpX = searchX + 1;
			tmpY = y;
		}
		else
		{
			searchX--;
			if (255 == pImg[pos + searchX] && 0 == pImg[pos + searchX + 1])
			{
				tmpX = searchX + 1;
				tmpY = y;
			}
			else
			{
				searchX += 2;
				if (255 == pImg[pos + searchX] && 0 == pImg[pos + searchX + 1])
				{
					tmpX = searchX + 1;
					tmpY = y;
				}
				/*else
				{
					searchX -= 3;
					if (255 == pImg[pos + searchX] && 0 == pImg[pos + searchX + 1])
					{
						tmpX = searchX + 1;
						tmpY = y;
					}
					else
					{
						searchX += 4;
						if (255 == pImg[pos + searchX] && 0 == pImg[pos + searchX + 1])
						{
							tmpX = searchX + 1;
							tmpY = y;
						}
					}
				}*/
			}
		}
		if (tmpX > 0 || tmpY > 0)
		{
			pntDown[nDown].x = tmpX;
			pntDown[nDown].y = tmpY;
			nDown++;
		}
		else
		{
			if (nDown < 1)
				break;
			if (y - pntDown[nDown].y > nMaxBreak)
				break;
		}
		pos += width;
	}
	if (nDown < nMinPnts)
		return 3;

	//calc edge line two ponts EdgeLeft[2]
	int nC = 0;
	tmpX = tmpY = 0;
	for (x = nUp * 7 / 10; x <= nUp * 8 / 10; x++)
	{
		tmpX += pntUp[x].x;
		tmpY += pntUp[x].y;
		nC++;
	}
	if (nC < 1)
		return 4;
	EdgeLeft[0].x = (tmpX + nC /2) / nC;
	EdgeLeft[0].y = (tmpY + nC /2) / nC;

	nC = 0;
	tmpX = tmpY = 0;
	for (x = nDown * 7 / 10; x <= nDown * 8 / 10; x++)
	{
		tmpX += pntDown[x].x;
		tmpY += pntDown[x].y;
		nC++;
	}
	if (nC < 1)
		return 5;
	EdgeLeft[1].x = (tmpX + nC /2) / nC;
	EdgeLeft[1].y = (tmpY + nC /2) / nC;

	return 0;
}

int CAaAlgorithm::detectBlobEdgeRight(const unsigned char * pImg, const int width, const int height, const CPoint Center, int blobWidth, CPoint EdgeRight[2])
{
	CPoint pntUp[CHART_BLOB_EDGE_MAX_PNTS];
	int nUp = 0;
	CPoint pntDown[CHART_BLOB_EDGE_MAX_PNTS];
	int nDown = 0;
	if (blobWidth > CHART_BLOB_EDGE_MAX_PNTS)
		blobWidth = CHART_BLOB_EDGE_MAX_PNTS;

	int nMinPnts = 4;
	int nMaxBreak = nMinPnts / 2 + 1;
	int nCenterShiftFactorY = 5;

	int seedX = 0, seedY = 0;
	int xs = 0, xe = 0, pos = 0;
	int x = 0, y = 0;

	//seed
	CPoint left, right;
	left.x = (m_sChartArea.TopLeft.x + m_sChartArea.BottomLeft.x) / 2;
	left.y = (m_sChartArea.TopLeft.y + m_sChartArea.BottomLeft.y) / 2;
	right.x = (m_sChartArea.TopRight.x + m_sChartArea.BottomRight.x) / 2;
	right.y = (m_sChartArea.TopRight.y + m_sChartArea.BottomRight.y) / 2;
	CPoint cn;
	cn.x = Center.x;
	cn.y = Center.y - blobWidth * (left.y - right.y) / (right.x - left.x) / nCenterShiftFactorY;
	xs = cn.x + 1;
	xe = cn.x + blobWidth;
	if (xe >= width)
		xe = width - 1;
	pos = cn.y * width;
	for (x = xs; x < xe; x++)
	{
		if (0 == pImg[pos + x] && 255 == pImg[pos + x + 1])
		{
			seedX = x;
			seedY = cn.y;
			break;
		}
	}
	if (0 == seedY)
		return 1;

	//up
	int tmpX = 0, tmpY = 0;
	int searchX = seedX;
	int ys = seedY;
	int ye = seedY - blobWidth;
	if (ye < 0)
		ye = 0;
	pos = ys * width;
	for (y = ys; y > ye; y--)
	{
		tmpX = tmpY = 0;
		if (0 == pImg[pos + searchX] && 255 == pImg[pos + searchX + 1])
		{
			tmpX = searchX;
			tmpY = y;
		}
		else
		{
			searchX--;
			if (0 == pImg[pos + searchX] && 255 == pImg[pos + searchX + 1])
			{
				tmpX = searchX;
				tmpY = y;
			}
			else
			{
				searchX += 2;
				if (0 == pImg[pos + searchX] && 255 == pImg[pos + searchX + 1])
				{
					tmpX = searchX;
					tmpY = y;
				}
				/*else
				{
					searchX -= 3;
					if (0 == pImg[pos + searchX] && 255 == pImg[pos + searchX + 1])
					{
						tmpX = searchX;
						tmpY = y;
					}
					else
					{
						searchX += 4;
						if (0 == pImg[pos + searchX] && 255 == pImg[pos + searchX + 1])
						{
							tmpX = searchX;
							tmpY = y;
						}
					}
				}*/
			}
		}
		if (tmpX > 0 || tmpY > 0)
		{
			pntUp[nUp].x = tmpX;
			pntUp[nUp].y = tmpY;
			nUp++;
		}
		else
		{
			if (nUp < 1)
				break;
			if (pntUp[nUp - 1].y - y > nMaxBreak)
				break;
		}
		pos -= width;
	}
	if (nUp < nMinPnts)
		return 2;

	//down
	searchX = seedX;
	ys = seedY;
	ye = seedY + blobWidth;
	if (ye > height - 1)
		ye = height - 1;
	pos = ys * width;
	for (y = ys; y < ye; y++)
	{
		tmpX = tmpY = 0;
		if (0 == pImg[pos + searchX] && 255 == pImg[pos + searchX + 1])
		{
			tmpX = searchX;
			tmpY = y;
		}
		else
		{
			searchX--;
			if (0 == pImg[pos + searchX] && 255 == pImg[pos + searchX + 1])
			{
				tmpX = searchX;
				tmpY = y;
			}
			else
			{
				searchX += 2;
				if (0 == pImg[pos + searchX] && 255 == pImg[pos + searchX + 1])
				{
					tmpX = searchX;
					tmpY = y;
				}
				/*else
				{
					searchX -= 3;
					if (0 == pImg[pos + searchX] && 255 == pImg[pos + searchX + 1])
					{
						tmpX = searchX;
						tmpY = y;
					}
					else
					{
						searchX += 4;
						if (0 == pImg[pos + searchX] && 255 == pImg[pos + searchX + 1])
						{
							tmpX = searchX;
							tmpY = y;
						}
					}
				}*/
			}
		}
		if (tmpX > 0 || tmpY > 0)
		{
			pntDown[nDown].x = tmpX;
			pntDown[nDown].y = tmpY;
			nDown++;
		}
		else
		{
			if (nDown < 1)
				break;
			if (y - pntDown[nDown].y > nMaxBreak)
				break;
		}
		pos += width;
	}
	if (nDown < nMinPnts)
		return 3;

	//calc edge line two ponts EdgeRight[2]
	int nC = 0;
	tmpX = tmpY = 0;
	for (x = nUp * 7 / 10; x <=nUp * 8 / 10; x++)
	{
		tmpX += pntUp[x].x;
		tmpY += pntUp[x].y;
		nC++;
	}
	if (nC < 1)
		return 4;
	EdgeRight[0].x = (tmpX + nC / 2) / nC;
	EdgeRight[0].y = (tmpY + nC / 2) / nC;

	nC = 0;
	tmpX = tmpY = 0;
	for (x = nDown * 7 / 10; x <= nDown * 8 / 10; x++)
	{
		tmpX += pntDown[x].x;
		tmpY += pntDown[x].y;
		nC++;
	}
	if (nC < 1)
		return 5;
	EdgeRight[1].x = (tmpX + nC / 2) / nC;
	EdgeRight[1].y = (tmpY + nC / 2) / nC;

	return 0;
}

int CAaAlgorithm::detectBlobEdgeUp(const unsigned char * pImg, const int width, const int height, const CPoint Center, int blobWidth, CPoint EdgeUp[2])
{
	CPoint pntLeft[CHART_BLOB_EDGE_MAX_PNTS];
	int nLeft = 0;
	CPoint pntRight[CHART_BLOB_EDGE_MAX_PNTS];
	int nRight = 0;
	if (blobWidth > CHART_BLOB_EDGE_MAX_PNTS)
		blobWidth = CHART_BLOB_EDGE_MAX_PNTS;

	int nMinPnts = 4;
	int nMaxBreak = nMinPnts / 2 + 1;
	int nCenterShiftFactorX = 5;

	int seedX = 0, seedY = 0;
	int ys = 0, ye = 0, pos = 0, posx = 0;
	int x = 0, y = 0;

	//seed
	CPoint up, down;
	up.x = (m_sChartArea.TopLeft.x + m_sChartArea.TopRight.x) / 2;
	up.y = (m_sChartArea.TopLeft.y + m_sChartArea.TopRight.y) / 2;
	down.x = (m_sChartArea.BottomLeft.x + m_sChartArea.BottomRight.x) / 2;
	down.y = (m_sChartArea.BottomLeft.y + m_sChartArea.BottomRight.y) / 2;
	CPoint cn;
	cn.x = Center.x + blobWidth * (up.x - down.x) / (down.y - up.y) / nCenterShiftFactorX;
	cn.y = Center.y;
	ys = cn.y - 1;
	ye = cn.y - blobWidth;
	if (ye < 0)
		ye = 0;
	pos = ys * width;
	for (y = ys - 1; y > ye; y--)
	{
		if (255 == pImg[pos + cn.x] && 0 == pImg[pos + cn.x + width])
		{
			seedX = cn.x;
			seedY = y + 1;
			break;
		}
		pos -= width;
	}
	if (0 == seedY)
		return 1;
	int seedYB = seedY;

	//left
	int tmpX = 0, tmpY = 0;
	int searchY = 0;
	int searchYpos = 0;
	int xs = seedX;
	int xe = seedX - blobWidth;
	if (xe < 0)
		xe = 0;
	pos = seedY * width;
	for (x = xs; x > xe; x--)
	{
		pos += searchYpos;
		seedY += searchY;
		posx = pos + x;
		tmpX = tmpY = 0;
		searchY = searchYpos = 0;
		if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos + width])
		{
			tmpX = x;
			tmpY = seedY + searchY + 1;
		}
		else
		{
			searchY--;
			searchYpos -= width;
			if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos + width])
			{
				tmpX = x;
				tmpY = seedY + searchY + 1;
			}
			else
			{
				searchY += 2;
				searchYpos += width*2;
				if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos + width])
				{
					tmpX = x;
					tmpY = seedY + searchY + 1;
				}
				/*else
				{
					searchY -= 3;
					searchYpos -= width * 3;
					if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos + width])
					{
						tmpX = x;
						tmpY = seedY + searchY + 1;
					}
					else
					{
						searchY += 4;
						searchYpos += width * 4;
						if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos + width])
						{
							tmpX = x;
							tmpY = seedY + searchY + 1;
						}
					}
				}*/
			}
		}
		if (tmpX > 0 || tmpY > 0)
		{
			pntLeft[nLeft].x = tmpX;
			pntLeft[nLeft].y = tmpY;
			nLeft++;
		}
		else
		{
			if (nLeft < 1)
				break;
			if (pntLeft[nLeft - 1].x - x > nMaxBreak)
				break;
		}
	}
	if (nLeft < nMinPnts)
		return 2;

	//right
	seedY = seedYB;
	searchY = searchYpos = 0;
	xs = seedX;
	xe = seedX + blobWidth;
	if (xe > width - 1)
		xe = width - 1;
	pos = seedY * width;
	for (x = xs; x < xe; x++)
	{
		pos += searchYpos;
		seedY += searchY;
		posx = pos + x;
		tmpX = tmpY = 0;
		searchY = searchYpos = 0;
		if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos + width])
		{
			tmpX = x;
			tmpY = seedY + searchY + 1;
		}
		else
		{
			searchY--;
			searchYpos -= width;
			if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos + width])
			{
				tmpX = x;
				tmpY = seedY + searchY + 1;
			}
			else
			{
				searchY += 2;
				searchYpos += width * 2;
				if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos + width])
				{
					tmpX = x;
					tmpY = seedY + searchY + 1;
				}
				/*else
				{
					searchY -= 3;
					searchYpos -= width * 3;
					if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos + width])
					{
						tmpX = x;
						tmpY = seedY + searchY + 1;
					}
					else
					{
						searchY += 4;
						searchYpos += width * 4;
						if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos + width])
						{
							tmpX = x;
							tmpY = seedY + searchY + 1;
						}
					}
				}*/
			}
		}
		if (tmpX > 0 || tmpY > 0)
		{
			pntRight[nRight].x = tmpX;
			pntRight[nRight].y = tmpY;
			nRight++;
		}
		else
		{
			if (nRight < 1)
				break;
			if (x - pntRight[nRight].x > nMaxBreak)
				break;
		}
	}
	if (nRight < nMinPnts)
		return 3;

	//calc edge line two ponts EdgeUp[2]
	int nC = 0;
	tmpX = tmpY = 0;
	for (x = nLeft * 7 / 10; x <= nLeft * 8 / 10; x++)
	{
		tmpX += pntLeft[x].x;
		tmpY += pntLeft[x].y;
		nC++;
	}
	if (nC < 1)
		return 4;
	EdgeUp[0].x = (tmpX + nC / 2) / nC;
	EdgeUp[0].y = (tmpY + nC / 2) / nC;

	nC = 0;
	tmpX = tmpY = 0;
	for (x = nRight * 7 / 10; x <= nRight * 8 / 10; x++)
	{
		tmpX += pntRight[x].x;
		tmpY += pntRight[x].y;
		nC++;
	}
	if (nC < 1)
		return 5;
	EdgeUp[1].x = (tmpX + nC / 2) / nC;
	EdgeUp[1].y = (tmpY + nC / 2) / nC;

	return 0;
}

int CAaAlgorithm::detectBlobEdgeDown(const unsigned char * pImg, const int width, const int height, const CPoint Center, int blobWidth, CPoint EdgeDown[2])
{
	CPoint pntLeft[CHART_BLOB_EDGE_MAX_PNTS];
	int nLeft = 0;
	CPoint pntRight[CHART_BLOB_EDGE_MAX_PNTS];
	int nRight = 0;
	if (blobWidth > CHART_BLOB_EDGE_MAX_PNTS)
		blobWidth = CHART_BLOB_EDGE_MAX_PNTS;

	int nMinPnts = 4;
	int nMaxBreak = nMinPnts / 2 + 1;
	int nCenterShiftFactorX = 5;

	int seedX = 0, seedY = 0;
	int ys = 0, ye = 0, pos = 0, posx = 0;
	int x = 0, y = 0;

	//seed
	CPoint up, down;
	up.x = (m_sChartArea.TopLeft.x + m_sChartArea.TopRight.x) / 2;
	up.y = (m_sChartArea.TopLeft.y + m_sChartArea.TopRight.y) / 2;
	down.x = (m_sChartArea.BottomLeft.x + m_sChartArea.BottomRight.x) / 2;
	down.y = (m_sChartArea.BottomLeft.y + m_sChartArea.BottomRight.y) / 2;
	CPoint cn;
	cn.x = Center.x - blobWidth * (up.x - down.x) / (down.y - up.y) / nCenterShiftFactorX;
	cn.y = Center.y;
	ys = cn.y + 1;
	ye = cn.y + blobWidth;
	if (ye > width - 1)
		ye = width - 1;
	pos = ys * width;
	for (y = ys + 1; y < ye; y++)
	{
		if (255 == pImg[pos + cn.x] && 0 == pImg[pos + cn.x - width])
		{
			seedX = cn.x;
			seedY = y - 1;
			break;
		}
		pos += width;
	}
	if (0 == seedY)
		return 1;
	int seedYB = seedY;

	//left
	int tmpX = 0, tmpY = 0;
	int searchY = 0;
	int searchYpos = 0;
	int xs = seedX;
	int xe = seedX - blobWidth;
	if (xe < 0)
		xe = 0;
	pos = seedY * width;
	for (x = xs; x > xe; x--)
	{
		pos += searchYpos;
		seedY += searchY;
		posx = pos + x;
		tmpX = tmpY = 0;
		searchY = searchYpos = 0;
		if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos - width])
		{
			tmpX = x;
			tmpY = seedY + searchY - 1;
		}
		else
		{
			searchY--;
			searchYpos -= width;
			if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos - width])
			{
				tmpX = x;
				tmpY = seedY + searchY - 1;
			}
			else
			{
				searchY += 2;
				searchYpos += width * 2;
				if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos - width])
				{
					tmpX = x;
					tmpY = seedY + searchY - 1;
				}
				/*else
				{
					searchY -= 3;
					searchYpos -= width * 3;
					if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos - width])
					{
						tmpX = x;
						tmpY = seedY + searchY - 1;
					}
					else
					{
						searchY += 4;
						searchYpos += width * 4;
						if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos - width])
						{
							tmpX = x;
							tmpY = seedY + searchY - 1;
						}
					}
				}*/
			}
		}
		if (tmpX > 0 || tmpY > 0)
		{
			pntLeft[nLeft].x = tmpX;
			pntLeft[nLeft].y = tmpY;
			nLeft++;
		}
		else
		{
			if (nLeft < 1)
				break;
			if (pntLeft[nLeft - 1].x - x > nMaxBreak)
				break;
		}
	}
	if (nLeft < nMinPnts)
		return 2;

	//right
	seedY = seedYB;
	searchY = searchYpos = 0;
	xs = seedX;
	xe = seedX + blobWidth;
	if (xe > width - 1)
		xe = width - 1;
	pos = seedY * width;
	for (x = xs; x < xe; x++)
	{
		pos += searchYpos;
		seedY += searchY;
		posx = pos + x;
		tmpX = tmpY = 0;
		searchY = searchYpos = 0;
		if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos - width])
		{
			tmpX = x;
			tmpY = seedY + searchY - 1;
		}
		else
		{
			searchY--;
			searchYpos -= width;
			if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos - width])
			{
				tmpX = x;
				tmpY = seedY + searchY - 1;
			}
			else
			{
				searchY += 2;
				searchYpos += width * 2;
				if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos - width])
				{
					tmpX = x;
					tmpY = seedY + searchY - 1;
				}
				/*else
				{
					searchY -= 3;
					searchYpos -= width * 3;
					if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos - width])
					{
						tmpX = x;
						tmpY = seedY + searchY - 1;
					}
					else
					{
						searchY += 4;
						searchYpos += width * 4;
						if (255 == pImg[posx + searchYpos] && 0 == pImg[posx + searchYpos - width])
						{
							tmpX = x;
							tmpY = seedY + searchY - 1;
						}
					}
				}*/
			}
		}
		if (tmpX > 0 || tmpY > 0)
		{
			pntRight[nRight].x = tmpX;
			pntRight[nRight].y = tmpY;
			nRight++;
		}
		else
		{
			if (nRight < 1)
				break;
			if (x - pntRight[nRight].x > nMaxBreak)
				break;
		}
	}
	if (nRight < nMinPnts)
		return 3;

	//calc edge line two ponts EdgeDown[2]
	int nC = 0;
	tmpX = tmpY = 0;
	for (x = nLeft * 7 / 10; x <= nLeft * 8 / 10; x++)
	{
		tmpX += pntLeft[x].x;
		tmpY += pntLeft[x].y;
		nC++;
	}
	if (nC < 1)
		return 4;
	EdgeDown[0].x = (tmpX + nC / 2) / nC;
	EdgeDown[0].y = (tmpY + nC / 2) / nC;

	nC = 0;
	tmpX = tmpY = 0;
	for (x = nRight * 7 / 10; x <= nRight * 8 / 10; x++)
	{
		tmpX += pntRight[x].x;
		tmpY += pntRight[x].y;
		nC++;
	}
	if (nC < 1)
		return 5;
	EdgeDown[1].x = (tmpX + nC / 2) / nC;
	EdgeDown[1].y = (tmpY + nC / 2) / nC;

	return 0;
}

int CAaAlgorithm::AaMtfSfr(const IMG_UBBUF src, double pMTF[CHART_BLOB_NUM])
{
	int nSFRwidth = 3;
	int nMTFfactor = 180;

	int pos = 0, pos1 = 0;
	int n = 0, m = 0;

	//initialize
	memset(pMTF, 0, CHART_BLOB_NUM * sizeof(double));

	int nGrayL = 0, nGrayR = 0, nGrayLn = 0, nGrayRn = 0;
	double grayL, grayR, dMtfL, dMtfR, dMtfU, dMtfD;
	int x = 0, x0 = 0, x1 = 0, x2 = 0, y = 0, y0 = 0, y1 = 0, y2 = 0, dy = 0, dx = 0;
	int xs, xe,ys,ye;
	
	for (n = 0; n < CHART_BLOB_NUM; n++)
	{
		//left
		y1 = m_sChartBlobs.EdgeLeft[n][0].y;
		y2 = m_sChartBlobs.EdgeLeft[n][1].y;
		if (y2 > src.size.height - 2)
			y2 = src.size.height - 2;
		dy = y2 - y1;
		if (dy < 1)
			return 1;
		x1 = m_sChartBlobs.EdgeLeft[n][0].x;
		x2 = m_sChartBlobs.EdgeLeft[n][1].x;
 
		nGrayL = nGrayR = nGrayLn = nGrayRn = 0;
		pos = y1 * src.size.width;
		for (y = y1; y <= y2; y++)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;

			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			for (x = xs; x <x0; x++)
			{
				nGrayL += src.ptr[pos + x];
				nGrayLn++;
			}

			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			for (x = x0+1; x <=xe; x++)
			{
				nGrayR += src.ptr[pos + x];
				nGrayRn++;
			}

			pos += src.size.width;
		}
		if (nGrayLn <= 0)
			return 1;
		grayL = double(nGrayL) / nGrayLn;
		if (nGrayRn <= 0)
			return 1;
		grayR = double(nGrayR) / nGrayRn;
		dMtfL = fabs(grayL - grayR) / nMTFfactor;

		//right
		y1 = m_sChartBlobs.EdgeRight[n][0].y;
		y2 = m_sChartBlobs.EdgeRight[n][1].y;
		if (y2 > src.size.height - 2)
			y2 = src.size.height - 2;
		dy = y2 - y1;
		if (dy < 1)
			return 2;
		x1 = m_sChartBlobs.EdgeRight[n][0].x;
		x2 = m_sChartBlobs.EdgeRight[n][1].x;

		nGrayL = nGrayR = nGrayLn = nGrayRn = 0;
		pos = y1 * src.size.width;
		for (y = y1; y <= y2; y++)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;

			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			for (x = xs; x <x0; x++)
			{
				nGrayL += src.ptr[pos + x];
				nGrayLn++;
			}

			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			for (x = x0+1; x <= xe; x++)
			{
				nGrayR += src.ptr[pos + x];
				nGrayRn++;
			}

			pos += src.size.width;
		}
		if (nGrayLn <= 0)
			return 1;
		grayL = double(nGrayL) / nGrayLn;
		if (nGrayRn <= 0)
			return 1;
		grayR = double(nGrayR) / nGrayRn;
		dMtfR = fabs(grayL - grayR) / nMTFfactor;

		//up
		x1 = m_sChartBlobs.EdgeUp[n][0].x;
		x2 = m_sChartBlobs.EdgeUp[n][1].x;
		if (x2 > src.size.width - 2)
			x2 = src.size.width - 2;
		dx = x2 - x1;
		if (dx < 1)
			return 1;
		y1 = m_sChartBlobs.EdgeUp[n][0].y;
		y2 = m_sChartBlobs.EdgeUp[n][1].y;

		nGrayL = nGrayR = nGrayLn = nGrayRn = 0;
		for (x = x1; x < x2; x ++)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;

			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			pos = ys * src.size.width;
			for (y = ys; y <y0; y++)
			{
				nGrayL += src.ptr[pos + x];
				nGrayLn++;
				pos += src.size.width;
			}

			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			pos = (y0+1) * src.size.width;
			for (y = y0 + 1; y <= ye; y++)
			{
				nGrayR += src.ptr[pos + x];
				nGrayRn++;
				pos += src.size.width;
			}
		}
		if (nGrayLn <= 0)
			return 1;
		grayL = double(nGrayL) / nGrayLn;
		if (nGrayRn <= 0)
			return 1;
		grayR = double(nGrayR) / nGrayRn;
		dMtfU = fabs(grayL - grayR) / nMTFfactor;

		//Down
		x1 = m_sChartBlobs.EdgeDown[n][0].x;
		x2 = m_sChartBlobs.EdgeDown[n][1].x;
		if (x2 > src.size.width - 2)
			x2 = src.size.width - 2;
		dx = x2 - x1;
		if (dx < 1)
			return 1;
		y1 = m_sChartBlobs.EdgeDown[n][0].y;
		y2 = m_sChartBlobs.EdgeDown[n][1].y;

		nGrayL = nGrayR = nGrayLn = nGrayRn = 0;
		for (x = x1; x < x2; x ++)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;

			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			pos = ys * src.size.width;
			for (y = ys; y <y0; y++)
			{
				nGrayL += src.ptr[pos + x];
				nGrayLn++;
				pos += src.size.width;
			}

			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			pos = (y0 + 1) * src.size.width;
			for (y = y0 + 1; y <= ye; y++)
			{
				nGrayR += src.ptr[pos + x];
				nGrayRn++;
				pos += src.size.width;
			}
		}
		if (nGrayLn <= 0)
			return 1;
		grayL = double(nGrayL) / nGrayLn;
		if (nGrayRn <= 0)
			return 1;
		grayR = double(nGrayR) / nGrayRn;
		dMtfD = fabs(grayL - grayR) / nMTFfactor;

		//MTF of one blob
		pMTF[n] = (dMtfL + dMtfR + dMtfU + dMtfD) / 4;
		//pMTF[n] = max(max(dMtfL, dMtfR), max(dMtfU, dMtfD));
	}

	return 0;
}

int CAaAlgorithm::AaMtfStdDev(const IMG_UBBUF src, double pMTF[CHART_BLOB_NUM])
{
	int nSFRwidth = 3;
	int nMTFfactor = 90;

	int pos = 0, pos1 = 0;
	int n = 0, m = 0;

	//initialize
	memset(pMTF, 0, CHART_BLOB_NUM * sizeof(double));

	int nGray = 0, nGrayn = 0;
	double grayAve, grayDev, dMtfL, dMtfR, dMtfU, dMtfD;
	int x = 0, x0 = 0, x1 = 0, x2 = 0, y = 0, y0 = 0, y1 = 0, y2 = 0, dy = 0, dx = 0;
	int xs, xe, ys, ye;

	for (n = 0; n < CHART_BLOB_NUM; n++)
	{
		//left
		y1 = m_sChartBlobs.EdgeLeft[n][0].y;
		y2 = m_sChartBlobs.EdgeLeft[n][1].y;
		if (y2 > src.size.height - 2)
			y2 = src.size.height - 2;
		dy = y2 - y1;
		if (dy < 1)
			return 1;
		x1 = m_sChartBlobs.EdgeLeft[n][0].x;
		x2 = m_sChartBlobs.EdgeLeft[n][1].x;
		//ave
		nGray = nGrayn = 0;
		pos = y1 * src.size.width;
		for (y = y1; y <= y2; y++)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;
			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			for (x = xs; x <=xe; x++)
			{
				nGray += src.ptr[pos + x];
				nGrayn++;
			}
			pos += src.size.width;
		}
		if (nGrayn <= 0)
			return 1;
		grayAve = double(nGray) / nGrayn;
		//StdDev
		grayDev = 0;
		pos = y1 * src.size.width;
		for (y = y1; y <= y2; y++)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;
			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			for (x = xs; x <= xe; x++)
			{
				grayDev += fabs(src.ptr[pos + x] - grayAve);
				//grayDev += (src.ptr[pos + x] - grayAve)*(src.ptr[pos + x] - grayAve);
			}
			pos += src.size.width;
		}
		grayDev = grayDev / nGrayn;
		//grayDev = sqrt(grayDev) / nGrayn;
		dMtfL = grayDev / nMTFfactor;

		//right
		y1 = m_sChartBlobs.EdgeRight[n][0].y;
		y2 = m_sChartBlobs.EdgeRight[n][1].y;
		if (y2 > src.size.height - 2)
			y2 = src.size.height - 2;
		dy = y2 - y1;
		if (dy < 1)
			return 2;
		x1 = m_sChartBlobs.EdgeRight[n][0].x;
		x2 = m_sChartBlobs.EdgeRight[n][1].x;
		//ave
		nGray = nGrayn = 0;
		pos = y1 * src.size.width;
		for (y = y1; y <= y2; y++)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;
			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			for (x = xs; x <= xe; x++)
			{
				nGray += src.ptr[pos + x];
				nGrayn++;
			}
			pos += src.size.width;
		}
		if (nGrayn <= 0)
			return 1;
		grayAve = double(nGray) / nGrayn;
		//StdDev
		grayDev = 0;
		pos = y1 * src.size.width;
		for (y = y1; y <= y2; y++)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;
			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			for (x = xs; x <= xe; x++)
			{
				grayDev += fabs(src.ptr[pos + x] - grayAve);
				//grayDev += (src.ptr[pos + x] - grayAve)*(src.ptr[pos + x] - grayAve);
			}

			pos += src.size.width;
		}
		grayDev = grayDev / nGrayn;
		//grayDev = sqrt(grayDev) / nGrayn;
		dMtfR = grayDev / nMTFfactor;

		//up
		x1 = m_sChartBlobs.EdgeUp[n][0].x;
		x2 = m_sChartBlobs.EdgeUp[n][1].x;
		if (x2 > src.size.width - 2)
			x2 = src.size.width - 2;
		dx = x2 - x1;
		if (dx < 1)
			return 1;
		y1 = m_sChartBlobs.EdgeUp[n][0].y;
		y2 = m_sChartBlobs.EdgeUp[n][1].y;
		//ave
		nGray = nGrayn = 0;
		for (x = x1; x < x2; x++)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;
			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			pos = ys * src.size.width;
			for (y = ys; y <= ye; y++)
			{
				nGray += src.ptr[pos + x];
				nGrayn++;
				pos += src.size.width;
			}
		}
		if (nGrayn <= 0)
			return 1;
		grayAve = double(nGray) / nGrayn;
		//StdDev
		grayDev = 0;
		for (x = x1; x < x2; x ++)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;
			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			pos = ys * src.size.width;
			for (y = ys; y <= ye; y++)
			{
				grayDev += fabs(src.ptr[pos + x] - grayAve);
				//grayDev += (src.ptr[pos + x] - grayAve)*(src.ptr[pos + x] - grayAve);
				pos += src.size.width;
			}
		}
		grayDev = grayDev / nGrayn;
		//grayDev = sqrt(grayDev) / nGrayn;
		dMtfU = grayDev / nMTFfactor;

		//down
		x1 = m_sChartBlobs.EdgeDown[n][0].x;
		x2 = m_sChartBlobs.EdgeDown[n][1].x;
		if (x2 > src.size.width - 2)
			x2 = src.size.width - 2;
		dx = x2 - x1;
		if (dx < 1)
			return 1;
		y1 = m_sChartBlobs.EdgeDown[n][0].y;
		y2 = m_sChartBlobs.EdgeDown[n][1].y;
		//ave
		nGray = nGrayn = 0;
		for (x = x1; x < x2; x++)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;
			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			pos = ys * src.size.width;
			for (y = ys; y <= ye; y++)
			{
				nGray += src.ptr[pos + x];
				nGrayn++;
				pos += src.size.width;
			}
		}
		if (nGrayn <= 0)
			return 1;
		grayAve = double(nGray) / nGrayn;
		//StdDev
		grayDev = 0;
		for (x = x1; x < x2; x++)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;
			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			pos = ys * src.size.width;
			for (y = ys; y <= ye; y++)
			{
				grayDev += fabs(src.ptr[pos + x] - grayAve);
				//grayDev += (src.ptr[pos + x] - grayAve)*(src.ptr[pos + x] - grayAve);
				pos += src.size.width;
			}
		}
		grayDev = grayDev / nGrayn;
		//grayDev = sqrt(grayDev) / nGrayn;
		dMtfD = grayDev / nMTFfactor;

		//MTF of one blob
		pMTF[n] = (dMtfL + dMtfR + dMtfU + dMtfD) / 4;
	}

	return 0;
}

int CAaAlgorithm::AaMtfMaxMin(const IMG_UBBUF src, double pMTF[CHART_BLOB_NUM])
{
	int nSFRwidth = 3;
	int nMTFfactor = 180;

	int pos = 0, pos1 = 0;
	int n = 0, m = 0;

	//initialize
	memset(pMTF, 0, CHART_BLOB_NUM * sizeof(double));

	int nCnt = 0, nMax = 0, nMin = 0, nMaxAve = 0, nMinAve = 0;
	double grayL, grayR, grayU, grayD, dMtfL, dMtfR, dMtfU, dMtfD;
	int x = 0, x0 = 0, x1 = 0, x2 = 0, y = 0, y0 = 0, y1 = 0, y2 = 0, dy = 0, dx = 0;
	int xs, xe, ys, ye;
	int w2 = src.size.width * 2;

	for (n = 0; n < CHART_BLOB_NUM; n++)
	{
		//left
		y1 = m_sChartBlobs.EdgeLeft[n][0].y;
		y2 = m_sChartBlobs.EdgeLeft[n][1].y;
		if (y2 > src.size.height - 2)
			y2 = src.size.height - 2;
		dy = y2 - y1;
		if (dy < 1)
			return 1;
		x1 = m_sChartBlobs.EdgeLeft[n][0].x;
		x2 = m_sChartBlobs.EdgeLeft[n][1].x;

		nCnt = nMaxAve = nMinAve = 0;
		pos = y1 * src.size.width;
		for (y = y1; y < y2; y+=2)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;

			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			nMax = 0;
			nMin = 255;
			for (x = xs; x <= xe; x++)
			{
				if(nMax < src.ptr[pos + x])
					nMax = src.ptr[pos + x];
				if (nMax < src.ptr[pos + x + src.size.width])
					nMax = src.ptr[pos + x + src.size.width];
				if (nMin > src.ptr[pos + x])
					nMin = src.ptr[pos + x];
				if (nMin > src.ptr[pos + x + src.size.width])
					nMin = src.ptr[pos + x + src.size.width];
			}

			nCnt++;
			nMaxAve += nMax;
			nMinAve += nMin;
			pos += w2;
		}
		if (nCnt < 1)
			return 1;
		grayL = double(nMaxAve) / nCnt;
		grayR = double(nMinAve) / nCnt;
		dMtfL = fabs(grayL - grayR) / nMTFfactor;

		//right
		y1 = m_sChartBlobs.EdgeRight[n][0].y;
		y2 = m_sChartBlobs.EdgeRight[n][1].y;
		if (y2 > src.size.height - 2)
			y2 = src.size.height - 2;
		dy = y2 - y1;
		if (dy < 1)
			return 2;
		x1 = m_sChartBlobs.EdgeRight[n][0].x;
		x2 = m_sChartBlobs.EdgeRight[n][1].x;

		nCnt = nMaxAve = nMinAve = 0;
		pos = y1 * src.size.width;
		for (y = y1; y <= y2; y += 2)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;

			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			nMax = 0;
			nMin = 255;
			for (x = xs; x <= xe; x++)
			{
				if (nMax < src.ptr[pos + x])
					nMax = src.ptr[pos + x];
				if (nMax < src.ptr[pos + x + src.size.width])
					nMax = src.ptr[pos + x + src.size.width];
				if (nMin > src.ptr[pos + x])
					nMin = src.ptr[pos + x];
				if (nMin > src.ptr[pos + x + src.size.width])
					nMin = src.ptr[pos + x + src.size.width];
			}

			nCnt++;
			nMaxAve += nMax;
			nMinAve += nMin;
			pos += w2;
		}
		if (nCnt < 1)
			return 2;
		grayL = double(nMaxAve) / nCnt;
		grayR = double(nMinAve) / nCnt;
		dMtfR = fabs(grayL - grayR) / nMTFfactor;

		//up
		x1 = m_sChartBlobs.EdgeUp[n][0].x;
		x2 = m_sChartBlobs.EdgeUp[n][1].x;
		if (x2 > src.size.width - 2)
			x2 = src.size.width - 2;
		dx = x2 - x1;
		if (dx < 1)
			return 1;
		y1 = m_sChartBlobs.EdgeUp[n][0].y;
		y2 = m_sChartBlobs.EdgeUp[n][1].y;

		nCnt = nMaxAve = nMinAve = 0;
		for (x = x1; x < x2; x += 2)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;

			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			nMax = 0;
			nMin = 255;
			pos = ys * src.size.width;
			for (y = ys; y <= ye; y++)
			{
				if (nMax < src.ptr[pos + x])
					nMax = src.ptr[pos + x];
				if (nMax < src.ptr[pos + x + 1])
					nMax = src.ptr[pos + x + 1];
				if (nMin > src.ptr[pos + x])
					nMin = src.ptr[pos + x];
				if (nMin > src.ptr[pos + x + 1])
					nMin = src.ptr[pos + x + 1];
				pos += src.size.width;
			}

			nCnt++;
			nMaxAve += nMax;
			nMinAve += nMin;
		}
		if (nCnt < 1)
			return 1;
		grayU = double(nMaxAve) / nCnt;
		grayD = double(nMinAve) / nCnt;
		dMtfU = fabs(grayU - grayD) / nMTFfactor;

		//down
		x1 = m_sChartBlobs.EdgeDown[n][0].x;
		x2 = m_sChartBlobs.EdgeDown[n][1].x;
		if (x2 > src.size.width - 2)
			x2 = src.size.width - 2;
		dx = x2 - x1;
		if (dx < 1)
			return 1;
		y1 = m_sChartBlobs.EdgeDown[n][0].y;
		y2 = m_sChartBlobs.EdgeDown[n][1].y;

		nCnt = nMaxAve = nMinAve = 0;
		for (x = x1; x < x2; x += 2)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;

			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			nMax = 0;
			nMin = 255;
			pos = ys * src.size.width;
			for (y = ys; y <= ye; y++)
			{
				if (nMax < src.ptr[pos + x])
					nMax = src.ptr[pos + x];
				if (nMax < src.ptr[pos + x + 1])
					nMax = src.ptr[pos + x + 1];
				if (nMin > src.ptr[pos + x])
					nMin = src.ptr[pos + x];
				if (nMin > src.ptr[pos + x + 1])
					nMin = src.ptr[pos + x + 1];
				pos += src.size.width;
			}

			nCnt++;
			nMaxAve += nMax;
			nMinAve += nMin;
		}
		if (nCnt < 1)
			return 1;
		grayU = double(nMaxAve) / nCnt;
		grayD = double(nMinAve) / nCnt;
		dMtfD = fabs(grayU - grayD) / nMTFfactor;

		//MTF of one blob
		pMTF[n] = (dMtfL + dMtfR + dMtfU + dMtfD) / 4;
		//pMTF[n] = max(max(dMtfL, dMtfR), max(dMtfU, dMtfD));
	}

	return 0;
}

int CAaAlgorithm::AaMtfGradMax(const IMG_UBBUF src, double pMTF[CHART_BLOB_NUM])
{
	int nSFRwidth = 3;
	int nMTFfactor = 150;

	int pos = 0, pos1 = 0;
	int n = 0, m = 0;

	//initialize
	memset(pMTF, 0, CHART_BLOB_NUM * sizeof(double));

	int nCnt = 0, nMax = 0, nMaxAve = 0, nGrad = 0;
	double grayL, grayR, grayU, grayD, dMtfL, dMtfR, dMtfU, dMtfD;
	int x = 0, x0 = 0, x1 = 0, x2 = 0, y = 0, y0 = 0, y1 = 0, y2 = 0, dy = 0, dx = 0;
	int xs, xe, ys, ye;

	for (n = 0; n < CHART_BLOB_NUM; n++)
	{
		//left
		y1 = m_sChartBlobs.EdgeLeft[n][0].y;
		y2 = m_sChartBlobs.EdgeLeft[n][1].y;
		if (y2 > src.size.height - 2)
			y2 = src.size.height - 2;
		dy = y2 - y1;
		if (dy < 1)
			return 1;
		x1 = m_sChartBlobs.EdgeLeft[n][0].x;
		x2 = m_sChartBlobs.EdgeLeft[n][1].x;

		nCnt = nMaxAve = 0;
		pos = y1 * src.size.width;
		for (y = y1; y < y2; y++)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;

			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			nMax = 0;
			for (x = xs; x <= xe; x++)
			{
				nGrad = src.ptr[pos + x] - src.ptr[pos + x + 1];
				if (nMax < nGrad)
					nMax = nGrad;
			}

			nCnt++;
			nMaxAve += nMax;
			pos += src.size.width;
		}
		if (nCnt < 1)
			return 1;
		grayL = double(nMaxAve) / nCnt;
		dMtfL = grayL / nMTFfactor;

		//right
		y1 = m_sChartBlobs.EdgeRight[n][0].y;
		y2 = m_sChartBlobs.EdgeRight[n][1].y;
		if (y2 > src.size.height - 2)
			y2 = src.size.height - 2;
		dy = y2 - y1;
		if (dy < 1)
			return 2;
		x1 = m_sChartBlobs.EdgeRight[n][0].x;
		x2 = m_sChartBlobs.EdgeRight[n][1].x;

		nCnt = nMaxAve = 0;
		pos = y1 * src.size.width;
		for (y = y1; y <= y2; y++)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;

			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			nMax = 0;
			for (x = xs; x <= xe; x++)
			{
				nGrad = src.ptr[pos + x] - src.ptr[pos + x - 1];
				if (nMax < nGrad)
					nMax = nGrad;
			}

			nCnt++;
			nMaxAve += nMax;
			pos += src.size.width;
		}
		if (nCnt < 1)
			return 2;
		grayR = double(nMaxAve) / nCnt;
		dMtfR = grayR / nMTFfactor;

		//up
		x1 = m_sChartBlobs.EdgeUp[n][0].x;
		x2 = m_sChartBlobs.EdgeUp[n][1].x;
		if (x2 > src.size.width - 2)
			x2 = src.size.width - 2;
		dx = x2 - x1;
		if (dx < 1)
			return 1;
		y1 = m_sChartBlobs.EdgeUp[n][0].y;
		y2 = m_sChartBlobs.EdgeUp[n][1].y;

		nCnt = nMaxAve = 0;
		for (x = x1; x < x2; x++)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;

			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			nMax = 0;
			pos = ys * src.size.width;
			for (y = ys; y <= ye; y++)
			{
				nGrad = src.ptr[pos + x] - src.ptr[pos + x + src.size.width];
				if (nMax < nGrad)
					nMax = nGrad;
				pos += src.size.width;
			}

			nCnt++;
			nMaxAve += nMax;
		}
		if (nCnt < 1)
			return 1;
		grayU = double(nMaxAve) / nCnt;
		dMtfU = grayU / nMTFfactor;

		//down
		x1 = m_sChartBlobs.EdgeDown[n][0].x;
		x2 = m_sChartBlobs.EdgeDown[n][1].x;
		if (x2 > src.size.width - 2)
			x2 = src.size.width - 2;
		dx = x2 - x1;
		if (dx < 1)
			return 1;
		y1 = m_sChartBlobs.EdgeDown[n][0].y;
		y2 = m_sChartBlobs.EdgeDown[n][1].y;

		nCnt = nMaxAve = 0;
		for (x = x1; x < x2; x++)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;

			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			nMax = 0;
			pos = ys * src.size.width;
			for (y = ys; y <= ye; y++)
			{
				nGrad = src.ptr[pos + x] - src.ptr[pos + x - src.size.width];
				if (nMax < nGrad)
					nMax = nGrad;
				pos += src.size.width;
			}

			nCnt++;
			nMaxAve += nMax;
		}
		if (nCnt < 1)
			return 1;
		grayD = double(nMaxAve) / nCnt;
		dMtfD = grayD / nMTFfactor;

		//MTF of one blob
		pMTF[n] = (dMtfL + dMtfR + dMtfU + dMtfD) / 4;
		//pMTF[n] = max(max(dMtfL, dMtfR), max(dMtfU, dMtfD));
	}

	return 0;
}

int CAaAlgorithm::AaMtfGradAve(const IMG_UBBUF src, double pMTF[CHART_BLOB_NUM])
{
	int nSFRwidth = 3;
	int nMTFfactor = 150;

	int pos = 0, pos1 = 0;
	int n = 0, m = 0;

	//initialize
	memset(pMTF, 0, CHART_BLOB_NUM * sizeof(double));

	int nCnt = 0, nGradAve = 0, nGrad = 0;
	double grayL, grayR, grayU, grayD, dMtfL, dMtfR, dMtfU, dMtfD;
	int x = 0, x0 = 0, x1 = 0, x2 = 0, y = 0, y0 = 0, y1 = 0, y2 = 0, dy = 0, dx = 0;
	int xs, xe, ys, ye;

	for (n = 0; n < CHART_BLOB_NUM; n++)
	{
		//left
		y1 = m_sChartBlobs.EdgeLeft[n][0].y;
		y2 = m_sChartBlobs.EdgeLeft[n][1].y;
		if (y2 > src.size.height - 2)
			y2 = src.size.height - 2;
		dy = y2 - y1;
		if (dy < 1)
			return 1;
		x1 = m_sChartBlobs.EdgeLeft[n][0].x;
		x2 = m_sChartBlobs.EdgeLeft[n][1].x;

		nCnt = nGradAve = 0;
		pos = y1 * src.size.width;
		for (y = y1; y < y2; y++)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;
			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			for (x = xs; x <= xe; x++)
			{
				nGrad = src.ptr[pos + x] - src.ptr[pos + x + 1];
				nGradAve += nGrad;
				nCnt++;
			}
			pos += src.size.width;
		}
		if (nCnt < 1)
			return 1;
		grayL = double(nGradAve) / nCnt;
		dMtfL = grayL / nMTFfactor;

		//right
		y1 = m_sChartBlobs.EdgeRight[n][0].y;
		y2 = m_sChartBlobs.EdgeRight[n][1].y;
		if (y2 > src.size.height - 2)
			y2 = src.size.height - 2;
		dy = y2 - y1;
		if (dy < 1)
			return 2;
		x1 = m_sChartBlobs.EdgeRight[n][0].x;
		x2 = m_sChartBlobs.EdgeRight[n][1].x;

		nCnt = nGradAve = 0;
		pos = y1 * src.size.width;
		for (y = y1; y <= y2; y++)
		{
			x0 = x1 + (y - y1) * (x2 - x1) / dy;
			xs = x0 - nSFRwidth;
			if (xs < 0)
				xs = 0;
			xe = x0 + nSFRwidth;
			if (xe >= src.size.width)
				xe = src.size.width - 1;
			for (x = xs; x <= xe; x++)
			{
				nGrad = src.ptr[pos + x] - src.ptr[pos + x - 1];
				nGradAve += nGrad;
				nCnt++;
			}
			pos += src.size.width;
		}
		if (nCnt < 1)
			return 2;
		grayR = double(nGradAve) / nCnt;
		dMtfR = grayR / nMTFfactor;

		//up
		x1 = m_sChartBlobs.EdgeUp[n][0].x;
		x2 = m_sChartBlobs.EdgeUp[n][1].x;
		if (x2 > src.size.width - 2)
			x2 = src.size.width - 2;
		dx = x2 - x1;
		if (dx < 1)
			return 1;
		y1 = m_sChartBlobs.EdgeUp[n][0].y;
		y2 = m_sChartBlobs.EdgeUp[n][1].y;

		nCnt = nGradAve = 0;
		for (x = x1; x < x2; x++)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;
			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			pos = ys * src.size.width;
			for (y = ys; y <= ye; y++)
			{
				nGrad = src.ptr[pos + x] - src.ptr[pos + x + src.size.width];
				nGradAve += nGrad;
				nCnt++;
				pos += src.size.width;
			}
		}
		if (nCnt < 1)
			return 1;
		grayU = double(nGradAve) / nCnt;
		dMtfU = grayU / nMTFfactor;

		//down
		x1 = m_sChartBlobs.EdgeDown[n][0].x;
		x2 = m_sChartBlobs.EdgeDown[n][1].x;
		if (x2 > src.size.width - 2)
			x2 = src.size.width - 2;
		dx = x2 - x1;
		if (dx < 1)
			return 1;
		y1 = m_sChartBlobs.EdgeDown[n][0].y;
		y2 = m_sChartBlobs.EdgeDown[n][1].y;

		nCnt = nGradAve = 0;
		for (x = x1; x < x2; x++)
		{
			y0 = y1 + (x - x1) * (y2 - y1) / dx;

			ys = y0 - nSFRwidth;
			if (ys < 0)
				ys = 0;
			ye = y0 + nSFRwidth;
			if (ye >= src.size.height - 1)
				ye = src.size.height - 2;
			pos = ys * src.size.width;
			for (y = ys; y <= ye; y++)
			{
				nGrad = src.ptr[pos + x] - src.ptr[pos + x - src.size.width];
				nGradAve += nGrad;
				nCnt++;
				pos += src.size.width;
			}
		}
		if (nCnt < 1)
			return 1;
		grayD = double(nGradAve) / nCnt;
		dMtfD = grayD / nMTFfactor;

		//MTF of one blob
		pMTF[n] = (dMtfL + dMtfR + dMtfU + dMtfD) / 4;
		//pMTF[n] = max(max(dMtfL, dMtfR), max(dMtfU, dMtfD));
	}

	return 0;
}

int CAaAlgorithm::AaLightDistribution(const IMG_UBBUF src, IMG_UBBUF dst)
{
	int nRangeR = 1;

	int status = 0;
	int x, y, pos;
	int size = src.size.width * src.size.height;

	int nMax = 0;
	for (x = 0; x < size; x++)
	{
		if (nMax < src.ptr[x])
			nMax = src.ptr[x];
	}

	int nT = nMax - m_nLightRangeD;
	for (x = 0; x < size; x++)
	{
		if (src.ptr[x] >= nT - nRangeR && src.ptr[x] <= nT + nRangeR)
			dst.ptr[x] = 255;
		else
			dst.ptr[x] = src.ptr[x];
	}

freeExit:
	return status;
}

int CAaAlgorithm::AaLightOpen(const IMG_UBBUF src, IMG_UBBUF dst)
{
	if (src.ptr == NULL)
		return 1;
	if (src.size.width < 100 || src.size.width > 2000 || src.size.height < 100 || src.size.height > 2000)
		return 1;
	if (dst.ptr == NULL)
		return 2;
	if (dst.size.width != src.size.width || dst.size.height != src.size.height)
		return 2;

	m_Alg2DBase.VisMorphologyOpen(src, dst);
	
	return 0;
}

int CAaAlgorithm::AaLightClose(const IMG_UBBUF src, IMG_UBBUF dst)
{
	if (src.ptr == NULL)
		return 1;
	if (src.size.width < 100 || src.size.width > 2000 || src.size.height < 100 || src.size.height > 2000)
		return 1;
	if (dst.ptr == NULL)
		return 2;
	if (dst.size.width != src.size.width || dst.size.height != src.size.height)
		return 2;

	m_Alg2DBase.VisMorphologyClose(src, dst);

	return 0;
}