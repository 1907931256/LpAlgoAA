#include "stdafx.h"
#include "BmPocketStatus.h"

//#define  OUT_HALF_LENGTH 640	//pocket半宽
//#define  HALF_LENGTH 380		//tray半宽
//
//#define GRAD_INSIDE	  200		//分割时hole里的梯度阈值
//#define GRAD_INSIDE_MAX	  500	//分割时hole里的最大梯度阈值
//#define GRAD_OUTSIDE  50		//分割tray里hole外区域的梯度阈值
//#define  GRAD_COUNT_MAX   5000	//对hole里超过最大梯度阈值的区域进行计数，超过此值则认为是异物
//
//#define HOLE_AREA_MIN	130000		//hole黑洞最小面积
//#define  TRAY_HALF_WIDTH 200		//hole半宽
//
//#define FOREIGN_BODY 150000		//外边缘异物面积判定

CBmPocketStatus::CBmPocketStatus()
{
	m_outHalfLength = 640;
	m_halfLength = 380;
	m_grayInside = 200;
	m_grayInsideMax = 500;
	m_grayOutside = 50;
	m_grayCountMax = 5000;
	m_holeAreaMin = 130000;
	m_trayHalfWidth = 200;
	m_foreignBody = 150000;
	m_grayMean = 0;
}

CBmPocketStatus::~CBmPocketStatus()
{
}

const size_t MAX_LEN = 128;
bool CBmPocketStatus::LoadParameter(char path[])
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
				if (0 == strcmp(LineBuf, "m_outHalfLength "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_outHalfLength);

				}
				else if (0 == strcmp(LineBuf, "m_halfLength "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_halfLength);
				}
				else if (0 == strcmp(LineBuf, "m_grayInside "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_grayInside);
				}
				else if (0 == strcmp(LineBuf, "m_grayInsideMax "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_grayInsideMax);
				}
				else if (0 == strcmp(LineBuf, "m_grayOutside "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_grayOutside);
				}
				else if (0 == strcmp(LineBuf, "m_grayCountMax "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_grayCountMax);
				}
				else if (0 == strcmp(LineBuf, "m_holeAreaMin "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_holeAreaMin);
				}
				else if (0 == strcmp(LineBuf, "m_trayHalfWidth "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_trayHalfWidth);
				}
				else if (0 == strcmp(LineBuf, "m_foreignBody "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_foreignBody);
				}
				else if (0 == strcmp(LineBuf, "m_grayMean "))
				{
					for (; *pos == ' '; pos++) {}
					sscanf(pos, "%d", &m_grayMean);
				}
				else
				{
					printf("Failed to read information from the file.");
					break;
				}
			}
		}

		fclose(configFile);
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

int CBmPocketStatus::threshold_OTSU_inv(unsigned char * ucpSrc, IppiSize imgSize, unsigned char * ucpDst)
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
					ucpDst[pos + j] = 255;
				}
				else
				{
					ucpDst[pos + j] = 0;
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


int CBmPocketStatus::threshold_OTSU(unsigned char * ucpSrc, IppiSize imgSize, unsigned char * ucpDst)
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


int CBmPocketStatus::morfDilateBorder_notInPlace_5by5(IMG_UBBUF src, unsigned char *pDst)
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

		status = ippiDilateBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, { roiSize.width,roiSize.height }, borderType, borderValue, pSpec, pBuffer);

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
int CBmPocketStatus::labelMarker(unsigned char *ucpSrc, IppiSize imgSize, int *intpDst, int &regionNum)
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


/**********************************************/
// getRegionInfo, blob分析
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
//	0 ：成功
//  
// Author: Jimmy Zhan  3/5/2017
/**********************************************/
int CBmPocketStatus::getRegionInfo(int * intSrc,
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

		//	cal regionGraCenPoint & regionWH 	//
		for (int k = 1; k < regionNum; k++)
		{
			if (regionArea[k] != 0)
			{
				regionGraCenPoint[k].x = xSum[k] / regionArea[k];
				regionGraCenPoint[k].y = ySum[k] / regionArea[k];
			}
			regionWH[k].width = regionPeakPoint[k].xMax - regionPeakPoint[k].xMin;
			regionWH[k].height = regionPeakPoint[k].yMax - regionPeakPoint[k].yMin;


		}

		//free
		if (xSum != nullptr)
			delete[] xSum;
		if (ySum != nullptr)
			delete[] ySum;

		return 0;
	}
	
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


void CBmPocketStatus::SobelFilter_8u16s_C1_5x5(IMG_UBYTE * pSrc, IppiSize roiSize, Ipp16s *pDst, Ipp32f *pAngle)
{
	IppiMaskSize mask = ippMskSize5x5;
	IppiBorderType bordertype = ippBorderRepl; //Border is replicated from the edge pixels
	Ipp16s *pHoriz, *pVert;

	int srcStep = roiSize.width * sizeof(Ipp8u);

	int dstStep = roiSize.width * sizeof(Ipp16s);
	int angleStep = roiSize.width * sizeof(Ipp32f);
	int bufferSize;
	int bufLen = roiSize.width * roiSize.height;
	IppStatus statusVert, statusHoriz, status;
	Ipp8u *pBuffer;
	IppNormType normType = ippNormL2;//input gradient magnitude

	pVert = (Ipp16s *)malloc(sizeof(Ipp16s)*bufLen);
	pHoriz = (Ipp16s *)malloc(sizeof(Ipp16s)*bufLen);

	ippiGradientVectorGetBufferSize(roiSize, mask, ipp16s, 1, &bufferSize);
	pBuffer = (Ipp8u *)malloc(bufferSize);
	ippiGradientVectorSobel_8u16s_C1R(pSrc, srcStep, pVert, dstStep, pHoriz, dstStep, pDst, dstStep, pAngle, angleStep, roiSize, mask, normType, bordertype, NULL, pBuffer);

	free(pVert);
	free(pHoriz);
	free(pBuffer);
}


int CBmPocketStatus::threshold_manual_inv(unsigned char * ucpSrc, IppiSize imgSize, unsigned char * ucpDst)
{
	__try
	{
		int status = ippStsNoErr;
		int width = imgSize.width;
		int height = imgSize.height;

		//	do threshold	//
		int srcStep = width * sizeof(unsigned char);
		int dstStep = srcStep;
		status = ippiThreshold_LTValGTVal_8u_C1R(ucpSrc, srcStep, ucpDst, dstStep, imgSize, 1, 255, 1, 0);

		return status;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}





int CBmPocketStatus::morfErodeBorder_notInPlace_7by7(IMG_UBBUF src, unsigned char *pDst)
{
	__try
	{
		IppStatus status = ippStsNoErr;

		unsigned char *pSrc = src.ptr;
		IMG_SIZE roiSize = src.size;

		IppiMorphState* pSpec = NULL;
		Ipp8u* pBuffer = NULL;
		Ipp8u pMask[7 * 7] = {
			1,0,0,0,0,0,1,
			0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,
			1,0,0,0,0,0,1
		};
		IppiSize maskSize = { 7, 7 };
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

		status = ippiErodeBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, { roiSize.width,roiSize.height }, borderType, borderValue, pSpec, pBuffer);

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
bool CBmPocketStatus::eraseEdgeRegion(
	regionDef::RegionPeakPoint regionPeakPoint,
	int width,
	int height
	)
{
	if (regionPeakPoint.xMin == 0 || regionPeakPoint.yMin == 0)
	{
		return true;
	}
	if (regionPeakPoint.xMax == width - 1 || regionPeakPoint.yMax == height - 1)
	{
		return true;
	}

	else
	{
		return false;
	}

}

IppStatus pyramid(IMG_UBBUF src, unsigned char * pDst, int & pyramid_width, int & pyramid_height, int level)
{
	IppStatus   status = ippStsNoErr;
	//ofstream outfile("pyramidData.txt");

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
	status = ippiPyramidGetSize(&pyrStructSize, &pyrBufferSize, level, { roiSize.width,roiSize.height }, rate);
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
	status = ippiPyramidInit(&pPyrStruct, level, { roiSize.width,roiSize.height }, rate, pPyrStrBuffer, pPyrBuffer);
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
	status = ippiPyramidLayerDownInit_8u_C1R((IppiPyramidDownState_8u_C1R**)&pPyrStruct->pState, { roiSize.width,roiSize.height }, rate, kernel, 3, IPPI_INTER_LINEAR, pPyrLStateBuf, pPyrLBuffer);
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

	//	test //
	//for (int i = 0; i < pPyrStruct->pRoi[level].height; i++)
	//{
	//	for (int j = 0; j < pPyrStruct->pRoi[level].width; j++)
	//	{
	//		outfile << (float)pPyrImage[level][i * pPyrStruct->pRoi[level].width + j] << " ";
	//	}
	//	outfile << endl;
	//}
	//outfile.close();



exit_proc:
	for (int i = 1; i <= level; i++)
		free(pPyrImage[i]);
	free(pPyrStrBuffer);
	free(pPyrBuffer);
	free(pPyrLBuffer);
	free(pPyrLStateBuf);
	//for (int i = 1; i <= level; i++)
	//	ippiFree(pPyrImage[i]);
	//ippiFree(pPyrLStateBuf);
	//ippiFree(pPyrLBuffer);
	//ippiFree(pPyrStrBuffer);
	//ippiFree(pPyrBuffer);
	goto exit;

exit:
	//printf("pyramidStatus: %s\n", ippGetStatusString(status));
	if (!status)
	{
	}
	return status;
}

/**********************************************/
// Vis_ForeignJudge, 功能说明:判断Socket中是否有异物。
// Input:
// 	Ipp8u *srcRoi, 输入图片(小图1/16大小)
//	IppiSize Roi,感兴趣区域大小(小图1/16大小)
//  	DUTOutPutParam param，芯片的特征点(原图大小)
//     ...
// Output:
// 	int &result, socket是否有异物（1为有，2为摆放标准）
//     ...
// Return:
//     0 - 正常
// Author: Huang Yige/05/02/2017
/**********************************************/
int Vis_ForeignJudge(Ipp8u *srcRoi, IppiSize Roi, int &result, DUTOutPutParam param, int angle)
{
	Ipp8u* JudgeRoi1;
	Ipp8u* JudgeRoi2;
	Ipp8u* JudgeRoi3;
	Ipp8u* JudgeRoi4;
	Ipp8u* JudgeStandard;

	IppiSize RoiStandard = { 120,80 };
	IppiSize Roi1;
	IppiSize Roi2;
	IppiSize Roi3;
	IppiSize Roi4;

	IMG_COORD JudgeRoiPoint1;
	IMG_COORD JudgeRoiPoint2;
	IMG_COORD JudgeRoiPoint3;
	IMG_COORD JudgeRoiPoint4;
	IMG_COORD JudgeStandardPoint;

	if (angle == 0)
	{
		JudgeStandardPoint = { (IMG_WORD)(param.cx - 60),(IMG_WORD)(param.cy - 60) };

		if (param.y1 < param.y2)
		{
			JudgeRoiPoint1 = { (IMG_WORD)(param.x1),(IMG_WORD)(param.y2) };
		}
		else
		{
			JudgeRoiPoint1 = { (IMG_WORD)(param.x1),(IMG_WORD)(param.y1) };
		}
		if (param.x2 < param.x3)
		{
			JudgeRoiPoint2 = { (IMG_WORD)(param.x2 - 80),(IMG_WORD)(param.y2) };
		}
		else
		{
			JudgeRoiPoint2 = { (IMG_WORD)(param.x3 - 80),(IMG_WORD)(param.y2) };
		}
		if (param.y4 > param.y3)
		{
			JudgeRoiPoint3 = { (IMG_WORD)(param.x4),(IMG_WORD)(param.y3 - 80) };
		}
		else
		{
			JudgeRoiPoint3 = { (IMG_WORD)(param.x4),(IMG_WORD)(param.y4 - 80) };
		}
		if (param.x1 > param.x4)
		{
			JudgeRoiPoint4 = { (IMG_WORD)(param.x1), (IMG_WORD)(param.y1) };
		}
		else
		{
			JudgeRoiPoint4 = { (IMG_WORD)(param.x4), (IMG_WORD)(param.y1) };
		}
		Roi1 = { (int)((param.x2) - (param.x1)),80 };
		Roi2 = { 80,(int)((param.y3) - (param.y2)) };
		Roi3 = { (int)((param.x3) - (param.x4)),80 };
		Roi4 = { 80,(int)((param.y4) - (param.y1)) };
	}
	if (angle == 180)
	{
		JudgeStandardPoint = { (IMG_WORD)(param.cx - 60),(IMG_WORD)(param.cy) };

		if (param.y3 < param.y4)
		{
			JudgeRoiPoint1 = { (IMG_WORD)(param.x3),(IMG_WORD)(param.y4) };
		}
		else
		{
			JudgeRoiPoint1 = { (IMG_WORD)(param.x3),(IMG_WORD)(param.y3) };
		}
		if (param.x4 < param.x1)
		{
			JudgeRoiPoint2 = { (IMG_WORD)(param.x4 - 80),(IMG_WORD)(param.y4) };
		}
		else
		{
			JudgeRoiPoint2 = { (IMG_WORD)(param.x1 - 80),(IMG_WORD)(param.y4) };
		}
		if (param.y2 > param.y1)
		{
			JudgeRoiPoint3 = { (IMG_WORD)(param.x2),(IMG_WORD)(param.y1 - 80) };
		}
		else
		{
			JudgeRoiPoint3 = { (IMG_WORD)(param.x2),(IMG_WORD)(param.y2 - 80) };
		}
		if (param.x3 > param.x2)
		{
			JudgeRoiPoint4 = { (IMG_WORD)(param.x3), (IMG_WORD)(param.y3) };
		}
		else
		{
			JudgeRoiPoint4 = { (IMG_WORD)(param.x2), (IMG_WORD)(param.y3) };
		}
		Roi1 = { (int)((param.x1) - (param.x2)),80 };
		Roi2 = { 80,(int)((param.y2) - (param.y3)) };
		Roi3 = { (int)((param.x4) - (param.x3)),80 };
		Roi4 = { 80,(int)((param.y1) - (param.y4)) };
	}
	/*IMG_COORD JudgeRoiPoint1 = { (int)(param.x1 / 4.0),(int)(param.y1 / 4.0) };
	IMG_COORD JudgeRoiPoint2= { (int)(param.x2 / 4.0) - 10,(int)(param.y2 / 4.0) };
	IMG_COORD JudgeRoiPoint3= { (int)(param.x4 / 4.0),(int)(param.y4 / 4.0) - 10 };
	IMG_COORD JudgeRoiPoint4 = { (int)(param.x1 / 4.0), (int)(param.y1 / 4.0) };
	IMG_COORD JudgeStandardPoint= { (int)(param.cx / 4.0) - 15,(int)(param.cy / 4.0) - 15 };*/

	/*IMG_WINDOW JudgeRoiSize1 ;
	JudgeRoiSize1.coWindowOff = { (int)(param.x1 / 4.0),(int)(param.y1 / 4.0) };
	JudgeRoiSize1.szWindowSize = { (int)((param.x2 / 4.0) - (param.x1 / 4.0)),10 };

	IMG_WINDOW JudgeRoiSize2;
	JudgeRoiSize2.coWindowOff = { (int)(param.x2 / 4.0) - 10,(int)(param.y2 / 4.0) };
	JudgeRoiSize2.szWindowSize = { 10,(int)((param.y3 / 4.0) - (param.y2 / 4.0)) };

	IMG_WINDOW JudgeRoiSize3;
	JudgeRoiSize3.coWindowOff = { (int)(param.x4 / 4.0),(int)(param.y4 / 4.0) - 10 };
	JudgeRoiSize3.szWindowSize = { (int)((param.x3 / 4.0) - (param.x4 / 4.0)),10 };

	IMG_WINDOW JudgeRoiSize4;
	JudgeRoiSize4.coWindowOff = { (int)(param.x1 / 4.0),(int)(param.y1 / 4.0) };
	JudgeRoiSize4.szWindowSize = { 10,(int)((param.y4 / 4.0) - (param.y1 / 4.0)) };

	IMG_WINDOW JudgeStandardSize;
	JudgeStandardSize.coWindowOff = { (int)(param.cx / 4.0) - 15,(int)(param.cy / 4.0) - 15 };
	JudgeStandardSize.szWindowSize = { 30,30 };
	*/
	JudgeRoi1 = (Ipp8u*)malloc(Roi1.width*Roi1.height * sizeof(Ipp8u));
	JudgeRoi2 = (Ipp8u*)malloc(Roi2.width*Roi2.height * sizeof(Ipp8u));
	JudgeRoi3 = (Ipp8u*)malloc(Roi3.width*Roi3.height * sizeof(Ipp8u));
	JudgeRoi4 = (Ipp8u*)malloc(Roi4.width*Roi4.height * sizeof(Ipp8u));
	JudgeStandard = (Ipp8u*)malloc(RoiStandard.width*RoiStandard.height * sizeof(Ipp8u));

	for (int i = 0; i < Roi1.height; i++)
	{
		for (int j = 0; j < Roi1.width; j++)
		{
			JudgeRoi1[i*Roi1.width + j] = srcRoi[(i + JudgeRoiPoint1.y)*Roi.width + (j + JudgeRoiPoint1.x)];
		}
	}
	for (int i = 0; i < Roi2.height; i++)
	{
		for (int j = 0; j < Roi2.width; j++)
		{
			JudgeRoi2[i*Roi2.width + j] = srcRoi[(i + JudgeRoiPoint2.y)*Roi.width + (j + JudgeRoiPoint2.x)];
		}
	}
	for (int i = 0; i < Roi3.height; i++)
	{
		for (int j = 0; j < Roi3.width; j++)
		{
			JudgeRoi3[i*Roi3.width + j] = srcRoi[(i + JudgeRoiPoint3.y)*Roi.width + (j + JudgeRoiPoint3.x)];
		}
	}
	for (int i = 0; i < Roi4.height; i++)
	{
		for (int j = 0; j < Roi4.width; j++)
		{
			JudgeRoi4[i*Roi4.width + j] = srcRoi[(i + JudgeRoiPoint4.y)*Roi.width + (j + JudgeRoiPoint4.x)];
		}
	}
	for (int i = 0; i < RoiStandard.height; i++)
	{
		for (int j = 0; j < RoiStandard.width; j++)
		{
			JudgeStandard[i*RoiStandard.width + j] = srcRoi[(i + JudgeStandardPoint.y)*Roi.width + (j + JudgeStandardPoint.x)];
		}
	}

	int srcDstStep1 = Roi1.width * sizeof(Ipp8u);
	int srcDstStep2 = Roi2.width * sizeof(Ipp8u);
	int srcDstStep3 = Roi3.width * sizeof(Ipp8u);
	int srcDstStep4 = Roi4.width * sizeof(Ipp8u);
	int srcDstStepStandard = RoiStandard.width * sizeof(Ipp8u);

	double pMean[5];

	ippiMean_8u_C1R(JudgeStandard, srcDstStepStandard, RoiStandard, &pMean[0]);
	ippiMean_8u_C1R(JudgeRoi1, srcDstStep1, Roi1, &pMean[1]);
	ippiMean_8u_C1R(JudgeRoi2, srcDstStep2, Roi2, &pMean[2]);
	ippiMean_8u_C1R(JudgeRoi3, srcDstStep3, Roi3, &pMean[3]);
	ippiMean_8u_C1R(JudgeRoi4, srcDstStep4, Roi4, &pMean[4]);

	int ErrorNum = 0;

	for (int i = 1; i < 5; i++)
	{
		if (angle == 0)
		{
			if (i == 5)
			{
				if (pMean[i] < pMean[0] * 0.53)
				{
					ErrorNum++;
				}
			}
			else
			{
				if (pMean[i] < pMean[0] * 0.6)
				{
					ErrorNum++;
				}
			}
		}
		if (angle == 180)
		{
			if (i == 3)
			{
				if (pMean[i] < pMean[0] * 0.53)
				{
					ErrorNum++;
				}
			}
			else
			{
				if (pMean[i] < pMean[0] * 0.6)
				{
					ErrorNum++;
				}
			}
		}
		
	}

	if (ErrorNum > 0)
	{
		result = 1;
	}
	else
	{
		result = 2;
	}

	free(JudgeRoi1);
	free(JudgeRoi2);
	free(JudgeRoi3);
	free(JudgeRoi4);
	free(JudgeStandard);
	return 0;
}


/**********************************************/
// judgeNormalDUT2, 判断异物是否为DUT且摆放正确
// Input :
// const IMG_UBBUF src, 输入图像
// int trayX, trayCenter x coordinate
// int trayY , trayCenter y coordinate
//
// Return:
//	true : 有DUT且摆放正常
//  false: 异物或者DUT摆放不正
// Author: Jimmy Zhan  3/5/2017
/**********************************************/
bool CBmPocketStatus::judgeNormalDUT(const IMG_UBBUF src,int trayX,int trayY)
{
	int status = 0;
	int width = src.size.width;
	int height = src.size.height;
	DUTOutPutParam outputPara;

	//coarse locate
	IMG_COORD point1, point2, point3, point4, lightCen;
	m_DUTTrayLocate.CoarseLocation(src, 0, point1, point2, point3, point4, lightCen);
	
	int angleFlag = 0;
	//accurate locate
	if (lightCen.x || lightCen.y)
	{
		if (lightCen.y >= trayY)
		{
			angleFlag = 0;
		}
		else
		{
			angleFlag = 180;
		}

		status = m_DUTTrayLocate.DUTLocation(src, angleFlag, outputPara);
		if (status == _CODE_OK)
		{
			//unsigned char *ucpPyramid = new unsigned char[width * height];
			//int pyWidth = 0;
			//int pyHeight = 0;
			//pyramid(src, ucpPyramid, pyWidth, pyHeight, 2);
			int result;
			status = Vis_ForeignJudge(src.ptr, { width,height }, result, outputPara, angleFlag);
			//delete[] ucpPyramid;
			if (result == 1)
			{
				return false;
			}
			if (result == 2)
			{
				return true;
			}
		}
		else
		{
			return false;
		}

	}
	else
	{
		return false;
	}

}

bool CBmPocketStatus::judgeNormalDUT2(const IMG_UBBUF src, int trayX, int trayY)
{
	int status = 0;
	DUTOutPutParam param;
	int angle = 0;

	status = m_judgeDUT.DUTLocation(src,param,angle);
	if (status == _CODE_DUT_NORMAL)
	{
		//if (angle == 180)
		//	printf("hahahahahahahahaha			");
		return true;

	}
	else		//_CODE_PROERROR or _CODE_LOCATE_FAIL
	{
		return false;
	}

}





/**********************************************/
// GetPocketStatus, pocket状态判断
// Input:
// IMG_UBBUF src; 输入图片
//
// Return:
//		_CODE_EMPTY,//空料；	14
//		_CODE_PROERROR, //物料异常	15
//		_CODE_DUT_NORMAL , //物料正常   16
//		_CODE_LOCATE_FAIL,//定位失败 4  
//		_CODE_THROW//无图-1
// Author: Jimmy Zhan  3/5/2017
/**********************************************/

_BACK_CODE CBmPocketStatus::GetPocketStatus(const IMG_UBBUF src)
{
	if (src.ptr == NULL)
	{
		return _CODE_THROW;
	}
	int devStatus = 0;
	
	int width = src.size.width;
	int height = src.size.height;

	double centerX = 0;
	double centerY = 0;
	double rotateAngle = 0;

	devStatus = m_pocketLocate.LoadParameter("BmPocketLocation.ini");
	if (devStatus != true)
	{
		return _CODE_FILE_LOST;
	}

	//m_DUTTrayLocate.LoadParameter("configurationfile.txt");		//undone

	devStatus = m_pocketLocate.PocketLocation(src, centerX, centerY, rotateAngle);
	if (devStatus != _CODE_OK)
	{
		return _CODE_LOCATE_FAIL;
	}

	//	cal global mean grayValue	//
	Ipp64f g_pMean = 0;
	Ipp64f g_pStdDev = 0;
	ippiMean_StdDev_8u_C1R(src.ptr, src.linestep, { src.size.width,src.size.height }, &g_pMean, &g_pStdDev);

	//adjust bias with Halcon
	centerX -= HALCON_X_BIAS;
	centerY -= HALCON_Y_BIAS;


	int newLength = 2 * m_halfLength ;
	IMG_UBBUF ubbPocketROI;
	IMG_UBYTE *ucpPocketROI = new IMG_UBYTE[newLength * newLength];
	memset(ucpPocketROI, 0, newLength * newLength);
	int xStart = centerX - m_halfLength ;	
	if (xStart < 0)	xStart = 0;
	int xEnd = centerX + m_halfLength ;
	if (xEnd > width - 1)	xEnd = width - 1;
	int yStart = centerY - m_halfLength ;
	if (yStart < 0)	yStart = 0;
	int yEnd = centerY + m_halfLength ;
	if (yEnd > height - 1)	yEnd = height - 1;

	for (int i = yStart; i < yEnd; i++)
	{
		memcpy(ucpPocketROI + (i - yStart) * newLength, src.ptr + i * width + xStart, newLength);
	}
	m_setVisBuf.set_IMG_UBBUF(ubbPocketROI, ucpPocketROI, { (IMG_UWORD)newLength,(IMG_UWORD)newLength }, newLength * sizeof(IMG_UBYTE));
	int newCenterX = m_halfLength ;
	int newCenterY = m_halfLength ;

	//		erode	maoci //
	IMG_UBBUF ubbErode;
	IMG_UBYTE *ucpErode = new IMG_UBYTE[newLength * newLength];
	m_setVisBuf.set_IMG_UBBUF(ubbErode, ucpErode, { (IMG_UWORD)newLength,(IMG_UWORD)newLength }, newLength * sizeof(IMG_UBYTE));
	morfErodeBorder_notInPlace_7by7(ubbPocketROI, ucpErode);
	///////////////////////////


	//	grad mat	//
	IMG_WBUF wbGrad;
	IMG_WORD *wpGrad = new IMG_WORD[newLength * newLength];
	IMG_REAL *rpAngle = new IMG_REAL[newLength * newLength];
	m_setVisBuf.set_IMG_WBUF(wbGrad, wpGrad, { (IMG_UWORD)newLength ,(IMG_UWORD)newLength }, newLength * sizeof(IMG_WORD));
	SobelFilter_8u16s_C1_5x5(ucpErode, { newLength,newLength }, wpGrad, rpAngle);
	/////////////////////////////////
	
	//	set mask	//
	int pos = 0;
	int grad_count = 0;
	for (int i = 0; i < newLength; i++)
	{
		for (int j = 0; j < newLength; j++)
		{
			if ((i > newLength / 2 - m_trayHalfWidth ) && (i < newLength / 2 + m_trayHalfWidth ) && (j > newLength / 2 - m_trayHalfWidth ) && (j < newLength / 2 + m_trayHalfWidth ))
			{
				//inside inside
				if (wpGrad[pos + j] > m_grayInsideMax )
				{
					grad_count++;
					/*delete[] wpGrad;
					delete[] rpAngle;

					delete[] ucpPocketROI;
					delete[] ucpErode;

					if (judgeNormalDUT2(src, centerX, centerY))
					{
						return _CODE_DUT_NORMAL;
					}
					else
					{
						return _CODE_PROERROR;
					}*/
				}

				//inside
				if (wpGrad[pos + j] < m_grayInside )
				{
					ucpPocketROI[pos + j] = 0;
				}
			}
			else
			{
				//ouside
				if (wpGrad[pos + j] < m_grayOutside )
				{
					ucpPocketROI[pos + j] = 0;
				}
			}
				
		}
		pos += newLength;
	}

	//大异物，非毛刺
	if (grad_count >= m_grayCountMax )
	{
		if(wpGrad != nullptr)
			delete[] wpGrad;
		if(rpAngle != nullptr)
			delete[] rpAngle;

		if(ucpPocketROI != nullptr)
			delete[] ucpPocketROI;
		if(ucpErode != nullptr)
			delete[] ucpErode;
		if (g_pMean < m_grayMean)
		{
			if (judgeNormalDUT(src, centerX, centerY))
			{
				return _CODE_DUT_NORMAL;
			}
			else
			{
				return _CODE_PROERROR;
			}
		}
		else
		{
			if (judgeNormalDUT2(src, centerX, centerY))
			{
				return _CODE_DUT_NORMAL;
			}
			else
			{
				return _CODE_PROERROR;
			}
		}
	}

	//统计黑洞面积
	IMG_UBBUF ubbThreshold;
	IMG_UBYTE *ucpThreshold = new IMG_UBYTE[newLength * newLength];
	threshold_manual_inv(ucpPocketROI, { newLength,newLength }, ucpThreshold);
	m_setVisBuf.set_IMG_UBBUF(ubbThreshold, ucpThreshold, { (IMG_UWORD)newLength ,(IMG_UWORD)newLength }, newLength * sizeof(IMG_UBYTE));

	int regionNum = 0;
	int *intpLabel = new int[newLength * newLength];
	devStatus = labelMarker(ucpThreshold, { newLength,newLength }, intpLabel, regionNum);
	if (devStatus != 0 || regionNum <= 0)
	{
		if (wpGrad != nullptr)
			delete[] wpGrad;
		if (rpAngle != nullptr)
			delete[] rpAngle;

		if (ucpPocketROI != nullptr)
			delete[] ucpPocketROI;
		if (ucpErode != nullptr)
			delete[] ucpErode;

		if (ucpThreshold != nullptr)
			delete[] ucpThreshold;
		if (intpLabel != nullptr)
			delete[] intpLabel;

		return _CODE_PROERROR;
	}
	IMG_INTBUF iBufLabel;
	m_setVisBuf.set_IMG_INTBUF(iBufLabel, intpLabel, { (IMG_UWORD)newLength,(IMG_UWORD)newLength }, newLength * sizeof(int));

	//	blob analyse	//
	int *pRegionArea = new int[regionNum];
	IppiPoint *pRegionCen = new IppiPoint[regionNum];
	regionDef::RegionPeakPoint *pRegionPeakPoint = new regionDef::RegionPeakPoint[regionNum];
	regionDef::RegionWidthHeight *pRegionWH = new regionDef::RegionWidthHeight[regionNum];
	ProjectScore *projectScore = new ProjectScore[regionNum];

	devStatus = getRegionInfo(intpLabel, { newLength,newLength }, regionNum, pRegionArea, pRegionCen, pRegionPeakPoint, pRegionWH, projectScore);
	/////////////////////
		
	//		select maxArea blob		//
	int _maxArea = 0;
	int _index = 0;
	for (int k = 2; k < regionNum; k++)
	{
		if (pRegionArea[k] > _maxArea)
		{
			if (!eraseEdgeRegion(pRegionPeakPoint[k], newLength, newLength))
			{
				_maxArea = pRegionArea[k];
				_index = k;
			}

		}
	}


	//	judge	//
	if (_maxArea < m_holeAreaMin )
	{
		if (wpGrad != nullptr)
			delete[] wpGrad;
		if (rpAngle != nullptr)
			delete[] rpAngle;

		if (ucpPocketROI != nullptr)
			delete[] ucpPocketROI;
		if (ucpErode != nullptr)
			delete[] ucpErode;
			
		if(ucpThreshold != nullptr)
			delete[] ucpThreshold;
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

		if (g_pMean < m_grayMean)
		{
			if (judgeNormalDUT(src, centerX, centerY))
			{
				return _CODE_DUT_NORMAL;
			}
			else
			{
				return _CODE_PROERROR;
			}
		}
		else
		{
			if (judgeNormalDUT2(src, centerX, centerY))
			{
				return _CODE_DUT_NORMAL;
			}
			else
			{
				return _CODE_PROERROR;
			}
		}
	}
	else
	{
		if (wpGrad != nullptr)
			delete[] wpGrad;
		if (rpAngle != nullptr)
			delete[] rpAngle;

		if (ucpPocketROI != nullptr)
			delete[] ucpPocketROI;
		if (ucpErode != nullptr)
			delete[] ucpErode;

		if (ucpThreshold != nullptr)
			delete[] ucpThreshold;
		if (intpLabel != nullptr)
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

		newLength = 2 * m_outHalfLength ;
		ucpPocketROI = new IMG_UBYTE[newLength * newLength];
		memset(ucpPocketROI, 0, newLength * newLength);
		xStart = centerX - m_outHalfLength  ;
		if (xStart < 0)	xStart = 0;
		xEnd = centerX + m_outHalfLength ;
		if (xEnd > width - 1)	xEnd = width - 1;
		yStart = centerY - m_outHalfLength ;
		if (yStart < 0)	yStart = 0;
		yEnd = centerY + m_outHalfLength ;
		if (yEnd > height - 1)	yEnd = height - 1;

		for (int i = yStart; i < yEnd; i++)
		{
			//memcpy(ucpPocketROI + (i - yStart) * newLength, src.ptr + i * width + xStart, newLength);
			for (int j = xStart; j < xEnd; j++)
			{
				ucpPocketROI[(i - yStart) * newLength + (j - xStart)] = src.ptr[i * width + j];
			}
		}
		m_setVisBuf.set_IMG_UBBUF(ubbPocketROI, ucpPocketROI, { (IMG_UWORD)newLength,(IMG_UWORD)newLength }, newLength * sizeof(IMG_UBYTE));
		int newCenterX = m_outHalfLength ;
		int newCenterY = m_outHalfLength ;

		ucpThreshold = new IMG_UBYTE[newLength * newLength];
		threshold_OTSU(ucpPocketROI, { newLength,newLength }, ucpThreshold);
		m_setVisBuf.set_IMG_UBBUF(ubbThreshold, ucpThreshold, { (IMG_UWORD)newLength ,(IMG_UWORD)newLength }, newLength * sizeof(IMG_UBYTE));
			
//			//		Erode		//
//			IMG_UBBUF ubbErode;
//			unsigned char *ucpErode = new unsigned char[newLength * newLength];
//			devStatus = morfErodeBorder_notInPlace_3by3(ubbThreshold, ucpErode);
//#ifdef DEBUG
//			assert(devStatus == 0);
//#endif // DEBUG
//			m_setVisBuf.set_IMG_UBBUF(ubbErode, ucpErode, { (IMG_UWORD)newLength,(IMG_UWORD)newLength }, newLength);

		//	set mask	//
		for (int i = newCenterY - m_halfLength ; i <= newCenterY + m_halfLength ;i++)
		{
			for (int j = newCenterX - m_halfLength ; j <= newCenterX + m_halfLength ; j++)
			{
				ucpThreshold[i * newLength + j] = 0;
			}
		}

		regionNum = 0;
		intpLabel = new int[newLength * newLength];
		devStatus = labelMarker(ucpThreshold, { newLength,newLength }, intpLabel, regionNum);
		if (devStatus != 0 || regionNum <= 0)
		{
			if (wpGrad != nullptr)
				delete[] wpGrad;
			if (rpAngle != nullptr)
				delete[] rpAngle;

			if (ucpPocketROI != nullptr)
				delete[] ucpPocketROI;
			if (ucpErode != nullptr)
				delete[] ucpErode;

			if (ucpThreshold != nullptr)
				delete[] ucpThreshold;
			if (intpLabel != nullptr)
				delete[] intpLabel;

			return _CODE_PROERROR;
		}
		m_setVisBuf.set_IMG_INTBUF(iBufLabel, intpLabel, { (IMG_UWORD)newLength,(IMG_UWORD)newLength }, newLength * sizeof(int));


		pRegionArea = new int[regionNum];
		pRegionCen = new IppiPoint[regionNum];
		pRegionPeakPoint = new regionDef::RegionPeakPoint[regionNum];
		pRegionWH = new regionDef::RegionWidthHeight[regionNum];
		projectScore = new ProjectScore[regionNum];

		devStatus = getRegionInfo(intpLabel, { newLength,newLength }, regionNum, pRegionArea, pRegionCen, pRegionPeakPoint, pRegionWH, projectScore);

		int k;
		for (k = 1; k < regionNum; k++)
		{
			if (pRegionArea[k] > m_foreignBody && (pRegionPeakPoint[k].xMin == 0 || pRegionPeakPoint[k].xMax == newLength - 1 ) )
			{
				break;
			}
		}
		if (k != regionNum)		
		{
			if(ucpPocketROI != nullptr)
				delete[] ucpPocketROI;

			//delete[] ucpErode;
			if(ucpThreshold != nullptr)
				delete[] ucpThreshold;
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

			if (g_pMean < m_grayMean)
			{
				if (judgeNormalDUT(src, centerX, centerY))
				{
					return _CODE_DUT_NORMAL;
				}
				else
				{
					return _CODE_PROERROR;
				}
			}
			else
			{
				if (judgeNormalDUT2(src, centerX, centerY))
				{
					return _CODE_DUT_NORMAL;
				}
				else
				{
					return _CODE_PROERROR;
				}
			}

		}
		else
		{
			if (ucpPocketROI != nullptr)
				delete[] ucpPocketROI;

			//delete[] ucpErode;
			if (ucpThreshold != nullptr)
				delete[] ucpThreshold;
			if (intpLabel != nullptr)
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
			return _CODE_EMPTY;
		}
			

	}


	
}


