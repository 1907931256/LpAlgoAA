#pragma once
#include "ViType.h"
#include <ipp.h>
#include <mkl.h>


class CVisAlg2DBase
{
public:
	CVisAlg2DBase();
	~CVisAlg2DBase();

	//Pyramid
	int VisPyramid(IMG_UBBUF src, unsigned char * pDst, int & pyramid_width, int & pyramid_height, int level);

	// Filter
	int VisFilterMean(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst, const unsigned char kernelSize);
	int VisFilterGaussian(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst, const unsigned char winWidth);
	int VisFilterMedian(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst, const unsigned char winWidth);

	// Fitting
	int VisFittingCircular(const float *point_pos, const int m, StructCircle &circular_fit, const int iteration_times);
	int VisFittingArc(const float *point_pos, const int m, StructArc &arc_fit, const int iteration_times);

	//Gradient
	int VisFilterSobel_8u8u(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst, const unsigned char winWidth);

	// Segment
	int VisSegmentManual(const unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char * pDst, const unsigned char nT = 128);
	int VisSegmentOtsu(const unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char * pDst);
	int VisCalcOtsu(const unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char & nThres);
	int VisSegmentDynamic(const unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char * pDst, const unsigned int avgWinWidth = 15);
	int VisSegmentCntDots(unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char * pDst, const double fWhiteRatio = 0.3);

	// Histogram
	int VisHistogram(IMG_UBBUF src, Ipp32u *pHist, int nBins = 255);

	// Morphology
	int VisMorphologyErode(IMG_UBBUF src, IMG_UBBUF dst);
	int VisMorphologyDilation(IMG_UBBUF src, IMG_UBBUF dst);
	int VisMorphologyOpen(IMG_UBBUF src, IMG_UBBUF dst);
	int VisMorphologyClose(IMG_UBBUF src, IMG_UBBUF dst);

	// Edge (canny ...)

};

