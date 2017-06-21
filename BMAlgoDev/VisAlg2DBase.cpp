#include "stdafx.h"
#include "VisAlg2DBase.h"
#include <math.h>
#include <stdexcept>
#include "BmErrorCode.h"

#define fitting_sigma 10
#define PI 3.14159265386

using namespace std;

CVisAlg2DBase::CVisAlg2DBase()
{
}


CVisAlg2DBase::~CVisAlg2DBase()
{
}

int CVisAlg2DBase::VisPyramid(IMG_UBBUF src, unsigned char * pDst, int & pyramid_width, int & pyramid_height, int level)
{
	__try
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
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/******************************  Filter  *****************************************/

int CVisAlg2DBase::VisFilterMean(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst, const unsigned char kernelSize)
{
	__try
	{
		unsigned char *src2;
		src2 = (Ipp8u*)malloc(kernelSize * kernelSize * sizeof(Ipp8u));
		for (int i = 0; i < kernelSize*kernelSize; i++)
		{
			src2[i] = 1;
		}


		IppStatus status = ippStsNoErr;
		IppiSize src1Size = { srcWidth ,srcHeight };
		int src1Step = srcWidth * sizeof(Ipp8u);

		IppiSize src2Size = { kernelSize,kernelSize };
		int src2Step = kernelSize * sizeof(Ipp8u);
		//IppEnum algType = (IppEnum)(ippAlgAuto | ippiROIFull | ippiNormNone);
		IppEnum algType = (IppEnum)(ippAlgAuto | ippiROIValid | ippiNormNone);
		int buffSize = 0;
		status = ippiConvGetBufferSize(src1Size, src2Size, ipp8u, 1, algType, &buffSize);
		if (status != ippStsNoErr)
		{
			free(src2);
			return -1;
		}


		//int divisor = 15;
		Ipp8u *buffer;
		buffer = (Ipp8u*)ippsMalloc_8u(buffSize);

		//unsigned char *dstt = (unsigned char*)malloc((srcWidth + kernelSize - 1)*(srcHeight + kernelSize - 1)*sizeof(unsigned char));
		//int dstStep = (srcWidth + kernelSize - 1)*sizeof(Ipp8u);
		unsigned char *dstt = (unsigned char*)malloc((srcWidth - kernelSize + 1)*(srcHeight - kernelSize + 1) * sizeof(unsigned char));
		int dstStep = (srcWidth - kernelSize + 1) * sizeof(Ipp8u);

		status = ippiConv_8u_C1R(src, src1Step, src1Size, src2, src2Step, src2Size, dstt, dstStep, kernelSize*kernelSize, algType, buffer);

		free(src2);
		ippsFree(buffer);
		if (status != ippStsNoErr)
		{
			free(dstt);
			return -1;
		}

		//for (int i = 0; i < srcHeight; i++)
		//{
		//	for (int j = 0; j < srcWidth; j++)
		//	{
		//		dst[j + i*srcWidth] = src[j + i*srcWidth];
		//	}
		//}
		memcpy(dst, src, srcWidth*srcHeight);

		int hs = kernelSize / 2;
		unsigned int pos1 = hs*srcWidth;
		unsigned int pos2 = 0;
		int w1 = srcWidth - kernelSize + 1;
		for (int i = hs; i < srcHeight - hs; i++)
		{
			memcpy(&dst[pos1 + hs], &dstt[pos2], w1);
			//for (int j = hs; j < srcWidth - hs; j++)
			//{
			//	dst[j + pos1] = dstt[(j - hs) + pos2];
			//}
			pos1 += srcWidth;
			pos2 += w1;
		}

		free(dstt);
		return 1;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

int CVisAlg2DBase::VisFilterGaussian(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst, const unsigned char winWidth)
{
	__try
	{
		IppiSize roiSize = { srcWidth,srcHeight };
		Ipp32u kernelSize = winWidth; //must be odd and greater or equal to 3.
		int tmpBufSize = 0, specSize = 0;
		Ipp32f sigma = float(winWidth);
		IppiBorderType borderType = ippBorderRepl;

		ippcvFilterGaussianSpec *spec = NULL;
		Ipp8u *buffer;

		//1、get buffer size
		IppStatus status1 = ippiFilterGaussianGetBufferSize(roiSize, kernelSize, ipp8u, 1, &specSize, &tmpBufSize);

		if (status1 != ippStsNoErr)
		{
			return -1;
		}

		//2、init
		spec = (IppFilterGaussianSpec*)ippsMalloc_8u(specSize);
		buffer = ippsMalloc_8u(tmpBufSize);
		IppStatus status2 = ippiFilterGaussianInit(roiSize, kernelSize, sigma, borderType, ipp8u, 1, spec, buffer);

		if (status2 != ippStsNoErr)
		{
			if (buffer != NULL)
			{
				ippsFree(buffer);
			}
			if (spec != NULL)
			{
				ippsFree(spec);
			}
			return -1;
		}

		//3、filter
		int srcStep = srcWidth * sizeof(unsigned char);
		Ipp8u borderValue = 0;
		IppStatus status3 = ippiFilterGaussianBorder_8u_C1R(src, srcStep, dst, srcStep, roiSize, borderValue, spec, buffer);


		if (status3 != ippStsNoErr)
		{
			if (buffer != NULL)
			{
				ippsFree(buffer);
			}
			if (spec != NULL)
			{
				ippsFree(spec);
			}
			return -1;
		}


		if (buffer != NULL)
		{
			ippsFree(buffer);
		}
		if (spec != NULL)
		{
			ippsFree(spec);
		}
		return 1;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

int CVisAlg2DBase::VisFilterMedian(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst, const unsigned char winWidth)
{
	__try
	{
		IppStatus status;

		int srcStep = srcWidth * sizeof(unsigned char);

		IppiSize dstSize = { srcWidth - 1,srcHeight - 1 };
		IppiSize maskSize = { winWidth,winWidth };
		IppDataType dataType = ipp8u;
		IppiBorderType borderType = ippBorderRepl;
		Ipp8u borderValue = NULL;

		int buffSize = 0;
		Ipp8u *buffer = NULL;

		status = ippiFilterMedianBorderGetBufferSize(dstSize, maskSize, dataType, 1, &buffSize);
		if (status != ippStsNoErr) return -1;

		buffer = ippsMalloc_8u(buffSize);
		status = ippiFilterMedianBorder_8u_C1R(src + srcStep, srcStep, dst, srcStep, dstSize, maskSize, borderType, borderValue, buffer);

		ippsFree(buffer);
		if (status != ippStsNoErr) return -1;

		return 1;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

int CVisAlg2DBase::VisFilterSobel_8u8u(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst, const unsigned char winWidth)
{
	__try
	{
		IppiMaskSize mask;
		if (3 == winWidth)
			mask = ippMskSize3x3;
		else if (5 == winWidth)
			mask = ippMskSize5x5;
		else if (7 == winWidth)
			mask = (IppiMaskSize)77;
		else
			return -1;

		IMG_UBYTE * pSrc = (IMG_UBYTE *)src;
		IppiSize roiSize;
		roiSize.width = srcWidth;
		roiSize.height = srcHeight;
		int bufLen = roiSize.width * roiSize.height;
		Ipp16s *pDst = new Ipp16s[bufLen];

		IppiBorderType bordertype = ippBorderRepl; //Border is replicated from the edge pixels
		Ipp16s *pHoriz, *pVert;

		int srcStep = roiSize.width * sizeof(Ipp8u);

		int dstStep = roiSize.width * sizeof(Ipp16s);
		int angleStep = roiSize.width * sizeof(Ipp32f);
		int bufferSize;

		Ipp8u *pBuffer;
		IppNormType normType = ippNormL2;//input gradient magnitude

		pVert = new Ipp16s[bufLen];
		pHoriz = new Ipp16s[bufLen];
		Ipp32f *pAngle = new Ipp32f[bufLen];
		//	pDst = new Ipp16s[bufLen];

		ippiGradientVectorGetBufferSize(roiSize, mask, ipp16s, 1, &bufferSize);
		pBuffer = new Ipp8u[bufferSize];
		ippiGradientVectorSobel_8u16s_C1R(pSrc, srcStep, pVert, dstStep, pHoriz, dstStep, pDst, dstStep, pAngle, angleStep, roiSize, mask, normType, bordertype, NULL, pBuffer);

		for (int i = 0; i < bufLen; i++)
		{
			//pDst[i] /= winWidth;
			if (pDst[i] > 255 || pDst[i] < -255)
				dst[i] = 255;
			else
				dst[i] = abs(pDst[i]);
		}

		delete[] pBuffer;
		delete[] pVert;
		delete[] pHoriz;
		delete[] pAngle;
		delete[] pDst;

		return 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


/**********************************************  Fitting  *******************************************/

/**********************************************/
//VisFittingCircular,拟合圆
// Input:
//    *point_pos,输入点坐标，默认第一个为起点，最后一个为终止点
//     m，输入点对的个数
// Output:
//    circular_fit, 圆的标准方程参数a,b,c
// Return:
//     0 - 正常
//     1 - 点数少于3
//     2 - 无法拟合成圆
//     ...
// Author: 申健成/20170227
/**********************************************/
int CVisAlg2DBase::VisFittingCircular(const float *point_pos, const int m, StructCircle &circular_fit, const int iteration_times)
{
	__try
	{
		int status = 0;
		if (m < 3)
		{
			//printf("point number <3,can't fitting circular\n");
			return 1;
		}
		int tempxnum = 0, tempynum = 0;
		for (int i = 0; i < m * 2; i = i + 2)
		{
			if (point_pos[0] == point_pos[i])
			{
				tempxnum++;
			}
			if (point_pos[1] == point_pos[i + 1])
			{
				tempynum++;
			}
		}
		if (tempxnum == m || tempynum == m)
		{
			//printf("It is a line,can't fitting circular\n");
			return 2;
		}

		float lastsum = 0, nowsum = 0;
		float *A;
		A = (float *)malloc(3 * sizeof(float));
		A[0] = A[1] = A[2] = 0;
		float *B, *C, *recordB, *recordC;
		B = (float *)malloc(3 * m * sizeof(float));
		C = (float *)malloc(m * sizeof(float));
		recordB = (float *)malloc(3 * m * sizeof(float));
		recordC = (float *)malloc(m * sizeof(float));
		float *error;
		float *error_weight;
		error_weight = (float *)malloc(m * sizeof(float));
		error = (float *)malloc(m * sizeof(float));
		for (int i = 0; i < m; i++)
		{
			error_weight[i] = 1.0;
			error[i] = 0;
		}
		int temp_time = 0;
		do {
			lastsum = nowsum;
			nowsum = 0;

			for (int i = 0, j = 0, k = 0; i < 3 * m; i = i + 3, j = j + 2, k++)
			{
				recordB[i] = B[i] = -2 * point_pos[j] * error_weight[k];
				recordB[i + 1] = B[i + 1] = -2 * point_pos[j + 1] * error_weight[k];
				recordB[i + 2] = B[i + 2] = 1 * error_weight[k];
				recordC[k] = C[k] = -(point_pos[j] * point_pos[j] + point_pos[j + 1] * point_pos[j + 1])* error_weight[k];
			}

			int info = LAPACKE_sgels(LAPACK_ROW_MAJOR, 'N', m, 3, 1, B, 3, C, 1);
			if (info != 0)
			{
				status = info;
				goto end;
			}
			A[0] = C[0];
			A[1] = C[1];
			A[2] = C[2];
			cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, 1, 3, 1, recordB, 3, A, 1, -1, recordC, 1);

			for (int i = 0; i < (m); i++)
			{
				error[i] = recordC[i] * recordC[i];
				nowsum = error[i] + nowsum;
			}
			for (int i = 0; i < m; i++)
			{
				error_weight[i] = float(1.0f / (1 + exp((error[i]) / (fitting_sigma*fitting_sigma))));
			}
			temp_time++;
		} while (fabs(nowsum - lastsum) > 0.0002 && temp_time < iteration_times);

		if (nowsum >= 1000000)
		{
			//printf("Too big error\n");
			goto end;
			return 3;
		}
		circular_fit.CirCen.x = C[0];
		circular_fit.CirCen.y = C[1];
		circular_fit.Radius = (float)sqrt(C[0] * C[0] + C[1] * C[1] - C[2]);

	end:
		if (error_weight != nullptr)
			free(error_weight);
		if (A != nullptr)
			free(A);
		if (B != nullptr)
			free(B);
		if (C != nullptr)
			free(C);
		if (recordB != nullptr)
			free(recordB);
		if (recordC != nullptr)
			free(recordC);
		if (error != nullptr)
			free(error);
		return status;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

int CVisAlg2DBase::VisFittingArc(const float *point_pos, const int m, StructArc &arc_fit, const int iteration_times)
{
	__try
	{
		int status = 0;
		if (m < 3)
		{
			//printf("point number <3,can't fitting circular\n");
			return 1;
		}
		int tempxnum = 0, tempynum = 0;
		for (int i = 0; i < m * 2; i = i + 2)
		{
			if (point_pos[0] == point_pos[i])
			{
				tempxnum++;
			}
			if (point_pos[1] == point_pos[i + 1])
			{
				tempynum++;
			}
		}
		if (tempxnum == m || tempynum == m)
		{
			//printf("It is a line,can't fitting circular\n");
			return 2;
		}

		float lastsum = 0, nowsum = 0;
		float *A;
		A = (float *)malloc(3 * sizeof(float));
		A[0] = A[1] = A[2] = 0;
		float *B, *C, *recordB, *recordC;
		B = (float *)malloc(3 * m * sizeof(float));
		C = (float *)malloc(m * sizeof(float));
		recordB = (float *)malloc(3 * m * sizeof(float));
		recordC = (float *)malloc(m * sizeof(float));
		float *error;
		float *error_weight;
		error_weight = (float *)malloc(m * sizeof(float));
		error = (float *)malloc(m * sizeof(float));
		for (int i = 0; i < m; i++)
		{
			error_weight[i] = 1.0;
			error[i] = 0;
		}
		int temp_time = 0;
		do {
			lastsum = nowsum;
			nowsum = 0;

			for (int i = 0, j = 0, k = 0; i < 3 * m; i = i + 3, j = j + 2, k++)
			{
				recordB[i] = B[i] = -2 * point_pos[j] * error_weight[k];
				recordB[i + 1] = B[i + 1] = -2 * point_pos[j + 1] * error_weight[k];
				recordB[i + 2] = B[i + 2] = 1 * error_weight[k];
				recordC[k] = C[k] = -(point_pos[j] * point_pos[j] + point_pos[j + 1] * point_pos[j + 1])* error_weight[k];
			}

			int info = LAPACKE_sgels(LAPACK_ROW_MAJOR, 'N', m, 3, 1, B, 3, C, 1);
			if (info != 0)
			{
				status = info;
				goto end;
			}
			A[0] = C[0];
			A[1] = C[1];
			A[2] = C[2];
			cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, 1, 3, 1, recordB, 3, A, 1, -1, recordC, 1);

			for (int i = 0; i < (m); i++)
			{
				error[i] = recordC[i] * recordC[i];
				nowsum = error[i] + nowsum;
			}
			for (int i = 0; i < m; i++)
			{
				error_weight[i] = float(1.0f / (1 + exp((error[i]) / (fitting_sigma*fitting_sigma))));
			}
			temp_time++;
		} while (fabs(nowsum - lastsum) > 0.0002 && temp_time < iteration_times);

		if (nowsum >= 1000000)
		{
			//printf("Too big error\n");
			return 3;
		}
		arc_fit.CirCen.x = C[0];
		arc_fit.CirCen.y = C[1];
		arc_fit.Radius = (float)sqrt(C[0] * C[0] + C[1] * C[1] - C[2]);
		float r = arc_fit.Radius;

		// 计算扫描角度
		float cos_central_angle = (float)(float(2 * r * r) -
			fabs(point_pos[0] - point_pos[2 * m - 2]) * fabs(point_pos[0] - point_pos[2 * m - 2]) -
			fabs(point_pos[1] - point_pos[2 * m - 1]) * fabs(point_pos[1] - point_pos[2 * m - 1])) / float(2 * r * r);

		//优劣弧判断  
		float d1 = (point_pos[0] - point_pos[2 * m - 2])*(point_pos[0] - point_pos[2 * m - 2]) +
			(point_pos[1] - point_pos[2 * m - 1])*(point_pos[1] - point_pos[2 * m - 1]);
		float d2 = (point_pos[0] - point_pos[m - 1])*(point_pos[0] - point_pos[m - 1]) +
			(point_pos[1] - point_pos[m])*(point_pos[1] - point_pos[m]);
		float d3 = (point_pos[2 * m - 2] - point_pos[m - 1])*(point_pos[2 * m - 2] - point_pos[m - 1]) +
			(point_pos[2 * m - 1] - point_pos[m])*(point_pos[2 * m - 1] - point_pos[m]);

		float costh = (float)((d2 + d3 - d1) / (2 * sqrt(d2)*sqrt(d3)));

		if (costh > 0)
		{
			costh = (float)(costh * 180 / PI);
		}
		else
		{
			costh = (float)(180 + costh * 180 / PI);
		}

		int flag = 3;//完整的用这个
		if (costh > 90)
		{
			flag = 0;//劣弧
		}
		else if (costh == 90)
		{
			flag = 2;//半弧
		}
		else
		{
			flag = 1;
		}
		//输出

		if (cos_central_angle > 0)
		{
			cos_central_angle = (float)(cos_central_angle * 180 / PI);
		}
		else
		{
			cos_central_angle = (float)(180 + cos_central_angle * 180 / PI);
		}

		if (flag == 3)//完整弧
		{
			arc_fit.sweepAngle = 360;
		}
		else if (flag == 2)//半弧
		{
			arc_fit.sweepAngle = 180;
		}
		else if (flag == 0)//劣弧
		{
			arc_fit.sweepAngle = cos_central_angle;
		}
		else if (flag == 1)//优弧
		{
			arc_fit.sweepAngle = cos_central_angle + 180;
		}

		// 计算起始角度
		float angle_th = 0;
		float record_x = point_pos[0] - arc_fit.CirCen.x;
		float record_y = point_pos[1] - arc_fit.CirCen.y;
		if (record_y >= 0)
		{
			angle_th = (float)(atan2(record_y, record_x));// * 180 / PI;
		}
		else
		{
			angle_th = (float)(360 + atan2(record_y, record_x));// * 180 / PI;//逆时针
		}
		arc_fit.startAngle = angle_th;

	end:
		if (error_weight != nullptr)
			free(error_weight);
		if (A != nullptr)
			free(A);
		if (B != nullptr)
			free(B);
		if (C != nullptr)
			free(C);
		if (recordB != nullptr)
			free(recordB);
		if (recordC != nullptr)
			free(recordC);
		if (error != nullptr)
			free(error);
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
//		pSrc ， 输入图像指针
//		nW,nH	图像宽高
//		nT	分割阈值
// Output:
//		pDst ， 输出图像指针
// Return:
//		ippstatus
// Liu Ping, 2017.05.06
/**********************************************/
int CVisAlg2DBase::VisSegmentManual(const unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char * pDst, const unsigned char nT)
{
	__try
	{
		IppiSize imgSize;
		imgSize.width = nW;
		imgSize.height = nH;
		int status = ippStsNoErr;
		int width = imgSize.width;
		int height = imgSize.height;

		//	do threshold	//
		int srcStep = width * sizeof(unsigned char);
		int dstStep = srcStep;
		status = ippiThreshold_LTValGTVal_8u_C1R(pSrc, srcStep, pDst, dstStep, imgSize, nT, 0, nT, 255);

		return status;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// threshold_OTSU, 自动阈值分割
// Input:
//		pSrc ， 输入图像指针
//		nW,nH	图像宽高
// Output:
//		pDst ， 输出图像指针
// Return:
//		ippstatus
// Liu Ping, 2017.05.07
/**********************************************/
int CVisAlg2DBase::VisSegmentOtsu(const unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char * pDst)
{
	__try
	{
		IppiSize imgSize;
		imgSize.width = nW;
		imgSize.height = nH;
		int status = ippStsNoErr;
		int width = imgSize.width;
		int height = imgSize.height;

		//	calc threshold	//
		int srcStep = width * sizeof(unsigned char);
		unsigned char nThreshold = 0;
		status = ippiComputeThreshold_Otsu_8u_C1R(pSrc, srcStep, imgSize, &nThreshold);
		if (status != 0)
			throw 1;

		//	do segment	//
		int dstStep = srcStep;
		status = ippiThreshold_LTValGTVal_8u_C1R(pSrc, srcStep, pDst, dstStep, imgSize, nThreshold, 0, nThreshold, 255);

		return status;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// threshold_OTSU, 自动阈值计算，不分割
// Input:
//		pSrc ， 输入图像指针
//		nW,nH	图像宽高
// Output:
//		pDst ， 输出图像指针
// Return:
//		ippstatus
// Liu Ping, 2017.05.07
/**********************************************/
int CVisAlg2DBase::VisCalcOtsu(const unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char & nThres)
{
	__try
	{
		IppiSize imgSize;
		imgSize.width = nW;
		imgSize.height = nH;
		int status = ippStsNoErr;
		int width = imgSize.width;
		int height = imgSize.height;

		//	calc threshold	//
		int srcStep = width * sizeof(unsigned char);
		status = ippiComputeThreshold_Otsu_8u_C1R(pSrc, srcStep, imgSize, &nThres);
		if (status != 0)
			throw 1;

		return status;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// threshold_Dynamic, 动态阈值分割
// Input:
//		pSrc ， 输入图像指针
//		nW,nH	图像宽高
// Output:
//		pDst ， 输出图像指针
// Return:
//		ippstatus
// Liu Ping, 2017.05.08
/**********************************************/
int CVisAlg2DBase::VisSegmentDynamic(const unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char * pDst, const unsigned int avgWinWidth)
{
	__try
	{
		IppiSize imgSize;
		imgSize.width = nW;
		imgSize.height = nH;
		int status = ippStsNoErr;
		int width = imgSize.width;
		int height = imgSize.height;
		int nS = width*height;

		// calc average image
		unsigned char * pThre = new unsigned char[nS];

		VisFilterMedian(pSrc, height, width, pThre, avgWinWidth);

		// do segment
		for (int j = 0; j < nS; j++)
		{
			if ((pSrc[j]) <= pThre[j])
			{
				pDst[j] = 0;
			}
			else
			{
				pDst[j] = 255;
			}
		}

		delete[] pThre;
		return status;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// threshold_CountDots, 数点阈值分割
// Input:
//		pSrc ， 输入图像指针
//		nW,nH	图像宽高
// Output:
//		pDst ， 输出图像指针
// Return:
//		ippstatus
// Liu Ping, 2017.05.09
/**********************************************/
int CVisAlg2DBase::VisSegmentCntDots(unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char * pDst, const double fWhiteRatio)
{
	__try
	{
		int nBins = 255;

		int status = ippStsNoErr;
		unsigned char nThreshold = 0;

		//	calc threshold	//
		Ipp32u *pHist = new Ipp32u[nBins];
		IMG_UBBUF src;
		src.ptr = pSrc;
		src.size.width = nW;
		src.size.height = nH;
		src.linestep = nW;

		//VisHistogram(src, pHist, nBins);
		//IppStatus status = ippStsNoErr;

		IppDataType dataType = ipp8u;
		IppiSize srcSize = { src.size.width,src.size.height };
		int srcStep = src.size.width * sizeof(unsigned char);

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

		//Ipp32u *pHist = (Ipp32u*)malloc((nBins) * sizeof(Ipp32u));
		Ipp8u *buffer = NULL;
		buffer = (Ipp8u*)malloc(bufferSize * sizeof(Ipp8u));

		status = ippiHistogram_8u_C1R(src.ptr, srcStep, srcSize, pHist, Spec, buffer);

		//free
		free(pLevels);
		free(buffer);
		ippsFree(Spec);

		unsigned int i, nCnt = 0;
		unsigned int nWhite = unsigned int(nW*nH*fWhiteRatio + 0.5);
		for (i = nBins - 1; i > 0; i--)
		{
			nCnt += pHist[i];
			if (nCnt > nWhite)
			{
				nThreshold = i;
				break;
			}
		}
		delete[] pHist;

		//	do segment	//
		//int srcStep = nW * sizeof(unsigned char);
		int dstStep = srcStep;
		IppiSize imgSize;
		imgSize.width = nW;
		imgSize.height = nH;
		status = ippiThreshold_LTValGTVal_8u_C1R(pSrc, srcStep, pDst, dstStep, imgSize, nThreshold, 0, nThreshold, 255);

		return 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// VisHistogram, 统计直方图
// Input:
//		pSrc ， 输入图像指针
//		nW,nH	图像宽高
// Output:
//		pDst ， 输出图像指针
// Return:
//		ippstatus
// Liu Ping, 2017.05.09
/**********************************************/
int CVisAlg2DBase::VisHistogram(IMG_UBBUF src, Ipp32u *pHist, int nBins)
{
	__try
	{
		//1、统计每个灰度值出现的次数
		IppStatus status = ippStsNoErr;

		IppDataType dataType = ipp8u;
		IppiSize srcSize = { src.size.width,src.size.height };
		int srcStep = src.size.width * sizeof(unsigned char);

		//int nBins = 255;
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

		//Ipp32u *pHist = (Ipp32u*)malloc((nBins) * sizeof(Ipp32u));
		Ipp8u *buffer = NULL;
		buffer = (Ipp8u*)malloc(bufferSize * sizeof(Ipp8u));

		status = ippiHistogram_8u_C1R(src.ptr, srcStep, srcSize, pHist, Spec, buffer);

		//free
		free(pLevels);
		free(buffer);
		ippsFree(Spec);

		if (status != ippStsNoErr)
		{
			return -1;
		}

		return 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// VisMorphologyErode, 形态学腐蚀, 窗宽(3 或 5)
// Input:
//		src		输入图像
// Output:
//		dst		输出图像
// Return:
//		ippstatus
// Liu Ping, 2017.05.09
/**********************************************/
int CVisAlg2DBase::VisMorphologyErode(IMG_UBBUF src, IMG_UBBUF dst)
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
		//Ipp8u pMask[5 * 5] = { 1, 1, 1,1,1,1, 1, 1,1,1,
		//	1,1, 0, 1,1,
		//	1, 1, 1,1,1,1, 1, 1,1,1 };
		//IppiSize maskSize = { 5, 5 };

		int srcStep = roiSize.width * sizeof(Ipp8u);
		int dstStep = roiSize.width * sizeof(Ipp8u);

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

		dst.size = src.size;
		dst.linestep = src.linestep;
		status = ippiErodeBorder_8u_C1R(pSrc, srcStep, dst.ptr, dstStep, { roiSize.width,roiSize.height }, borderType, borderValue, pSpec, pBuffer);

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
// VisMorphologyDilation, 形态学膨胀, 窗宽(3 或 5)
// Input:
//		src		输入图像	
// Output:
//		dst		输出图像
// Return:
//		ippstatus
// Liu Ping, 2017.05.09
/**********************************************/
int CVisAlg2DBase::VisMorphologyDilation(IMG_UBBUF src, IMG_UBBUF dst)
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
		//Ipp8u pMask[5 * 5] = { 1, 1, 1,1,1,1, 1, 1,1,1,
		//	1,1, 0, 1,1,
		//	1, 1, 1,1,1,1, 1, 1,1,1 };
		//IppiSize maskSize = { 5, 5 };

		int srcStep = roiSize.width * sizeof(Ipp8u);
		int dstStep = roiSize.width * sizeof(Ipp8u);

		int specSize = 0, bufferSize = 0;
		IppiBorderType borderType = ippBorderRepl;
		Ipp8u borderValue = 0;

		status = ippiMorphologyBorderGetSize_8u_C1R({ roiSize.width,roiSize.height }, maskSize, &specSize, &bufferSize);
		if (status != ippStsNoErr)
			return status;

		pSpec = (IppiMorphState*)ippsMalloc_8u(specSize);
		pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

		status = ippiMorphologyBorderInit_8u_C1R({ roiSize.width,roiSize.height }, pMask, maskSize, pSpec, pBuffer);
		if (status != ippStsNoErr) {
			ippsFree(pBuffer);
			ippsFree(pSpec);
			return status;
		}

		dst.size = src.size;
		dst.linestep = src.linestep;
		status = ippiDilateBorder_8u_C1R(pSrc, srcStep, dst.ptr, dstStep, { roiSize.width,roiSize.height }, borderType, borderValue, pSpec, pBuffer);

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
// VisMorphologyDilation, 形态学open, 窗宽(3 或 5)
// Input:
//		src		输入图像	
// Output:
//		dst		输出图像
// Return:
//		ippstatus
// Liu Ping, 2017.05.09
/**********************************************/
int CVisAlg2DBase::VisMorphologyOpen(IMG_UBBUF src, IMG_UBBUF dst)
{
	__try//try//
	{
		IppStatus status = ippStsNoErr;

		IppiMorphAdvState *pSpec = NULL;
		Ipp8u* pBuffer = NULL;

		Ipp8u pMask[9] = { 1,1,1,1,0,1,1,1,1 };
		IppiSize maskSize = { 3,3 };
		//Ipp8u pMask[5 * 5] = { 1, 1, 1,1,1,1, 1, 1,1,1,
		//	1,1, 0, 1,1,
		//	1, 1, 1,1,1,1, 1, 1,1,1 };
		//IppiSize maskSize = { 5, 5 };

		Ipp8u *srcRoi = src.ptr;
		IppiSize Roi;
		Roi.width = src.size.width;
		Roi.height = src.size.height;

		Ipp8u *dstRoi;
		dstRoi = dst.ptr;

		int srcStep = Roi.width * sizeof(Ipp8u);
		int dstStep = Roi.width * sizeof(Ipp8u);
		int dstSize = Roi.width;

		int specSize = 0, bufferSize = 0;
		IppiBorderType borderType = ippBorderRepl;
		Ipp8u borderValue = 0;

		status = ippiMorphAdvGetSize_8u_C1R(Roi, maskSize, &specSize, &bufferSize);
		if (status != ippStsNoErr)
			return status;

		pSpec = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
		pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

		status = ippiMorphAdvInit_8u_C1R(Roi, pMask, maskSize, pSpec, pBuffer);
		if (status != ippStsNoErr)
		{
			ippsFree(pBuffer);
			ippsFree(pSpec);
			return status;
		}

		status = ippiMorphOpenBorder_8u_C1R(dstRoi, srcStep, srcRoi, dstStep, Roi, borderType, borderValue, pSpec, pBuffer);

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
// VisMorphologyDilation, 形态学close, 窗宽(3 或 5)
// Input:
//		src		输入图像	
// Output:
//		dst		输出图像
// Return:
//		ippstatus
// Liu Ping, 2017.05.09
/**********************************************/
int CVisAlg2DBase::VisMorphologyClose(IMG_UBBUF src, IMG_UBBUF dst)
{
	__try//try//
	{
		IppStatus status = ippStsNoErr;

		IppiMorphAdvState *pSpec = NULL;
		Ipp8u* pBuffer = NULL;

		Ipp8u pMask[9] = { 1,1,1,1,0,1,1,1,1 };
		IppiSize maskSize = { 3,3 };
		//Ipp8u pMask[5 * 5] = { 1, 1, 1,1,1,1, 1, 1,1,1,
		//	1,1, 0, 1,1,
		//	1, 1, 1,1,1,1, 1, 1,1,1 };
		//IppiSize maskSize = { 5, 5 };

		Ipp8u *srcRoi = src.ptr;
		IppiSize Roi;
		Roi.width = src.size.width;
		Roi.height = src.size.height;

		Ipp8u *dstRoi;
		dstRoi = dst.ptr;

		int srcStep = Roi.width * sizeof(Ipp8u);
		int dstStep = Roi.width * sizeof(Ipp8u);
		int dstSize = Roi.width;

		int specSize = 0, bufferSize = 0;
		IppiBorderType borderType = ippBorderRepl;
		Ipp8u borderValue = 0;

		status = ippiMorphAdvGetSize_8u_C1R(Roi, maskSize, &specSize, &bufferSize);
		if (status != ippStsNoErr)
			return status;

		pSpec = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
		pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

		status = ippiMorphAdvInit_8u_C1R(Roi, pMask, maskSize, pSpec, pBuffer);
		if (status != ippStsNoErr)
		{
			ippsFree(pBuffer);
			ippsFree(pSpec);
			return status;
		}

		status = ippiMorphCloseBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec, pBuffer);

		ippsFree(pBuffer);
		ippsFree(pSpec);

		return status;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}