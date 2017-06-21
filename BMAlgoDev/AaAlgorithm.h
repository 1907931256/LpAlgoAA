#pragma once

#include "ViType.h"
#include "VisAlg2DBase.h"
#include "VisAlgBlob.h"

struct AaStructChartArea
{
	CPoint TopLeft;
	CPoint TopRight;
	CPoint BottomLeft;
	CPoint BottomRight;
};
#define CHART_BLOB_NUM 9
#define CHART_BLOB_EDGE_MAX_PNTS 150
#define MTF_MAX_Z_POS 50
struct AaStructChartBlobs
{
	AaStructChartArea Blobs[CHART_BLOB_NUM];
	CPoint Centers[CHART_BLOB_NUM];
	CPoint EdgeLeft[CHART_BLOB_NUM][2];
	CPoint EdgeRight[CHART_BLOB_NUM][2];
	CPoint EdgeUp[CHART_BLOB_NUM][2];
	CPoint EdgeDown[CHART_BLOB_NUM][2];
};
struct AaStructMTF
{
	double ppMTF[MTF_MAX_Z_POS][CHART_BLOB_NUM];
	int nMTFzpos;
};

class CAaAlgorithm
{
public:
	CAaAlgorithm();
	~CAaAlgorithm();

	int AaChartArea(const IMG_UBBUF src, IMG_UBBUF dst, AaStructChartArea & sChartArea);
	int AaChartBlobs(const IMG_UBBUF src, IMG_UBBUF dst, AaStructChartBlobs & sChartBlobs);
	int AaMtfSfr(const IMG_UBBUF src, double pMTF[CHART_BLOB_NUM]);
	int AaMtfStdDev(const IMG_UBBUF src, double pMTF[CHART_BLOB_NUM]);
	int AaMtfMaxMin(const IMG_UBBUF src, double pMTF[CHART_BLOB_NUM]);
	int AaMtfGradMax(const IMG_UBBUF src, double pMTF[CHART_BLOB_NUM]);
	int AaMtfGradAve(const IMG_UBBUF src, double pMTF[CHART_BLOB_NUM]);

	int AaLightDistribution(const IMG_UBBUF src, IMG_UBBUF dst);
	int m_nLightRangeD;
	int AaLightOpen(const IMG_UBBUF src, IMG_UBBUF dst);
	int AaLightClose(const IMG_UBBUF src, IMG_UBBUF dst);

private:
	int detectBlobEdgeLeft(const unsigned char * pImg, const int width, const int height, const CPoint Center, int blobWidth, CPoint EdgeLeft[2]);
	int detectBlobEdgeRight(const unsigned char * pImg, const int width, const int height, const CPoint Center, int blobWidth, CPoint EdgeRight[2]);
	int detectBlobEdgeUp(const unsigned char * pImg, const int width, const int height, const CPoint Center, int blobWidth, CPoint EdgeUp[2]);
	int detectBlobEdgeDown(const unsigned char * pImg, const int width, const int height, const CPoint Center, int blobWidth, CPoint EdgeDown[2]);

private:
	CVisAlg2DBase m_Alg2DBase;
	CVisAlgBlob m_AlgBlob;

	AaStructChartArea m_sChartArea;
	AaStructChartBlobs m_sChartBlobs;
	AaStructMTF m_sMTF;
	int	m_nBlobNum;
	int m_nMaxBlobArea;
	int m_nBlobWidth;

	float m_fDistScaleX1;
	float m_fDistScaleX2;
	float m_fDistScaleY1;
	float m_fDistScaleY2;
	int	m_shift_factor;
	int m_nBlobRangeWidth;

};

