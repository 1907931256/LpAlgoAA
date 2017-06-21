#include "stdafx.h"
#include "BmSocketStatus.h"

using namespace std;

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
// Author: Huang Yige/05/02/2017
/**********************************************/
_BACK_CODE CBmSocketStatus::VisSS_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma)
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

#ifdef BmSocketLocatingCoarse
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
//     ...
// Return:
//     0 - 正常
//     -1 - 暂无待定
//     -2 - 暂无待定
// Author: Huang Yige/05/02/2017
/**********************************************/
_BACK_CODE CBmSocketStatus::VisSS_Binaryzation(Ipp8u *srcRoi, IppiSize Roi)
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

		ippiThreshold_GTVal_8u_C1IR(srcRoi, srcDstStep, Roi, threshold - 1, 255);

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
// Author: Huang Yige/05/02/2017
/**********************************************/
_BACK_CODE CBmSocketStatus::VisSS_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, int &markersNum)
{
	__try
	{
		int markerStep = Roi.width * sizeof(Ipp8u);
		int minLabel = 1;
		int maxLabel = 200;
		Ipp8u *pBuffer = NULL;
		int bufferSize = 0;

		ippiLabelMarkersGetBufferSize_8u_C1R(Roi, &bufferSize);

		pBuffer = ippsMalloc_8u(bufferSize);

		ippiLabelMarkers_8u_C1IR(srcRoi, markerStep, Roi, minLabel, maxLabel, ippiNormInf, &markersNum, pBuffer);

#ifdef BmSocketLocatingCoarse
		FILE *hp3;
		hp3 = fopen("SS_LabelMarker.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp3, "%d ", 20 * srcRoi[i*Roi.width + j]);
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
//  int flag,开闭运算类型（0为开闭运算都有，且模板都为3*3；1为开闭运算都有，开运算模板5*5，闭运算模板3*3；2为开闭运算都有，闭运算模板5*5，开运算模板3*3；
//							3为开运算，模板为3*3；4为闭运算，模板为3*3；5为开运算，模板为5*5；6为闭运算，模板为5*5）
//     ...
// Output:
// 	Ipp8u *srcRoi, 输出图片
//     ...
// Return:
//     0 - 正常
//     -1 - 暂无待定
//     -2 - 暂无待定
// Author: Huang Yige/05/02/2017
/**********************************************/
_BACK_CODE CBmSocketStatus::VisSS_OpenClose(Ipp8u *srcRoi, IppiSize Roi,int flag)
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

		//6为闭运算，模板为5*5
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
//     -1 - 特征区域不足两个
// Author: Huang Yige/03/18/2017
/**********************************************/
_BACK_CODE CBmSocketStatus::VisSS_Screen(Ipp8u *srcRoi, IppiSize Roi, standardPoint &twoPoint)
{
	__try
	{
		m_signNum.clear();
		m_signEdge.clear();
		m_signReal.clear();
		m_diff.clear();
		Ipp8u* dstRoi;
		dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

		int RowStep = 0;

		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				if (i > InputPara.m_CircleRoi1.coWindowOff.y - 1 && i < InputPara.m_CircleRoi1.coWindowOff.y + InputPara.m_CircleRoi1.szWindowSize.height&&
					j > InputPara.m_CircleRoi1.coWindowOff.x - 1 && j < InputPara.m_CircleRoi1.coWindowOff.x + InputPara.m_CircleRoi1.szWindowSize.width)
				{
					dstRoi[RowStep + j] = srcRoi[RowStep + j];
				}
				else if (i > InputPara.m_CircleRoi2.coWindowOff.y - 1 && i < InputPara.m_CircleRoi2.coWindowOff.y + InputPara.m_CircleRoi2.szWindowSize.height&&
					j > InputPara.m_CircleRoi2.coWindowOff.x - 1 && j < InputPara.m_CircleRoi2.coWindowOff.x + InputPara.m_CircleRoi2.szWindowSize.width)
				{
					dstRoi[RowStep + j] = srcRoi[RowStep + j];
				}
				else
				{
					dstRoi[RowStep + j] = 0;
				}
			}
			RowStep += Roi.width;
		}

		VisSS_OpenClose(dstRoi, Roi, InputPara.m_OpenCloseflag[0]);

		int markersNum = 0;

		VisSS_LabelMarker(dstRoi, Roi, markersNum);

		int *Area;
		Area = (int*)malloc(markersNum * sizeof(int));

		VisSS_GetArea(dstRoi, Roi, markersNum, Area);

		RowStep = 0;

		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{

				if (Area[dstRoi[RowStep + j] - 1] < InputPara.m_ScreenAreaMin)
				{
					dstRoi[RowStep + j] = 0;
				}
			}
			RowStep += Roi.width;
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
		int diffX = 0, diffY = 0;

		for (int k = 0; k < markersNum; k++)
		{
			feature.width = BlobEdge[k].max_x - BlobEdge[k].min_x;
			feature.height = BlobEdge[k].max_y - BlobEdge[k].min_y;

			diffX = feature.width - InputPara.m_featureCircleRadius;
			diffY = feature.height - InputPara.m_featureCircleRadius;

			if ((abs(diffX) < InputPara.m_edgeError.x || abs(diffY) < InputPara.m_edgeError.y))
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

			RowStep = 0;

			for (int i = 0; i < Roi.height; i++)
			{
				for (int j = 0; j < Roi.width; j++)
				{
					if (dstRoi[RowStep + j] != m_signNum[0] && dstRoi[RowStep + j] != m_signNum[1])
					{
						dstRoi[RowStep + j] = 0;
					}
				}
				RowStep += Roi.width;
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

			/*if (m_signReal.size() <1||m_signEdge.size() < m_signReal[0].x|| m_signEdge.size() < m_signReal[0].y)
			{
				free(dstRoi);
				free(Area);
				free(BlobEdge);
				return _CODE_THROW;
			}

			int abcd = m_signReal[0].x;
			int abcde = m_signReal[0].y;*/
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

			RowStep = 0;

			for (int i = 0; i < Roi.height; i++)
			{
				for (int j = 0; j < Roi.width; j++)
				{
					if (dstRoi[RowStep + j] != m_signNum[m_signReal[0].x] && dstRoi[RowStep + j] != m_signNum[m_signReal[0].y])
					{
						dstRoi[RowStep + j] = 0;
					}
					else
					{
						dstRoi[RowStep + j] = 255;
					}
				}
				RowStep = RowStep + Roi.width;
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
_BACK_CODE CBmSocketStatus::VisSS_GetArea(Ipp8u *srcRoi, IppiSize Roi, int &markersNum, int *&Area)
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
_BACK_CODE CBmSocketStatus::VisSS_GetEdge(Ipp8u *srcRoi, IppiSize Roi, int &markersNum, RoiFeature *&BlobEdge)
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
// VisSS_GetBlobThreshold, 功能说明:获取动态阈值对图像进行二值化。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
//     ...
// Return:
//     0 - 正常
// Author: Huang Yige/05/02/2017
/**********************************************/
_BACK_CODE CBmSocketStatus::VisSS_GetBlobThreshold(Ipp8u *srcRoi, IppiSize Roi)
{
	__try
	{
		Ipp8u* dstRoi;
		dstRoi = (Ipp8u*)malloc(Roi.width * sizeof(Ipp8u));

		int SmallRoiY = (int)((float)Roi.height / (float)BlobThreshold);
		int srcDstStep = Roi.width * sizeof(Ipp8u);

		Ipp8u* SmallRoi1;
		SmallRoi1 = (Ipp8u*)malloc(Roi.width*SmallRoiY * sizeof(Ipp8u));
		IppiSize ThresholdRoiFirst = { Roi.width ,SmallRoiY };

		Ipp8u* SmallRoiLast;
		SmallRoiLast = (Ipp8u*)malloc(Roi.width*(Roi.height - (BlobThreshold - 1) * SmallRoiY) * sizeof(Ipp8u));
		IppiSize ThresholdRoiLast = { Roi.width ,Roi.height - (BlobThreshold - 1) * SmallRoiY };

		/*Ipp8u* pMinThreshold;
		pMinThreshold = (Ipp8u*)malloc(BlobThreshold * sizeof(Ipp8u));*/
		Ipp64f* pMinThreshold;
		pMinThreshold = (Ipp64f*)malloc(BlobThreshold * sizeof(Ipp64f));

		Ipp64f* pMinFilter;
		pMinFilter = (Ipp64f*)malloc(Roi.height * sizeof(Ipp64f));

		IppiSize FilterRoi = { Roi.width,1 };

		for (int i = 0; i < (BlobThreshold - 1); i++)
		{
			for (int j = 0; j < Roi.width*SmallRoiY; j++)
			{
				SmallRoi1[j] = srcRoi[i*(Roi.width*SmallRoiY) + j];
			}

			//ippiMin_8u_C1R(SmallRoi1, srcDstStep, ThresholdRoiFirst, &pMinThreshold[i]);
			ippiMean_8u_C1R(SmallRoi1, srcDstStep, ThresholdRoiFirst, &pMinThreshold[i]);
		}

		for (int j = 0; j < ThresholdRoiLast.width*ThresholdRoiLast.height; j++)
		{
			SmallRoiLast[j] = srcRoi[(Roi.width*SmallRoiY) * (BlobThreshold - 1) + j];
		}

		//ippiMin_8u_C1R(SmallRoiLast, srcDstStep, ThresholdRoiLast, &pMinThreshold[(BlobThreshold - 1)]);
		ippiMean_8u_C1R(SmallRoiLast, srcDstStep, ThresholdRoiLast, &pMinThreshold[(BlobThreshold - 1)]);

		//最小二乘法求取直线
		float A = 0.0, B = 0.0;
		if (BlobThreshold == 5)
		{
			A = (float)((5.0 * (0.5 * (float)pMinThreshold[0] + 1.5 * (float)pMinThreshold[1] + 2.5 * (float)pMinThreshold[2] + 3.5 * (float)pMinThreshold[3] + 4.5 * (float)pMinThreshold[4]) -
				(0.5 + 1.5 + 2.5 + 3.5 + 4.5)*(float)(pMinThreshold[0] + pMinThreshold[1] + pMinThreshold[2] + pMinThreshold[3] + pMinThreshold[4])) /
				(5.0*(0.5*0.5 + 1.5*1.5 + 2.5*2.5 + 3.5*3.5 + 4.5*4.5) - (0.5 + 1.5 + 2.5 + 3.5 + 4.5)*(0.5 + 1.5 + 2.5 + 3.5 + 4.5)));
			B = (float)((pMinThreshold[0] + pMinThreshold[1] + pMinThreshold[2] + pMinThreshold[3] + pMinThreshold[4]) / 5.0 -
				A * (0.5 + 1.5 + 2.5 + 3.5 + 4.5) / 5.0);
		}
		if (BlobThreshold == 6)
		{
			A = (float)((6.0 * (0.5 * (float)pMinThreshold[0] + 1.5 * (float)pMinThreshold[1] + 2.5 * (float)pMinThreshold[2] + 3.5 * (float)pMinThreshold[3] + 4.5 * (float)pMinThreshold[4] + 5.5 * (float)pMinThreshold[5]) -
				(0.5 + 1.5 + 2.5 + 3.5 + 4.5 + 5.5)*((float)pMinThreshold[0] + (float)pMinThreshold[1] + (float)pMinThreshold[2] + (float)pMinThreshold[3] + (float)pMinThreshold[4] + (float)pMinThreshold[5])) /
				(6.0*(0.5*0.5 + 1.5*1.5 + 2.5*2.5 + 3.5*3.5 + 4.5*4.5 + 5.5*5.5) - (0.5 + 1.5 + 2.5 + 3.5 + 4.5 + 5.5)*(0.5 + 1.5 + 2.5 + 3.5 + 4.5 + 5.5)));
			B = (float)(((float)pMinThreshold[0] + (float)pMinThreshold[1] + (float)pMinThreshold[2] + (float)pMinThreshold[3] + (float)pMinThreshold[4] + (float)pMinThreshold[5]) / 6.0 -
				A * (0.5 + 1.5 + 2.5 + 3.5 + 4.5 + 5.5) / 6.0);
		}
		if (BlobThreshold == 7)
		{
			A = (float)((7.0 * (0.5 * (float)pMinThreshold[0] + 1.5 * (float)pMinThreshold[1] + 2.5 * (float)pMinThreshold[2] + 3.5 * (float)pMinThreshold[3] + 4.5 * (float)pMinThreshold[4] + 5.5 * (float)pMinThreshold[5] + 6.5 * (float)pMinThreshold[6]) -
				(0.5 + 1.5 + 2.5 + 3.5 + 4.5 + 5.5 + 6.5)*((float)pMinThreshold[0] + (float)pMinThreshold[1] + (float)pMinThreshold[2] + (float)pMinThreshold[3] + (float)pMinThreshold[4] + (float)pMinThreshold[5] + (float)pMinThreshold[6])) /
				(7.0*(0.5*0.5 + 1.5*1.5 + 2.5*2.5 + 3.5*3.5 + 4.5*4.5 + 5.5*5.5 + 6.5*6.5) - (0.5 + 1.5 + 2.5 + 3.5 + 4.5 + 5.5 + 6.5)*(0.5 + 1.5 + 2.5 + 3.5 + 4.5 + 5.5 + 6.5)));
			B = (float)(((float)pMinThreshold[0] + (float)pMinThreshold[1] + (float)pMinThreshold[2] + (float)pMinThreshold[3] + (float)pMinThreshold[4] + (float)pMinThreshold[5] + (float)pMinThreshold[6]) / 7.0 -
				A * (0.5 + 1.5 + 2.5 + 3.5 + 4.5 + 5.5 + 6.5) / 7.0);
		}


		float coodrate_x = 0.0;

		for (int i = 0; i < Roi.height; i++)
		{
			coodrate_x = (float)i / (float)SmallRoiY;

			pMinFilter[i] = B + A*coodrate_x;
		}

		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				dstRoi[j] = srcRoi[i*Roi.width + j];
			}

			if ((int)pMinFilter[i] - 5 < 0)
			{
				ippiThreshold_LTValGTVal_8u_C1IR(dstRoi, srcDstStep, FilterRoi, 0, 255, 0, 0);
			}
			else
			{
				ippiThreshold_LTValGTVal_8u_C1IR(dstRoi, srcDstStep, FilterRoi, (int)pMinFilter[i] - 5, 255, (int)pMinFilter[i] - 5, 0);

			}

			for (int j = 0; j < Roi.width; j++)
			{
				srcRoi[i*Roi.width + j] = dstRoi[j];
			}
		}

		/*for (int i = 0; i < (BlobThreshold - 1); i++)
		{
			for (int j = 0; j < Roi.width*SmallRoiY; j++)
			{
				SmallRoi1[j] = srcRoi[i*(Roi.width*SmallRoiY) + j];
			}

			ippiThreshold_LTValGTVal_8u_C1IR(SmallRoi1, srcDstStep, ThresholdRoiFirst, (int)(2.0* pMinThreshold[i]), 255, (int)(2.0* pMinThreshold[i]), 0);

			ippiThreshold_GTVal_8u_C1IR(SmallRoi1, srcDstStep, ThresholdRoiFirst, (int)(2.0* pMinThreshold[i]) - 1, 255);

			for (int j = 0; j < Roi.width*SmallRoiY; j++)
			{
				srcRoi[i*(Roi.width*SmallRoiY) + j] = SmallRoi1[j];;
			}
		}

		for (int j = 0; j < ThresholdRoiLast.width*ThresholdRoiLast.height; j++)
		{
			SmallRoiLast[j] = srcRoi[(Roi.width*SmallRoiY) * (BlobThreshold - 1) + j];
		}

		ippiThreshold_LTValGTVal_8u_C1IR(SmallRoiLast, srcDstStep, ThresholdRoiLast, (int)(2.0* pMinThreshold[(BlobThreshold - 1)]), 255, (int)(2.0* pMinThreshold[(BlobThreshold - 1)]), 0);

		ippiThreshold_GTVal_8u_C1IR(SmallRoiLast, srcDstStep, ThresholdRoiLast, (int)(2.0* pMinThreshold[(BlobThreshold - 1)]) - 1, 255);

		for (int j = 0; j < ThresholdRoiLast.width*ThresholdRoiLast.height; j++)
		{
			srcRoi[(Roi.width*SmallRoiY) * (BlobThreshold - 1) + j] = SmallRoiLast[j];
		}*/

#ifdef DEBUG_BmSocketStatus
		FILE *hp25;
		hp25 = fopen("VisSS_GetLinearThreshold.txt", "w+");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp25, "%d ", srcRoi[i*Roi.width + j]);
			}
			fprintf(hp25, "\n");
		}
		fclose(hp25);
#endif // DEBUG_BmSocketStatus

		free(dstRoi);
		free(SmallRoiLast);
		free(SmallRoi1);
		free(pMinThreshold);
		free(pMinFilter);
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// VisSS_EmptyJudge, 功能说明:判断socket是否为空。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
// 	int &result, socket是否为空（0为空）
//     ...
// Return:
//     0 - 正常
// Author: Huang Yige/05/02/2017
/**********************************************/
_BACK_CODE CBmSocketStatus::VisSS_EmptyJudge(Ipp8u *srcRoi, IppiSize Roi, int &result)
{
	__try
	{
		Ipp8u* JudgeRoiMask;
		JudgeRoiMask = (Ipp8u*)malloc(InputPara.m_EmptyJudgeRoi.szWindowSize.width*InputPara.m_EmptyJudgeRoi.szWindowSize.height * sizeof(Ipp8u));

		for (int i = 0; i < InputPara.m_EmptyJudgeRoi.szWindowSize.height; i++)
		{
			for (int j = 0; j < InputPara.m_EmptyJudgeRoi.szWindowSize.width; j++)
			{
				JudgeRoiMask[i*InputPara.m_EmptyJudgeRoi.szWindowSize.width + j] = srcRoi[(i + InputPara.m_EmptyJudgeRoi.coWindowOff.y)*Roi.width + (j + InputPara.m_EmptyJudgeRoi.coWindowOff.x)];
			}
		}

		IppiSize JudgeRoiSize = { InputPara.m_EmptyJudgeRoi.szWindowSize.width,InputPara.m_EmptyJudgeRoi.szWindowSize.height };

		int markersNum = 0;

		VisSS_LabelMarker(JudgeRoiMask, JudgeRoiSize, markersNum);

		if (markersNum < 2)
		{
			result = 1;
			free(JudgeRoiMask);
			return _CODE_THROW;
		}

		int *Area;
		Area = (int*)malloc(markersNum * sizeof(int));

		VisSS_GetArea(JudgeRoiMask, JudgeRoiSize, markersNum, Area);

#ifdef DEBUG_BmSocketStatus
		for (int i = 0; i < markersNum; i++)
		{
			printf("Num[%d]=%d\n", i + 1, Area[i]);
		}
#endif // DEBUG_BmSocketStatus



		//float AreaBlob = 0.0;
		/*float AreaMin = 200.0;
		float AreaMax = 800.0 ;*/

		for (int i = 0; i <InputPara.m_EmptyJudgeRoi.szWindowSize.height; i++)
		{
			for (int j = 0; j < InputPara.m_EmptyJudgeRoi.szWindowSize.width; j++)
			{
				//AreaBlob = (float)Area[JudgeRoiMask[i*JudgeRoi.szWindowSize.width + j] - 1] / ((float)JudgeRoi.szWindowSize.height*(float)JudgeRoi.szWindowSize.width);

				if (Area[JudgeRoiMask[i*InputPara.m_EmptyJudgeRoi.szWindowSize.width + j] - 1] < InputPara.m_EmptyJudgeAreaMin||
					Area[JudgeRoiMask[i*InputPara.m_EmptyJudgeRoi.szWindowSize.width + j] - 1]>InputPara.m_EmptyJudgeAreaMax)
				{
					JudgeRoiMask[i*InputPara.m_EmptyJudgeRoi.szWindowSize.width + j] = 0;
				}
			}
		}

		VisSS_LabelMarker(JudgeRoiMask, JudgeRoiSize, markersNum);

		if (markersNum != 2)
		{
			result = 1;
			free(Area);
			free(JudgeRoiMask);
			return _CODE_THROW;
		}

		//IMG_RCOORD *CenterPoint;
		RoiFeature *BlobEdge;
		//CenterPoint = (IMG_RCOORD*)malloc(markersNum * sizeof(IMG_RCOORD));
		BlobEdge = (RoiFeature*)malloc(markersNum * sizeof(RoiFeature));

		VisSS_GetEdge(JudgeRoiMask, JudgeRoiSize, markersNum, BlobEdge);

		int markersNum1 = 0;

		/*VisSS_LabelMarker(srcRoi, Roi, markersNum1);

		IMG_RCOORD *CenterPoint1;
		RoiEdge *BlobEdge1;
		CenterPoint1 = (IMG_RCOORD*)malloc(markersNum1 * sizeof(IMG_RCOORD));
		BlobEdge1 = (RoiEdge*)malloc(markersNum1 * sizeof(RoiEdge));

		VisSS_GetEdge(srcRoi, Roi, markersNum1, CenterPoint1, BlobEdge1);*/

		int StandardX[6] = { 0 };
		int StandardY[6] = { 0 };
		int CircleY[6] = { 0 };

		if (BlobEdge[0].CenterX < BlobEdge[1].CenterX)
		{
			StandardX[0] = (int)BlobEdge[0].CenterX + InputPara.m_EmptyJudgeRoi.coWindowOff.x - InputPara.m_EmptyJudgeColumn[0];
			StandardX[1] = (int)BlobEdge[1].CenterX + InputPara.m_EmptyJudgeRoi.coWindowOff.x + InputPara.m_EmptyJudgeColumn[1];
			StandardX[2] = (int)StandardX[0] - InputPara.m_EmptyJudgeColumn[2];
			StandardX[3] = (int)StandardX[1] + InputPara.m_EmptyJudgeColumn[2];
			StandardX[4] = (int)StandardX[0] + InputPara.m_EmptyJudgeColumn[3];
			StandardX[5] = (int)StandardX[1] - InputPara.m_EmptyJudgeColumn[3];
		}
		else
		{
			StandardX[0] = (int)BlobEdge[1].CenterX + InputPara.m_EmptyJudgeRoi.coWindowOff.x - InputPara.m_EmptyJudgeColumn[0];
			StandardX[1] = (int)BlobEdge[0].CenterX + InputPara.m_EmptyJudgeRoi.coWindowOff.x + InputPara.m_EmptyJudgeColumn[1];
			StandardX[2] = (int)StandardX[0] - InputPara.m_EmptyJudgeColumn[2];
			StandardX[3] = (int)StandardX[1] + InputPara.m_EmptyJudgeColumn[2];
			StandardX[4] = (int)StandardX[0] + InputPara.m_EmptyJudgeColumn[3];
			StandardX[5] = (int)StandardX[1] - InputPara.m_EmptyJudgeColumn[3];
		}


		CircleY[0] = (int)BlobEdge[0].CenterY + InputPara.m_EmptyJudgeRoi.coWindowOff.y - InputPara.m_EmptyJudgeRow[0];
		CircleY[1] = (int)BlobEdge[0].CenterY + InputPara.m_EmptyJudgeRoi.coWindowOff.y + InputPara.m_EmptyJudgeRow[1];
		CircleY[2] = (int)BlobEdge[0].CenterY + InputPara.m_EmptyJudgeRoi.coWindowOff.y + InputPara.m_EmptyJudgeRow[2];
		CircleY[3] = (int)BlobEdge[0].CenterY + InputPara.m_EmptyJudgeRoi.coWindowOff.y + InputPara.m_EmptyJudgeRow[3];
		CircleY[4] = (int)BlobEdge[0].CenterY + InputPara.m_EmptyJudgeRoi.coWindowOff.y + InputPara.m_EmptyJudgeRow[4];
		CircleY[5] = (int)BlobEdge[0].CenterY + InputPara.m_EmptyJudgeRoi.coWindowOff.y + InputPara.m_EmptyJudgeRow[5];


		StandardY[1] = (int)((CircleY[0] + CircleY[1]) / 2.0);
		StandardY[2] = (int)((CircleY[1] + CircleY[2]) / 2.0);
		StandardY[3] = (int)((CircleY[2] + CircleY[3]) / 2.0);
		StandardY[4] = (int)((CircleY[3] + CircleY[4]) / 2.0);
		StandardY[5] = (int)((CircleY[4] + CircleY[5]) / 2.0);
		StandardY[0] = StandardY[1] - InputPara.m_EmptyJudgeCircle;

		int LineNum = 6;
		for (int i = 0; i < 6; i++)
		{
			if (StandardY[i] > Roi.height)
			{
				LineNum--;
			}
		}

		int CircleLineNum = 6;
		for (int i = 0; i < 6; i++)
		{
			if (CircleY[i] > Roi.height)
			{
				CircleLineNum--;
			}
		}

		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				if ((j > StandardX[0] && j < StandardX[4]) && i < StandardY[2])
				{
					srcRoi[i*Roi.width + j] = 0;
				}
				if ((j > StandardX[5] && j < StandardX[1]) && i < StandardY[2])
				{
					srcRoi[i*Roi.width + j] = 0;
				}
				if ((j>StandardX[1] || j<StandardX[0]) && i > CircleY[5] + 13)
				{
					srcRoi[i*Roi.width + j] = 0;
				}

				if (j<StandardX[1] && j>StandardX[0] && i < CircleY[0] + 8.0)
				{
					srcRoi[i*Roi.width + j] = 0;
				}
				if (j<StandardX[2] || j>StandardX[3])
				{
					srcRoi[i*Roi.width + j] = 0;
				}
				if (i < StandardY[0])
				{
					srcRoi[i*Roi.width + j] = 0;
				}

				if (j == StandardX[0] || j == StandardX[1])
				{
					srcRoi[i*Roi.width + j] = 0;
				}
				if (j < StandardX[0])
				{
					for (int k = 0; k < LineNum; k++)
					{
						if (i == StandardY[k])
						{
							srcRoi[i*Roi.width + j] = 0;
						}
					}
				}
				if (j > StandardX[1])
				{
					for (int k = 0; k < LineNum; k++)
					{
						if (i == StandardY[k] + 1)
						{
							srcRoi[i*Roi.width + j] = 0;
						}
					}
				}
			}
		}

		VisSS_LabelMarker(srcRoi, Roi, markersNum);

		int *AreaOriginal;
		AreaOriginal = (int*)malloc(markersNum * sizeof(int));

		VisSS_GetArea(srcRoi, Roi, markersNum, AreaOriginal);

	#ifdef DEBUG_BmSocketStatus
		for (int i = 0; i < markersNum; i++)
		{
			printf("Num[%d]=%d\n", i + 1, AreaOriginal[i]);
	}
	#endif // DEBUG_BmSocketStatus

		int smallCircle = 0;
		int whiteBlob = 0;


		for (int i = 0; i <Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{


				if (AreaOriginal[srcRoi[i*Roi.width + j] - 1] < InputPara.m_AreaMinIncomplete)
				{
					srcRoi[i*Roi.width + j] = 0;
				}
			}
		}


		for (int i = 0; i < markersNum; i++)
		{


			if (AreaOriginal[i] > InputPara.m_AreaMinOriginal&&AreaOriginal[i] < InputPara.m_AreaMidOriginal)
			{
				smallCircle++;
			}

			if (AreaOriginal[i] > InputPara.m_AreaMaxOriginal)
			{
				whiteBlob++;
			}
		}

		int *EmptyBlobWhite;
		EmptyBlobWhite = (int*)malloc(12 * sizeof(int));

		for (int k = 0; k < 12; k++)
		{
			EmptyBlobWhite[k] = 0;
		}

		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				if (j > StandardX[2] && j < StandardX[0])
				{
					for (int k = 0; k < 5; k++)
					{
						if (i > StandardY[k] && i < StandardY[k + 1])
						{
							if (srcRoi[i*Roi.width + j] != 0)
							{
								EmptyBlobWhite[2 * k]++;
							}
						}
					}
					if (i > StandardY[5])
					{
						if (srcRoi[i*Roi.width + j] != 0)
						{
							EmptyBlobWhite[10]++;
						}
					}
				}
				if (j > StandardX[1] && j < StandardX[3])
				{
					for (int k = 0; k < 5; k++)
					{
						if (i > StandardY[k] && i < StandardY[k + 1])
						{
							if (srcRoi[i*Roi.width + j] != 0)
							{
								EmptyBlobWhite[2 * k + 1]++;
							}
						}
					}
					if (i > StandardY[5])
					{
						if (srcRoi[i*Roi.width + j] != 0)
						{
							EmptyBlobWhite[11]++;
						}
					}
				}
			}
		}

		int SignCircleNum = 0;

		if (CircleY[5] + 13 < Roi.height)
		{
			if (smallCircle == 14 && whiteBlob == 1)
			{
				if (EmptyBlobWhite[2] < EmptyBlobWhite[0] * InputPara.m_EmptyBlobWhiteRatio || EmptyBlobWhite[3] < EmptyBlobWhite[1] * InputPara.m_EmptyBlobWhiteRatio)
				{
					result = 1;
				}
				else if (EmptyBlobWhite[6] > InputPara.m_EmptyBlobWhiteLine || EmptyBlobWhite[8] > InputPara.m_EmptyBlobWhiteLine)
				{
					result = 1;
				}
				else
				{
					result = 0;
				}
			}
			else if (smallCircle == 13 && whiteBlob == 1)
			{
				if (EmptyBlobWhite[2] < EmptyBlobWhite[0] * InputPara.m_EmptyBlobWhiteRatio || EmptyBlobWhite[3] < EmptyBlobWhite[1] * InputPara.m_EmptyBlobWhiteRatio)
				{
					SignCircleNum++;
				}
				for (int k = 0; k < 12; k++)
				{
					if (k == 11)
					{
						if ((EmptyBlobWhite[10] < InputPara.m_AreaMinIncomplete ))
						{
							SignCircleNum++;
						}
					}
					else
					{
						if ((float)EmptyBlobWhite[k] < InputPara.m_AreaMinOriginal)
						{
							SignCircleNum++;
						}
					}
				
				}
				if (SignCircleNum == 0)
				{
					if (EmptyBlobWhite[6] > InputPara.m_EmptyBlobWhiteLine || EmptyBlobWhite[8] > InputPara.m_EmptyBlobWhiteLine)
					{
						result = 1;
					}
					else
					{
						result = 0;
					}
				}
				else
				{
					result = 1;
				}
			}
			else
			{
				result = 1;
			}
			
		}
		if (CircleY[5] > Roi.height&&StandardY[5] < Roi.height)
		{
			if (smallCircle == 12 && whiteBlob == 1)
			{
				result = 0;
				if (EmptyBlobWhite[2] < EmptyBlobWhite[0] * InputPara.m_EmptyBlobWhiteRatio || EmptyBlobWhite[3] < EmptyBlobWhite[1] * InputPara.m_EmptyBlobWhiteRatio)
				{
					result = 1;
				}
				else if (EmptyBlobWhite[6] > InputPara.m_EmptyBlobWhiteLine || EmptyBlobWhite[8] > InputPara.m_EmptyBlobWhiteLine)
				{
					result = 1;
				}
				else
				{
					result = 0;
				}
			}
			else
			{
				result = 1;
			}
			
		}
		if (CircleY[4] < Roi.height&&StandardY[5] > Roi.height-1)
		{
			if (smallCircle == 12 && whiteBlob == 1)
			{
				if (EmptyBlobWhite[2] < EmptyBlobWhite[0] * InputPara.m_EmptyBlobWhiteRatio || EmptyBlobWhite[3] < EmptyBlobWhite[1] * InputPara.m_EmptyBlobWhiteRatio)
				{
					result = 1;
				}
				else if (EmptyBlobWhite[6] > InputPara.m_EmptyBlobWhiteLine || EmptyBlobWhite[8] > InputPara.m_EmptyBlobWhiteLine)
				{
					result = 1;
				}
				else
				{
					result = 0;
				}
			}
			else
			{
				result = 1;
			}
			
		}

	#ifdef DEBUG_BmSocketStatus
		FILE *hp45;
		hp45 = fopen("VisSS_EmptyJudge.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp45, "%d ", srcRoi[i*Roi.width + j]);
			}
			fprintf(hp45, "\n");
		}
		fclose(hp45);
	#endif

		free(AreaOriginal);
		free(Area);
		free(BlobEdge);
		free(JudgeRoiMask);
		free(EmptyBlobWhite);
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// Vis_ForeignJudge, 功能说明:判断Socket中dut摆放是否正确。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//  DUTOutPutParam param，芯片的特征点
//  int angle，输入角度
//     ...
// Output:
// 	int &result, socket是否有异物（1为有，2为摆放标准）
//     ...
// Return:
//     0 - 正常
// Author: Huang Yige/05/02/2017
/**********************************************/
_BACK_CODE CBmSocketStatus::Vis_ForeignJudge(Ipp8u *srcRoi, IppiSize Roi, int &result, DUTOutPutParam param, int angle)
{
	__try
	{
		Ipp8u* JudgeRoi1;
		Ipp8u* JudgeRoi2;
		Ipp8u* JudgeRoi3;
		Ipp8u* JudgeRoi4;
		Ipp8u* JudgeStandard;

		//IppiSize RoiStandard = { 120,80 };
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
			JudgeStandardPoint = { (IMG_WORD)(param.cx - InputPara.m_ForeignJudgeStaPointValue),(IMG_WORD)(param.cy - InputPara.m_ForeignJudgeStaPointValue) };

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
				JudgeRoiPoint2 = { (IMG_WORD)(param.x2 - InputPara.m_ForeignJudgeRoiPointValue),(IMG_WORD)(param.y2) };
			}
			else
			{
				JudgeRoiPoint2 = { (IMG_WORD)(param.x3 - InputPara.m_ForeignJudgeRoiPointValue),(IMG_WORD)(param.y2) };
			}
			if (param.y4 > param.y3)
			{
				JudgeRoiPoint3 = { (IMG_WORD)(param.x4),(IMG_WORD)(param.y3 - InputPara.m_ForeignJudgeRoiPointValue) };
			}
			else
			{
				JudgeRoiPoint3 = { (IMG_WORD)(param.x4),(IMG_WORD)(param.y4 - InputPara.m_ForeignJudgeRoiPointValue) };
			}
			if (param.x1 > param.x4)
			{
				JudgeRoiPoint4 = { (IMG_WORD)(param.x1), (IMG_WORD)(param.y1) };
			}
			else
			{
				JudgeRoiPoint4 = { (IMG_WORD)(param.x4), (IMG_WORD)(param.y1) };
			}
			Roi1 = { (int)((param.x2) - (param.x1)),InputPara.m_ForeignJudgeRoiPointValue };
			Roi2 = { InputPara.m_ForeignJudgeRoiPointValue,(int)((param.y3) - (param.y2)) };
			Roi3 = { (int)((param.x3) - (param.x4)),InputPara.m_ForeignJudgeRoiPointValue };
			Roi4 = { InputPara.m_ForeignJudgeRoiPointValue,(int)((param.y4) - (param.y1)) };
		}
		if (angle == 180)
		{
			JudgeStandardPoint = { (IMG_WORD)(param.cx - InputPara.m_ForeignJudgeStaPointValue),(IMG_WORD)(param.cy) };

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
				JudgeRoiPoint2 = { (IMG_WORD)(param.x4 - InputPara.m_ForeignJudgeRoiPointValue),(IMG_WORD)(param.y4) };
			}
			else
			{
				JudgeRoiPoint2 = { (IMG_WORD)(param.x1 - InputPara.m_ForeignJudgeRoiPointValue),(IMG_WORD)(param.y4) };
			}
			if (param.y2 > param.y1)
			{
				JudgeRoiPoint3 = { (IMG_WORD)(param.x2),(IMG_WORD)(param.y1 - InputPara.m_ForeignJudgeRoiPointValue) };
			}
			else
			{
				JudgeRoiPoint3 = { (IMG_WORD)(param.x2),(IMG_WORD)(param.y2 - InputPara.m_ForeignJudgeRoiPointValue) };
			}
			if (param.x3 > param.x2)
			{
				JudgeRoiPoint4 = { (IMG_WORD)(param.x3), (IMG_WORD)(param.y3) };
			}
			else
			{
				JudgeRoiPoint4 = { (IMG_WORD)(param.x2), (IMG_WORD)(param.y3) };
			}
			Roi1 = { (int)((param.x1) - (param.x2)),InputPara.m_ForeignJudgeRoiPointValue };
			Roi2 = { InputPara.m_ForeignJudgeRoiPointValue,(int)((param.y2) - (param.y3)) };
			Roi3 = { (int)((param.x4) - (param.x3)),InputPara.m_ForeignJudgeRoiPointValue };
			Roi4 = { InputPara.m_ForeignJudgeRoiPointValue,(int)((param.y1) - (param.y4)) };
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
		JudgeStandard = (Ipp8u*)malloc(InputPara.m_ForeignJudgeRoiStandard.width*InputPara.m_ForeignJudgeRoiStandard.height * sizeof(Ipp8u));

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
		for (int i = 0; i < InputPara.m_ForeignJudgeRoiStandard.height; i++)
		{
			for (int j = 0; j < InputPara.m_ForeignJudgeRoiStandard.width; j++)
			{
				JudgeStandard[i*InputPara.m_ForeignJudgeRoiStandard.width + j] = srcRoi[(i + JudgeStandardPoint.y)*Roi.width + (j + JudgeStandardPoint.x)];
			}
		}

		int srcDstStep1 = Roi1.width * sizeof(Ipp8u);
		int srcDstStep2 = Roi2.width * sizeof(Ipp8u);
		int srcDstStep3 = Roi3.width * sizeof(Ipp8u);
		int srcDstStep4 = Roi4.width * sizeof(Ipp8u);
		int srcDstStepStandard = InputPara.m_ForeignJudgeRoiStandard.width * sizeof(Ipp8u);

		double pMean[5];

		ippiMean_8u_C1R(JudgeStandard, srcDstStepStandard, InputPara.m_ForeignJudgeRoiStandard, &pMean[0]);
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
					if (pMean[i] < pMean[0] * InputPara.m_ForeignAreapMean)
					{
						ErrorNum++;
					}
				}
				else
				{
					if (pMean[i] < pMean[0] * InputPara.m_ForeignAreapMeanAbnormal)
					{
						ErrorNum++;
					}
				}
			}
			if (angle == 180)
			{
				if (i == 3)
				{
					if (pMean[i] < pMean[0] * InputPara.m_ForeignAreapMeanAbnormal)
					{
						ErrorNum++;
					}
				}
				else
				{
					if (pMean[i] < pMean[0] * InputPara.m_ForeignAreapMean)
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
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// Vis_StandardJudge, 功能说明:判断Socket中是否有异物。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//  DUTOutPutParam param，芯片的特征点
//     ...
// Output:
// 	int &result, socket是否有异物（1为有，2为摆放标准）
//     ...
// Return:
//     0 - 正常
// Author: Huang Yige/05/02/2017
/*********************************************/
_BACK_CODE CBmSocketStatus::VisSS_LocationJudge(DUTOutPutParam param, standardPoint twoPoint, int angle, int &result)
{
	__try
	{
		if (angle == 0)
		{
			int StandardRow1 = 0, StandardRow2 = 0, StandardColumn = 0;
			if (twoPoint.leftPoint.min_x < twoPoint.rightPoint.min_x)
			{
				StandardRow1 = twoPoint.leftPoint.min_x * 4 - InputPara.m_LocationJudgeRowLeft;
				StandardRow2 = twoPoint.leftPoint.max_x * 4 + InputPara.m_LocationJudgeRowRight;
				StandardColumn = twoPoint.leftPoint.max_y * 4 + InputPara.m_LocationJudgeColumnUp;
			}
			else
			{
				StandardRow1 = twoPoint.rightPoint.min_x * 4 - InputPara.m_LocationJudgeRowLeft;
				StandardRow2 = twoPoint.rightPoint.max_x * 4 + InputPara.m_LocationJudgeRowRight;
				StandardColumn = twoPoint.rightPoint.max_y * 4 + InputPara.m_LocationJudgeColumnUp;
			}

			if (param.x1<StandardRow1 || param.x4<StandardRow1 || param.x2>StandardRow2 || param.x3>StandardRow2 || param.y1 < StandardColumn || param.y2 < StandardColumn)
			{
				result = 1;
			}
			else
			{
				result = 2;
			}
		}
		if (angle == 180)
		{
			int StandardRow1 = 0, StandardRow2 = 0, StandardColumn = 0;
			if (twoPoint.leftPoint.min_x > twoPoint.rightPoint.min_x)
			{
				StandardRow1 = twoPoint.leftPoint.min_x * 4 - InputPara.m_LocationJudgeRowRight;
				StandardRow2 = twoPoint.leftPoint.max_x * 4 + InputPara.m_LocationJudgeRowLeft;
				StandardColumn = twoPoint.leftPoint.max_y * 4 - InputPara.m_LocationJudgeColumnUp;
			}
			else
			{
				StandardRow1 = twoPoint.rightPoint.min_x * 4 - InputPara.m_LocationJudgeRowRight;
				StandardRow2 = twoPoint.rightPoint.max_x * 4 + InputPara.m_LocationJudgeRowLeft;
				StandardColumn = twoPoint.rightPoint.max_y * 4 - InputPara.m_LocationJudgeColumnUp;
			}
			if (param.x1 > StandardRow2 || param.x4 > StandardRow2 || param.x2<StandardRow1 || param.x3<StandardRow1 || param.y1>StandardColumn || param.y2>StandardColumn)
			{
				result = 1;
			}
			else
			{
				result = 2;
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
// Vis_StandardJudge, 功能说明:判断Socket中是否有异物。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//  DUTOutPutParam param，芯片的特征点
//     ...
// Output:
// 	int &result, socket是否有异物（1为有，2为摆放标准）
//     ...
// Return:
//     0 - 正常
// Author: Huang Yige/05/02/2017
/*********************************************/
_BACK_CODE CBmSocketStatus::VisSS_AngleJudge(DUTOutPutParam param, int angle, int &result)
{
	__try
	{
		if (angle == 0)
		{
			int resultNum = 0;

			float moduleA = sqrt((param.x1 - param.x2)*(param.x1 - param.x2) + (param.y1 - param.y2)*(param.y1 - param.y2));
			float moduleB = sqrt((param.x1 - param.x4)*(param.x1 - param.x4) + (param.y1 - param.y4)*(param.y1 - param.y4));

			float AngelLines = ((param.x1 - param.x2)*(param.x1 - param.x4) + (param.y1 - param.y2)*(param.y1 - param.y4)) / (moduleA*moduleB);
			AngelLines = (float)((float)acos(AngelLines)*180.0 / PI);

			if (fabs(AngelLines - InputPara.m_AngleStandard) > InputPara.m_AngleDis)
			{
				resultNum++;
			}

			IMG_RCOORD MidPoint1 = { (param.x1 + param.x2) / 2.0,(param.y1 + param.y2) / 2.0 };
			IMG_RCOORD MidPoint2 = { (param.x3 + param.x4) / 2.0,(param.y3 + param.y4) / 2.0 };

			float DisLine = sqrt((MidPoint1.x - MidPoint2.x)*(MidPoint1.x - MidPoint2.x) + (MidPoint1.y - MidPoint2.y)*(MidPoint1.y - MidPoint2.y));


			if (fabs(DisLine - InputPara.m_DisStandard) > InputPara.m_DisDis)
			{
				resultNum++;
			}

			if (resultNum == 2)
			{
				result = 1;
			}
			else
			{
				result = 2;
			}
		}
		else
		{
			int resultNum = 0;

			float moduleA = sqrt((param.x1 - param.x2)*(param.x1 - param.x2) + (param.y1 - param.y2)*(param.y1 - param.y2));
			float moduleB = sqrt((param.x1 - param.x4)*(param.x1 - param.x4) + (param.y1 - param.y4)*(param.y1 - param.y4));

			float AngelLines = ((param.x1 - param.x2)*(param.x1 - param.x4) + (param.y1 - param.y2)*(param.y1 - param.y4)) / (moduleA*moduleB);
			AngelLines = (float)((float)acos(AngelLines)*180.0 / PI);

			if (fabs(AngelLines - InputPara.m_AngleStandard) > InputPara.m_AngleDis)
			{
				resultNum++;
			}

			IMG_RCOORD MidPoint1 = { (param.x1 + param.x2) / 2.0,(param.y1 + param.y2) / 2.0 };
			IMG_RCOORD MidPoint2 = { (param.x3 + param.x4) / 2.0,(param.y3 + param.y4) / 2.0 };

			float DisLine = sqrt((MidPoint1.x - MidPoint2.x)*(MidPoint1.x - MidPoint2.x) + (MidPoint1.y - MidPoint2.y)*(MidPoint1.y - MidPoint2.y));

			if (fabs(DisLine - InputPara.m_DisStandard) > InputPara.m_DisDis)
			{
				resultNum++;
			}

			if (resultNum == 2)
			{
				result = 1;
			}
			else
			{
				result = 2;
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
// GetSocketStatus, 功能说明:判断Socket是否为空
// Input:
// 	const IMG_UBBUF srcRoi, 输入图片
//	const int &angleFlg,输入角度
//     ...
// Output:
//     ...
// Return:
//     0 - 正常
//     _CODE_OK - Socket为有
//     _CODE_PROERROR - Socket为有异物
//     _CODE_EMPTY - Socket为空
//	   _CODE_FILE_LOST - 文件丢失
//	   _CODE_ANGLE_FAIL - 输入的角度标识既不是0度也不是180度
//	   _CODE_LOCATE_FAIL - 定位失败
// Author: Huang Yige/05/02/2017
/**********************************************/
_BACK_CODE  CBmSocketStatus::GetSocketStatus(const IMG_UBBUF srcRoi, const int &angleFlg, int &result)
{
	__try
	{
		//LoadPackagedLibrary("SocketStatus.txt");
		char configurationfile1[100];

		sprintf_s(configurationfile1, ".\\configurationfile.ini");

		//int result = 0;

		_BACK_CODE test= _CODE_OK;

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

		if (angleFlg == 180)
		{
			for (int i = 0; i < Roi.height; i++)
			{
				for (int j = 0; j < Roi.width; j++)
				{
					src1[i*Roi.width + j] = src[(Roi.height - i - 1)*Roi.width + Roi.width - j] - 1;
				}
			}
			memcpy(src, src1, Roi.width*Roi.height * sizeof(Ipp8u));
		}

		memcpy(dst, src, Roi.width*Roi.height * sizeof(Ipp8u));

		VisSS_Binaryzation(src, Roi);

		//VisSS_OpenClose(src, Roi, 0);

		standardPoint twoPoint;
		twoPoint.leftPoint.max_x = 0;
		twoPoint.leftPoint.min_x = 0;
		twoPoint.leftPoint.max_y = 0;
		twoPoint.leftPoint.min_y = 0;
		twoPoint.leftPoint.CenterX = 0;
		twoPoint.leftPoint.CenterY = 0;
		twoPoint.rightPoint.max_x = 0;
		twoPoint.rightPoint.min_x = 0;
		twoPoint.rightPoint.max_y = 0;
		twoPoint.rightPoint.min_y = 0;
		twoPoint.rightPoint.CenterX = 0;
		twoPoint.rightPoint.CenterY = 0;

		int value = 0;

		value = VisSS_Screen(src, Roi, twoPoint);

		if (value != 0)
		{
			free(src);
			free(dst);
			free(src1);
			return _CODE_LOCATE_FAIL;
		}

		if (twoPoint.leftPoint.min_x < 20)
		{
			free(src);
			free(dst);
			free(src1);
			return _CODE_LOCATE_FAIL;
		}

		IMG_WINDOW JudgeRoi;

		int RoiX = twoPoint.leftPoint.CenterX - InputPara.m_RoiX;
		int RoiY = twoPoint.leftPoint.CenterY + InputPara.m_RoiY;

		if (RoiX < 0)
		{
			JudgeRoi.coWindowOff.x = 0;
		}
		else
		{
			JudgeRoi.coWindowOff.x = (int)RoiX;
		}
		JudgeRoi.coWindowOff.y = (int)RoiY;
		if (RoiX + InputPara.m_RoiW > Roi.width)
		{
			JudgeRoi.szWindowSize.width = Roi.width - JudgeRoi.coWindowOff.x;
		}
		else
		{
			JudgeRoi.szWindowSize.width = (IMG_UWORD)InputPara.m_RoiW;
		}
		if (RoiY + InputPara.m_RoiH > Roi.height)
		{
			JudgeRoi.szWindowSize.height = Roi.height - JudgeRoi.coWindowOff.y;
		}
		else
		{
			JudgeRoi.szWindowSize.height = (IMG_UWORD)InputPara.m_RoiH;
		}

		Ipp8u* dstRoi;
		dstRoi = (Ipp8u*)malloc(JudgeRoi.szWindowSize.width*JudgeRoi.szWindowSize.height * sizeof(Ipp8u));

		for (int i = 0; i < JudgeRoi.szWindowSize.height; i++)
		{
			for (int j = 0; j < JudgeRoi.szWindowSize.width; j++)
			{
				dstRoi[i*JudgeRoi.szWindowSize.width + j] = dst[(i + JudgeRoi.coWindowOff.y)*Roi.width + (j + JudgeRoi.coWindowOff.x)];
			}
		}

		Ipp8u* dstRoi1;
		dstRoi1 = (Ipp8u*)malloc(JudgeRoi.szWindowSize.width*JudgeRoi.szWindowSize.height * sizeof(Ipp8u));

		memcpy(dstRoi1, dstRoi, JudgeRoi.szWindowSize.width*JudgeRoi.szWindowSize.height * sizeof(Ipp8u));

		IppiSize DstRoi = { JudgeRoi.szWindowSize.width , JudgeRoi.szWindowSize.height };
	
#ifdef DEBUG_BmSocketStatus
	FILE *hp8;
	hp8 = fopen("SS_JudgeRoi.txt", "w");
	for (int i = 0; i < JudgeRoi.szWindowSize.height; i++)
	{
		for (int j = 0; j < JudgeRoi.szWindowSize.width; j++)
		{
			fprintf(hp8, "%d ", dstRoi[i*JudgeRoi.szWindowSize.width + j]);
		}
		fprintf(hp8, "\n");
	}
	fclose(hp8);
	
#endif // DEBUG_BmSocketStatus

		VisSS_GetBlobThreshold(dstRoi, DstRoi);

		VisSS_OpenClose(dstRoi, DstRoi, InputPara.m_OpenCloseflag[1]);
		//VisSS_Erode(dstRoi, DstRoi);

		VisSS_EmptyJudge(dstRoi, DstRoi, result);

		//判断是否为空
		if (result == 0)
		{
			test = _CODE_EMPTY;
			free(dstRoi);
			free(dst);
			free(src);
			free(src1);
			free(dstRoi1); 
			return test;
		}

		//判断是否为异物或者有料
		if (result == 1)
		{
			test = _CODE_OK;
		
			DUTOutPutParam param;
			param.angle = 0.0;
			param.cx = 0.0;
			param.cy = 0.0;
			param.dist = 0.0;
			param.x1 = 0.0;
			param.x2 = 0.0;
			param.x3 = 0.0;
			param.x4 = 0.0;
			param.y1 = 0.0;
			param.y2 = 0.0;
			param.y3 = 0.0;
			param.y4 = 0.0;
			test = _CODE_PROERROR;


			bool boolreturn1 = m_p.LoadParameter(configurationfile1);

			//double t = (double)cvGetTickCount();


			test= m_p.DUTLocation(srcRoi, angleFlg, param);

			/*t = (double)cvGetTickCount() - t;
			printf("exec time = %gms   \n", t / (cvGetTickFrequency() * 1000));*/


			if (test != _CODE_OK)
			{
				test = _CODE_PROERROR;
				result = 1;
			}
			else
			{
				Roi = { srcRoi.size.width,srcRoi.size.height };
				Vis_ForeignJudge(srcRoi.ptr, Roi, result, param, angleFlg);

				if (result == 1)
				{
					test = _CODE_PROERROR;
					result = 2;
				}
				else
				{
					VisSS_LocationJudge(param, twoPoint, angleFlg, result);
					if (result == 1)
					{
						test = _CODE_PROERROR;
						result = 3;
					}
					else
					{
						VisSS_AngleJudge(param, angleFlg, result);
						if (result == 1)
						{
							test = _CODE_PROERROR;
							result = 4;
						}
						else
						{
							test = _CODE_OK;
						}
					}
				
				}
			
			}

			free(dstRoi);
			free(dst);
			free(src);
			free(src1);
			free(dstRoi1);
			return test;
		}

		free(dstRoi);
		free(dst);
		free(src);
		free(src1);
		free(dstRoi1);
		return test;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

bool CBmSocketStatus::LoadParameter(char path[])
{
	LPSTR temp = (LPSTR)new char[MAX_PATH];

	GetPrivateProfileStringA("BmSocketStatus", "threshold1 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_threshold[0] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "threshold2 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_threshold[1] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "threshold3 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_threshold[2] = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "CircleRoi1x ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi1.coWindowOff.x = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "CircleRoi1y ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi1.coWindowOff.y = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "CircleRoi1width ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi1.szWindowSize.width = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "CircleRoi1height ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi1.szWindowSize.height = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "CircleRoi2x ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi2.coWindowOff.x = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "CircleRoi2y ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi2.coWindowOff.y = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "CircleRoi2width ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi2.szWindowSize.width = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "CircleRoi2height ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_CircleRoi2.szWindowSize.height = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "ScreenAreaMin ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_ScreenAreaMin = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "OpenCloseflag1 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_OpenCloseflag[0] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "OpenCloseflag2 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_OpenCloseflag[1] = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "featureCircleRadius ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_featureCircleRadius = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "errorX ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_edgeError.x = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "errorY ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_edgeError.y = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "featureCircleDisX ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_featureCircleDis.x = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "featureCircleDisY ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_featureCircleDis.y = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "errorRoate", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "0"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_errorRoate = (double)atof((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "errorDis", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "0"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_errorDis = (double)atof((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeRoiX ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeRoi.coWindowOff.x = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeRoiY ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeRoi.coWindowOff.y = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeRoiWidth ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeRoi.szWindowSize.width = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeRoiHeight ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeRoi.szWindowSize.height = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeColumn1 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeColumn[0] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeColumn2 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeColumn[1] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeColumn3 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeColumn[2] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeColumn4 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeColumn[3] = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeRow1 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeRow[0] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeRow2 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeRow[1] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeRow3 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeRow[2] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeRow4 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeRow[3] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeRow5 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeRow[4] = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeRow6 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeRow[5] = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "EmptyJudgeCircle ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyJudgeCircle = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "AreaMinIncomplete ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_AreaMinIncomplete = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "AreaMinOriginal ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_AreaMinOriginal = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "AreaMidOriginal ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_AreaMidOriginal = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "AreaMaxOriginal ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_AreaMaxOriginal = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "ForJuRoiStaWidth ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_ForeignJudgeRoiStandard.width = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "ForJuRoiStaHeight ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_ForeignJudgeRoiStandard.height = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "ForJuStaPointValue ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_ForeignJudgeStaPointValue = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "ForJuRoiPointValue ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_ForeignJudgeRoiPointValue = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "ForAreapMean", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "0"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_ForeignAreapMean = (double)atof((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "LocationJudgeRowLeft ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_LocationJudgeRowLeft = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "LocationJudgeRowRight ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_LocationJudgeRowRight = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "LocationJudgeColumnUp ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_LocationJudgeColumnUp = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "m_RoiX ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_RoiX = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "m_RoiY ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_RoiY = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "m_RoiW ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_RoiW = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketStatus", "m_RoiH ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_RoiH = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "ForAreapMeanAb ", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "0"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_ForeignAreapMeanAbnormal = (double)atof((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "EmptyBlobWhiteRatio ", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "0"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyBlobWhiteRatio = (double)atof((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "EmptyBlobWhiteLine ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_EmptyBlobWhiteLine = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "AngleStandard ", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "0"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_AngleStandard = (double)atof((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "AngleDis ", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "0"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_AngleDis = (double)atof((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "DisStandard ", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "0"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_DisStandard = (double)atof((char*)temp);

	GetPrivateProfileStringA("BmSocketStatus", "DisDis ", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "0"))
	{
		delete[] temp;
		return 0;
	}
	InputPara.m_DisDis = (double)atof((char*)temp);

	delete[] temp;
	return TRUE;
}