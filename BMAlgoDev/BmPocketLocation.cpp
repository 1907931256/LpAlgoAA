#include "stdafx.h"
#include "Vis_assist.h"
#include "BmPocketLocation.h"
//#include<afx.h>
#include<windows.h>
using namespace std;

#define DEBUG

//#define  AREA_MIN 28800 //1800 * 16
//#define  AREA_MAX 80000 //5000 * 16
//#define  T_AREA_MIN 48000
//#define  T_AREA_MAX 92800
//#define  T_WIDTH_MAX 220		//old 88
//#define  T_HEIGHT_MAX 940 //235*4
//
#define T_HEIGHT_WIDTH_RATIO_MIN 4 
#define T_HEIGHT_WIDTH_RATIO_MAX 7
//
//#define  X_BIAS 440
//#define  Y_BIAS 440
//
//#define X_BIAS_ROI 16
//#define Y_BIAS_ROI 160
//#define ROI_WIDTH 280
//#define ROI_HEIGHT 120
//
//#define BLOCK_HEIGHT 100	//25 * 4
//#define  BLOCK_WIDTH 220	//55 * 4
//#define  BLOCK_SHIFT_X	128	//32 * 4
//#define  BLOCK_SHIFT_Y	504	//126 * 4
//
//#define  PROJECT_DIFF 2400	//600*4
//
//#define  DUT_AREA_MIN	240000	//15000 * 16
//
//#define LSHAPE_TURN 17600	//1100 * 16

#define  HALF_LENGTH 380	//同pocketStatus，判断tray是否在图内

CBmPocketLocation::CBmPocketLocation()
{
	m_pyramidLevel = 2;
	m_ucThreshold = 26;
	m_whiteValue = 220;
	m_meanValueMin = 30;
	m_meanValue = 40;
	m_stdDevValue = 26;
	m_areaMin = 28800;
	m_areaMax = 72000;
	m_LwidthMax = 140;
	m_LwidthMin = 100;
	m_LheightMax = 140;
	m_LheightMin = 100;
	m_DUT_areaMin = 88000;
	m_TareaMin = 48000;
	m_TareaMax = 92800;
	m_TwidthMax = 220;
	m_TheightMax = 940;
	m_projectPercent = 0.3;
	m_regionAreaPercent = 0.5;
	m_projectDiff = 2200;
	m_margin = 80;
	m_limit_Lwidth = 96;
	m_xBias = 440;
	m_yBias = 440;
	m_BlockHeight = 100;
	m_BlockWidth = 220;
	m_BlockShift_x = 128;
	m_BlockShift_y = 504;
	m_LshapeTurn = 17600;
	m_cenXjudge = 200;
	m_cenYjudge = 300;
	m_LshapeDiffX = 10;
	m_LshapeDiffY = 10;
	m_hisErasePro = 0.002;
	m_hisEraseMean = 60;

	if (m_pyramidLevel > 0)
	{
		m_areaMin /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));
		m_areaMax /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));
		m_TareaMin /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));
		m_TareaMax /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));
		m_TwidthMax /= pow(2, m_pyramidLevel);
		m_TheightMax /= pow(2, m_pyramidLevel);
		m_DUT_areaMin /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));

		m_margin /= pow(2, m_pyramidLevel);
		m_limit_Lwidth /= pow(2, m_pyramidLevel);

		m_xBias /= pow(2, m_pyramidLevel);
		m_yBias /= pow(2, m_pyramidLevel);


		m_BlockHeight /= pow(2, m_pyramidLevel);
		m_BlockWidth /= pow(2, m_pyramidLevel);
		m_BlockShift_x /= pow(2, m_pyramidLevel);
		m_BlockShift_y /= pow(2, m_pyramidLevel);

		m_projectDiff /= pow(2, m_pyramidLevel);

		m_LshapeTurn /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));
		m_cenXjudge /= pow(2, m_pyramidLevel);
		m_cenYjudge /= pow(2, m_pyramidLevel);
	}

	m_Tshape.index = 0;
	m_Tshape.pos = { 0,0 };
	m_areaProperIndex.clear();
	m_vecDUTindex.clear();
	m_LshapeIndex.clear();
	m_cenPoint = { 0,0 };
}

CBmPocketLocation::~CBmPocketLocation()
{
	std::vector <int>().swap(m_areaProperIndex);
	std::vector<std::vector <LShape> >().swap(m_LshapeIndex);
	std::vector <int>().swap(m_vecDUTindex);
}


int CBmPocketLocation::pyramid(IMG_UBBUF src, unsigned char * pDst, int & pyramid_width, int & pyramid_height, int level)
{
	__try
	{
		IppStatus   status = ippStsNoErr;

	IppiPyramid*  pPyrStruct = NULL;
	unsigned char **  pPyrImage = NULL;

	//init var
	//double sigma = 3;
	unsigned char* pSrc = src.ptr;
	IMG_SIZE roiSize = src.size;

	if (!pSrc) { status = ippStsNoMemErr; goto exit; }

	int srcStep = roiSize.width * sizeof(unsigned char);
	float      rate = 2.f;                  // Neighbour levels ratio
	signed short kernel[3] = { 1,1,1 };		// Separable symmetric kernel of odd length

											//signed short *kernel = (signed short *)malloc(3 * sizeof(signed short));
											//__GetGaussianKernel_dim1(kernel, 3, sigma);	// preserved

	int pyrBufferSize = 0;
	int pyrStructSize = 0;
	unsigned char   *pPyrBuffer = NULL;
	unsigned char   *pPyrStrBuffer = NULL;

	int      pyrLStateSize = 0;
	int      pyrLBufferSize = 0;
	unsigned char   *pPyrLStateBuf = NULL;
	unsigned char   *pPyrLBuffer = NULL;

	// Computes the temporary work buffer size
	status = ippiPyramidGetSize(&pyrStructSize, &pyrBufferSize, level,{ roiSize.width,roiSize.height }, rate);
	if (status < 0) goto exit;

	//pPyrBuffer = ippsMalloc_8u(pyrBufferSize);
	//pPyrStrBuffer = ippsMalloc_8u(pyrStructSize);
	pPyrBuffer = (unsigned char*)malloc(pyrBufferSize * sizeof(unsigned char));
	pPyrStrBuffer = (unsigned char*)malloc(pyrStructSize * sizeof(unsigned char));	//not pop
	if ((pyrBufferSize && !pPyrBuffer) || (pyrStructSize && !pPyrStrBuffer)) {
		status = ippStsNoMemErr; goto exit_proc;
	}

	// Initializes Gaussian structure for pyramids
	//pPyrStruct = (IppiPyramid*)malloc(level * sizeof(IppiPyramid));	
	status = ippiPyramidInit(&pPyrStruct, level,{ roiSize.width,roiSize.height }, rate, pPyrStrBuffer, pPyrBuffer);
	if (status < 0) goto exit_proc;

	// ????????????????Correct maximum scale level 
	level = pPyrStruct->level;

	// Allocate structures to calculate pyramid layers
	status = ippiPyramidLayerDownGetSize_8u_C1R({ roiSize.width,roiSize.height }, rate, 3, &pyrLStateSize, &pyrLBufferSize);
	if (status < 0) goto exit_proc;

	//pPyrLStateBuf = ippsMalloc_8u(pyrLStateSize);
	//pPyrLBuffer = ippsMalloc_8u(pyrLBufferSize);
	pPyrLStateBuf = (unsigned char*)malloc(pyrLStateSize * sizeof(unsigned char));
	pPyrLBuffer = (unsigned char*)malloc(pyrLBufferSize * sizeof(unsigned char));
	if ((pyrLStateSize && !pPyrLStateBuf) || (pyrLBufferSize && !pPyrLBuffer)) { status = ippStsNoMemErr; goto exit; }

	// Initialize the structure for creating a lower pyramid layer
	status = ippiPyramidLayerDownInit_8u_C1R((IppiPyramidDownState_8u_C1R**)&pPyrStruct->pState,{ roiSize.width,roiSize.height }, rate, kernel, 3, IPPI_INTER_LINEAR, pPyrLStateBuf, pPyrLBuffer);
	if (status < 0) goto exit_proc;

	// Allocate pyramid layers
	pPyrImage = pPyrStruct->pImage;
	pPyrImage[0] = pSrc;
	pPyrStruct->pStep[0] = srcStep;

	for (int i = 1; i <= level; i++)
	{
		//pPyrImage[i] = ippiMalloc_8u_C1(pPyrStruct->pRoi[i].width, pPyrStruct->pRoi[i].height, &pPyrStruct->pStep[i]);
		pPyrImage[i] = (unsigned char*)malloc((pPyrStruct->pRoi[i].width) * (pPyrStruct->pRoi[i].height) * sizeof(unsigned char));
		pPyrStruct->pStep[i] = (pPyrStruct->pRoi[i].width) * sizeof(unsigned char);
		if (!pPyrImage[i]) { status = ippStsNoMemErr; goto exit_proc; }
	}

	// Perform downsampling of the image with 5x5 Gaussian kernel
	for (int i = 1; i <= level; i++)
	{
		status = ippiPyramidLayerDown_8u_C1R(pPyrImage[i - 1], pPyrStruct->pStep[i - 1], pPyrStruct->pRoi[i - 1], pPyrImage[i], pPyrStruct->pStep[i], pPyrStruct->pRoi[i], (IppiPyramidDownState_8u_C1R*)pPyrStruct->pState);
		if (status < 0) goto exit_proc;

	}

	for (int i = 0; i < pPyrStruct->pRoi[level].height; i++)
	{
		for (int j = 0; j < pPyrStruct->pRoi[level].width; j++)
		{
			pDst[i * pPyrStruct->pRoi[level].width + j] = pPyrImage[level][i * pPyrStruct->pRoi[level].width + j];
		}
	}
	pyramid_width = pPyrStruct->pRoi[level].width;
	pyramid_height = pPyrStruct->pRoi[level].height;




exit_proc:
	for (int i = 1; i <= level; i++)
		free(pPyrImage[i]);
	free(pPyrStrBuffer);
	free(pPyrBuffer);
	free(pPyrLBuffer);
	free(pPyrLStateBuf);

	goto exit;

exit:

	if (!status)
	{
	}
		return status;
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{

		return _CODE_THROW;
	}
}


//直方图均衡化
//HistogramEqualization直方图均衡化：
//Input
//IMG_UBBUF src  输入图像
//OutPut
//IMG_UBBUF dst  输出图像
//Return
// 0   正常
//-1   异常
//作者/日期  姜贺/20170407
int CBmPocketLocation::VisHistogramEqualization(IMG_UBBUF src, IMG_UBBUF dst)
{
	__try
	{
		//1、统计每个灰度值出现的次数
		IppStatus status = ippStsNoErr;

		IppDataType dataType = ipp8u;
		IppiSize srcSize = { src.size.width,src.size.height };
		int srcStep = src.size.width * sizeof(unsigned char);

		int nBins = 255;
		int nLevels[] = { nBins + 1 };
		int uniform = 1;
		int specSize = 0;
		int bufferSize = 0;

		status = ippiHistogramGetBufferSize(dataType, srcSize, nLevels, 1, uniform, &specSize, &bufferSize);
		if (status != ippStsNoErr) return -1;

		Ipp32f lowerLevel[] = { 0 };
		Ipp32f upperLevel[] = { 255 };
		IppiHistogramSpec *Spec = NULL;
		Spec = (IppiHistogramSpec*)ippsMalloc_8u(specSize);
		status = ippiHistogramUniformInit(dataType, lowerLevel, upperLevel, nLevels, 1, Spec);
		if (status != ippStsNoErr)
		{
			ippsFree(Spec);
			return -1;
		}

		Ipp32f *pLevels, *ppLevels[1];
		pLevels = (Ipp32f*)malloc((nBins + 1) * sizeof(Ipp32f));

		ppLevels[0] = pLevels;
		status = ippiHistogramGetLevels(Spec, ppLevels);
		//printf("%d \n",pLevels);
		//num = nLevels[0];

		Ipp32u *pHist;
		pHist = (Ipp32u*)malloc((nBins + 1) * sizeof(Ipp32u));
		Ipp8u *buffer = NULL;
		buffer = (Ipp8u*)malloc(bufferSize * sizeof(Ipp8u));

		status = ippiHistogram_8u_C1R(src.ptr, srcStep, srcSize, pHist, Spec, buffer);

		free(buffer);
		ippsFree(Spec);

		if (status != ippStsNoErr)
		{
			return -1;
		}

		/*FILE *Hist;
		Hist = fopen("E:\\ProjectBlob\\Hist.txt", "w");
		for (int i = 0; i < 255; i++)
		{
		fprintf(Hist, "%d   %d\n",i, pHist[i]);
		}*/


		//2、统计归一化的直方图,存放在数组 double Prob[256]
		double *Prob = (double*)malloc(256 * sizeof(double));
		int N = src.size.height*src.size.width;
		Prob[0] = pHist[0] * 1.0 / N;
		for (int i = 1; i < 256; i++)
		{
			for (int j = 0; j < i; j++)
			{
				Prob[i] = Prob[i - 1] + pHist[i] * 1.0 / N;
			}
		}

		//3、计算新的像素值
		//找出最大、最小灰度值
		int maxVal = src.ptr[0];
		int minVal = src.ptr[0];
		for (int i = 0; i < N; i++)
		{
			if (src.ptr[i] > maxVal)
			{
				maxVal = src.ptr[i];
			}
			else
			{
				if (src.ptr[i] < minVal)
				{
					minVal = src.ptr[i];
				}
			}
		}

		//计算新的灰度值
		int tem = 0;
		for (int i = 0; i < N; i++)
		{
			tem = (int)(Prob[src.ptr[i]] * (maxVal - minVal) + minVal);
			if (tem > 255)
			{
				dst.ptr[i] = 255;
			}
			else
			{
				if (tem < 0)
				{
					dst.ptr[i] = 0;
				}
				else
				{
					dst.ptr[i] = tem;
				}
			}
		}

		//free
		free(Prob);
		free(pHist);
		free(pLevels);
		return 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


//直方图规定化
//HistogramSpecification图像直方图规定化功能说明：给定一幅图和规定的均值和方差，计算规定化的图像
//Input
//src  输入图像
//meanValue  规定的灰度的均值
//variance   规定的灰度的标准差
//Output
//dst   由给定的均值和标准差规定化的图像
//Return
// 0   正常
//-1   异常
//作者/日期  姜贺/20170407
int CBmPocketLocation::VisHistogramSpecification(IMG_UBBUF src, float meanValue, float stdDevValue, IMG_UBBUF dst)
{
	__try
	{
		//0、图像信息判断
		if (src.ptr == NULL || src.size.height < 1 || src.size.width < 1 || src.linestep != src.size.width)
		{
			return -1;
		}

		memcpy(dst.ptr, src.ptr, src.size.height*src.size.width * sizeof(unsigned char));

		//1、Compute the mean and stand eviation
		IppStatus status = ippStsNoErr;

		int srcStep = src.size.width * sizeof(unsigned char);
		IppiSize srcSize = { src.size.width,src.size.height };
		Ipp64f pMean = 0;
		Ipp64f pStdDev = 0;
		status = ippiMean_StdDev_8u_C1R(src.ptr, srcStep, srcSize, &pMean, &pStdDev);

		if (status != ippStsNoErr) return -1;

		//2、改变平均值
		int meanSub = (int)(meanValue - pMean);
		int tem1 = 0;
		/*for (int i = 0; i < src.size.width*src.size.height; i++)
		{
		tem1 = src.ptr[i] + meanSub;
		if (tem1 > 255)
		{
		tem1 = 255;
		}
		if (tem1 < 0)
		{
		tem1 = 0;
		}
		dst.ptr[i] = tem1;
		}
		*/
		//printf("%lf\n", pMean);
		//printf("%lf\n",pStdDev);

		/*FILE *Mea;
		Mea = fopen("E:\\ProjectBlob\\MeanChange.txt", "w");
		for (int i = 0; i<src.size.height; i++)
		{
		for (int j = 0; j < src.size.width; j++)
		{
		fprintf(Mea, "%d   ", dst.ptr[j + i*(src.size.width)]);
		}
		fprintf(Mea, "\n");
		}
		fclose(Mea);*/

		//3、改变标准差
		//int stdDevSub = stdDevValue - pStdDev;
		//int N = src.size.height*src.size.width;
		float stdDevDivide = stdDevValue / pStdDev;

		int pos = 0;//src.size.width*796;//0;
		float newValue = 0;
		int tem2 = 0;
		for (int i = 0; i < src.size.height; i++)
		{
			for (int j = 0; j < src.size.width; j++)
			{
				//改变平均值
				tem1 = src.ptr[i] + meanSub;
				if (tem1 > 255)
				{
					tem1 = 255;
				}
				if (tem1 < 0)
				{
					tem1 = 0;
				}
				dst.ptr[i] = tem1;

				//改变标准差
				if (dst.ptr[j + pos] >= meanValue)
				{
					newValue = stdDevDivide*(dst.ptr[j + pos] - meanValue);
					tem2 = (int)(newValue + meanValue);

					if (tem2 > 255)
					{
						tem2 = 255;
					}
					if (tem2 < 0)
					{
						tem2 = 0;
					}
					dst.ptr[j + pos] = tem2;
				}
				else
				{
					newValue = stdDevDivide*(meanValue - src.ptr[j + pos]);
					tem2 = (int)(meanValue - newValue);

					if (tem2 > 255)
					{
						tem2 = 255;
					}
					if (tem2 < 0)
					{
						tem2 = 0;
					}
					dst.ptr[j + pos] = tem2;
				}
			}
			pos += src.size.width;
		}
		status = ippiMean_StdDev_8u_C1R(dst.ptr, srcStep, srcSize, &pMean, &pStdDev);
		return 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// threshold_OTSU, OTSU阈值分割
// Input:
// unsigned char * ucpSrc ， 输入图像指针
// IppiSize imgSize, roi大小
//
// Output:
// unsigned char * ucpDst ， 输出图像指针
//
// Return:
//	ippstatus
// Author: Jimmy Zhan  3/5/2017
/**********************************************/
int CBmPocketLocation::threshold_OTSU(unsigned char * ucpSrc, IppiSize imgSize, unsigned char * ucpDst)
{
	__try
	{
		int status = ippStsNoErr;
		int width = imgSize.width;
		int height = imgSize.height;

		//	do threshold	//
		int srcStep = width * sizeof(unsigned char);
		unsigned char *pThreshold = ucpSrc;
		status = ippiComputeThreshold_Otsu_8u_C1R(ucpSrc, srcStep, imgSize, pThreshold);

		unsigned char OSTUthreshold = *pThreshold;		//important
		int pos = 0;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if ((ucpSrc[pos + j]) <= OSTUthreshold)
				{
					ucpDst[pos + j] = 0;
				}
				else
				{
					ucpDst[pos + j] = 255;
				}
			}
			pos += width;
		}

		return status;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// threshold_manual, 手动阈值分割
// Input:
// unsigned char * ucpSrc ， 输入图像指针
// IppiSize imgSize, roi大小
//
// Output:
// unsigned char * ucpDst ， 输出图像指针
//
// Return:
//	ippstatus
//
// Author: Jimmy Zhan  3/5/2017
/**********************************************/
int CBmPocketLocation::threshold_manual(unsigned char * ucpSrc, IppiSize imgSize, unsigned char * ucpDst)
{
	__try
	{
		int status = ippStsNoErr;
		int width = imgSize.width;
		int height = imgSize.height;

		//	do threshold	//
		int srcStep = width * sizeof(unsigned char);
		int dstStep = srcStep;
		status = ippiThreshold_LTValGTVal_8u_C1R(ucpSrc, srcStep, ucpDst, dstStep, imgSize, m_ucThreshold, 0, m_ucThreshold, 255);

		return status;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


/**********************************************/
// labelMarker, 连通域标记
// Input:
// unsigned char * ucpSrc ， 输入图像指针
// IppiSize imgSize, roi大小
//
// Output:
// int * intpDst, 标记图像
// int & regionNum， 连通域数目
//
// Return:
//	ippstatus
// Author: Jimmy Zhan  3/5/2017
/**********************************************/
int CBmPocketLocation::labelMarker(unsigned char *ucpSrc, IppiSize imgSize, int *intpDst, int &regionNum)
{
	__try
	{
		int status = 0;

		int srcStep = imgSize.width * sizeof(Ipp8u);
		int dstStep = imgSize.width * sizeof(Ipp32s);
		int minLabel = 1;                      /* Minimal label value */
		int maxLabel = imgSize.width * imgSize.height + 1;     /* Maximal label value */
		int markersNum;                        /* Pointer to number of markers */
		Ipp8u* pBuffer = NULL;                 /* Pointer to the work buffer */
		int bufferSize = 0;

		/* Calculate size of temporary buffer */
		status = ippiLabelMarkersGetBufferSize_8u32s_C1R(imgSize, &bufferSize);
		pBuffer = ippsMalloc_8u(bufferSize);

		status = ippiLabelMarkers_8u32s_C1R(ucpSrc, srcStep, intpDst, dstStep, imgSize, minLabel, maxLabel, ippiNormInf, &markersNum, pBuffer);
		regionNum = markersNum + 1;

		//free
		ippiFree(pBuffer);

		return status;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


int CBmPocketLocation::morfDilateBorder_notInPlace_3by3(IMG_UBBUF src, unsigned char *pDst)
{
	__try
	{
		IppStatus status = ippStsNoErr;

		unsigned char *pSrc = src.ptr;
		IMG_SIZE roiSize = src.size;

		IppiMorphState* pSpec = NULL;
		Ipp8u* pBuffer = NULL;
		Ipp8u pMask[3 * 3] = { 1, 1, 1,
			1, 0, 1,
			1, 1, 1 };
		IppiSize maskSize = { 3, 3 };
		int srcStep = roiSize.width * sizeof(Ipp8u);
		int dstStep = roiSize.width * sizeof(Ipp8u);
		//int dstSize = roiSize.width;

		int specSize = 0, bufferSize = 0;
		IppiBorderType borderType = ippBorderRepl;
		Ipp8u borderValue = 0;

		status = ippiMorphologyBorderGetSize_8u_C1R({ roiSize.width,roiSize.height }, maskSize, &specSize, &bufferSize);
		if (status != ippStsNoErr) return status;

		pSpec = (IppiMorphState*)ippsMalloc_8u(specSize);
		pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

		status = ippiMorphologyBorderInit_8u_C1R({ roiSize.width,roiSize.height }, pMask, maskSize, pSpec, pBuffer);
		if (status != ippStsNoErr) {
			ippsFree(pBuffer);
			ippsFree(pSpec);
			return status;
		}

		status = ippiDilateBorder_8u_C1R(pSrc, srcStep, pDst, dstStep,{ roiSize.width,roiSize.height }, borderType, borderValue, pSpec, pBuffer);

		ippsFree(pBuffer);
		ippsFree(pSpec);

		return status;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


int CBmPocketLocation::morfErodeBorder_notInPlace_3by3(IMG_UBBUF src, unsigned char *pDst)
{
	__try
	{
		IppStatus status = ippStsNoErr;

		unsigned char *pSrc = src.ptr;
		IMG_SIZE roiSize = src.size;

		IppiMorphState* pSpec = NULL;
		Ipp8u* pBuffer = NULL;
		Ipp8u pMask[3 * 3] = { 1, 1, 1,
			1, 0, 1,
			1, 1, 1 };
		IppiSize maskSize = { 3, 3 };
		int srcStep = roiSize.width * sizeof(Ipp8u);
		int dstStep = roiSize.width * sizeof(Ipp8u);
		//int dstSize = roiSize.width;

		int specSize = 0, bufferSize = 0;
		IppiBorderType borderType = ippBorderRepl;
		Ipp8u borderValue = 0;

		status = ippiMorphologyBorderGetSize_8u_C1R({ roiSize.width,roiSize.height }, maskSize, &specSize, &bufferSize);
		if (status != ippStsNoErr) return status;

		pSpec = (IppiMorphState*)ippsMalloc_8u(specSize);
		pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

		status = ippiMorphologyBorderInit_8u_C1R({ roiSize.width,roiSize.height }, pMask, maskSize, pSpec, pBuffer);
		if (status != ippStsNoErr) {
			ippsFree(pBuffer);
			ippsFree(pSpec);
			return status;
		}

		status = ippiErodeBorder_8u_C1R(pSrc, srcStep, pDst, dstStep,{ roiSize.width,roiSize.height }, borderType, borderValue, pSpec, pBuffer);

		ippsFree(pBuffer);
		ippsFree(pSpec);

		return status;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


int CBmPocketLocation::morfErodeBorder_notInPlace_5by5(IMG_UBBUF src, unsigned char *pDst)
{
	__try
	{
		IppStatus status = ippStsNoErr;

		unsigned char *pSrc = src.ptr;
		IMG_SIZE roiSize = src.size;

		IppiMorphState* pSpec = NULL;
		Ipp8u* pBuffer = NULL;
		Ipp8u pMask[5 * 5] = { 1, 1, 1,1,1,
			1,1, 0, 1,1,
			1, 1, 1,1,1 };
		IppiSize maskSize = { 5, 5 };
		int srcStep = roiSize.width * sizeof(Ipp8u);
		int dstStep = roiSize.width * sizeof(Ipp8u);
		//int dstSize = roiSize.width;

		int specSize = 0, bufferSize = 0;
		IppiBorderType borderType = ippBorderRepl;
		Ipp8u borderValue = 0;

		status = ippiMorphologyBorderGetSize_8u_C1R({ roiSize.width,roiSize.height }, maskSize, &specSize, &bufferSize);
		if (status != ippStsNoErr) return status;

		pSpec = (IppiMorphState*)ippsMalloc_8u(specSize);
		pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

		status = ippiMorphologyBorderInit_8u_C1R({ roiSize.width,roiSize.height }, pMask, maskSize, pSpec, pBuffer);
		if (status != ippStsNoErr) {
			ippsFree(pBuffer);
			ippsFree(pSpec);
			return status;
		}

		status = ippiErodeBorder_8u_C1R(pSrc, srcStep, pDst, dstStep,{ roiSize.width,roiSize.height }, borderType, borderValue, pSpec, pBuffer);

		ippsFree(pBuffer);
		ippsFree(pSpec);

		return status;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


/**********************************************/
// eraseEdgeRegion, 排除边界连通域
// Return:
//	true : 排除
//  false:不排除
// Author: Jimmy Zhan  3/5/2017
/**********************************************/
bool CBmPocketLocation::eraseEdgeRegion(
	regionDef::RegionPeakPoint regionPeakPoint,
	int width,
	int height
)
{
	__try
	{
		if (regionPeakPoint.xMin == 0 && regionPeakPoint.xMax == width - 1)
		{
			return true;
		}
		if (regionPeakPoint.yMin == 0 && regionPeakPoint.yMax == height - 1)
		{
			return true;
		}
		if (regionPeakPoint.xMin == 0 && regionPeakPoint.yMin == 0)
		{
			return true;
		}
		if (regionPeakPoint.xMin == 0 && regionPeakPoint.yMax == height - 1)
		{
			return true;
		}
		if (regionPeakPoint.xMax == width - 1 && regionPeakPoint.yMin == 0)
		{
			return true;
		}
		if (regionPeakPoint.xMax == width - 1 && regionPeakPoint.yMax == height - 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}




/**********************************************/
// eraseCenterRegion, 排除中间（T型块）间的连通域
// Return:
//	true : 排除
//  false:不排除
// Author: Jimmy Zhan  3/5/2017
/**********************************************/
bool CBmPocketLocation::eraseCenterRegion(
	IppiPoint regionGraCenPoint,
	regionDef::RegionPeakPoint regionPeakPoint,
	int margin
)
{
	__try
	{
		if (regionGraCenPoint.y > regionPeakPoint.yMin + margin && regionGraCenPoint.y < regionPeakPoint.yMax - margin)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}

/**********************************************/
// judgeRealLShape, 判断连通域是否为真正L型
// Return:
//	true : 是要找的L型
//  false: 不是要找的L型
// Author: Jimmy Zhan  3/5/2017
/**********************************************/
bool CBmPocketLocation::judgeRealLShape(
	int * intSrc,
	int width,
	int height,
	int index,
	int limit_Lwidth,
	IppiPoint regionGraCenPoint,
	regionDef::RegionWidthHeight regionWidthHeight,
	int regionNum
)
{
	__try
	{
		int xMin = width - 1;
		int xMax = 0;
		for (int i = regionGraCenPoint.y - 1; i <= regionGraCenPoint.y + 1; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (intSrc[i * width + j] == index)
				{
					if (j < xMin)
						xMin = j;
					if (j > xMax)
						xMax = j;
				}
			}
		}


		int L_width = xMax - xMin;
		if (L_width > limit_Lwidth)
		{
			return false;
		}
		else
		{
			int turnArea = 0;
			switch (regionNum)
			{
			case 0:
				for (int i = regionGraCenPoint.y; i > 0; i--)
				{
					for (int j = regionGraCenPoint.x; j > 0; j--)
					{
						if (intSrc[i * width + j] == index)
						{
							turnArea++;
						}
					}
				}
				break;

			case 1:
				for (int i = regionGraCenPoint.y; i > 0; i--)
				{
					for (int j = regionGraCenPoint.x; j < width; j++)
					{
						if (intSrc[i * width + j] == index)
						{
							turnArea++;
						}
					}
				}
				break;

			case 2:
				for (int i = regionGraCenPoint.y; i < height; i++)
				{
					for (int j = regionGraCenPoint.x; j > 0; j--)
					{
						if (intSrc[i * width + j] == index)
						{
							turnArea++;
						}
					}
				}
				break;

			case 3:
				for (int i = regionGraCenPoint.y; i < height; i++)
				{
					for (int j = regionGraCenPoint.x; j < width; j++)
					{
						if (intSrc[i * width + j] == index)
						{
							turnArea++;
						}
					}
				}
				break;
			default:
				break;
			}
			if (turnArea > m_LshapeTurn)
			{
				return false;
			}
			else
			{
				if (regionWidthHeight.width > m_LwidthMax || regionWidthHeight.width < m_LwidthMin || regionWidthHeight.height > m_LheightMax || regionWidthHeight.height < m_LheightMin)
				{
					return false;
				}
				else
				{
					return true;
				}

			}
		}

	}

		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

}


/**********************************************/
// getRegionInfo, blob分析找L型
// Input:
// int * intSrc ， 输入已标记过的图像
// IppiSize imgSize, roi大小
// int regionNum， 连通域数量
// 
// Output:
// int * regionArea, 各连通域面积
// IppiPoint * regionGraCenPoint， 各连通域重心
// RegionPeakPoint * regionPeakPoint， 各连通域极值点
// RegionWidthHeight * regionWH, 各连通域宽高
// ProjectScore *projectScore,各连通域x和y方向的投影分数
//
// Return:
//	0 ：至少找到一个L型
//  -1: 找不到L型
// Author: Jimmy Zhan  3/5/2017
/**********************************************/
int CBmPocketLocation::getRegionInfo(int * intSrc, 
									IppiSize imgSize, 
									int regionNum, 
									int *regionArea, 
									IppiPoint *regionGraCenPoint,
									regionDef::RegionPeakPoint *regionPeakPoint,
									regionDef::RegionWidthHeight *regionWH,
									ProjectScore *projectScore
									)
{
	__try
	{
		int width = imgSize.width;
		int height = imgSize.height;

		//init
		memset(regionArea, 0, regionNum * sizeof(int));
		for (int i = 0; i < regionNum; i++)
		{
			regionPeakPoint[i].xMax = 0;
			regionPeakPoint[i].xMin = width - 1;
			regionPeakPoint[i].yMax = 0;
			regionPeakPoint[i].yMin = height - 1;
			regionWH[i].height = 0;
			regionWH[i].width = 0;
			regionGraCenPoint[i].x = 0;
			regionGraCenPoint[i].y = 0;
		}
		m_Tshape.index = 0;
		m_Tshape.pos = { 0,0 };

		//for calculate gravaty-center
		int *xSum = new int[regionNum];
		int *ySum = new int[regionNum];
		memset(xSum, 0, regionNum * sizeof(int));
		memset(ySum, 0, regionNum * sizeof(int));


		int pos = 0;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int val = intSrc[pos + j];
				regionArea[val]++;
				xSum[val] += j;
				ySum[val] += i;
				if (j < regionPeakPoint[val].xMin)
					regionPeakPoint[val].xMin = j;
				if (j > regionPeakPoint[val].xMax)
					regionPeakPoint[val].xMax = j;
				if (i < regionPeakPoint[val].yMin)
					regionPeakPoint[val].yMin = i;
				if (i > regionPeakPoint[val].yMax)
					regionPeakPoint[val].yMax = i;

			}
			pos += width;
		}

		//	cal regionGraCenPoint & regionWH and find 'T' shape	//
		for (int k = 1; k < regionNum; k++)
		{
			if (regionArea[k] != 0)
			{
				regionGraCenPoint[k].x = xSum[k] / regionArea[k];
				regionGraCenPoint[k].y = ySum[k] / regionArea[k];
			}
			regionWH[k].width = regionPeakPoint[k].xMax - regionPeakPoint[k].xMin;
			regionWH[k].height = regionPeakPoint[k].yMax - regionPeakPoint[k].yMin;
			bool flag1 = eraseEdgeRegion(regionPeakPoint[k], width, height);
			if (!flag1)
			{
				if (regionArea[k] > m_TareaMin && regionArea[k] < m_TareaMax)
				{
					float h2wRatio = (float)regionWH[k].height / regionWH[k].width;
					if (h2wRatio < T_HEIGHT_WIDTH_RATIO_MAX && h2wRatio > T_HEIGHT_WIDTH_RATIO_MIN && regionWH[k].width < m_TwidthMax && regionWH[k].height < m_TheightMax)
					{
						if (regionPeakPoint[k].xMin != 0 && regionPeakPoint[k].xMax != width - 1 && regionPeakPoint[k].yMin != 0 && regionPeakPoint[k].yMax != height - 1)
						{
							m_Tshape.index = k;
							m_Tshape.pos = { regionGraCenPoint[k].x,regionGraCenPoint[k].y };
						}

					}
				}

			}

		}


		//	find 'L' shape in all connect region	//
		m_areaProperIndex.clear();
		m_LshapeIndex.clear();
		m_LshapeIndex.resize(4);
		for (int k = 1; k < regionNum; k++)
		{
			//edgeRegion erase
			bool flag1 = eraseEdgeRegion(regionPeakPoint[k], width, height);
			bool flag2;
			if (m_Tshape.index != 0)
			{
				flag2 = eraseCenterRegion(regionGraCenPoint[k], regionPeakPoint[m_Tshape.index], m_margin);
			}
			else
			{
				flag2 = false;
			}
			if (!flag1 && !flag2)
			{
				//area select 
				if (regionArea[k] > m_areaMin && regionArea[k] < m_areaMax)
				{
					m_areaProperIndex.push_back(k);

					int xStart = regionPeakPoint[k].xMin;
					int xEnd = regionPeakPoint[k].xMax;
					int yStart = regionPeakPoint[k].yMin;
					int yEnd = regionPeakPoint[k].yMax;
					
					if (abs(m_projectPercent) > 0.5)
						m_projectPercent = 0.5;
					//region 1
					if (regionGraCenPoint[k].x > 0 && regionGraCenPoint[k].x <= width / 2 && regionGraCenPoint[k].y > 0 && regionGraCenPoint[k].y <= height / 2)
					{
						//cal project score
						//pro y
						for (int i = yStart; i <= yStart + m_projectPercent * (yEnd - yStart); i++)
						{
							for (int j = xStart; j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectY++;
							}
						}

						//pro x
						for (int i = yStart; i <= yEnd; i++)
						{
							for (int j = xStart; j <= xStart + m_projectPercent * (xEnd - xStart); j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectX++;
							}
						}
						int voteMin = m_regionAreaPercent * regionArea[k];
						if (projectScore[k].projectX > voteMin || projectScore[k].projectY > voteMin)
						{
							if (abs(projectScore[k].projectX - projectScore[k].projectY) < m_projectDiff)
							{
								LShape _Lshape;
								_Lshape.index = k;
								_Lshape.pos = { (int)(regionGraCenPoint[k].x),(int)(regionGraCenPoint[k].y) };

								if (judgeRealLShape(intSrc, width, height, k, m_limit_Lwidth, _Lshape.pos,regionWH[k],0)  )
								{
									if (abs(regionGraCenPoint[k].x - width / 2) > m_cenXjudge || abs(regionGraCenPoint[k].y - height / 2) > m_cenYjudge)
									{
										if (m_LshapeIndex[0].size())
										{
											if (_Lshape.pos.x < m_LshapeIndex[0][0].pos.x && _Lshape.pos.y < m_LshapeIndex[0][0].pos.y)		//no inner shape
											{
												m_LshapeIndex[0].pop_back();
												m_LshapeIndex[0].push_back(_Lshape);
											}
										}
										else
										{
											m_LshapeIndex[0].push_back(_Lshape);
										}
									}

								}
							}

						}
					}
					
					//region 2
					if (regionGraCenPoint[k].x > width / 2 && regionGraCenPoint[k].x <= width - 1 && regionGraCenPoint[k].y > 0 && regionGraCenPoint[k].y <= height / 2)
					{
						//cal project score
						//pro y
						for (int i = yStart; i <= yStart + m_projectPercent * (yEnd - yStart); i++)
						{
							for (int j = xStart; j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectY++;
							}
						}

						//pro x
						for (int i = yStart; i <= yEnd; i++)
						{
							for (int j = xEnd - m_projectPercent * (xEnd - xStart); j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectX++;
							}
						}
						int voteMin = m_regionAreaPercent * regionArea[k];
						if (projectScore[k].projectX > voteMin || projectScore[k].projectY > voteMin)
						{
							if (abs(projectScore[k].projectX - projectScore[k].projectY) < m_projectDiff)
							{
								LShape _Lshape;
								_Lshape.index = k;
								_Lshape.pos = { (int)(regionGraCenPoint[k].x),(int)(regionGraCenPoint[k].y) };
								if (judgeRealLShape(intSrc, width, height, k, m_limit_Lwidth, _Lshape.pos, regionWH[k],1))
								{
									if (abs(regionGraCenPoint[k].x - width / 2) > m_cenXjudge || abs(regionGraCenPoint[k].y - height / 2) > m_cenYjudge)
									{
										if (m_LshapeIndex[1].size())
										{
											if (_Lshape.pos.x > m_LshapeIndex[1][0].pos.x  && _Lshape.pos.y < m_LshapeIndex[1][0].pos.y)
											{
												m_LshapeIndex[1].pop_back();
												m_LshapeIndex[1].push_back(_Lshape);
											}
										}
										else
										{
											m_LshapeIndex[1].push_back(_Lshape);
										}
									}

								}
							}

						}
					}


					//region 3
					if (regionGraCenPoint[k].x > 0 && regionGraCenPoint[k].x <= width / 2 && regionGraCenPoint[k].y > height / 2 && regionGraCenPoint[k].y <= height - 1)
					{
						//cal project score
						//pro y
						for (int i = yEnd - m_projectPercent * (yEnd - yStart); i <= yEnd; i++)
						{
							for (int j = xStart; j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectY++;
							}
						}

						//pro x
						for (int i = yStart; i <= yEnd; i++)
						{
							for (int j = xStart; j <= xStart + m_projectPercent * (xEnd - xStart); j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectX++;
							}
						}
						int voteMin = m_regionAreaPercent * regionArea[k];
						if (projectScore[k].projectX > voteMin || projectScore[k].projectY > voteMin)
						{
							if (abs(projectScore[k].projectX - projectScore[k].projectY) < m_projectDiff)
							{
								LShape _Lshape;
								_Lshape.index = k;
								_Lshape.pos = { (int)(regionGraCenPoint[k].x),(int)(regionGraCenPoint[k].y) };
								if (judgeRealLShape(intSrc, width, height, k, m_limit_Lwidth, _Lshape.pos, regionWH[k],2))
								{
									if (abs(regionGraCenPoint[k].x - width / 2) > m_cenXjudge || abs(regionGraCenPoint[k].y - height / 2) > m_cenYjudge)
									{
										if (m_LshapeIndex[2].size())
										{
											if (_Lshape.pos.x < m_LshapeIndex[2][0].pos.x && _Lshape.pos.y > m_LshapeIndex[2][0].pos.y)
											{
												m_LshapeIndex[2].pop_back();
												m_LshapeIndex[2].push_back(_Lshape);
											}
										}
										else
										{
											m_LshapeIndex[2].push_back(_Lshape);
										}
									}

								}
							}

						}
					}

					//region 4
					if (regionGraCenPoint[k].x > width / 2 && regionGraCenPoint[k].x <= width - 1 && regionGraCenPoint[k].y > height / 2 && regionGraCenPoint[k].y <= height - 1)
					{
						//cal project score
						//pro y
						for (int i = yEnd - m_projectPercent * (yEnd - yStart); i <= yEnd; i++)
						{
							for (int j = xStart; j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectY++;
							}
						}

						//pro x
						for (int i = yStart; i <= yEnd; i++)
						{
							for (int j = xEnd - m_projectPercent * (xEnd - xStart); j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectX++;
							}
						}
						int voteMin = m_regionAreaPercent * regionArea[k];
						if (projectScore[k].projectX > voteMin || projectScore[k].projectY > voteMin)
						{
							if (abs(projectScore[k].projectX - projectScore[k].projectY) < m_projectDiff)
							{
								LShape _Lshape;
								_Lshape.index = k;
								_Lshape.pos = { (int)(regionGraCenPoint[k].x),(int)(regionGraCenPoint[k].y) };
								if (judgeRealLShape(intSrc, width, height, k, m_limit_Lwidth, _Lshape.pos, regionWH[k],3))
								{
									if (abs(regionGraCenPoint[k].x - width / 2) > m_cenXjudge || abs(regionGraCenPoint[k].y - height / 2) > m_cenYjudge)
									{
										if (m_LshapeIndex[3].size())
										{
											if (_Lshape.pos.x > m_LshapeIndex[3][0].pos.x && _Lshape.pos.y > m_LshapeIndex[3][0].pos.y)
											{
												m_LshapeIndex[3].pop_back();
												m_LshapeIndex[3].push_back(_Lshape);
											}
										}
										else
										{
											m_LshapeIndex[3].push_back(_Lshape);
										}
									}

								}
							}

						}
					}
					//end 
				}
			}


		}


		//free
		if(xSum != nullptr)
			delete[] xSum;
		if(ySum != nullptr)
			delete[] ySum;

		int _size = 0;
		for (int i = 0; i < m_LshapeIndex.size(); i++)
		{
			_size += m_LshapeIndex[i].size();
		}
		if (_size == 0 || _size > 4)
		{
			return -1;
		}
		else
		{
			return 0;
		}

	}
	
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

int CBmPocketLocation::getRegionInfo2(int * intSrc,
	IppiSize imgSize,
	int regionNum,
	int *regionArea,
	IppiPoint *regionGraCenPoint,
	regionDef::RegionPeakPoint *regionPeakPoint,
	regionDef::RegionWidthHeight *regionWH,
	ProjectScore *projectScore
)
{
	__try
	{
		int width = imgSize.width;
		int height = imgSize.height;

		//init
		memset(regionArea, 0, regionNum * sizeof(int));
		for (int i = 0; i < regionNum; i++)
		{
			regionPeakPoint[i].xMax = 0;
			regionPeakPoint[i].xMin = width - 1;
			regionPeakPoint[i].yMax = 0;
			regionPeakPoint[i].yMin = height - 1;
			regionWH[i].height = 0;
			regionWH[i].width = 0;
			regionGraCenPoint[i].x = 0;
			regionGraCenPoint[i].y = 0;
		}
		m_Tshape.index = 0;
		m_Tshape.pos = { 0,0 };

		//for calculate gravaty-center
		int *xSum = new int[regionNum];
		int *ySum = new int[regionNum];
		memset(xSum, 0, regionNum * sizeof(int));
		memset(ySum, 0, regionNum * sizeof(int));


		int pos = 0;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int val = intSrc[pos + j];
				regionArea[val]++;
				xSum[val] += j;
				ySum[val] += i;
				if (j < regionPeakPoint[val].xMin)
					regionPeakPoint[val].xMin = j;
				if (j > regionPeakPoint[val].xMax)
					regionPeakPoint[val].xMax = j;
				if (i < regionPeakPoint[val].yMin)
					regionPeakPoint[val].yMin = i;
				if (i > regionPeakPoint[val].yMax)
					regionPeakPoint[val].yMax = i;

			}
			pos += width;
		}

		//	cal regionGraCenPoint & regionWH and find 'T' shape	//
		for (int k = 1; k < regionNum; k++)
		{
			if (regionArea[k] != 0)
			{
				regionGraCenPoint[k].x = xSum[k] / regionArea[k];
				regionGraCenPoint[k].y = ySum[k] / regionArea[k];
			}
			regionWH[k].width = regionPeakPoint[k].xMax - regionPeakPoint[k].xMin;
			regionWH[k].height = regionPeakPoint[k].yMax - regionPeakPoint[k].yMin;
			bool flag1 = eraseEdgeRegion(regionPeakPoint[k], width, height);
			if (!flag1)
			{
				if (regionArea[k] > m_TareaMin && regionArea[k] < m_TareaMax)
				{
					float h2wRatio = (float)regionWH[k].height / regionWH[k].width;
					if (h2wRatio < T_HEIGHT_WIDTH_RATIO_MAX && h2wRatio > T_HEIGHT_WIDTH_RATIO_MIN && regionWH[k].width < m_TwidthMax  && regionWH[k].height < m_TheightMax)
					{
						if (regionPeakPoint[k].xMin != 0 && regionPeakPoint[k].xMax != width - 1 && regionPeakPoint[k].yMin != 0 && regionPeakPoint[k].yMax != height - 1)
						{
							m_Tshape.index = k;
							m_Tshape.pos = { regionGraCenPoint[k].x,regionGraCenPoint[k].y };
						}

					}
				}

			}

		}


		//	find 'L' shape in all connect region	//
		m_areaProperIndex.clear();
		m_LshapeIndex.clear();
		m_LshapeIndex.resize(4);
		for (int k = 1; k < regionNum; k++)
		{
			//edgeRegion erase
			bool flag1 = eraseEdgeRegion(regionPeakPoint[k], width, height);
			bool flag2;
			if (m_Tshape.index != 0)
			{
				flag2 = eraseCenterRegion(regionGraCenPoint[k], regionPeakPoint[m_Tshape.index], m_margin);
			}
			else
			{
				flag2 = false;
			}
			if (!flag1 && !flag2)
			{
				//area select 
				if (regionArea[k] > m_areaMin && regionArea[k] < m_areaMax)
				{
					m_areaProperIndex.push_back(k);

					int xStart = regionPeakPoint[k].xMin;
					int xEnd = regionPeakPoint[k].xMax;
					int yStart = regionPeakPoint[k].yMin;
					int yEnd = regionPeakPoint[k].yMax;
					if (abs(m_projectPercent) > 0.5)
						m_projectPercent = 0.5;

					//region 1
					if (regionGraCenPoint[k].x > 0 && regionGraCenPoint[k].x <= width / 2 && regionGraCenPoint[k].y > 0 && regionGraCenPoint[k].y <= height / 2)
					{
						//cal project score
						//pro y
						for (int i = yStart; i <= yStart + m_projectPercent * (yEnd - yStart); i++)
						{
							for (int j = xStart; j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectY++;
							}
						}

						//pro x
						for (int i = yStart; i <= yEnd; i++)
						{
							for (int j = xStart; j <= xStart + m_projectPercent * (xEnd - xStart); j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectX++;
							}
						}
						int voteMin = m_regionAreaPercent * regionArea[k];
						if (projectScore[k].projectX > voteMin || projectScore[k].projectY > voteMin)
						{
							if (abs(projectScore[k].projectX - projectScore[k].projectY) < m_projectDiff)
							{
								LShape _Lshape;
								_Lshape.index = k;
								_Lshape.pos = { (int)(regionGraCenPoint[k].x),(int)(regionGraCenPoint[k].y) };
								if (judgeRealLShape(intSrc, width, height, k, m_limit_Lwidth, _Lshape.pos, regionWH[k],0))
								{
									if (m_LshapeIndex[0].size())
									{
										if (_Lshape.pos.x < m_LshapeIndex[0][0].pos.x && _Lshape.pos.y < m_LshapeIndex[0][0].pos.y)		//no inner shape
										{
											m_LshapeIndex[0].pop_back();
											m_LshapeIndex[0].push_back(_Lshape);
										}
									}
									else
									{
										m_LshapeIndex[0].push_back(_Lshape);
									}
								}

							}

						}
					}

					//region 2
					if (regionGraCenPoint[k].x > width / 2 && regionGraCenPoint[k].x <= width - 1 && regionGraCenPoint[k].y > 0 && regionGraCenPoint[k].y <= height / 2)
					{
						//cal project score
						//pro y
						for (int i = yStart; i <= yStart + m_projectPercent * (yEnd - yStart); i++)
						{
							for (int j = xStart; j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectY++;
							}
						}

						//pro x
						for (int i = yStart; i <= yEnd; i++)
						{
							for (int j = xEnd - m_projectPercent * (xEnd - xStart); j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectX++;
							}
						}
						int voteMin = m_regionAreaPercent * regionArea[k];
						if (projectScore[k].projectX > voteMin || projectScore[k].projectY > voteMin)
						{
							if (abs(projectScore[k].projectX - projectScore[k].projectY) < m_projectDiff)
							{
								LShape _Lshape;
								_Lshape.index = k;
								_Lshape.pos = { (int)(regionGraCenPoint[k].x),(int)(regionGraCenPoint[k].y) };
								if (judgeRealLShape(intSrc, width, height, k, m_limit_Lwidth, _Lshape.pos, regionWH[k],1))
								{
									if (m_LshapeIndex[1].size())
									{
										if (_Lshape.pos.x > m_LshapeIndex[1][0].pos.x  && _Lshape.pos.y < m_LshapeIndex[1][0].pos.y)
										{
											m_LshapeIndex[1].pop_back();
											m_LshapeIndex[1].push_back(_Lshape);
										}
									}
									else
									{
										m_LshapeIndex[1].push_back(_Lshape);
									}

								}
							}

						}
					}


					//region 3
					if (regionGraCenPoint[k].x > 0 && regionGraCenPoint[k].x <= width / 2 && regionGraCenPoint[k].y > height / 2 && regionGraCenPoint[k].y <= height - 1)
					{
						//cal project score
						//pro y
						for (int i = yEnd - m_projectPercent * (yEnd - yStart); i <= yEnd; i++)
						{
							for (int j = xStart; j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectY++;
							}
						}

						//pro x
						for (int i = yStart; i <= yEnd; i++)
						{
							for (int j = xStart; j <= xStart + m_projectPercent * (xEnd - xStart); j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectX++;
							}
						}
						int voteMin = m_regionAreaPercent * regionArea[k];
						if (projectScore[k].projectX > voteMin || projectScore[k].projectY > voteMin)
						{
							if (abs(projectScore[k].projectX - projectScore[k].projectY) < m_projectDiff)
							{
								LShape _Lshape;
								_Lshape.index = k;
								_Lshape.pos = { (int)(regionGraCenPoint[k].x),(int)(regionGraCenPoint[k].y) };
								if (judgeRealLShape(intSrc, width, height, k, m_limit_Lwidth, _Lshape.pos, regionWH[k],2))
								{
									if (m_LshapeIndex[2].size())
									{
										if (_Lshape.pos.x < m_LshapeIndex[2][0].pos.x && _Lshape.pos.y > m_LshapeIndex[2][0].pos.y)
										{
											m_LshapeIndex[2].pop_back();
											m_LshapeIndex[2].push_back(_Lshape);
										}
									}
									else
									{
										m_LshapeIndex[2].push_back(_Lshape);
									}

								}
							}

						}
					}

					//region 4
					if (regionGraCenPoint[k].x > width / 2 && regionGraCenPoint[k].x <= width - 1 && regionGraCenPoint[k].y > height / 2 && regionGraCenPoint[k].y <= height - 1)
					{
						//cal project score
						//pro y
						for (int i = yEnd - m_projectPercent * (yEnd - yStart); i <= yEnd; i++)
						{
							for (int j = xStart; j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectY++;
							}
						}

						//pro x
						for (int i = yStart; i <= yEnd; i++)
						{
							for (int j = xEnd - m_projectPercent * (xEnd - xStart); j <= xEnd; j++)
							{
								if (intSrc[i * width + j] == k)
									projectScore[k].projectX++;
							}
						}
						int voteMin = m_regionAreaPercent * regionArea[k];
						if (projectScore[k].projectX > voteMin || projectScore[k].projectY > voteMin)
						{
							if (abs(projectScore[k].projectX - projectScore[k].projectY) < m_projectDiff)
							{
								LShape _Lshape;
								_Lshape.index = k;
								_Lshape.pos = { (int)(regionGraCenPoint[k].x),(int)(regionGraCenPoint[k].y) };
								if (judgeRealLShape(intSrc, width, height, k, m_limit_Lwidth, _Lshape.pos, regionWH[k],3))
								{
									if (m_LshapeIndex[3].size())
									{
										if (_Lshape.pos.x > m_LshapeIndex[3][0].pos.x && _Lshape.pos.y > m_LshapeIndex[3][0].pos.y)
										{
											m_LshapeIndex[3].pop_back();
											m_LshapeIndex[3].push_back(_Lshape);
										}
									}
									else
									{
										m_LshapeIndex[3].push_back(_Lshape);
									}

								}
							}

						}
					}
					//end 
				}
			}


		}


		//free
		if(xSum != nullptr)
			delete[] xSum;
		if (ySum != nullptr)
			delete[] ySum;

		int _size = 0;
		for (int i = 0; i < m_LshapeIndex.size(); i++)
		{
			_size += m_LshapeIndex[i].size();
		}
		if (_size == 0 || _size > 4)
		{
			return -1;
		}
		else
		{
			return 0;
		}

	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


/**********************************************/
// LShapeSelect, 剔除假的L型块
// Author: Jimmy Zhan  5/5/2017
/**********************************************/
int CBmPocketLocation::LShapeSelect()
{
	__try
	{
		if (m_LshapeIndex[0].size() && m_LshapeIndex[1].size())
		{
			LShape shape0 = m_LshapeIndex[0][0];
			LShape shape1 = m_LshapeIndex[1][0];
			if (abs(shape0.pos.y - shape1.pos.y) > m_LshapeDiffY)
			{
				if (shape0.pos.y > shape1.pos.y)
				{
					m_LshapeIndex[0].pop_back();
				}
				else
				{
					m_LshapeIndex[1].pop_back();
				}
			}
		}

		if (m_LshapeIndex[2].size() && m_LshapeIndex[3].size())
		{
			LShape shape2 = m_LshapeIndex[2][0];
			LShape shape3 = m_LshapeIndex[3][0];
			if (abs(shape2.pos.y - shape3.pos.y) > m_LshapeDiffY)
			{
				if (shape2.pos.y < shape3.pos.y)
				{
					m_LshapeIndex[2].pop_back();
				}
				else
				{
					m_LshapeIndex[3].pop_back();
				}
			}
		}

		if (m_LshapeIndex[0].size() && m_LshapeIndex[2].size())
		{
			LShape shape0 = m_LshapeIndex[0][0];
			LShape shape2 = m_LshapeIndex[2][0];
			if (abs(shape0.pos.x - shape2.pos.x) > m_LshapeDiffX)
			{
				if (shape0.pos.x > shape2.pos.x)
				{
					m_LshapeIndex[0].pop_back();
				}
				else
				{
					m_LshapeIndex[2].pop_back();
				}
			}
		}

		if (m_LshapeIndex[1].size() && m_LshapeIndex[3].size())
		{
			LShape shape1 = m_LshapeIndex[1][0];
			LShape shape3 = m_LshapeIndex[3][0];
			if (abs(shape1.pos.x - shape3.pos.x) > m_LshapeDiffX)
			{
				if (shape1.pos.x < shape3.pos.x)
				{
					m_LshapeIndex[1].pop_back();
				}
				else
				{
					m_LshapeIndex[3].pop_back();
				}
			}
		}
		return 0;
	}

		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}



//int CBmPocketLocation::linePointSelect(int * intSrc,
//									IppiSize imgSize, 
//									int index,
//									int regionRef,
//									IppiPoint regionGraCenPoint,
//									std::vector<IMG_RCOORD>&PointCor)
//{
//	int width = imgSize.width;
//	int height = imgSize.height;
//	int xStart;
//	int yStart;
//	int xEnd;
//	int yEnd;
//	switch (regionRef)
//	{
//	case 0:
//		 xStart = regionGraCenPoint.x - m_xBias_ROI;	
//		 if (xStart < 0)	xStart = 0;
//		 yStart = regionGraCenPoint.y - m_yBias_ROI;
//		 if (yStart < 0)	yStart = 0;
//		for (int i = yStart; i <= yStart + m_ROIheight; i++)
//		{
//			for (int j = xStart; j <= xStart + m_ROIwidth; j++)
//			{
//				if (intSrc[i * width + j] == index && intSrc[(i + 1) * width + j] == 0)
//				{
//					PointCor.push_back({ (float)j,(float)i });
//				}
//			}
//		}
//		break;
//
//	case 1:
//		 xEnd = regionGraCenPoint.x + m_xBias_ROI;
//		 if (xEnd > width - 1)	xEnd = width - 1;
//		 yStart = regionGraCenPoint.y - m_yBias_ROI;
//		 if (yStart < 0)	yStart = 0;
//		for (int i = yStart; i <= yStart + m_ROIheight; i++)
//		{
//			for (int j = xEnd - m_ROIwidth; j <= xEnd; j++)
//			{
//				if (intSrc[i * width + j] == index && intSrc[(i + 1) * width + j] == 0)
//				{
//					PointCor.push_back({ (float)j,(float)i });
//				}
//			}
//		}
//		break;
//
//	case 2:
//		 xStart = regionGraCenPoint.x - m_xBias_ROI;
//		 if (xStart < 0)	xStart = 0;
//		 yEnd = regionGraCenPoint.y + m_yBias_ROI;
//		 if (yEnd > height - 1)	yEnd = height - 1;
//		for (int i = yEnd - m_ROIheight; i <= yEnd; i++)
//		{
//			for (int j = xStart; j <= xStart + m_ROIwidth; j++)
//			{
//				if (intSrc[i * width + j] == index && intSrc[(i - 1) * width + j] == 0)
//				{
//					PointCor.push_back({ (float)j,(float)i });
//				}
//			}
//		}
//		break;
//
//	case 3:
//		 xEnd = regionGraCenPoint.x + m_xBias_ROI;
//		 if (xEnd > width - 1)	xEnd = width - 1;
//		 yEnd = regionGraCenPoint.y + m_yBias_ROI;
//		 if (yEnd > height - 1)	yEnd = height - 1;
//		for (int i = yEnd - m_ROIheight; i <= yEnd; i++)
//		{
//			for (int j = xEnd - m_ROIwidth; j <= xEnd; j++)
//			{
//				if (intSrc[i * width + j] == index && intSrc[(i - 1) * width + j] == 0)
//				{
//					PointCor.push_back({ (float)j,(float)i });
//				}
//			}
//		}
//		break;
//
//	default:
//		break;
//	}
//
//	return 0;
//}

const size_t MAX_LEN = 128;
//bool CBmPocketLocation::LoadParameter(char *path)
//{
//	int status = 0;
//
//	LPSTR temp = (LPSTR)new char[MAX_PATH];
//
//	GetPrivateProfileStringA("Param", "m_pyramidLevel ", "", temp, MAX_PATH, path);
//	m_pyramidLevel = atoi((char*)temp);
//
//
//	delete temp;
//	return status;
//}

bool CBmPocketLocation::LoadParameter(char path[])
{
	__try
	{
		FILE *configFile = fopen(path, "r");
		if (!configFile)
		{
			return false;
		}
		char LineBuf[MAX_LEN] = { 0 };
		while (NULL != fgets(LineBuf, sizeof(LineBuf), configFile))
		{
			size_t bufLen = strlen(LineBuf);
			if ('\r' == LineBuf[bufLen - 1] || '\n' == LineBuf[bufLen - 1])
			{
				LineBuf[bufLen - 1] = '\0';
			}

			char *pos = strchr(LineBuf, '=');
			if (NULL != pos)
			{
				*pos = '\0';
				pos++;
				if (0 == strcmp(LineBuf, "m_pyramidLevel "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d",&m_pyramidLevel);

				}
				else if (0 == strcmp(LineBuf, "m_ucThreshold "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_ucThreshold);
				}
				else if (0 == strcmp(LineBuf, "m_whiteValue "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_whiteValue);
				}
				else if (0 == strcmp(LineBuf, "m_meanValueMin "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%f", &m_meanValueMin);
				}
				else if (0 == strcmp(LineBuf, "m_meanValue "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%f", &m_meanValue);
				}
				else if (0 == strcmp(LineBuf, "m_stdDevValue "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%f", &m_stdDevValue);
				}
				else if (0 == strcmp(LineBuf, "m_areaMin "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_areaMin);
				}
				else if (0 == strcmp(LineBuf, "m_areaMax "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_areaMax);
				}
				else if (0 == strcmp(LineBuf, "m_LwidthMax "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_LwidthMax);
				}
				else if (0 == strcmp(LineBuf, "m_LwidthMin "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_LwidthMin);
				}
				else if (0 == strcmp(LineBuf, "m_LheightMax "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_LheightMax);
				}
				else if (0 == strcmp(LineBuf, "m_LheightMin "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_LheightMin);
				}
				else if (0 == strcmp(LineBuf, "m_DUT_areaMin "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_DUT_areaMin);
				}
				else if (0 == strcmp(LineBuf, "m_TareaMin "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_TareaMin);
				}
				else if (0 == strcmp(LineBuf, "m_TareaMax "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_TareaMax);
				}
				else if (0 == strcmp(LineBuf, "m_TwidthMax "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_TwidthMax);
				}
				else if (0 == strcmp(LineBuf, "m_TheightMax "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_TheightMax);
				}
				else if (0 == strcmp(LineBuf, "m_projectPercent "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%f", &m_projectPercent);
				}
				else if (0 == strcmp(LineBuf, "m_regionAreaPercent "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%f", &m_regionAreaPercent);
				}
				else if (0 == strcmp(LineBuf, "m_projectDiff "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_projectDiff);
				}
				else if (0 == strcmp(LineBuf, "m_margin "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_margin);
				}
				else if (0 == strcmp(LineBuf, "m_limit_Lwidth "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_limit_Lwidth);
				}
				else if (0 == strcmp(LineBuf, "m_xBias "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_xBias);
				}
				else if (0 == strcmp(LineBuf, "m_yBias "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_yBias);
				}
				else if (0 == strcmp(LineBuf, "m_BlockHeight "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_BlockHeight);
				}
				else if (0 == strcmp(LineBuf, "m_BlockWidth "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_BlockWidth);
				}
				else if (0 == strcmp(LineBuf, "m_BlockShift_x "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_BlockShift_x);
				}
				else if (0 == strcmp(LineBuf, "m_BlockShift_y "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_BlockShift_y);
				}
				else if (0 == strcmp(LineBuf, "m_LshapeTurn "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_LshapeTurn);
				}
				else if (0 == strcmp(LineBuf, "m_cenXjudge "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_cenXjudge);
				}
				else if (0 == strcmp(LineBuf, "m_cenYjudge "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_cenYjudge);
				}
				else if (0 == strcmp(LineBuf, "m_LshapeDiffX "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_LshapeDiffX);
				}
				else if (0 == strcmp(LineBuf, "m_LshapeDiffY "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_LshapeDiffY);
				}
				else if (0 == strcmp(LineBuf, "m_hisErasePro "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%f", &m_hisErasePro);
				}
				else if (0 == strcmp(LineBuf, "m_hisEraseMean "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_hisEraseMean);
				}
				else
				{
					printf("Failed to read information from the file.");
					break;
				}
			}
		}

		fclose(configFile);


		if (m_pyramidLevel > 0)
		{
			m_areaMin /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));
			m_areaMax /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));
			m_TareaMin /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));
			m_TareaMax /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));
			m_TwidthMax /= pow(2, m_pyramidLevel);
			m_TheightMax /= pow(2, m_pyramidLevel);
			m_DUT_areaMin /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));

			m_margin /= pow(2, m_pyramidLevel);
			m_limit_Lwidth /= pow(2, m_pyramidLevel);

			m_xBias /= pow(2, m_pyramidLevel);
			m_yBias /= pow(2, m_pyramidLevel);


			m_BlockHeight /= pow(2, m_pyramidLevel);
			m_BlockWidth /= pow(2, m_pyramidLevel);
			m_BlockShift_x /= pow(2, m_pyramidLevel);
			m_BlockShift_y /= pow(2, m_pyramidLevel);

			m_projectDiff /= pow(2, m_pyramidLevel);

			m_LshapeTurn /= (pow(2, m_pyramidLevel) * pow(2, m_pyramidLevel));
			m_cenXjudge /= pow(2, m_pyramidLevel);
			m_cenYjudge /= pow(2, m_pyramidLevel);
		}
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}



/**********************************************/
// Vis_HistogramExample, 统计直方图
// Author: Yige?
/**********************************************/
int CBmPocketLocation::Vis_HistogramExample(Ipp8u *srcRoi, IppiSize Roi, Ipp32u pHistVec[256])
{
	__try
	{
		const int HEIGHT = Roi.height;
	const int WIDTH = Roi.width;

	IppStatus sts;
	{
		const int nBins = 256;
		int nLevels[] = { nBins + 1 };
		Ipp32f lowerLevel[] = { 0 };
		Ipp32f upperLevel[] = { 256 };
		Ipp32f pLevels[nBins + 1], *ppLevels[1];
		int sizeHistObj, sizeBuffer;

		IppiHistogramSpec* pHistObj;
		Ipp8u* pBuffer;
		//Ipp32u pHistVec[nBins];

		// get sizes for spec and buffer
		ippiHistogramGetBufferSize(ipp8u, Roi, nLevels, 1/*nChan*/, 1/*uniform*/, &sizeHistObj, &sizeBuffer);

		pHistObj = (IppiHistogramSpec*)ippsMalloc_8u(sizeHistObj);
		pBuffer = (Ipp8u*)ippsMalloc_8u(sizeBuffer);
		// initialize spec
		ippiHistogramUniformInit(ipp8u, lowerLevel, upperLevel, nLevels, 1, pHistObj);

		// check levels of bins
		ppLevels[0] = pLevels;
		sts = ippiHistogramGetLevels(pHistObj, ppLevels);

		// calculate histogram
		sts = ippiHistogram_8u_C1R(srcRoi, WIDTH, Roi, pHistVec, pHistObj, pBuffer);

		ippsFree(pHistObj);
		ippsFree(pBuffer);
	}
	return 0;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


void CBmPocketLocation::PocketCoarseLocate_old(const IMG_UBBUF src, int &xMin,int &xMax)
{

	int m_stripWidth = 10;
	int m_histModesDiff = 30;

	int width = src.size.width;
	int height = src.size.height;
	xMin = 0;
	xMax = width - 1;
	Ipp32u *pHistVec = new Ipp32u[256];
	

	for (int j = 0; j < width / 2; j += m_stripWidth)
	{
		memset(pHistVec, 0, sizeof(Ipp32u) * 256);
		Ipp8u *srcRoi1 = new Ipp8u[m_stripWidth * height];
		for (int i = 0; i < height; i++)
		{
			memcpy(srcRoi1 + i * m_stripWidth, src.ptr + i * width + j, m_stripWidth);
		}
		Vis_HistogramExample(srcRoi1, { m_stripWidth,height }, pHistVec);
		int modesIndex1 = 255;
		Ipp32u modesMax1 = 0;
		for (int index = 5; index <= 250; index++)
		{
			if (pHistVec[index] > modesMax1)
			{
				modesMax1 = pHistVec[index];
				modesIndex1 = index;
			}
		}

		IMG_UBBUF ubbsrcRoi1;
		m_setVisBuf.set_IMG_UBBUF(ubbsrcRoi1, srcRoi1, { (IMG_UWORD)m_stripWidth,(IMG_UWORD)height }, m_stripWidth);

		memset(pHistVec, 0, sizeof(Ipp32u) * 256);
		Ipp8u *srcRoi2 = new Ipp8u[m_stripWidth * height];
		for (int i = 0; i < height; i++)
		{
			memcpy(srcRoi2 + i * m_stripWidth, src.ptr + i * width + j + m_stripWidth, m_stripWidth);
		}
		Vis_HistogramExample(srcRoi2, { m_stripWidth,height }, pHistVec);
		int modesIndex2 = 255;
		Ipp32u modesMax2 = 0;
		for (int index = 5; index <= 250; index++)
		{
			if (pHistVec[index] > modesMax1)
			{
				modesMax2 = pHistVec[index];
				modesIndex2 = index;
			}
		}
		IMG_UBBUF ubbsrcRoi2;
		m_setVisBuf.set_IMG_UBBUF(ubbsrcRoi2, srcRoi2, { (IMG_UWORD)m_stripWidth,(IMG_UWORD)height }, m_stripWidth);
		
		if (modesIndex1 - modesIndex2 > m_histModesDiff)
		{
			xMin = j;
			if(srcRoi1 != nullptr)
				delete[] srcRoi1;
			if (srcRoi2 != nullptr)
				delete[] srcRoi2;
			break;
		}
		else if (modesIndex2 - modesIndex1 > m_histModesDiff)
		{
			xMin = 0;
			if (srcRoi1 != nullptr)
				delete[] srcRoi1;
			if (srcRoi2 != nullptr)
				delete[] srcRoi2;
			break;
		}

		if (srcRoi1 != nullptr)
			delete[] srcRoi1;
		if (srcRoi2 != nullptr)
			delete[] srcRoi2;
	}


	for (int j = width - m_stripWidth; j > width / 2 + m_stripWidth; j -= m_stripWidth)
	{
		memset(pHistVec, 0, sizeof(Ipp32u) * 256);
		Ipp8u *srcRoi1 = new Ipp8u[m_stripWidth * height];
		for (int i = 0; i < height; i++)
		{
			memcpy(srcRoi1 + i * m_stripWidth, src.ptr + i * width + j, m_stripWidth);
		}
		Vis_HistogramExample(srcRoi1, { m_stripWidth,height }, pHistVec);
		int modesIndex1 = 255;
		Ipp32u modesMax1 = 0;
		for (int index = 5; index <= 250; index++)
		{
			if (pHistVec[index] > modesMax1)
			{
				modesMax1 = pHistVec[index];
				modesIndex1 = index;
			}
		}

		IMG_UBBUF ubbsrcRoi1;
		m_setVisBuf.set_IMG_UBBUF(ubbsrcRoi1, srcRoi1, { (IMG_UWORD)m_stripWidth,(IMG_UWORD)height }, m_stripWidth);

		memset(pHistVec, 0, sizeof(Ipp32u) * 256);
		Ipp8u *srcRoi2 = new Ipp8u[m_stripWidth * height];
		for (int i = 0; i < height; i++)
		{
			memcpy(srcRoi2 + i * m_stripWidth, src.ptr + i * width + j - m_stripWidth, m_stripWidth);
		}
		Vis_HistogramExample(srcRoi2, { m_stripWidth,height }, pHistVec);
		int modesIndex2 = 255;
		Ipp32u modesMax2 = 0;
		for (int index = 5; index <= 250; index++)
		{
			if (pHistVec[index] > modesMax1)
			{
				modesMax2 = pHistVec[index];
				modesIndex2 = index;
			}
		}
		IMG_UBBUF ubbsrcRoi2;
		m_setVisBuf.set_IMG_UBBUF(ubbsrcRoi2, srcRoi2, { (IMG_UWORD)m_stripWidth,(IMG_UWORD)height }, m_stripWidth);

		if (modesIndex1 - modesIndex2 > m_histModesDiff)
		{
			xMax = j;
			if (srcRoi1 != nullptr)
				delete[] srcRoi1;
			if (srcRoi2 != nullptr)
				delete[] srcRoi2;
			break;
		}
		else if(modesIndex2 - modesIndex1 > m_histModesDiff)
		{
			xMax = width - 1;
			if (srcRoi1 != nullptr)
				delete[] srcRoi1;
			if (srcRoi2 != nullptr)
				delete[] srcRoi2;
			break;
		}

		if (srcRoi1 != nullptr)
			delete[] srcRoi1;
		if (srcRoi2 != nullptr)
			delete[] srcRoi2;
	}

	if (pHistVec != nullptr)
		delete pHistVec;
}


int CBmPocketLocation::PocketCoarseLocate(const IMG_UBBUF src, int &xMin, int &xMax)
{
	__try
	{
		
		int m_stripWidth = 5;
		float m_meanDiffPro = 1.1;

		int width = src.size.width;
		int height = src.size.height;
		xMin = 0;
		xMax = width - 1;


		for (int j = 0; j < width / 2; j += m_stripWidth)
		{
			Ipp64f pMean1 = 0;
			Ipp64f pStdDev1 = 0;
			Ipp8u *srcRoi1 = new Ipp8u[m_stripWidth * height];
			for (int i = 0; i < height; i++)
			{
				memcpy(srcRoi1 + i * m_stripWidth, src.ptr + i * width + j, m_stripWidth);
			}
			ippiMean_StdDev_8u_C1R(srcRoi1, m_stripWidth, { m_stripWidth,height }, &pMean1, &pStdDev1);
			IMG_UBBUF ubbsrcRoi1;
			m_setVisBuf.set_IMG_UBBUF(ubbsrcRoi1, srcRoi1, { (IMG_UWORD)m_stripWidth,(IMG_UWORD)height }, m_stripWidth);


			Ipp64f pMean2 = 0;
			Ipp64f pStdDev2 = 0;
			Ipp8u *srcRoi2 = new Ipp8u[m_stripWidth * height];
			for (int i = 0; i < height; i++)
			{
				memcpy(srcRoi2 + i * m_stripWidth, src.ptr + i * width + j + m_stripWidth, m_stripWidth);
			}
			ippiMean_StdDev_8u_C1R(srcRoi2, m_stripWidth, { m_stripWidth,height }, &pMean2, &pStdDev2);
			IMG_UBBUF ubbsrcRoi2;
			m_setVisBuf.set_IMG_UBBUF(ubbsrcRoi2, srcRoi2, { (IMG_UWORD)m_stripWidth,(IMG_UWORD)height }, m_stripWidth);


			if (pMean1 > m_meanDiffPro * pMean2)
			{
				xMin = j;
				if (srcRoi1 != nullptr)
					delete[] srcRoi1;
				if (srcRoi2 != nullptr)
					delete[] srcRoi2;
				break;
			}
			else if (pMean2 > m_meanDiffPro * pMean1)
			{
				xMin = 0;
				if (srcRoi1 != nullptr)
					delete[] srcRoi1;
				if (srcRoi2 != nullptr)
					delete[] srcRoi2;
				break;
			}

			if (srcRoi1 != nullptr)
				delete[] srcRoi1;
			if (srcRoi2 != nullptr)
				delete[] srcRoi2;
		}


		for (int j = width - m_stripWidth; j > width / 2 + m_stripWidth; j -= m_stripWidth)
		{
			Ipp64f pMean1 = 0;
			Ipp64f pStdDev1 = 0;
			Ipp8u *srcRoi1 = new Ipp8u[m_stripWidth * height];
			for (int i = 0; i < height; i++)
			{
				memcpy(srcRoi1 + i * m_stripWidth, src.ptr + i * width + j, m_stripWidth);
			}
			ippiMean_StdDev_8u_C1R(srcRoi1, m_stripWidth, { m_stripWidth,height }, &pMean1, &pStdDev1);
			IMG_UBBUF ubbsrcRoi1;
			m_setVisBuf.set_IMG_UBBUF(ubbsrcRoi1, srcRoi1, { (IMG_UWORD)m_stripWidth,(IMG_UWORD)height }, m_stripWidth);

			Ipp64f pMean2 = 0;
			Ipp64f pStdDev2 = 0;
			Ipp8u *srcRoi2 = new Ipp8u[m_stripWidth * height];
			for (int i = 0; i < height; i++)
			{
				memcpy(srcRoi2 + i * m_stripWidth, src.ptr + i * width + j - m_stripWidth, m_stripWidth);
			}
			ippiMean_StdDev_8u_C1R(srcRoi2, m_stripWidth, { m_stripWidth,height }, &pMean2, &pStdDev2);
			IMG_UBBUF ubbsrcRoi2;
			m_setVisBuf.set_IMG_UBBUF(ubbsrcRoi2, srcRoi2, { (IMG_UWORD)m_stripWidth,(IMG_UWORD)height }, m_stripWidth);

			if (pMean1 > m_meanDiffPro * pMean2)
			{
				xMax = j;
				if (srcRoi1 != nullptr)
					delete[] srcRoi1;
				if (srcRoi2 != nullptr)
					delete[] srcRoi2;
				break;
			}
			else if (pMean2 > m_meanDiffPro * pMean1)
			{
				xMax = width - 1;
				if (srcRoi1 != nullptr)
					delete[] srcRoi1;
				if (srcRoi2 != nullptr)
					delete[] srcRoi2;
				break;
			}

			if (srcRoi1 != nullptr)
				delete[] srcRoi1;
			if (srcRoi2 != nullptr)
				delete[] srcRoi2;
		}

		return 0;

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


/**********************************************/
// erase_inverseDUT, 将翻转DUT的亮部涂黑，以便能够分割出L型块
// Input:
// IMG_UBBUF src ， 输入图像
// erasePro，去除比例
//
// Author: Jimmy Zhan  16/5/2017
/**********************************************/
int CBmPocketLocation::erase_inverseDUT(IMG_UBBUF src, float erasePro)
{
	unsigned char * pSrc = src.ptr;
	int width = src.size.width;
	int height = src.size.height;
	int sumPixels = width * height;
	int truncation = 0;

	Ipp32u *pHistVec = new Ipp32u[256];
	Vis_HistogramExample(pSrc, { width,height }, pHistVec);
	
	int temp = 0;
	for (int i = 0; i < 256; i++)
	{
		temp += pHistVec[i];
		if (temp >= sumPixels * (1 - erasePro))
		{
			truncation = i;
			break;
		}
	}

	int pos = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (pSrc[pos + j] > truncation)
			{
				pSrc[pos + j] = 0;
			}
		}
		pos += width;
	}

	delete[] pHistVec;
	return 0;
}


/**********************************************/
// HistogramFilter, 将DUT上某些较亮的区域涂成设定值，以便能够分割出L型块
// Input:
// IMG_UBBUF ubbSrc ， 输入图像
// Ipp32u * pHistVec，输入直方图
// float filterPro， 去除比例
// IMG_UBYTE setValue， 设定的值
//
// Author: Jimmy Zhan  16/5/2017
/**********************************************/
void CBmPocketLocation::HistogramFilter(IMG_UBBUF ubbSrc, Ipp32u *pHistVec, float filterPro,IMG_UBYTE setValue)
{
	int width = ubbSrc.size.width;
	int height = ubbSrc.size.height;
	int mapSize = width * height;
	unsigned char *indexFlag = new unsigned char[256];
	memset(indexFlag, 0, 256);

	for (int i = 128; i < 256; i++)
	{
		float tempPro = (float)pHistVec[i] / mapSize;
		if (tempPro < filterPro)
		{
			indexFlag[i] = 1;
		}
	}

	//reset src
	int pos = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			IMG_UBYTE tempVal = ubbSrc.ptr[pos + j];
			if (indexFlag[tempVal] == 1)
			{
				ubbSrc.ptr[pos + j] = setValue;
			}
		}
		pos += width;
	}

	if(indexFlag != nullptr)
		delete[] indexFlag;
}


/**********************************************/
// PocketLocation, pocket定位
// Input:
//	IMG_UBBUF src; 输入图片
//	double &x, pocket中心x坐标
//	double &y, pocket中心y坐标
//  double &angle, pocket角度
//
// Return:
//
//		_CODE_OK , //定位成功   0
//		_CODE_LOCATE_FAIL,//定位失败 4  
//		_CODE_THROW//无图-1
// Author: Jimmy Zhan  3/5/2017
/**********************************************/
_BACK_CODE CBmPocketLocation::PocketLocation_Old(const IMG_UBBUF src,
	double &x, double &y, double &angle)
{
	if (src.ptr == NULL)
	{
		return _CODE_THROW;
	}
	int devStatus = 0;

	int width = src.size.width;
	int height = src.size.height;
	
	//	pyramid		//
	unsigned char *ucpPyramid = new unsigned char[width * height];
	int pyWidth = 0;
	int pyHeight = 0;
	if (m_pyramidLevel > 0)
	{
		devStatus = pyramid(src, ucpPyramid, pyWidth, pyHeight, m_pyramidLevel);
#ifdef DEBUG
		assert(devStatus == 0);
#endif // DEBUG
	}
	else
	{
		memcpy(ucpPyramid, src.ptr, width * height * sizeof(unsigned char));
		pyWidth = width;
		pyHeight = height;
	}

	IMG_UBBUF ubbPyramid;
	m_setVisBuf.set_IMG_UBBUF(ubbPyramid, ucpPyramid, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);
	////////////////////////////////////////////



	//	HistogramSpecification	//
	//	去除亮白区域的影响!!!!!!!!	//
	int pos = 0;
	for (int i = 0; i < pyHeight; i++)
	{
		for (int j = 0; j < pyWidth; j++)
		{
			if ((ucpPyramid[pos + j]) >= m_whiteValue)		//将亮圆部分填黑，以便OTSU找准
			{
				ucpPyramid[pos + j] = 0;
			}
		}
		pos += pyWidth;
	}
	////////////////////////////////


	IMG_UBBUF ubbHisSpec;
	unsigned char *ucpHisSpec = new unsigned char[pyWidth * pyHeight];
	m_setVisBuf.set_IMG_UBBUF(ubbHisSpec, ucpHisSpec, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

	Ipp64f pMean = 0;
	Ipp64f pStdDev = 0;
	ippiMean_StdDev_8u_C1R(ubbPyramid.ptr, ubbPyramid.linestep, {ubbPyramid.size.width,ubbPyramid.size.height}, &pMean, &pStdDev);
	if (pMean < m_meanValueMin)	//图像太暗
	{
		//先均衡化
		devStatus = VisHistogramEqualization(ubbPyramid, ubbHisSpec);	
#ifdef DEBUG
		assert(devStatus == 0);
#endif // DEBUG

		//再规定化
		//devStatus = VisHistogramSpecification(ubbHisSpec, m_meanValue, m_stdDevValue, ubbHisSpec);
#ifdef DEBUG
		assert(devStatus == 0);
#endif // DEBUG
	}
	else 
	{
		//直接规定化
		devStatus = VisHistogramSpecification(ubbPyramid, m_meanValue, m_stdDevValue, ubbHisSpec);
#ifdef DEBUG
		assert(devStatus == 0);
#endif // DEBUG
	}
	///////////////////////////////////////////


	//	threshold	//
	IMG_UBBUF ubbThreshold;
	unsigned char *ucpThreshold = new unsigned char[pyWidth * pyHeight];
	devStatus = threshold_OTSU(ucpHisSpec, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, ucpThreshold);
#ifdef DEBUG
	assert(devStatus == 0);
#endif // DEBUG
	m_setVisBuf.set_IMG_UBBUF(ubbThreshold, ucpThreshold, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);
	//////////////////////////////////////////

	//		dialate		//
	IMG_UBBUF ubbDilate;
	unsigned char *ucpDilate = new unsigned char[pyWidth * pyHeight];
	devStatus = morfDilateBorder_notInPlace_3by3(ubbThreshold, ucpDilate);
#ifdef DEBUG
	assert(devStatus == 0);
#endif // DEBUG
	m_setVisBuf.set_IMG_UBBUF(ubbDilate, ucpDilate, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);
	//////////////////////////////////////////
	//		Erode		//
	IMG_UBBUF ubbErode;
	unsigned char *ucpErode = new unsigned char[pyWidth * pyHeight];
	devStatus = morfErodeBorder_notInPlace_3by3(ubbDilate, ucpErode);
#ifdef DEBUG
	assert(devStatus == 0);
#endif // DEBUG
	m_setVisBuf.set_IMG_UBBUF(ubbErode, ucpErode, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);
	//////////////////////////////////////////


	//	label region	//
	int regionNum = 0;
	int *intpLabel = new int[pyWidth * pyHeight];
	devStatus = labelMarker(ucpErode, { pyWidth,pyHeight }, intpLabel, regionNum);
	IMG_INTBUF iBufLabel;
	m_setVisBuf.set_IMG_INTBUF(iBufLabel, intpLabel, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth * sizeof(int));
	///////////////////////

	//	blob analyse	//
	int *pRegionArea = new int[regionNum];
	IppiPoint *pRegionCen = new IppiPoint[regionNum];
	regionDef::RegionPeakPoint *pRegionPeakPoint = new regionDef::RegionPeakPoint[regionNum];
	regionDef::RegionWidthHeight *pRegionWH = new regionDef::RegionWidthHeight[regionNum];
	ProjectScore *projectScore = new ProjectScore[regionNum];

	devStatus = getRegionInfo(intpLabel, { pyWidth,pyHeight }, regionNum, pRegionArea, pRegionCen, pRegionPeakPoint, pRegionWH,projectScore);
	/////////////////////

	//	retry  dump  DUT	//
	if (devStatus == -1)
	{
		//copy ucpPyramid//
		unsigned char *ucpPyramid_clone = new unsigned char[pyWidth * pyHeight];
		memcpy(ucpPyramid_clone, ucpPyramid, pyWidth * pyHeight);
		IMG_UBBUF ubbPyramid_clone;
		m_setVisBuf.set_IMG_UBBUF(ubbPyramid_clone, ucpPyramid_clone, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

		//	find DUT(area max)	//
		vector <int> vecDUTindex;
		vecDUTindex.clear();
		for (int k = 1; k < regionNum; k++)
		{
			if (pRegionArea[k] > m_DUT_areaMin)
			{
				vecDUTindex.push_back(k);
			}
		}

		for (int k = 0; k < vecDUTindex.size(); k++)
		{
			int val = vecDUTindex[k];
			for (int i = 0; i < pyHeight; i++)
			{
				for (int j = 0; j < pyWidth; j++)
				{
					if (intpLabel[i * pyWidth + j] == val)
					{
						ucpPyramid_clone[i * pyWidth + j] = 0;
					}
				}
			}
		}
		if(projectScore != nullptr)
			delete[] projectScore;
		if (pRegionWH != nullptr)
			delete[] pRegionWH;
		if (pRegionPeakPoint != nullptr)
			delete[] pRegionPeakPoint;
		if (pRegionArea != nullptr)
			delete[] pRegionArea;
		if (pRegionCen != nullptr)
			delete[] pRegionCen;

		m_setVisBuf.set_IMG_UBBUF(ubbHisSpec, ucpHisSpec, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

		//pMean = 0;
		//pStdDev = 0;
		//ippiMean_StdDev_8u_C1R(ubbPyramid_clone.ptr, ubbPyramid_clone.linestep, { ubbPyramid_clone.size.width,ubbPyramid_clone.size.height }, &pMean, &pStdDev);
		//if (pMean < m_meanValueMin)	//图像太暗
		//{
		//	//先均衡化
		//	devStatus = VisHistogramEqualization(ubbPyramid_clone, ubbHisSpec);
		//}
		//else
		//{
			//直接规定化
			devStatus = VisHistogramSpecification(ubbPyramid_clone, m_meanValue, m_stdDevValue, ubbHisSpec);

		//}
		///////////////////////////////////////////

		devStatus = threshold_OTSU(ucpHisSpec, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, ucpThreshold);
		m_setVisBuf.set_IMG_UBBUF(ubbThreshold, ucpThreshold, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

		devStatus = morfErodeBorder_notInPlace_3by3(ubbThreshold, ucpErode);
		m_setVisBuf.set_IMG_UBBUF(ubbErode, ucpErode, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

		devStatus = morfDilateBorder_notInPlace_3by3(ubbErode, ucpDilate);
		m_setVisBuf.set_IMG_UBBUF(ubbDilate, ucpDilate, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);


		devStatus = labelMarker(ucpDilate, { pyWidth,pyHeight }, intpLabel, regionNum);
		m_setVisBuf.set_IMG_INTBUF(iBufLabel, intpLabel, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth * sizeof(int));

		//realloc
		pRegionArea = new int[regionNum];
		pRegionCen = new IppiPoint[regionNum];
		pRegionPeakPoint = new regionDef::RegionPeakPoint[regionNum];
		pRegionWH = new regionDef::RegionWidthHeight[regionNum];
		projectScore = new ProjectScore[regionNum];

		devStatus = getRegionInfo2(intpLabel, { pyWidth,pyHeight }, regionNum, pRegionArea, pRegionCen, pRegionPeakPoint, pRegionWH, projectScore);
		if (devStatus == -1)
		{	
			//	继续删大亮块	//
			vecDUTindex.clear();
			for (int k = 1; k < regionNum; k++)
			{
				if (pRegionArea[k] > m_DUT_areaMin)
				{
					vecDUTindex.push_back(k);
				}
			}

			for (int k = 0; k < vecDUTindex.size(); k++)
			{
				int val = vecDUTindex[k];
				for (int i = 0; i < pyHeight; i++)
				{
					for (int j = 0; j < pyWidth; j++)
					{
						if (intpLabel[i * pyWidth + j] == val)
						{
							ucpPyramid_clone[i * pyWidth + j] = 0;
						}
					}
				}
			}
			if (projectScore != nullptr)
				delete[] projectScore;
			if (pRegionWH != nullptr)
				delete[] pRegionWH;
			if (pRegionPeakPoint != nullptr)
				delete[] pRegionPeakPoint;
			if (pRegionArea != nullptr)
				delete[] pRegionArea;
			if (pRegionCen != nullptr)
				delete[] pRegionCen;

			//devStatus = VisHistogramSpecification(ubbPyramid_clone, m_meanValue, m_stdDevValue, ubbHisSpec);
			devStatus = threshold_OTSU(ucpPyramid_clone, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, ucpThreshold);

			devStatus = morfErodeBorder_notInPlace_3by3(ubbThreshold, ucpErode);
			m_setVisBuf.set_IMG_UBBUF(ubbErode, ucpErode, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);
			devStatus = morfDilateBorder_notInPlace_3by3(ubbErode, ucpDilate);
			m_setVisBuf.set_IMG_UBBUF(ubbDilate, ucpDilate, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

			devStatus = labelMarker(ucpDilate, { pyWidth,pyHeight }, intpLabel, regionNum);

			//realloc
			pRegionArea = new int[regionNum];
			pRegionCen = new IppiPoint[regionNum];
			pRegionPeakPoint = new regionDef::RegionPeakPoint[regionNum];
			pRegionWH = new regionDef::RegionWidthHeight[regionNum];
			projectScore = new ProjectScore[regionNum];

			devStatus = getRegionInfo2(intpLabel, { pyWidth,pyHeight }, regionNum, pRegionArea, pRegionCen, pRegionPeakPoint, pRegionWH, projectScore);

		}
		
		if(ucpPyramid_clone != nullptr)
			delete[] ucpPyramid_clone;
	}
	////////////////////////////////////

	//		retry	histogram	//
	if (devStatus == -1)
	{
		if (projectScore != nullptr)
			delete[] projectScore;
		if (pRegionWH != nullptr)
			delete[] pRegionWH;
		if (pRegionPeakPoint != nullptr)
			delete[] pRegionPeakPoint;
		if (pRegionArea != nullptr)
			delete[] pRegionArea;
		if (pRegionCen != nullptr)
			delete[] pRegionCen;
		devStatus = VisHistogramSpecification(ubbPyramid, m_meanValue, m_stdDevValue, ubbHisSpec);
		
		devStatus = threshold_OTSU(ucpHisSpec, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, ucpThreshold);
		m_setVisBuf.set_IMG_UBBUF(ubbThreshold, ucpThreshold, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

		devStatus = morfDilateBorder_notInPlace_3by3(ubbThreshold, ucpDilate);
		m_setVisBuf.set_IMG_UBBUF(ubbDilate, ucpDilate, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);
		
		devStatus = morfErodeBorder_notInPlace_3by3(ubbDilate, ucpErode);
		m_setVisBuf.set_IMG_UBBUF(ubbErode, ucpErode, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

		devStatus = labelMarker(ucpErode, { pyWidth,pyHeight }, intpLabel, regionNum);
		m_setVisBuf.set_IMG_INTBUF(iBufLabel, intpLabel, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth * sizeof(int));
	
		//realloc
		pRegionArea = new int[regionNum];
		pRegionCen = new IppiPoint[regionNum];
		pRegionPeakPoint = new regionDef::RegionPeakPoint[regionNum];
		pRegionWH = new regionDef::RegionWidthHeight[regionNum];
		projectScore = new ProjectScore[regionNum];

		devStatus = getRegionInfo(intpLabel, { pyWidth,pyHeight }, regionNum, pRegionArea, pRegionCen, pRegionPeakPoint, pRegionWH, projectScore);
	}


	//		retry	threshold	//
	if (devStatus == -1)
	{
		if (projectScore != nullptr)
			delete[] projectScore;
		if (pRegionWH != nullptr)
			delete[] pRegionWH;
		if (pRegionPeakPoint != nullptr)
			delete[] pRegionPeakPoint;
		if (pRegionArea != nullptr)
			delete[] pRegionArea;
		if (pRegionCen != nullptr)
			delete[] pRegionCen;

		devStatus = VisHistogramSpecification(ubbPyramid, m_meanValue, m_stdDevValue, ubbHisSpec);

		devStatus = threshold_manual(ucpHisSpec, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, ucpThreshold);//threshold_OTSU(ucpHisSpec, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, ucpThreshold);
		m_setVisBuf.set_IMG_UBBUF(ubbThreshold, ucpThreshold, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

		devStatus = morfErodeBorder_notInPlace_3by3(ubbThreshold, ucpErode);
		m_setVisBuf.set_IMG_UBBUF(ubbErode, ucpErode, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

		devStatus = morfDilateBorder_notInPlace_3by3(ubbErode, ucpDilate);
		m_setVisBuf.set_IMG_UBBUF(ubbDilate, ucpDilate, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

		devStatus = labelMarker(ucpDilate, { pyWidth,pyHeight }, intpLabel, regionNum);
		m_setVisBuf.set_IMG_INTBUF(iBufLabel, intpLabel, { (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth * sizeof(int));

		//realloc
		pRegionArea = new int[regionNum];
		pRegionCen = new IppiPoint[regionNum];
		pRegionPeakPoint = new regionDef::RegionPeakPoint[regionNum];
		pRegionWH = new regionDef::RegionWidthHeight[regionNum];
		projectScore = new ProjectScore[regionNum];

		devStatus = getRegionInfo(intpLabel, { pyWidth,pyHeight }, regionNum, pRegionArea, pRegionCen, pRegionPeakPoint, pRegionWH, projectScore);
	}


	//		cal result		//
	float slope = 0, b = 0, sigma = 2.0;
	std::vector<IMG_RCOORD>PointCor;
	PointCor.clear();
	
	if (devStatus == 0)
	{
		LShapeSelect();		//	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		float angleMin = 90;
		//	cal center  //
		int Lnum = 0;
		m_cenPoint.x = 0;
		m_cenPoint.y = 0;
		for (int i = 0; i < m_LshapeIndex.size(); i++)
		{
			if (i == 0 && m_LshapeIndex[i].size())
			{
				m_cenPoint.x += m_LshapeIndex[i][0].pos.x + m_xBias;
				m_cenPoint.y += m_LshapeIndex[i][0].pos.y + m_yBias;
				Lnum++;

				break;
			}
			if (i == 1 && m_LshapeIndex[i].size())
			{
				m_cenPoint.x += m_LshapeIndex[i][0].pos.x - m_xBias;
				m_cenPoint.y += m_LshapeIndex[i][0].pos.y + m_yBias;
				Lnum++;

			}
			if (i == 2 && m_LshapeIndex[i].size())
			{
				m_cenPoint.x += m_LshapeIndex[i][0].pos.x + m_xBias;
				m_cenPoint.y += m_LshapeIndex[i][0].pos.y - m_yBias;
				Lnum++;

			}
			if (i == 3 && m_LshapeIndex[i].size())
			{
				m_cenPoint.x += m_LshapeIndex[i][0].pos.x - m_xBias;
				m_cenPoint.y += m_LshapeIndex[i][0].pos.y - m_yBias;
				Lnum++;
			}
		}
		//GET MEAN
		m_cenPoint.x /= Lnum;
		m_cenPoint.y /= Lnum;		

		//	trace back to origin	//
		for (int i = 0; i < m_LshapeIndex.size(); i++)
		{
			for (int j = 0; j < m_LshapeIndex[i].size(); j++)
			{
				m_LshapeIndex[i][j].pos.x *= pow(2, m_pyramidLevel);
				m_LshapeIndex[i][j].pos.y *= pow(2, m_pyramidLevel);
			}

		}

		//	cal theta	//
		float x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		//find top block
		float xSum = 0, ySum = 0;int num1 = 0, num2 = 0; int xStart, yStart;
		xStart = m_cenPoint.x - m_BlockShift_x;
		if (xStart < 0)	xStart = 0;
		yStart = m_cenPoint.y - m_BlockShift_y;
		if (yStart < 0)	yStart = 0;
		for (int i = yStart; i <= yStart + m_BlockHeight; i++)
		{
			for (int j = xStart;j <= xStart + m_BlockWidth; j++)
			{
				if (intpLabel[i * pyWidth + j] != 0)
				{
					xSum += j;
					ySum += i;
					num1++;
				}
			}
		}
		if (num1)
		{
			x1 = xSum / num1;
			y1 = ySum / num1;
		}



		//find bottom block
		xSum = 0; ySum = 0; num2 = 0;
		xStart = m_cenPoint.x - m_BlockShift_x;
		if (xStart < 0)	xStart = 0;
		yStart = m_cenPoint.y + m_BlockShift_y;
		if (yStart > pyHeight - 1)	yStart = pyHeight - 1;
		for (int i = yStart - m_BlockHeight; i <= yStart; i++)
		{
			for (int j = xStart; j <= xStart + m_BlockWidth; j++)
			{
				if (intpLabel[i * pyWidth + j] != 0)
				{
					xSum += j;
					ySum += i;
					num2++;
				}
			}
		}
		if (num2)
		{
			x2 = xSum / num2;
			y2 = ySum / num2;
		}

		
		if (!num1 || !num2)
		{
			angle = 90;

		}
		else
		{
			angle =  90 - atan2((y2 - y1),(x2 - x1) ) / PI * 180 + 90;
			m_cenPoint.x += x1 + x2;
			m_cenPoint.y += y1 + y2;
			m_cenPoint.x /= 3;
			m_cenPoint.y /= 3;
		}

		//		result		//
		m_cenPoint.x *= pow(2, m_pyramidLevel);
		m_cenPoint.y *= pow(2, m_pyramidLevel);


		x = m_cenPoint.x + HALCON_X_BIAS;
		y = m_cenPoint.y + HALCON_Y_BIAS;
		angle = angle + HALCON_ANGLE_BIAS;
	}



	//delete
	if (ucpErode != nullptr)
		delete[] ucpErode;
	if(ucpDilate != nullptr)
		delete[] ucpDilate;

	if (projectScore != nullptr)
		delete[] projectScore;
	if (pRegionWH != nullptr)
		delete[] pRegionWH;
	if (pRegionPeakPoint != nullptr)
		delete[] pRegionPeakPoint;
	if (pRegionArea != nullptr)
		delete[] pRegionArea;
	if (pRegionCen != nullptr)
		delete[] pRegionCen;

	if(ucpPyramid != nullptr)
		delete[] ucpPyramid;
	if(ucpHisSpec != nullptr)
		delete[] ucpHisSpec;
	if(ucpThreshold != nullptr)
		delete[] ucpThreshold;
	if(intpLabel != nullptr)
		delete[] intpLabel;

	if (devStatus == 0)
		return _CODE_OK;
	else
		return _CODE_LOCATE_FAIL;
}


_BACK_CODE CBmPocketLocation::PocketLocation(const IMG_UBBUF src, double & x, double & y, double & angle)
{
	if (src.ptr == NULL)
	{
		return _CODE_THROW;
	}
	int devStatus = 0;

	int width = src.size.width;
	int height = src.size.height;

	//	pyramid		//
	unsigned char *ucpPyramid = new unsigned char[width * height];
	int pyWidth = 0;
	int pyHeight = 0;
	if (m_pyramidLevel > 0)
	{
		devStatus = pyramid(src, ucpPyramid, pyWidth, pyHeight, m_pyramidLevel);
		if (devStatus != 0)
		{
			delete[] ucpPyramid;
			return _CODE_LOCATE_FAIL;
		}
	}
	else
	{
		memcpy(ucpPyramid, src.ptr, width * height * sizeof(unsigned char));
		pyWidth = width;
		pyHeight = height;
	}

	IMG_UBBUF ubbPyramid;
	m_setVisBuf.set_IMG_UBBUF(ubbPyramid, ucpPyramid,{ (IMG_UWORD)pyWidth,(IMG_UWORD)pyHeight }, pyWidth);

	//		locate pocket	//
	int xLeft = 0;
	int xRight = 0;
	PocketCoarseLocate(ubbPyramid,xLeft,xRight);
	int newWidth = xRight - xLeft;
	unsigned char *ucpCut = new unsigned char[newWidth * pyHeight];
	for (int i = 0; i < pyHeight; i++)
	{
		memcpy(ucpCut + i * newWidth, ucpPyramid + i * pyWidth + xLeft, newWidth);
	}
	IMG_UBBUF ubbCut;
	m_setVisBuf.set_IMG_UBBUF(ubbCut, ucpCut,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, newWidth);


	//	histogram analyse	//
	Ipp32u *pHistVec = new Ipp32u[256];
	Vis_HistogramExample(ucpCut,{ newWidth,pyHeight }, pHistVec);
	IMG_UBBUF ubbHisShow;
	IMG_UBYTE *ucpHisShow = new IMG_UBYTE[256 * 10000];
	memset(ucpHisShow, 0, sizeof(IMG_UBYTE) * 256 * 10000);
	for (int k = 0; k < 256; k++)
	{
		int propotion = (float)pHistVec[k] / (newWidth * pyHeight) * 10000;
		for (int i = 0; i < propotion; i++)
		{
			ucpHisShow[i * 256 + k] = 255;
		}
	}
	m_setVisBuf.set_IMG_UBBUF(ubbHisShow, ucpHisShow,{ (IMG_UWORD)256,(IMG_UWORD)(10000) }, 256);

	//	histogram erase  (erase light area)//
	Ipp64f pMean = 0;
	Ipp64f pStdDev = 0;
	ippiMean_StdDev_8u_C1R(ubbCut.ptr, ubbCut.linestep,{ ubbCut.size.width,ubbCut.size.height }, &pMean, &pStdDev);
	if (pMean < m_hisEraseMean)
		HistogramFilter(ubbCut, pHistVec, m_hisErasePro, (IMG_UBYTE)pMean);
	
	if(ucpHisShow != nullptr)
		delete[] ucpHisShow;
	if(pHistVec != nullptr)
		delete[] pHistVec;
	////////////////////////////////////

	//	threshold	//
	IMG_UBBUF ubbThreshold;
	unsigned char *ucpThreshold = new unsigned char[newWidth * pyHeight];
	devStatus = threshold_OTSU(ucpCut,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, ucpThreshold);
	m_setVisBuf.set_IMG_UBBUF(ubbThreshold, ucpThreshold,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, newWidth);

	//		Erode		//
	IMG_UBBUF ubbErode;
	unsigned char *ucpErode = new unsigned char[newWidth * pyHeight];
	devStatus = morfErodeBorder_notInPlace_3by3(ubbThreshold, ucpErode);
	m_setVisBuf.set_IMG_UBBUF(ubbErode, ucpErode,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, newWidth);
	//////////////////////////////////////////

	//		dialate		//
	IMG_UBBUF ubbDilate;
	unsigned char *ucpDilate = new unsigned char[newWidth * pyHeight];
	devStatus = morfDilateBorder_notInPlace_3by3(ubbErode, ucpDilate);
	m_setVisBuf.set_IMG_UBBUF(ubbDilate, ucpDilate,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, newWidth);

	//	label region	//
	int regionNum = 0;
	int *intpLabel = new int[newWidth * pyHeight];
	devStatus = labelMarker(ucpDilate,{ newWidth,pyHeight }, intpLabel, regionNum);
	if (devStatus != 0 || regionNum <= 0)
	{
		if (ucpPyramid != nullptr)
			delete[] ucpPyramid;
		if (ucpCut != nullptr)
			delete[] ucpCut;
		if (ucpThreshold != nullptr)
			delete[] ucpThreshold;
		if (ucpErode != nullptr)
			delete[] ucpErode;
		if (ucpDilate != nullptr)
			delete[] ucpDilate;
		if (intpLabel != nullptr)
			delete[] intpLabel;
		return _CODE_LOCATE_FAIL;
	}
	IMG_INTBUF iBufLabel;
	m_setVisBuf.set_IMG_INTBUF(iBufLabel, intpLabel,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, newWidth * sizeof(int));
	///////////////////////

	//	blob analyse	//
	int *pRegionArea = new int[regionNum];
	IppiPoint *pRegionCen = new IppiPoint[regionNum];
	regionDef::RegionPeakPoint *pRegionPeakPoint = new regionDef::RegionPeakPoint[regionNum];
	regionDef::RegionWidthHeight *pRegionWH = new regionDef::RegionWidthHeight[regionNum];
	ProjectScore *projectScore = new ProjectScore[regionNum];

	devStatus = getRegionInfo(intpLabel,{ newWidth,pyHeight }, regionNum, pRegionArea, pRegionCen, pRegionPeakPoint, pRegionWH, projectScore);

	//	retry  dump  DUT	//
	if (devStatus == -1)
	{
		//copy ucpPyramid//
		unsigned char *ucpCut_Clone = new unsigned char[newWidth * pyHeight];
		memcpy(ucpCut_Clone, ucpCut, newWidth * pyHeight);
		IMG_UBBUF ubbCut_Clone;
		m_setVisBuf.set_IMG_UBBUF(ubbCut_Clone, ucpCut_Clone,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, newWidth);

		//	find DUT(area max)	//
		//vector <int> vecDUTindex;
		m_vecDUTindex.clear();
		for (int k = 1; k < regionNum; k++)
		{
			if (pRegionArea[k] > m_DUT_areaMin)
			{
				m_vecDUTindex.push_back(k);
			}
		}
		for (int k = 0; k < m_vecDUTindex.size(); k++)
		{
			int val = m_vecDUTindex[k];
			for (int i = 0; i < pyHeight; i++)
			{
				for (int j = 0; j < newWidth; j++)
				{
					if (intpLabel[i * newWidth + j] == val)
					{
						ucpCut_Clone[i * newWidth + j] = 0;
					}
				}
			}
		}

		//	去除亮白区域的影响!!!!!!!!	//
		int pos = 0;
		for (int i = 0; i < pyHeight; i++)
		{
			for (int j = 0; j < newWidth; j++)
			{
				if ((ucpCut_Clone[pos + j]) >= m_whiteValue)		//将亮圆部分填黑，以便OTSU找准
				{
					ucpCut_Clone[pos + j] = 0;
				}
			}
			pos += newWidth;
		}
		////////////////////////////////

		if (projectScore != nullptr)
			delete[] projectScore;
		if (pRegionWH != nullptr)
			delete[] pRegionWH;
		if (pRegionPeakPoint != nullptr)
			delete[] pRegionPeakPoint;
		if (pRegionArea != nullptr)
			delete[] pRegionArea;
		if (pRegionCen != nullptr)
			delete[] pRegionCen;

		IMG_UBBUF ubbHisSpec;
		unsigned char *ucpHisSpec = new unsigned char[newWidth * pyHeight];
		m_setVisBuf.set_IMG_UBBUF(ubbHisSpec, ucpHisSpec,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, newWidth);
		//devStatus = VisHistogramSpecification(ubbCut_Clone, m_meanValue, m_stdDevValue, ubbHisSpec);

		devStatus = threshold_OTSU(ucpCut_Clone,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, ucpThreshold);
		m_setVisBuf.set_IMG_UBBUF(ubbThreshold, ucpThreshold,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, newWidth);

		devStatus = labelMarker(ucpThreshold,{ newWidth,pyHeight }, intpLabel, regionNum);
		if (devStatus != 0 || regionNum <= 0)
		{
			if (ucpPyramid != nullptr)
				delete[] ucpPyramid;
			if (ucpCut != nullptr)
				delete[] ucpCut;
			if (ucpThreshold != nullptr)
				delete[] ucpThreshold;
			if (ucpErode != nullptr)
				delete[] ucpErode;
			if (ucpDilate != nullptr)
				delete[] ucpDilate;
			if (intpLabel != nullptr)
				delete[] intpLabel;
			return _CODE_LOCATE_FAIL;
		}
		m_setVisBuf.set_IMG_INTBUF(iBufLabel, intpLabel,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, newWidth * sizeof(int));

		//realloc
		pRegionArea = new int[regionNum];
		pRegionCen = new IppiPoint[regionNum];
		pRegionPeakPoint = new regionDef::RegionPeakPoint[regionNum];
		pRegionWH = new regionDef::RegionWidthHeight[regionNum];
		projectScore = new ProjectScore[regionNum];

		devStatus = getRegionInfo2(intpLabel,{ newWidth,pyHeight }, regionNum, pRegionArea, pRegionCen, pRegionPeakPoint, pRegionWH, projectScore);

		if (devStatus == -1)
		{
			//	继续删大亮块	//
			m_vecDUTindex.clear();
			for (int k = 1; k < regionNum; k++)
			{
				if (pRegionArea[k] > m_DUT_areaMin)
				{
					m_vecDUTindex.push_back(k);
				}
			}
			for (int k = 0; k < m_vecDUTindex.size(); k++)
			{
				int val = m_vecDUTindex[k];
				for (int i = 0; i < pyHeight; i++)
				{
					for (int j = 0; j < newWidth; j++)
					{
						if (intpLabel[i * newWidth + j] == val)
						{
							ucpCut_Clone[i * newWidth + j] = 0;
						}
					}
				}
			}
			if (projectScore != nullptr)
				delete[] projectScore;
			if (pRegionWH != nullptr)
				delete[] pRegionWH;
			if (pRegionPeakPoint != nullptr)
				delete[] pRegionPeakPoint;
			if (pRegionArea != nullptr)
				delete[] pRegionArea;
			if (pRegionCen != nullptr)
				delete[] pRegionCen;

			pHistVec = new Ipp32u[256];
			Vis_HistogramExample(ucpCut_Clone,{ newWidth,pyHeight }, pHistVec);
			//	histogram erase  //
			pMean = 0;
			pStdDev = 0;
			ippiMean_StdDev_8u_C1R(ubbCut_Clone.ptr, ubbCut_Clone.linestep,{ ubbCut_Clone.size.width,ubbCut_Clone.size.height }, &pMean, &pStdDev);
			if (pMean < m_hisEraseMean)
				HistogramFilter(ubbCut_Clone, pHistVec, m_hisErasePro, (IMG_UBYTE)pMean);
			if(pHistVec != nullptr)
				delete[] pHistVec;
			////////////////////////////////////


			devStatus = threshold_OTSU(ucpCut_Clone,{ (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, ucpThreshold);
			devStatus = labelMarker(ucpThreshold,{ newWidth,pyHeight }, intpLabel, regionNum);
			if (devStatus != 0 || regionNum <= 0)
			{
				if (ucpPyramid != nullptr)
					delete[] ucpPyramid;
				if (ucpCut != nullptr)
					delete[] ucpCut;
				if (ucpThreshold != nullptr)
					delete[] ucpThreshold;
				if (ucpErode != nullptr)
					delete[] ucpErode;
				if (ucpDilate != nullptr)
					delete[] ucpDilate;
				if (intpLabel != nullptr)
					delete[] intpLabel;
				return _CODE_LOCATE_FAIL;
			}

			//realloc
			pRegionArea = new int[regionNum];
			pRegionCen = new IppiPoint[regionNum];
			pRegionPeakPoint = new regionDef::RegionPeakPoint[regionNum];
			pRegionWH = new regionDef::RegionWidthHeight[regionNum];
			projectScore = new ProjectScore[regionNum];

			devStatus = getRegionInfo2(intpLabel,{ newWidth,pyHeight }, regionNum, pRegionArea, pRegionCen, pRegionPeakPoint, pRegionWH, projectScore);

		}
		if(ucpCut_Clone != nullptr)
			delete[]	ucpCut_Clone;
		if(ucpHisSpec != nullptr)
			delete[]	ucpHisSpec;
	}
	////////////////////////////////////

	//	erase inverse dut	//
	if (devStatus == -1)
	{
		if (projectScore != nullptr)
			delete[] projectScore;
		if (pRegionWH != nullptr)
			delete[] pRegionWH;
		if (pRegionPeakPoint != nullptr)
			delete[] pRegionPeakPoint;
		if (pRegionArea != nullptr)
			delete[] pRegionArea;
		if (pRegionCen != nullptr)
			delete[] pRegionCen;

		erase_inverseDUT(ubbCut, 0.1);
		devStatus = threshold_OTSU(ucpCut, { (IMG_UWORD)newWidth,(IMG_UWORD)pyHeight }, ucpThreshold);
		devStatus = morfDilateBorder_notInPlace_3by3(ubbThreshold, ucpDilate);
		devStatus = labelMarker(ucpDilate, { newWidth,pyHeight }, intpLabel, regionNum);
		if (devStatus != 0 || regionNum <= 0)
		{
			if (ucpPyramid != nullptr)
				delete[] ucpPyramid;
			if (ucpCut != nullptr)
				delete[] ucpCut;
			if (ucpThreshold != nullptr)
				delete[] ucpThreshold;
			if (ucpErode != nullptr)
				delete[] ucpErode;
			if (ucpDilate != nullptr)
				delete[] ucpDilate;
			if (intpLabel != nullptr)
				delete[] intpLabel;
			return _CODE_LOCATE_FAIL;
		}

		//realloc
		pRegionArea = new int[regionNum];
		pRegionCen = new IppiPoint[regionNum];
		pRegionPeakPoint = new regionDef::RegionPeakPoint[regionNum];
		pRegionWH = new regionDef::RegionWidthHeight[regionNum];
		projectScore = new ProjectScore[regionNum];
		devStatus = getRegionInfo2(intpLabel, { newWidth,pyHeight }, regionNum, pRegionArea, pRegionCen, pRegionPeakPoint, pRegionWH, projectScore);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////


	//		cal result		//
	if (devStatus == 0)
	{
		LShapeSelect();		//	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		for (int i = 0; i < m_LshapeIndex.size(); i++)
		{
			for (int j = 0; j < m_LshapeIndex[i].size(); j++)
			{
 				m_LshapeIndex[i][j].pos.x += xLeft;
			}
		}

		float angleMin = 90;
		//	cal center  //
		int Lnum = 0;
		m_cenPoint.x = 0;
		m_cenPoint.y = 0;
		for (int i = 0; i < m_LshapeIndex.size(); i++)
		{
			if (i == 0 && m_LshapeIndex[i].size())
			{
				m_cenPoint.x += m_LshapeIndex[i][0].pos.x + m_xBias;
				m_cenPoint.y += m_LshapeIndex[i][0].pos.y + m_yBias;
				Lnum++;

				break;
			}
			if (i == 1 && m_LshapeIndex[i].size())
			{
				m_cenPoint.x += m_LshapeIndex[i][0].pos.x - m_xBias;
				m_cenPoint.y += m_LshapeIndex[i][0].pos.y + m_yBias;
				Lnum++;

			}
			if (i == 2 && m_LshapeIndex[i].size())
			{
				m_cenPoint.x += m_LshapeIndex[i][0].pos.x + m_xBias;
				m_cenPoint.y += m_LshapeIndex[i][0].pos.y - m_yBias;
				Lnum++;

			}
			if (i == 3 && m_LshapeIndex[i].size())
			{
				m_cenPoint.x += m_LshapeIndex[i][0].pos.x - m_xBias;
				m_cenPoint.y += m_LshapeIndex[i][0].pos.y - m_yBias;
				Lnum++;
			}
		}
		//GET MEAN
		m_cenPoint.x /= Lnum;
		m_cenPoint.y /= Lnum;

		//	trace back to origin	//
		for (int i = 0; i < m_LshapeIndex.size(); i++)
		{
			for (int j = 0; j < m_LshapeIndex[i].size(); j++)
			{
				m_LshapeIndex[i][j].pos.x *= pow(2, m_pyramidLevel);
				m_LshapeIndex[i][j].pos.y *= pow(2, m_pyramidLevel);
			}

		}

		//	cal theta	//
		float x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		//find top block
		float xSum = 0, ySum = 0; int num1 = 0, num2 = 0; int xStart, yStart;
		xStart = m_cenPoint.x - m_BlockShift_x - xLeft;
		if (xStart < 0)	xStart = 0;
		yStart = m_cenPoint.y - m_BlockShift_y;
		if (yStart < 0)	yStart = 0;
		for (int i = yStart; i <= yStart + m_BlockHeight; i++)
		{
			for (int j = xStart; j <= xStart + m_BlockWidth; j++)
			{
				if (intpLabel[i * newWidth + j] != 0)
				{
					xSum += j;
					ySum += i;
					num1++;
				}
			}
		}
		if (num1)
		{
			x1 = xSum / num1;
			y1 = ySum / num1;
		}


		//find bottom block
		xSum = 0; ySum = 0; num2 = 0;
		xStart = m_cenPoint.x - m_BlockShift_x - xLeft;
		if (xStart < 0)	xStart = 0;
		yStart = m_cenPoint.y + m_BlockShift_y;
		if (yStart > pyHeight - 1)	yStart = pyHeight - 1;
		for (int i = yStart - m_BlockHeight; i <= yStart; i++)
		{
			for (int j = xStart; j <= xStart + m_BlockWidth; j++)
			{
				if (intpLabel[i * newWidth + j] != 0)
				{
					xSum += j;
					ySum += i;
					num2++;
				}
			}
		}
		if (num2)
		{
			x2 = xSum / num2;
			y2 = ySum / num2;
		}


		if (!num1 || !num2)
		{
			angle = 90;

		}
		else
		{
			angle = 90 - atan2((y2 - y1), (x2 - x1)) / PI * 180 + 90;
			m_cenPoint.x += x1 + x2 + 2 * xLeft;
			m_cenPoint.y += y1 + y2;
			m_cenPoint.x /= 3;
			m_cenPoint.y /= 3;
		}

		//		result		//
		m_cenPoint.x *= pow(2, m_pyramidLevel);
		m_cenPoint.y *= pow(2, m_pyramidLevel);

		x = m_cenPoint.x + HALCON_X_BIAS;
		y = m_cenPoint.y + HALCON_Y_BIAS;
		angle = angle + HALCON_ANGLE_BIAS;
		//printf("angle = %lf,   ", angle);
	}

	if(ucpPyramid != nullptr)
		delete[] ucpPyramid;
	if(ucpCut != nullptr)
		delete[] ucpCut;
	if(ucpThreshold != nullptr)
		delete[] ucpThreshold;
	if(ucpErode != nullptr)
		delete[] ucpErode;
	if(ucpDilate != nullptr)
		delete[] ucpDilate;
	if(intpLabel != nullptr)
		delete[] intpLabel;

	if (projectScore != nullptr)
		delete[] projectScore;
	if (pRegionWH != nullptr)
		delete[] pRegionWH;
	if (pRegionPeakPoint != nullptr)
		delete[] pRegionPeakPoint;
	if (pRegionArea != nullptr)
		delete[] pRegionArea;
	if (pRegionCen != nullptr)
		delete[] pRegionCen;

	if (devStatus == 0)
	{
		if (m_cenPoint.x - HALF_LENGTH >= 0 && m_cenPoint.x + HALF_LENGTH < width && m_cenPoint.y - HALF_LENGTH >= 0 && m_cenPoint.y + HALF_LENGTH < height)
		{
			return _CODE_OK;
		}
		else
		{
			return _CODE_LOCATE_FAIL;
		}
	}
	else
		return _CODE_LOCATE_FAIL;

}

