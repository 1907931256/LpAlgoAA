#include "stdafx.h"
#include "VisAlgBlob.h"


CVisAlgBlob::CVisAlgBlob()
{
	m_minLabel = 100;
	m_maxLabel = 250;
	m_minAreaBlobIndex = 0;
	m_maxAreaBlobIndex = 0;
	memset(m_pBlobAreas, 0, 256 * sizeof(int));
}


CVisAlgBlob::~CVisAlgBlob()
{
}

/**********************************************/
// VisBC_LabelMarker, ����˵��:��ͼ���е�Blob�������ȡ��
// Input:
// 	Ipp8u *srcRoi, ����ͼƬ
//	IppiSize Roi,����Ȥ�����С
//     ...
// Output:
// 	int &markersNum, ���Blob��ĸ���
//     ...
// Return:
//     0 - ����
//     -1 - ���޴���
//     -2 - ���޴���
// Author: /04/27/2017
/**********************************************/
int CVisAlgBlob::BlobLabelMarker(IMG_UBBUF src,int &markersNum)
{
	__try
	{
		if (src.ptr == NULL || src.size.width < 10 || src.size.width > 5000 || src.size.height < 10 || src.size.height > 5000)
			throw 1;

		IppStatus status;
		Ipp8u *srcRoi = src.ptr;
		IppiSize Roi = { src.size.width,src.size.height };
		int markerStep = Roi.width * sizeof(Ipp8u);

		markersNum = 0;
	
		Ipp8u *pBuffer = NULL;
		int bufferSize = 0;

		status = ippiLabelMarkersGetBufferSize_8u_C1R(Roi, &bufferSize);
		if (status != ippStsNoErr)
			goto freeExit;

		pBuffer = ippsMalloc_8u(bufferSize);

		ippiLabelMarkers_8u_C1IR(srcRoi, markerStep, Roi, m_minLabel, m_maxLabel, ippiNormInf, &markersNum, pBuffer);

freeExit:
		ippsFree(pBuffer);
		return status;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}
}

int CVisAlgBlob::BlobAreas(IMG_UBBUF src)
{
	__try
	{
		if (src.ptr == NULL || src.size.width < 10 || src.size.width > 5000 || src.size.height < 10 || src.size.height > 5000)
			throw 1;

		Ipp8u *srcRoi = src.ptr;
		IppiSize Roi = { src.size.width,src.size.height };

		memset(m_pBlobAreas, 0, 256 * sizeof(int));
		int n;
		for (n = 0; n  < Roi.width*Roi.height; n ++)
		{
			m_pBlobAreas[srcRoi[n]]++;
		}

		int maxArea = 0;
		int minArea = 99999999;
		for (n = m_minLabel; n <= m_maxLabel; n++)
		{
			if (maxArea < m_pBlobAreas[n])
			{
				maxArea = m_pBlobAreas[n];
				m_maxAreaBlobIndex = n;
			}
			if (minArea > m_pBlobAreas[n])
			{
				minArea = m_pBlobAreas[n];
				m_minAreaBlobIndex = n;
			}
		}

		return 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}
}

int CVisAlgBlob::GetMaxAreaBlob(IMG_UBBUF src, int &maxBlobArea)
{
	__try
	{
		if (src.ptr == NULL || src.size.width < 10 || src.size.width > 5000 || src.size.height < 10 || src.size.height > 5000)
			throw 1;

		Ipp8u *srcRoi = src.ptr;
		IppiSize Roi = { src.size.width,src.size.height };

		int n;
		for (n = 0; n < Roi.width*Roi.height; n++)
		{
			if(srcRoi[n] != 0 && srcRoi[n] != m_maxAreaBlobIndex)
				srcRoi[n] = 0;
		}

		maxBlobArea = m_pBlobAreas[m_maxAreaBlobIndex];

		return 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}
}