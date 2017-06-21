#include "stdafx.h"
#include "BmSocketLocatingCoarse.h"

CBmSocketLocatingCoarse::CBmSocketLocatingCoarse()
{
	InputPara.m_threshold[0] = 230;
	InputPara.m_threshold[1] = 200;
	InputPara.m_threshold[2] = 170;
	InputPara.m_CircleRoi1.coWindowOff = { 123,0 };
	InputPara.m_CircleRoi1.szWindowSize = { 200,123 };
	InputPara.m_CircleRoi2.coWindowOff = { 285,139 };
	InputPara.m_CircleRoi2.szWindowSize = { 120,167 };
	InputPara.m_ScreenAreaMin = 500;
	InputPara.m_ScreenAreaMax = 2000;
	InputPara.m_OpenCloseflag[0] = 0;
	InputPara.m_OpenCloseflag[1] = 0;
	InputPara.m_featureCircleRadius = 44;
	InputPara.m_edgeError = { 20,10 };
	InputPara.m_featureCircleDis = { 144,172 };
	InputPara.m_errorRoate = 0.15;
	InputPara.m_errorDis = 8.447;
	m_getRoi = nullptr;
	m_getRoiZero = nullptr;
};
CBmSocketLocatingCoarse::~CBmSocketLocatingCoarse()
{
	vector<int>().swap(m_signNum);
	vector<RoiFeature>().swap(m_signEdge);
	vector<IMG_COORD>().swap(m_signReal);
	vector<float>().swap(m_diff);
	if (m_getRoi != nullptr)
	{
		free(m_getRoi);
		m_getRoi = nullptr;
	}
	if (m_getRoiZero != nullptr)
	{
		free(m_getRoiZero);
		m_getRoiZero = nullptr;
	}
};

/**********************************************/
// VisSS_Pyramid, 功能说明:金字塔，将图像进行缩小。
// Input:
// 	IMG_UBBUF src, 输入图片
//	int level,用户指定的金字塔层数
//	double sigma,参数
//     ...
// Output:
// 	IppiPyramid*&pPyrStruct, 金字塔结构
//	Ipp8u ** &pPyrImage 金字塔每一层中对应的图像的点的灰度值
//     ...
// Return:
//     0 - 正常
//     -1 - 暂无待定
//     -2 - 暂无待定
// Author: Huang Yige/03/18/2017
/**********************************************/
_BACK_CODE CBmSocketLocatingCoarse::VisSS_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma)
{
	__try
	{
		int srcStep = src.size.width * sizeof(Ipp8u);
		Ipp32f      rate = 2.f;                  // Neighbour levels ratio
		Ipp16s kernel[3] = { 0,3,0 };		// Separable symmetric kernel of odd length

		IppiSize roiSize = { src.size.width ,src.size.height };

		int pyrBufferSize = 0;
		int pyrStructSize = 0;
		IppiPyramid *pPyrStruct = NULL;
		Ipp8u       *pPyrBuffer = NULL;
		Ipp8u       *pPyrStrBuffer = NULL;

		int      pyrLStateSize = 0;
		int      pyrLBufferSize = 0;
		Ipp8u   *pPyrLStateBuf = NULL;
		Ipp8u   *pPyrLBuffer = NULL;
		Ipp8u **pPyrImage = NULL;

		// Computes the temporary work buffer size
		ippiPyramidGetSize(&pyrStructSize, &pyrBufferSize, level, roiSize, rate);


		pPyrBuffer = (Ipp8u*)malloc(pyrBufferSize * sizeof(Ipp8u));
		pPyrStrBuffer = (Ipp8u*)malloc(pyrStructSize * sizeof(Ipp8u));

		// Initializes Gaussian structure for pyramids
		ippiPyramidInit(&pPyrStruct, level, roiSize, rate, pPyrStrBuffer, pPyrBuffer);

		// Correct maximum scale level 
		level = pPyrStruct->level;

		// Allocate structures to calculate pyramid layers
		ippiPyramidLayerDownGetSize_8u_C1R(roiSize, rate, 3, &pyrLStateSize, &pyrLBufferSize);

		pPyrLStateBuf = (Ipp8u*)malloc(pyrLStateSize * sizeof(Ipp8u));
		pPyrLBuffer = (Ipp8u*)malloc(pyrLBufferSize * sizeof(Ipp8u));

		// Initialize the structure for creating a lower pyramid layer
		ippiPyramidLayerDownInit_8u_C1R((IppiPyramidDownState_8u_C1R**)&pPyrStruct->pState, roiSize, rate, kernel, 3, IPPI_INTER_LINEAR, pPyrLStateBuf, pPyrLBuffer);

		// Allocate pyramid layers
		pPyrImage = pPyrStruct->pImage;
		pPyrImage[0] = src.ptr;
		pPyrStruct->pStep[0] = srcStep;
		for (int i = 1; i <= level; i++)
		{
			pPyrImage[i] = (Ipp8u*)malloc((pPyrStruct->pRoi[i].width) * (pPyrStruct->pRoi[i].height) * sizeof(Ipp8u));
			pPyrStruct->pStep[i] = (pPyrStruct->pRoi[i].width) * sizeof(Ipp8u);
		}

		// Perform downsampling of the image with 5x5 Gaussian kernel
		for (int i = 1; i <= level; i++)
		{
			ippiPyramidLayerDown_8u_C1R(pPyrImage[i - 1], pPyrStruct->pStep[i - 1], pPyrStruct->pRoi[i - 1], pPyrImage[i], pPyrStruct->pStep[i], pPyrStruct->pRoi[i], (IppiPyramidDownState_8u_C1R*)pPyrStruct->pState);
		}

		Roi.width = pPyrStruct->pRoi[2].width;
		Roi.height = pPyrStruct->pRoi[2].height;

		//srcRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));
		memcpy(srcRoi, pPyrImage[2], Roi.width*Roi.height * sizeof(Ipp8u));

#ifdef DEBUG_BmSocketStatus
		FILE *hp1;
		hp1 = fopen("SS_pyramid.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp1, "%d ", srcRoi[i*Roi.width + j]);
			}
			fprintf(hp1, "\n");
	}
		fclose(hp1);
#endif // !1

		for (int i = 1; i <= level; i++)
			free(pPyrImage[i]);
		free(pPyrStrBuffer);
		free(pPyrBuffer);
		free(pPyrLBuffer);
		free(pPyrLStateBuf);
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// VisSS_Binaryzation, 功能说明:对图像进行二值化。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
// 	IMG_UBBUF src,输出图像
//     ...
// Return:
//     0 - 正常
//     -1 - 暂无待定
//     -2 - 暂无待定
// Author: Huang Yige/03/18/2017
/**********************************************/
_BACK_CODE CBmSocketLocatingCoarse::VisSS_Binaryzation(Ipp8u *srcRoi, IppiSize Roi)
{
	__try
	{
		Ipp8u pMin = 0;

		int threshold = 0;
		int srcDstStep = Roi.width * sizeof(Ipp8u);

		ippiMin_8u_C1R(srcRoi, srcDstStep, Roi, &pMin);


		if (pMin > 10)
		{
			threshold = InputPara.m_threshold[0];

		}
		if (pMin > 6 && pMin < 11)
		{
			threshold = InputPara.m_threshold[1];
		}
		if (pMin < 7)
		{
			threshold = InputPara.m_threshold[2];
		}

		ippiThreshold_LTValGTVal_8u_C1IR(srcRoi, srcDstStep, Roi, threshold, 0, threshold, 255);



#ifdef BmSocketLocatingCoarse
		FILE *hp2;
		hp2 = fopen("SS_Binaryzation.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp2, "%d ", srcRoi[i*Roi.width + j]);
			}
			fprintf(hp2, "\n");
		}
		fclose(hp2);
#endif
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// VisSS_LabelMarker, 功能说明:对图像中的Blob块进行提取。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
// 	int &markersNum, 输出Blob块的个数
//     ...
// Return:
//     0 - 正常
//     -1 - 暂无待定
//     -2 - 暂无待定
// Author: Huang Yige/03/18/2017
/**********************************************/
_BACK_CODE CBmSocketLocatingCoarse::VisSS_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, int &markersNum)
{
	__try
	{
		//IppiSize Roi = { src.size.width,src.size.height };
		int markerStep = Roi.width * sizeof(Ipp8u);
		int minLabel = 1;
		int maxLabel = 200;
		//int markersNum = 0;
		Ipp8u *pBuffer = NULL;
		int bufferSize = 0;

		ippiLabelMarkersGetBufferSize_8u_C1R(Roi, &bufferSize);

		pBuffer = ippsMalloc_8u(bufferSize);

		ippiLabelMarkers_8u_C1IR(srcRoi, markerStep, Roi, minLabel, maxLabel, ippiNormInf, &markersNum, pBuffer);

		//printf("markersNum=%d\n", markersNum);


#ifdef DEBUG_BmSocketStatus
		FILE *hp3;
		hp3 = fopen("SS_LabelMarker.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp3, "%d ", srcRoi[i*Roi.width + j]);
			}
			fprintf(hp3, "\n");
	}
		fclose(hp3);
#endif

		ippsFree(pBuffer);
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// VisSS_OpenClose, 功能说明:对图像进行开运算，消除小的Blob。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
// 	Ipp8u *srcRoi, 输出图片
//     ...
// Return:
//     0 - 正常
//     -1 - 暂无待定
//     -2 - 暂无待定
// Author: Huang Yige/03/18/2017
/**********************************************/
_BACK_CODE CBmSocketLocatingCoarse::VisSS_OpenClose(Ipp8u *srcRoi, IppiSize Roi,int &flag)
{
	__try
	{
		//0为开闭运算都有，且模板都为3*3
		if (flag == 0)
		{
			IppiMorphAdvState *pSpec = NULL;
			Ipp8u* pBuffer = NULL;
			Ipp8u pMask[9] = { 1,1,1,1,0,1,1,1,1 };
			IppiSize maskSize = { 3,3 };

			Ipp8u *dstRoi;
			dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

			int srcStep = Roi.width * sizeof(Ipp8u);
			int dstStep = Roi.width * sizeof(Ipp8u);
			int dstSize = Roi.width;

			int specSize = 0, bufferSize = 0;
			IppiBorderType borderType = ippBorderRepl;
			Ipp8u borderValue = 0;

			ippiMorphAdvGetSize_8u_C1R(Roi, maskSize, &specSize, &bufferSize);

			pSpec = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
			pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

			ippiMorphAdvInit_8u_C1R(Roi, pMask, maskSize, pSpec, pBuffer);

			ippiMorphOpenBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec, pBuffer);

			ippiMorphCloseBorder_8u_C1R(dstRoi, srcStep, srcRoi, dstStep, Roi, borderType, borderValue, pSpec, pBuffer);

			ippsFree(pBuffer);
			ippsFree(pSpec);
			free(dstRoi);
		}

		//1为开闭运算都有，开运算模板5*5，闭运算模板3*3
		if (flag == 1)
		{
			IppiMorphAdvState *pSpec1 = NULL;
			Ipp8u* pBuffer1 = NULL;
			Ipp8u pMask1[25] = { 1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1 };
			IppiSize maskSize1 = { 5,5 };

			IppiMorphAdvState *pSpec2 = NULL;
			Ipp8u* pBuffer2 = NULL;
			Ipp8u pMask2[9] = { 1,1,1,1,0,1,1,1,1 };
			IppiSize maskSize2 = { 3,3 };

			Ipp8u *dstRoi;
			dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

			int srcStep = Roi.width * sizeof(Ipp8u);
			int dstStep = Roi.width * sizeof(Ipp8u);
			int dstSize = Roi.width;

			int specSize = 0, bufferSize = 0;
			IppiBorderType borderType = ippBorderRepl;
			Ipp8u borderValue = 0;

			ippiMorphAdvGetSize_8u_C1R(Roi, maskSize1, &specSize, &bufferSize);

			pSpec1 = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
			pBuffer1 = (Ipp8u*)ippsMalloc_8u(bufferSize);

			ippiMorphAdvInit_8u_C1R(Roi, pMask1, maskSize1, pSpec1, pBuffer1);

			ippiMorphOpenBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec1, pBuffer1);

			specSize = 0;
			bufferSize = 0;

			ippiMorphAdvGetSize_8u_C1R(Roi, maskSize2, &specSize, &bufferSize);

			pSpec2 = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
			pBuffer2 = (Ipp8u*)ippsMalloc_8u(bufferSize);

			ippiMorphAdvInit_8u_C1R(Roi, pMask2, maskSize2, pSpec2, pBuffer2);

			ippiMorphCloseBorder_8u_C1R(dstRoi, srcStep, srcRoi, dstStep, Roi, borderType, borderValue, pSpec2, pBuffer2);

			ippsFree(pBuffer1);
			ippsFree(pSpec1);
			ippsFree(pBuffer2);
			ippsFree(pSpec2);
			free(dstRoi);
		}

		//2为开闭运算都有，闭运算模板5*5，开运算模板3*3
		if (flag == 2)
		{
			IppiMorphAdvState *pSpec1 = NULL;
			Ipp8u* pBuffer1 = NULL;
			Ipp8u pMask1[25] = { 1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1 };
			IppiSize maskSize1 = { 5,5 };

			IppiMorphAdvState *pSpec2 = NULL;
			Ipp8u* pBuffer2 = NULL;
			Ipp8u pMask2[9] = { 1,1,1,1,0,1,1,1,1 };
			IppiSize maskSize2 = { 3,3 };

			Ipp8u *dstRoi;
			dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

			int srcStep = Roi.width * sizeof(Ipp8u);
			int dstStep = Roi.width * sizeof(Ipp8u);
			int dstSize = Roi.width;

			int specSize = 0, bufferSize = 0;
			IppiBorderType borderType = ippBorderRepl;
			Ipp8u borderValue = 0;

			ippiMorphAdvGetSize_8u_C1R(Roi, maskSize2, &specSize, &bufferSize);

			pSpec2 = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
			pBuffer2 = (Ipp8u*)ippsMalloc_8u(bufferSize);

			ippiMorphAdvInit_8u_C1R(Roi, pMask2, maskSize2, pSpec2, pBuffer2);

			ippiMorphOpenBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec2, pBuffer2);


			specSize = 0;
			bufferSize = 0;

			ippiMorphAdvGetSize_8u_C1R(Roi, maskSize1, &specSize, &bufferSize);

			pSpec1 = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
			pBuffer1 = (Ipp8u*)ippsMalloc_8u(bufferSize);

			ippiMorphAdvInit_8u_C1R(Roi, pMask1, maskSize1, pSpec1, pBuffer1);

			ippiMorphCloseBorder_8u_C1R(dstRoi, srcStep, srcRoi, dstStep, Roi, borderType, borderValue, pSpec1, pBuffer1);

			ippsFree(pBuffer1);
			ippsFree(pSpec1);
			ippsFree(pBuffer2);
			ippsFree(pSpec2);
			free(dstRoi);
		}

		//3为开运算，模板为3*3
		if (flag == 3)
		{
			IppiMorphAdvState *pSpec = NULL;
			Ipp8u* pBuffer = NULL;
			Ipp8u pMask[9] = { 1,1,1,1,0,1,1,1,1 };
			IppiSize maskSize = { 3,3 };

			Ipp8u *dstRoi;
			dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

			int srcStep = Roi.width * sizeof(Ipp8u);
			int dstStep = Roi.width * sizeof(Ipp8u);
			int dstSize = Roi.width;

			int specSize = 0, bufferSize = 0;
			IppiBorderType borderType = ippBorderRepl;
			Ipp8u borderValue = 0;

			ippiMorphAdvGetSize_8u_C1R(Roi, maskSize, &specSize, &bufferSize);

			pSpec = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
			pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

			ippiMorphAdvInit_8u_C1R(Roi, pMask, maskSize, pSpec, pBuffer);

			ippiMorphOpenBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec, pBuffer);

			memcpy(srcRoi, dstRoi, Roi.width*Roi.height * sizeof(Ipp8u));

			ippsFree(pBuffer);
			ippsFree(pSpec);
			free(dstRoi);
		}

		//4为闭运算，模板为3*3
		if (flag == 4)
		{
			IppiMorphAdvState *pSpec = NULL;
			Ipp8u* pBuffer = NULL;
			Ipp8u pMask[9] = { 1,1,1,1,0,1,1,1,1 };
			IppiSize maskSize = { 3,3 };

			Ipp8u *dstRoi;
			dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

			int srcStep = Roi.width * sizeof(Ipp8u);
			int dstStep = Roi.width * sizeof(Ipp8u);
			int dstSize = Roi.width;

			int specSize = 0, bufferSize = 0;
			IppiBorderType borderType = ippBorderRepl;
			Ipp8u borderValue = 0;

			ippiMorphAdvGetSize_8u_C1R(Roi, maskSize, &specSize, &bufferSize);

			pSpec = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
			pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

			ippiMorphAdvInit_8u_C1R(Roi, pMask, maskSize, pSpec, pBuffer);

			ippiMorphCloseBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec, pBuffer);

			memcpy(srcRoi, dstRoi, Roi.width*Roi.height * sizeof(Ipp8u));

			ippsFree(pBuffer);
			ippsFree(pSpec);
			free(dstRoi);
		}

		//5为开运算，模板为5*5
		if (flag == 5)
		{
			IppiMorphAdvState *pSpec = NULL;
			Ipp8u* pBuffer = NULL;
			Ipp8u pMask[25] = { 1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1 };
			IppiSize maskSize = { 5,5 };

			Ipp8u *dstRoi;
			dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

			int srcStep = Roi.width * sizeof(Ipp8u);
			int dstStep = Roi.width * sizeof(Ipp8u);
			int dstSize = Roi.width;

			int specSize = 0, bufferSize = 0;
			IppiBorderType borderType = ippBorderRepl;
			Ipp8u borderValue = 0;

			ippiMorphAdvGetSize_8u_C1R(Roi, maskSize, &specSize, &bufferSize);

			pSpec = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
			pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

			ippiMorphAdvInit_8u_C1R(Roi, pMask, maskSize, pSpec, pBuffer);

			ippiMorphOpenBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec, pBuffer);

			memcpy(srcRoi, dstRoi, Roi.width*Roi.height * sizeof(Ipp8u));

			ippsFree(pBuffer);
			ippsFree(pSpec);
			free(dstRoi);
		}

		//4为闭运算，模板为5*5
		if (flag == 6)
		{
			IppiMorphAdvState *pSpec = NULL;
			Ipp8u* pBuffer = NULL;
			Ipp8u pMask[25] = { 1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1 };
			IppiSize maskSize = { 5,5 };

			Ipp8u *dstRoi;
			dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

			int srcStep = Roi.width * sizeof(Ipp8u);
			int dstStep = Roi.width * sizeof(Ipp8u);
			int dstSize = Roi.width;

			int specSize = 0, bufferSize = 0;
			IppiBorderType borderType = ippBorderRepl;
			Ipp8u borderValue = 0;

			ippiMorphAdvGetSize_8u_C1R(Roi, maskSize, &specSize, &bufferSize);

			pSpec = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
			pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

			ippiMorphAdvInit_8u_C1R(Roi, pMask, maskSize, pSpec, pBuffer);

			ippiMorphCloseBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec, pBuffer);

			memcpy(srcRoi, dstRoi, Roi.width*Roi.height * sizeof(Ipp8u));

			ippsFree(pBuffer);
			ippsFree(pSpec);
			free(dstRoi);
		}


#ifdef BmSocketLocatingCoarse
		FILE *hp4;
		hp4 = fopen("SS_Open.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp4, "%d ", srcRoi[i*Roi.width + j]);
			}
			fprintf(hp4, "\n");
		}
		fclose(hp4);

#endif // !1


		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// VisSS_Screen, 功能说明:寻找图像中的特征圆。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
// 	IMG_WINDOW JudgeRoi, 选取Roi的特征
//     ...
// Return:
//     0 - 正常
//     _CODE_THROW - 特征区域不足两个
// Author: Huang Yige/03/18/2017
/**********************************************/
_BACK_CODE CBmSocketLocatingCoarse::VisSS_Screen(Ipp8u *srcRoi, IppiSize Roi, standardPoint &twoPoint)
{
	/*vector<int>().swap(m_signNum);
	vector<RoiFeature>().swap(m_signEdge);
	vector<IMG_COORD>().swap(m_signReal);
	vector<float>().swap(m_diff);*/
	__try
	{
		m_signNum.clear();
		m_signEdge.clear();
		m_signReal.clear();
		m_diff.clear();
		Ipp8u* dstRoi;
		dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				if (i > InputPara.m_CircleRoi1.coWindowOff.y - 1 && i < InputPara.m_CircleRoi1.coWindowOff.y + InputPara.m_CircleRoi1.szWindowSize.height&&
					j > InputPara.m_CircleRoi1.coWindowOff.x - 1 && j < InputPara.m_CircleRoi1.coWindowOff.x + InputPara.m_CircleRoi1.szWindowSize.width)
				{
					dstRoi[i*Roi.width + j] = srcRoi[i*Roi.width + j];
				}
				else if (i > InputPara.m_CircleRoi2.coWindowOff.y - 1 && i < InputPara.m_CircleRoi2.coWindowOff.y + InputPara.m_CircleRoi2.szWindowSize.height&&
					j > InputPara.m_CircleRoi2.coWindowOff.x - 1 && j < InputPara.m_CircleRoi2.coWindowOff.x + InputPara.m_CircleRoi2.szWindowSize.width)
				{
					dstRoi[i*Roi.width + j] = srcRoi[i*Roi.width + j];
				}
				else
				{
					dstRoi[i*Roi.width + j] = 0;
				}
			}
		}

		VisSS_OpenClose(dstRoi, Roi, InputPara.m_OpenCloseflag[0]);

		int markersNum = 0;

		VisSS_LabelMarker(dstRoi, Roi, markersNum);

		int *Area;
		Area = (int*)malloc(markersNum * sizeof(int));

		VisSS_GetArea(dstRoi, Roi, markersNum, Area);

		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{

				if (Area[dstRoi[i*Roi.width + j] - 1] < InputPara.m_ScreenAreaMin)
				{
					dstRoi[i*Roi.width + j] = 0;
				}
			}
		}

		VisSS_LabelMarker(dstRoi, Roi, markersNum);

		if (markersNum < 2)
		{
			free(dstRoi);
			free(Area);
			return _CODE_THROW;
		}

		RoiFeature *BlobEdge;
		BlobEdge = (RoiFeature*)malloc(markersNum * sizeof(RoiFeature));

		VisSS_GetEdge(dstRoi, Roi, markersNum, BlobEdge);

		IppiSize feature;
		float  roateTest = 0.0;
		int diffX = 0, diffY = 0;


		for (int k = 0; k < markersNum; k++)
		{
			feature.width = BlobEdge[k].max_x - BlobEdge[k].min_x;
			feature.height = BlobEdge[k].max_y - BlobEdge[k].min_y;

			diffX = feature.width - InputPara.m_featureCircleRadius;
			diffY = feature.height - InputPara.m_featureCircleRadius;

			if ((abs(diffX) < InputPara.m_edgeError.x || abs(diffY) < InputPara.m_edgeError.y) /*&& roateTest > 0.67*/)
			{
				m_signNum.push_back(k + 1);
				m_signEdge.push_back(BlobEdge[k]);
			}
		}

		if (m_signEdge.size() < 2)
		{
			free(dstRoi);
			free(Area);
			free(BlobEdge);
			return _CODE_THROW;
		}
		if (m_signEdge.size() == 2)
		{
			float dis = sqrt((float)(InputPara.m_featureCircleDis.x*InputPara.m_featureCircleDis.x + InputPara.m_featureCircleDis.y*InputPara.m_featureCircleDis.y));
			float  disReal = 0.0;
			int scaleX = 0, scaleY = 0, disX = 0, disY = 0;
			float roate = (float)InputPara.m_featureCircleDis.y / (float)InputPara.m_featureCircleDis.x;
			float realRoate = 0.0;
			int Num = 0;
			int conditionNum = 0;

			if (m_signEdge[0].max_x > m_signEdge[1].max_x)
			{
				scaleY = m_signEdge[0].max_y - m_signEdge[1].max_y;
				scaleX = m_signEdge[0].min_x - m_signEdge[1].min_x;
			}
			else
			{
				scaleY = m_signEdge[1].max_y - m_signEdge[0].max_y;
				scaleX = m_signEdge[1].min_x - m_signEdge[0].min_x;
			}

			realRoate = (float)scaleY / (float)scaleX;

			if (realRoate < 0 || fabs(realRoate - roate) > InputPara.m_errorRoate)
			{
				free(dstRoi);
				free(Area);
				free(BlobEdge);
				return _CODE_THROW;
			}

			disY = abs(m_signEdge[0].max_y - m_signEdge[1].max_y);
			disX = abs(m_signEdge[0].min_x - m_signEdge[1].min_x);
			disReal = abs(sqrt((float)(disY*disY + disX*disX)) - dis);

			if (disReal > InputPara.m_errorDis)
			{
				free(dstRoi);
				free(Area);
				free(BlobEdge);
				return _CODE_THROW;
			}

			twoPoint.leftPoint = m_signEdge[0];
			twoPoint.rightPoint = m_signEdge[1];
			for (int i = 0; i < Roi.height; i++)
			{
				for (int j = 0; j < Roi.width; j++)
				{
					if (dstRoi[i*Roi.width + j] != m_signNum[0] && dstRoi[i*Roi.width + j] != m_signNum[1])
					{
						dstRoi[i*Roi.width + j] = 0;
					}
				}
			}
		}
		else
		{
			int scaleX = 0, scaleY = 0;
			float roate = (float)InputPara.m_featureCircleDis.y / (float)InputPara.m_featureCircleDis.x;
			float realRoate = 0.0;
			IMG_COORD signReal1 = { 0,0 };

			int Num = 0;

			int conditionNum = 0;

			for (unsigned int i = 0; i < m_signEdge.size(); i++)
			{
				for (unsigned int j = i + 1; j < m_signEdge.size(); j++)
				{
					if (m_signEdge[j].max_x > m_signEdge[i].max_x)
					{
						scaleY = m_signEdge[j].max_y - m_signEdge[i].max_y;
						scaleX = m_signEdge[j].min_x - m_signEdge[i].min_x;
					}
					else
					{
						scaleY = m_signEdge[i].max_y - m_signEdge[j].max_y;
						scaleX = m_signEdge[i].min_x - m_signEdge[j].min_x;
					}

					realRoate = (float)scaleY / (float)scaleX;

					if (realRoate > 0 && fabs(realRoate - roate) < InputPara.m_errorRoate)
					{
						m_diff.push_back(realRoate);
						signReal1 = { (IMG_WORD)i ,(IMG_WORD)j };
						m_signReal.push_back(signReal1);
					}

#ifdef DEBUG_BmSocketStatus
					printf("roate=%f\n", realRoate);
#endif // DEBUG_BmSocketStatus
				}
			}
			if (m_diff.size() < 1)
			{
				free(dstRoi);
				free(Area);
				free(BlobEdge);
				return _CODE_THROW;
			}

			float dis = sqrt((float)(InputPara.m_featureCircleDis.x*InputPara.m_featureCircleDis.x + InputPara.m_featureCircleDis.y*InputPara.m_featureCircleDis.y));
			float disReal = 0.0;
			int disX = 0, disY = 0;

			disY = abs(m_signEdge[m_signReal[0].x].max_y - m_signEdge[m_signReal[0].y].max_y);
			disX = abs(m_signEdge[m_signReal[0].x].min_x - m_signEdge[m_signReal[0].y].min_x);
			disReal = abs(sqrt((float)(disY*disY + disX*disX)) - dis);

			if (disReal > InputPara.m_errorDis)
			{
				free(dstRoi);
				free(Area);
				free(BlobEdge);
				return _CODE_THROW;
			}

			twoPoint.leftPoint = m_signEdge[m_signReal[0].x];
			twoPoint.rightPoint = m_signEdge[m_signReal[0].y];

			for (int i = 0; i < Roi.height; i++)
			{
				for (int j = 0; j < Roi.width; j++)
				{
					if (dstRoi[i*Roi.width + j] != m_signNum[m_signReal[0].x] && dstRoi[i*Roi.width + j] != m_signNum[m_signReal[0].y])
					{
						dstRoi[i*Roi.width + j] = 0;
					}
					else
					{
						dstRoi[i*Roi.width + j] = 255;
					}
				}
			}
		}

#ifdef DEBUG_BmSocketStatus
		FILE *hp7;
		hp7 = fopen("SS_EdgeScreen.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp7, "%d ", dstRoi[i*Roi.width + j]);
			}
			fprintf(hp7, "\n");
		}
		fclose(hp7);
#endif // DEBUG_BmSocketStatus

		free(dstRoi);
		free(Area);
		free(BlobEdge);

		return _CODE_OK;

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// VisSS_GetArea, 功能说明:寻找图像中的Blob块的面积。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
// 	int &markersNum, Blob块的个数
//  int *&Area，Blob块的面积
//     ...
// Return:
//     0 - 正常
// Author: Huang Yige/05/02/2017
/**********************************************/
_BACK_CODE CBmSocketLocatingCoarse::VisSS_GetArea(Ipp8u *srcRoi, IppiSize Roi, int &markersNum, int *&Area)
{
	__try
	{
		for (int i = 0; i < markersNum; i++)
		{
			Area[i] = 0;
		}

		int grade = 0;

		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				grade = srcRoi[i*Roi.width + j];
				if (grade > 0)
				{
					Area[grade - 1]++;
				}
			}
		}
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// VisSS_GetEdge, 功能说明:寻找图像中的Blob的边缘和中心点。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
// 	int &markersNum, Blob块的个数
//  IMG_RCOORD *&CenterPoint,Blob块的中心点
//  RoiEdge *&BlobEdge,Blob块的边缘
//     ...
// Return:
//     0 - 正常
// Author: Huang Yige/05/02/2017
/**********************************************/
_BACK_CODE CBmSocketLocatingCoarse::VisSS_GetEdge(Ipp8u *srcRoi, IppiSize Roi, int &markersNum, RoiFeature *&BlobEdge)
{
	__try
	{
		int *sum_x, *sum_y, *num, *min_x, *max_x, *min_y, *max_y;
		sum_x = (int*)malloc(markersNum * sizeof(int));
		sum_y = (int*)malloc(markersNum * sizeof(int));
		num = (int*)malloc(markersNum * sizeof(int));
		min_x = (int*)malloc(markersNum * sizeof(int));
		max_x = (int*)malloc(markersNum * sizeof(int));
		min_y = (int*)malloc(markersNum * sizeof(int));
		max_y = (int*)malloc(markersNum * sizeof(int));
		for (int i = 0; i < markersNum; i++)
		{
			sum_x[i] = 0;
			sum_y[i] = 0;
			num[i] = 0;
			min_x[i] = 0;
			max_x[i] = 0;
			min_y[i] = 0;
			max_y[i] = 0;
		}

		//IMG_RCOORD Point;
		RoiFeature edge;

		for (int k = 0; k < markersNum; k++)
		{
			//printf("signBlob=%d\n", signBlob[k]);
			for (int i = 0; i < Roi.height; i++)
			{
				for (int j = 0; j < Roi.width; j++)
				{
					if (srcRoi[i*Roi.width + j] == k + 1)
					{
						sum_x[k] += j;
						sum_y[k] += i;
						num[k]++;
						if (num[k] == 1)
						{
							min_x[k] = j;
							min_y[k] = i;
							max_x[k] = j;
							max_y[k] = i;
						}

						if (num[k] > 1)
						{
							if (j < min_x[k])
							{
								min_x[k] = j;
							}
							if (j > max_x[k])
							{
								max_x[k] = j;
							}
							if (i < min_y[k])
							{
								min_y[k] = i;
							}
							if (i > max_y[k])
							{
								max_y[k] = i;
							}
						}
					}
				}
			}
			edge.CenterX = (float)sum_x[k] / (float)num[k];
			edge.CenterY = (float)sum_y[k] / (float)num[k];
			edge.max_x = max_x[k];
			edge.max_y = max_y[k];
			edge.min_x = min_x[k];
			edge.min_y = min_y[k];
			//CenterPoint[k] = Point;
			BlobEdge[k] = edge;
		}

		free(sum_x);
		free(sum_y);
		free(num);
		free(min_x);
		free(min_y);
		free(max_x);
		free(max_y);
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


/**********************************************/
// SocketLocatingCoarse, 功能说明:对图像中的两个圆进行粗定位。
// Input:
// 	IMG_UBBUF srcRoi, 输入图片
//	const int &angleFlg, 输入角度标识（0或180度）
//     ...
// Output:
//     ...
// Return:
//     0 - 正常
//     _CODE_FILE_LOST - 图片出错
//     _CODE_ANGLE_FAIL - 输入角度出错
//	   _CODE_THROW - 定位失败
// Author: Huang Yige/03/18/2017
/**********************************************/
_BACK_CODE  CBmSocketLocatingCoarse::SocketLocatingCoarse(const IMG_UBBUF srcRoi, const int &angleFlg)
{
	__try
	{
		_BACK_CODE test;

		if (srcRoi.ptr == nullptr || srcRoi.size.width == 0 || srcRoi.size.height == 0 || srcRoi.linestep != srcRoi.size.width)
		{
			test = _CODE_FILE_LOST;
			return test;
		}

		if (angleFlg != 0 && angleFlg != 180)
		{
			test = _CODE_ANGLE_FAIL;
			return test;
		}

		IppiSize Roi;
		Roi.width = srcRoi.size.width;
		Roi.height = srcRoi.size.height;

		int level = 2;
		double sigma = 6.0;

		Ipp8u* src;
		src = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

		VisSS_Pyramid(srcRoi, src, Roi, level, sigma);

		Ipp8u* dst;
		dst = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

		Ipp8u* src1;
		src1 = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

		memcpy(dst, src, Roi.width*Roi.height * sizeof(Ipp8u));

		if (angleFlg == 180)
		{
			//		IMG_UBBUF prbSrc;
			//		prbSrc.size = { (IMG_UWORD)Roi.width, (IMG_UWORD)Roi.height };
			//		prbSrc.linestep = prbSrc.size.width;
			//		prbSrc.ptr = (Ipp8u*)malloc(prbSrc.size.width*prbSrc.size.height * sizeof(Ipp8u));
			//		memcpy(prbSrc.ptr, src, prbSrc.size.width*prbSrc.size.height * sizeof(Ipp8u));
			//
			//		IMG_UBBUF prbDst;
			//		prbDst.size = { (IMG_UWORD)Roi.width, (IMG_UWORD)Roi.height };
			//		prbDst.linestep = prbDst.size.width;
			//		prbDst.ptr = (Ipp8u*)malloc(prbDst.size.width*prbDst.size.height * sizeof(Ipp8u));
			//
			//		IMG_RCOORD prcoSrcCenter = { (float)(Roi.width - 1) / 2.0,(float)(Roi.height - 1) / 2.0 };
			//		IMG_RCOORD prcoDstCenter = { (float)(Roi.width - 1) / 2.0,(float)(Roi.height - 1) / 2.0 };
			//		IMG_SIZE pszOp = { Roi.width,Roi.height };
			//		RTS_R_8uC1(prbSrc, prcoSrcCenter, prbDst, prcoDstCenter, PI, pszOp, 1);
			//		memcpy(src, prbDst.ptr, prbDst.size.width*prbDst.size.height * sizeof(Ipp8u));
			//
			//#ifdef DEBUG_BmSocketStatus
			//		FILE *hp40;
			//		hp40 = fopen("SS_RTS_R_8uC1.txt", "w");
			//		for (int i = 0; i < Roi.height; i++)
			//		{
			//			for (int j = 0; j < Roi.width; j++)
			//			{
			//				fprintf(hp40, "%d ", src[i*Roi.width + j]);
			//			}
			//			fprintf(hp40, "\n");
			//		}
			//		fclose(hp40);
			//#endif // !1
			//
			//		free(prbDst.ptr);
			//		free(prbSrc.ptr);

			for (int i = 0; i < Roi.height; i++)
			{
				for (int j = 0; j < Roi.width; j++)
				{
					src1[i*Roi.width + j] = src[(Roi.height - i - 1)*Roi.width + Roi.width - j - 1];
				}
			}
			memcpy(src, src1, Roi.width*Roi.height * sizeof(Ipp8u));
		}

		if (m_getRoiZero != nullptr)
		{
			free(m_getRoiZero);
		}
		m_getRoiZero = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

		memcpy(m_getRoiZero, src, Roi.width*Roi.height * sizeof(Ipp8u));

		VisSS_Binaryzation(src, Roi);

		VisSS_OpenClose(src, Roi,InputPara.m_OpenCloseflag[0]);

		standardPoint twoPoint;
		twoPoint.leftPoint.max_x = 0;
		twoPoint.leftPoint.min_x = 0;
		twoPoint.leftPoint.max_y = 0;
		twoPoint.leftPoint.min_y = 0;
		twoPoint.leftPoint.CenterX = 0.0;
		twoPoint.leftPoint.CenterY = 0.0;
		twoPoint.rightPoint.max_x = 0;
		twoPoint.rightPoint.min_x = 0;
		twoPoint.rightPoint.max_y = 0;
		twoPoint.rightPoint.min_y = 0;
		twoPoint.rightPoint.CenterX = 0.0;
		twoPoint.rightPoint.CenterY = 0.0;

		int value = 0;

		value = VisSS_Screen(src, Roi, twoPoint);

		if (value != 0)
		{
			test = _CODE_THROW;
			free(src);
			free(dst);
			free(src1);
			return test;
		}

		standardPoint EdgeSize = twoPoint;
		m_normPointZero = twoPoint;

		if (angleFlg == 180)
		{
			if (Roi.width - twoPoint.leftPoint.max_x - 1 > Roi.width - twoPoint.leftPoint.min_x - 1)
			{

				EdgeSize.leftPoint.max_x = Roi.width - twoPoint.leftPoint.max_x - 1;
				EdgeSize.leftPoint.min_x = Roi.width - twoPoint.leftPoint.min_x - 1;
				//EdgeSize.leftPoint.CenterX= Roi.width - EdgeSize.leftPoint.CenterX - 1;
			}
			else
			{
				EdgeSize.leftPoint.min_x = Roi.width - twoPoint.leftPoint.max_x - 1;
				EdgeSize.leftPoint.max_x = Roi.width - twoPoint.leftPoint.min_x - 1;
				//EdgeSize.leftPoint.CenterX= Roi.width - EdgeSize.leftPoint.CenterX - 1;
			}
			if (Roi.width - twoPoint.leftPoint.max_y - 1 > Roi.width - twoPoint.leftPoint.min_y - 1)
			{
				EdgeSize.leftPoint.max_y = Roi.height - twoPoint.leftPoint.max_y - 1;
				EdgeSize.leftPoint.min_y = Roi.height - twoPoint.leftPoint.min_y - 1;
			}
			else
			{
				EdgeSize.leftPoint.min_y = Roi.height - twoPoint.leftPoint.max_y - 1;
				EdgeSize.leftPoint.max_y = Roi.height - twoPoint.leftPoint.min_y - 1;
			}

			if (Roi.width - twoPoint.rightPoint.max_x - 1 > Roi.width - twoPoint.rightPoint.min_x - 1)
			{
				EdgeSize.rightPoint.max_x = Roi.width - twoPoint.rightPoint.max_x - 1;
				EdgeSize.rightPoint.min_x = Roi.width - twoPoint.rightPoint.min_x - 1;
			}
			else
			{
				EdgeSize.rightPoint.min_x = Roi.width - twoPoint.rightPoint.max_x - 1;
				EdgeSize.rightPoint.max_x = Roi.width - twoPoint.rightPoint.min_x - 1;
			}
			if (Roi.width - twoPoint.rightPoint.max_y - 1 > Roi.width - twoPoint.rightPoint.min_y - 1)
			{
				EdgeSize.rightPoint.max_y = Roi.height - twoPoint.rightPoint.max_y - 1;
				EdgeSize.rightPoint.min_y = Roi.height - twoPoint.rightPoint.min_y - 1;
			}
			else
			{
				EdgeSize.rightPoint.min_y = Roi.height - twoPoint.rightPoint.max_y - 1;
				EdgeSize.rightPoint.max_y = Roi.height - twoPoint.rightPoint.min_y - 1;
			}
			EdgeSize.leftPoint.CenterX = Roi.width - twoPoint.leftPoint.CenterX - 1;
			EdgeSize.leftPoint.CenterY = Roi.width - twoPoint.leftPoint.CenterY - 1;
			EdgeSize.rightPoint.CenterX = Roi.width - twoPoint.rightPoint.CenterX - 1;
			EdgeSize.rightPoint.CenterY = Roi.width - twoPoint.rightPoint.CenterY - 1;
		}

		if (m_getRoi != nullptr)
		{
			free(m_getRoi);
		}
		m_getRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

		memcpy(m_getRoi, dst, Roi.width*Roi.height * sizeof(Ipp8u));
		m_GetRoi = { Roi.width,Roi.height };

		m_normPoint = EdgeSize;

		free(dst);
		free(src);
		free(src1);
		test = _CODE_OK;
		return test;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

int CBmSocketLocatingCoarse::VisSS_GetPicture(IMG_UBBUF &GetRoi)
{
	memcpy(GetRoi.ptr, m_getRoi, m_GetRoi.height*m_GetRoi.width * sizeof(Ipp8u));
	GetRoi.size = { (IMG_UWORD)m_GetRoi.width ,(IMG_UWORD)m_GetRoi.height };

	return 0;
}
int CBmSocketLocatingCoarse::VisSS_GetPoint(standardPoint &twoPoint)
{
	twoPoint = m_normPoint;
	return 0;
}

int CBmSocketLocatingCoarse::VisSS_GetPictureZero(IMG_UBBUF &GetRoi)
{
	memcpy(GetRoi.ptr, m_getRoiZero, m_GetRoi.height*m_GetRoi.width * sizeof(Ipp8u));
	GetRoi.size = { (IMG_UWORD)m_GetRoi.width ,(IMG_UWORD)m_GetRoi.height };
	return 0;
}
int CBmSocketLocatingCoarse::VisSS_GetPointZero(standardPoint &twoPoint)
{
	twoPoint = m_normPointZero;
	return 0;
}

bool CBmSocketLocatingCoarse::LoadParameter(char path[])
{
	LPSTR temp = (LPSTR)new char[MAX_PATH];

	GetPrivateProfileStringA("BmSocketLocatingCoarse", "threshold1 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_threshold[0] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLocatingCoarse", "threshold2 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_threshold[1] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLocatingCoarse", "threshold3 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_threshold[2] = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLocatingCoarse", "CircleRoi1x ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi1.coWindowOff.x = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLocatingCoarse", "CircleRoi1y ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi1.coWindowOff.y = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLocatingCoarse", "CircleRoi1width ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi1.szWindowSize.width = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLocatingCoarse", "CircleRoi1height ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi1.szWindowSize.height = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLocatingCoarse", "CircleRoi2x ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi2.coWindowOff.x = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLocatingCoarse", "CircleRoi2y ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi2.coWindowOff.y = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLocatingCoarse", "CircleRoi2width ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi2.szWindowSize.width = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLocatingCoarse", "CircleRoi2height ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi2.szWindowSize.height = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLocatingCoarse", "ScreenAreaMin ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_ScreenAreaMin = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLocatingCoarse", "OpenCloseflag1 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_OpenCloseflag[0] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLocatingCoarse", "OpenCloseflag2 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_OpenCloseflag[1] = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLocatingCoarse", "featureCircleRadius ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_featureCircleRadius = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLocatingCoarse", "errorX ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_edgeError.x = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLocatingCoarse", "errorY ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_edgeError.y = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLocatingCoarse", "featureCircleDisX ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_featureCircleDis.x = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLocatingCoarse", "featureCircleDisY ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_featureCircleDis.y = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLocatingCoarse", "errorRoate", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_errorRoate = (double)atof((char*)temp);

	GetPrivateProfileStringA("BmSocketLocatingCoarse", "errorDis", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_errorDis = (double)atof((char*)temp);

	delete[] temp;
	return TRUE;
}