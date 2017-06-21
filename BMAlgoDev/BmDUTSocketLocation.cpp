
#include "stdafx.h"
#include "BmDUTSocketLocation.h"





	//������
	//pyramid����������˵��������һ��ͼ��Ͳ����������߲��ͼ������
	//
	//Input
	//  IMG_UBBUF src   ����ͼƬ
	//  int level  ����������
	//
	//Output
	// unsigned char* pDst  ��߲�ͼ������
	// int &pyramid_width  ��߲�ͼ��Ŀ�
	//int &pyramid_height  ��߲�ͼ��ĸ�
	//
	//Return
	// ippStsNoErr  ����
	//����   �쳣
	//����/����: ����/20170406
IppStatus CBmDUTSocketLocation::pyramid(IMG_UBBUF src,
	//IppiPyramid*  &pPyrStruct,
	//unsigned char **  &pPyrImage, 
	unsigned char* pDst,
	int &pyramid_width,
	int &pyramid_height,
	int level)
{
	_try{
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
		   return ippStsUnknownFeature;
	   }
}

//IppStatus VisPyramids(unsigned char* pSrc, IppiSize roiSize, IppiPyramid*  &pPyrStruct, unsigned char **&pPyrImage, int level)
//{
//	IppStatus   status = ippStsNoErr;
//	if (!pSrc) { status = ippStsNoMemErr; goto exit; }
//	int srcStep = roiSize.width * sizeof(unsigned char);
//	float      rate = 2.f;                  // Neighbour levels ratio
//	signed short kernel[3] = { 1,1,1 };		// Separable symmetric kernel of odd length
//											//IMG_WORD *kernel = (IMG_WORD *)pool.PMalloc(3 * sizeof(IMG_WORD));
//											//__GetGaussianKernel_dim1(kernel, 3, sigma);	// preserved
//
//	int i = 0;
//
//	int pyrBufferSize = 0;
//	int pyrStructSize = 0;
//	unsigned char *pPyrBuffer = NULL;
//	unsigned char *pPyrStrBuffer = NULL;
//
//	int pyrLStateSize = 0;
//	int pyrLBufferSize = 0;
//	unsigned char   *pPyrLStateBuf = NULL;
//	unsigned char   *pPyrLBuffer = NULL;
//
//	// Computes the temporary work buffer size
//	status = ippiPyramidGetSize(&pyrStructSize, &pyrBufferSize, level, roiSize, rate);
//	if (status < 0) goto exit;
//
//	pPyrBuffer = (unsigned char*)malloc(pyrBufferSize * sizeof(unsigned char));
//	pPyrStrBuffer = (unsigned char*)malloc(pyrStructSize * sizeof(unsigned char));	//not pop
//	if ((pyrBufferSize && !pPyrBuffer) || (pyrStructSize && !pPyrStrBuffer)) {
//		status = ippStsNoMemErr; goto exit;
//	}
//
//	// Initializes Gaussian structure for pyramids
//	//pPyrStruct = (IppiPyramid*)malloc(level+1 * sizeof(IppiPyramid));	
//	status = ippiPyramidInit(&pPyrStruct, level, roiSize, rate, pPyrStrBuffer, pPyrBuffer);
//	if (status < 0) goto exit;
//
//	// ????????????????Correct maximum scale level 
//	level = pPyrStruct->level;
//
//	// Allocate structures to calculate pyramid layers
//	status = ippiPyramidLayerDownGetSize_8u_C1R(roiSize, rate, 3, &pyrLStateSize, &pyrLBufferSize);
//	if (status < 0) goto exit;
//
//	pPyrLStateBuf = (unsigned char*)malloc(pyrLStateSize * sizeof(unsigned char));
//	pPyrLBuffer = (unsigned char*)malloc(pyrLBufferSize * sizeof(unsigned char));
//	if ((pyrLStateSize && !pPyrLStateBuf) || (pyrLBufferSize && !pPyrLBuffer)) { status = ippStsNoMemErr; goto exit; }
//
//	// Initialize the structure for creating a lower pyramid layer
//	status = ippiPyramidLayerDownInit_8u_C1R((IppiPyramidDownState_8u_C1R**)&pPyrStruct->pState, roiSize, rate, kernel, 3, IPPI_INTER_LINEAR, pPyrLStateBuf, pPyrLBuffer);
//	if (status < 0) goto exit;
//
//	// Allocate pyramid layers
//	pPyrImage = pPyrStruct->pImage;
//	pPyrImage[0] = pSrc;
//	pPyrStruct->pStep[0] = srcStep;
//
//	for (i = 1; i <= level; i++)
//	{
//		pPyrImage[i] = (unsigned char*)malloc((pPyrStruct->pRoi[i].width) * (pPyrStruct->pRoi[i].height) * sizeof(unsigned char));
//		pPyrStruct->pStep[i] = (pPyrStruct->pRoi[i].width) * sizeof(unsigned char);
//		if (!pPyrImage[i]) { status = ippStsNoMemErr; goto exit; }
//	}
//
//	// Perform downsampling of the image with 5x5 Gaussian kernel
//	for (i = 1; i <= level; i++)
//	{
//		status = ippiPyramidLayerDown_8u_C1R(pPyrImage[i - 1], pPyrStruct->pStep[i - 1], pPyrStruct->pRoi[i - 1], pPyrImage[i], pPyrStruct->pStep[i], pPyrStruct->pRoi[i], (IppiPyramidDownState_8u_C1R*)pPyrStruct->pState);
//		if (status < 0) goto exit;
//
//	}
//
//	free(pPyrBuffer);
//	//pool.PFree(pPyrStrBuffer);
//	free(pPyrLStateBuf);
//	free(pPyrLBuffer);
//
//exit:
//	;
//
//	return status;
//}

//ֱ��ͼ�涨��
//VisHistogSpeciͼ��ֱ��ͼ�涨������˵��������һ��ͼ�͹涨�ľ�ֵ�ͷ������涨����ͼ��
//
//Input
//src  ����ͼ��
//meanValue  �涨�ĻҶȵľ�ֵ
//variance   �涨�ĻҶȵı�׼��
//
//Output
//dst   �ɸ����ľ�ֵ�ͱ�׼��涨����ͼ��
//Return
// 0   ����
//-1   �쳣
//����/����: ����/20170406
int  CBmDUTSocketLocation::VisHistogSpeci(IMG_UBBUF src, float meanValue, float stdDevValue, IMG_UBBUF dst)
{
	_try{
	//0��ͼ����Ϣ�ж�
	if (src.ptr == NULL || src.size.height < 1 || src.size.width < 1 || src.linestep != src.size.width)
	{
		return -1;
	}

	memcpy(dst.ptr, src.ptr, src.size.height*src.size.width * sizeof(unsigned char));

	//1��Compute the mean and stand eviation
	IppStatus status = ippStsNoErr;

	int srcStep = src.size.width * sizeof(unsigned char);
	IppiSize srcSize = { src.size.width,src.size.height };
	Ipp64f pMean = 0;
	Ipp64f pStdDev = 0;
	status = ippiMean_StdDev_8u_C1R(src.ptr, srcStep, srcSize, &pMean, &pStdDev);

	if (status != ippStsNoErr) return -1;

	//2���ı�ƽ��ֵ
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

	//3���ı��׼��
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
			//�ı�ƽ��ֵ
			tem1 = src.ptr[j + pos] + meanSub;
			if (tem1 > 255)
			{
				tem1 = 255;
			}
			if (tem1 < 0)
			{
				tem1 = 0;
			}
			dst.ptr[j + pos] = tem1;

			//�ı��׼��
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
	ippiMean_StdDev_8u_C1R(dst.ptr, srcStep, srcSize, &pMean, &pStdDev);
	//printf("%lf\n", pMean);
	//printf("%lf\n", pStdDev);
	return 0;
	  }
	  __except (EXCEPTION_EXECUTE_HANDLER)
	  {
		  return _CODE_THROW;
	  }
}


//��˹�˲�
//VisGaussianFilter����˵��������ipp��˹�˲�����ippiFilterGaussianBorder_8u_C1R
//
//Input
//unsigned char *src, int srcHeight, int srcWidth   ����ͼ��
//float sigma  Standard deviation of the Gaussian kernel.
//unsigned int kernelSize  �˵ĳߴ磬���ڵ���3������
//
//Output
//unsigned char *dst ���ͼ��������ͼ����һ�¡�
//
//Return
// 1   ����
// -1  �쳣
//����/����: ����/20170406
int  CBmDUTSocketLocation::VisGaussianFilter(unsigned char *src, int srcHeight, int srcWidth, float sigma, unsigned int kernelSize,
	unsigned char *dst)
{
	_try{
	IppiSize roiSize = { srcWidth,srcHeight };
	//Ipp32u kernelSize = 15; //must be odd and greater or equal to 3.
	int tmpBufSize = 0, specSize = 0;
	//Ipp32f sigma = 30.0f;//0.35f;
	IppiBorderType borderType = ippBorderRepl;

	ippcvFilterGaussianSpec *spec = NULL;
	Ipp8u *buffer;

	//1��get buffer size
	IppStatus status1 = ippiFilterGaussianGetBufferSize(roiSize, kernelSize, ipp8u, 1, &specSize, &tmpBufSize);

	if (status1 != ippStsNoErr)
	{
		return -1;
	}

	//2��init
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

	//3��filter
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


//��ֵ�˲�
//VisMeanFilter����˵��������ipp�������ippiConv_8u_C1R,
//�˲����ͼ���СΪ(srcWidth - kernelSize + 1)*(srcHeight - kernelSize + 1)����Ե�Ҷ�ֵ��ԭͼ��ͬ��
//
//Input
//unsigned char *src, int srcHeight, int srcWidth   ����ͼ��
//unsigned int kernelSize  �˵ĳߴ磬���ڵ���3������
//int divisor   ��ȡΪkernelSize*kernelSize�������ڸ�ֵ��Ϊ��ͼ������Ҷ�ֵ�����˵������������ڸ�ֵ��Ϊ��ͼ������˵�������
//               ����û�����0����ʹ��Ĭ��ֵ��
//
//Output
//unsigned char *dst ���ͼ��������ͼ����һ�¡�
//
//Return
// 1   ����
// -1  �쳣
//����/����: ����/20170406


int  CBmDUTSocketLocation::VisFilterMean(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst,
	const unsigned char kernelSize, unsigned int divisor)
{
	_try{
	//���kernelSize���Ǵ��ڵ���3������������-1.
	if (kernelSize < 3)
	{
		return -1;
	}
	else
	{
		if (!(kernelSize % 2))
		{
			return -1;
		}
	}

	if (divisor == 0)
	{
		divisor = kernelSize*kernelSize;
	}

	unsigned char *src2;
	src2 = (Ipp8u*)malloc(kernelSize * kernelSize);// *sizeof(Ipp8u));
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
	memcpy(dst, src, srcWidth*srcHeight * sizeof(unsigned char));

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
	return 0;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

//��ֵ�˲�
//Input
//unsigned char *src, int srcHeight, int srcWidth   ����ͼ��
//const unsigned char winWidth  �˵ĳߴ磬���ڵ���3������
//output
//unsigned char *dst ���ͼ��������ͼ����һ�¡�
//
//Return
// 0   ����
// -1  �쳣
//����/����  ����/20170407
int CBmDUTSocketLocation::VisFilterMedian(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst, const unsigned int winWidth)
{
	//���kernelSize���Ǵ��ڵ���3������������ - 1.
	_try{
	if (winWidth < 3)
	{
		return -1;
	}
	else
	{
		if (!(winWidth % 2))
		{
			return -1;
		}
	}


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

	return 0;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}




//��ͨ��
//findOneConnCom_floodFill����˵��: �ڶ�ֵͼ������һ����ͨ��
//
//�������
// Ipp8u *srcSeg, int srcRows, int srcCols,   ����ͼ����Ϣ
// int newVal  �ҵ���ͨ���Ϊ��ֵ
//
//�������:
// IppiConnectedComp &pRegion �����ͨ����Ϣ
//����/����: ����/20170406
void  CBmDUTSocketLocation::findOneConnCom_floodFill(Ipp8u *srcSeg, int srcRows, int srcCols, int newVal, IppiConnectedComp &pRegion)
{
//	_try{
	IppiPoint seed = { 0,0 };
	//Ѱ����ͨ��ĵ�һ���㣬�����ӵ㣨seed point��
	int flag = 0;
	for (int i = 0; i < srcRows; i++)
	{
		for (int j = 0; j < srcCols; j++)
		{
			if (srcSeg[j + i*srcCols] == 1)
			{
				seed.x = j;
				seed.y = i;
				flag = 1;
				break;
			}
			
		}
		if (flag)
			break;
	}

	int pBufSize = 0;
	int srcStep = srcCols * sizeof(Ipp8u);
	IppiSize roiSize = { srcCols,srcRows };

	ippiFloodFillGetSize(roiSize, &pBufSize);

	Ipp8u *pBuffer = NULL;
	pBuffer = (Ipp8u*)malloc(pBufSize * sizeof(Ipp8u));
	//IppiConnectedComp pRegion = {0};
	ippiFloodFill_8Con_8u_C1IR(srcSeg, srcStep, roiSize, seed, newVal, &pRegion, pBuffer);
	free(pBuffer);
	//}
	//	__except (EXCEPTION_EXECUTE_HANDLER)
	//{
	//	return _CODE_THROW;
	//}
}

// connComFloodFilling����˵��:  �ڶ�ֵͼ�����ҳ�������ͨ��
//
//�������
// Ipp8u *srcSeg, int srcRows, int srcCols   ����ͼ��
// int threNum  ��ͨ������ص�ĸ���
//
//�������:
// IppiConnectedComp *&pRegion  ���е���ͨ����Ϣ
//int &ccNum  ��ͨ��ĸ���
//
//����ֵ:
//
//����/����: ����/20170406
void  CBmDUTSocketLocation::connComFloodFilling(Ipp8u *srcSeg, int srcRows, int srcCols, int threNum, IppiConnectedComp *&pRegion, int &ccNum)
{
	int newVal = 2;//�ҵ�����ͨ���ø�ֵ���
	int i = 0;
	//IppiConnectedComp *pRegion;
	vector<IppiConnectedComp> connCom;
	connCom.clear();
	while (threNum > 0)
	{
		IppiConnectedComp Region = { 0 };
		findOneConnCom_floodFill(srcSeg, srcRows, srcCols, newVal, Region);
		connCom.push_back(Region);

		newVal++;
		threNum = threNum - Region.area;
		i++;
	}
	ccNum = connCom.size();
	pRegion = (IppiConnectedComp*)malloc(ccNum * sizeof(IppiConnectedComp));
	for (int p = 0; p < connCom.size(); p++)
	{
		pRegion[p] = connCom[p];
	}
}

// getRegionInfo����˵��:  ������ͨ��Ŀ��ߡ���ֵ�㡢���ĵ����Ϣ��
//
//�������
// unsigned char *pMarker, IppiSize roiSize  ����ͼ�񣬱������ͨ���ͼ��
// int regionNum   ��ͨ��ĸ���
//
//�������:
// int *regionArea  ������ͨ������
//RegionPeakPoint *regionPeakPoint   ������ͨ��ļ�ֵ��
// RegionWidthHeight *regionWH   ������ͨ��Ŀ��
//IppiPoint *regionGraCenPoint   ������ͨ������ĵ�
//
//����ֵ:
// 1   ����
// -1  �쳣
//����/����: ����/20170406
int CBmDUTSocketLocation::getRegionInfo(unsigned char * intSrc,IppiSize imgSize,int regionNum,int *regionArea,RegionPeakPoint *regionPeakPoint,RegionWidthHeight *regionWH,IppiPoint *regionGraCenPoint)
{
	__try
	{
		if (regionNum <= 0)
			return -1;
		int width = imgSize.width;
		int height = imgSize.height;

		//init
		memset(regionArea, 0, regionNum * sizeof(int));
		for (int i = 2; i < regionNum + 2; i++)
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
		int *xSum = new int[regionNum + 2];
		int *ySum = new int[regionNum + 2];
		memset(xSum, 0, (regionNum + 2) * sizeof(int));
		memset(ySum, 0, (regionNum + 2) * sizeof(int));


		int pos = 0;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int val = intSrc[pos + j];
				if (val >= 2 || val < regionNum + 2)

				{
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

			}
			pos += width;
		}

		//	cal regionGraCenPoint & regionWH 	//
		for (int k = 2; k < regionNum + 2; k++)
		{
			if (regionArea[k] != 0)
			{
				regionGraCenPoint[k].x = xSum[k] / regionArea[k];
				regionGraCenPoint[k].y = ySum[k] / regionArea[k];
			}
			regionWH[k].width = regionPeakPoint[k].xMax - regionPeakPoint[k].xMin;
			regionWH[k].height = regionPeakPoint[k].yMax - regionPeakPoint[k].yMin;


		}

		if (xSum != nullptr)
			delete[] xSum;
		if (ySum != nullptr)
			delete[] ySum;

		return 1;
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}
//int CBmDUTSocketLocation::getRegionInfo(unsigned char *pMarker,IppiSize roiSize,int regionNum,int *regionArea,RegionPeakPoint *regionPeakPoint,RegionWidthHeight *regionWH,IppiPoint *regionGraCenPoint)
//{
//	_try{
//	//	unsigned char *pMarker = src.ptr;
//	//IMG_SIZE roiSize = src.size;
//	//init
//	memset(regionArea, 0, sizeof(int) * regionNum);
//	for (int i = 0; i < regionNum; i++)
//	{
//		regionPeakPoint[i].xMax = 0;
//		regionPeakPoint[i].xMin = 0;
//		regionPeakPoint[i].yMax = 0;
//		regionPeakPoint[i].yMin = 0;
//		regionWH[i].height = 0;
//		regionWH[i].width = 0;
//		regionGraCenPoint[i].x = 0;
//		regionGraCenPoint[i].y = 0;
//	}
//
//
//	//	calculate region area and region width / height
//	for (int val = 2; val < regionNum + 2; val++)
//	{
//
//		int Xmin = roiSize.width - 1;
//		int Xmax = 0;
//		int Ymin = roiSize.height - 1;
//		int Ymax = 0;
//		int xSum = 0;
//		int ySum = 0;
//		for (int i = 0; i < roiSize.height; i++)
//		{
//			for (int j = 0; j < roiSize.width; j++)
//			{
//				if (pMarker[i * roiSize.width + j] == val)
//				{
//					regionArea[val]++;
//					xSum += j;
//					ySum += i;
//					if (j < Xmin)	Xmin = j;
//					if (j > Xmax) Xmax = j;
//					if (i < Ymin)	Ymin = i;
//					if (i > Ymax)	Ymax = i;
//				}
//			}
//		}
//
//		if (Xmin < 0 || Xmax > roiSize.width - 1 || Ymin < 0 || Ymax > roiSize.height)
//			return -1;
//
//		regionPeakPoint[val].xMin = Xmin;
//		regionPeakPoint[val].xMax = Xmax;
//		regionPeakPoint[val].yMin = Ymin;
//		regionPeakPoint[val].yMax = Ymax;
//
//		if (Xmax > Xmin)
//			regionWH[val].width = Xmax - Xmin;
//		if (Ymax > Ymin)
//			regionWH[val].height = Ymax - Ymin;
//
//		if (regionArea[val] > 0)
//		{
//			regionGraCenPoint[val].x = xSum / regionArea[val];
//			regionGraCenPoint[val].y = ySum / regionArea[val];
//		}
//
//	}
//
//	return 1;
//	}
//		__except (EXCEPTION_EXECUTE_HANDLER)
//	{
//		return _CODE_THROW;
//	}
//}
///////**********************************************/
// threshold_CountDots, ������ֵ�ָ�
// Input:
//                pSrc �� ����ͼ��ָ��
//                nW,nH      ͼ����
// Output:
//                pDst �� ���ͼ��ָ��
// Return:
//                ippstatus
// Liu Ping, 2017.05.09
/**********************************************/
int CBmDUTSocketLocation::VisSegmentCntDots(unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char * pDst, const double fWhiteRatio)
{
	_try{
	//{
	unsigned int nBins = 255;

	int status = ippStsNoErr;
	unsigned char nThreshold = 0;

	//       calc threshold   //
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
	int nWhite = int(nW*nH*fWhiteRatio + 0.5);
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

	//       do segment       //
	//int srcStep = nW * sizeof(unsigned char);
	int dstStep = srcStep;
	IppiSize imgSize;
	imgSize.width = nW;
	imgSize.height = nH;
	status = ippiThreshold_LTValGTVal_8u_C1R(pSrc, srcStep, pDst, dstStep, imgSize, nThreshold, 0, nThreshold, 255);

	return nThreshold;
}
__except (EXCEPTION_EXECUTE_HANDLER)
{
	return _CODE_THROW;
}
}

int CBmDUTSocketLocation::VisHistogram(IMG_UBBUF src, float percent, int &threshold)
{
	//1��ͳ��ÿ���Ҷ�ֵ���ֵĴ���
	_try{
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
	pHist = (Ipp32u*)malloc((nBins) * sizeof(Ipp32u));
	Ipp8u *buffer = NULL;
	buffer = (Ipp8u*)malloc(bufferSize * sizeof(Ipp8u));

	status = ippiHistogram_8u_C1R(src.ptr, srcStep, srcSize, pHist, Spec, buffer);

	//free(buffer);
	//ippsFree(Spec);

	if (status != ippStsNoErr)
	{
		free(pLevels);
		free(pHist);
		free(buffer);
		ippsFree(Spec);
		return -1;
	}

	/*FILE *Hist;
	Hist = fopen("E:\\ProjectBlob\\Hist.txt", "w");
	for (int i = 0; i < 255; i++)
	{
	fprintf(Hist, "%d   %d\n",i, pHist[i]);
	}*/


	//2��ͳ�ƹ�һ����ֱ��ͼ,��������� double Prob[256]
	double *Prob = (double*)malloc(256 * sizeof(double));
	int N = src.size.height*src.size.width;
	Prob[0] = pHist[0] * 1.0 / N;
	for (int i = 1; i < 256; i++)
	{
		for (int j = 0; j < i; j++)
		{
			Prob[i] = Prob[i - 1] + pHist[i] * 1.0 / N;
			if (Prob[i - 1] <= 1 - percent && Prob[i] >= 1 - percent)
			{
				threshold = i;
			}

		}
	}

	//3�������µ�����ֵ
	//�ҳ������С�Ҷ�ֵ
	/*int maxVal = src.ptr[0];
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
	}*/

	free(Prob);
	free(pLevels);
	free(pHist);
	free(buffer);
	ippsFree(Spec);
	return 0;
	 }
	 __except (EXCEPTION_EXECUTE_HANDLER)
	 {
		 return _CODE_THROW;
	 }
}


//Return 
//0  ����
//-1 �쳣
int CBmDUTSocketLocation::VisOpen(unsigned char* src, int srcWidth, int srcHeight, unsigned char*dst)
{
	_try{
	IppStatus status;

	Ipp8u pMask[3 * 3] = { 1,1,1,1,0,1,1,1,1 };
	//Ipp8u pMask[5 * 5] = { 1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1 ,1,1,1,1,1,1,1,1};
	//Ipp8u pMask[1 * 5] = {1,1,0,1,1 };
	IppiSize maskSize = { 3,3 };
	//IppiSize maskSize = { 5,5 };
	IppiSize roiSize = { srcWidth,srcHeight };

	int pSpecSize;
	int pBufferSize;
	status = ippiMorphAdvGetSize_8u_C1R(roiSize, maskSize, &pSpecSize, &pBufferSize);
	if (status != ippStsNoErr)
	{
		return -1;
	}


	IppiMorphAdvState *pMorpfSpec = NULL;
	pMorpfSpec = (IppiMorphAdvState*)ippsMalloc_8u(pSpecSize);
	Ipp8u *pBuffer = NULL;
	pBuffer = (Ipp8u*)ippsMalloc_8u(pBufferSize);
	status = ippiMorphAdvInit_8u_C1R(roiSize, pMask, maskSize, pMorpfSpec, pBuffer);
	if (status != ippStsNoErr)
	{
		ippsFree(pMorpfSpec);
		ippsFree(pBuffer);
		return -1;
	}

	int srcStep = srcWidth * sizeof(Ipp8u);
	int dstStep = srcStep;
	IppiBorderType borderType = ippBorderRepl;
	Ipp8u borderValue = 0;
	status = ippiMorphOpenBorder_8u_C1R(src, srcStep, dst, dstStep, roiSize, borderType, borderValue, pMorpfSpec, pBuffer);
	if (status != ippStsNoErr)
	{
		ippsFree(pMorpfSpec);
		ippsFree(pBuffer);
		return -1;
	}

	ippsFree(pMorpfSpec);
	ippsFree(pBuffer);
	return 0;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


// ����˵��:  �������������ͨ��Ŀ��ߡ���ֵ�㡢���ĵ����Ϣ��
//
//�������
// unsigned char *src1, int srcHeight, int srcWidth, ����ͼ�� 
//
//�������:
// unsigned char *src  �������ͨ���ͼ��
// int &ccNum ��ͨ��ĸ���
//IppiPoint &cen ���������ͨ������ĵ�
// int &w  ���������ͨ��Ŀ�
//int &h  ���������ͨ��ĸ�
//int &left ���������ͨ�����ֵ��
//int &right ���������ͨ����Ҽ�ֵ��
// int &up ���������ͨ����ϼ�ֵ��
// int &down ���������ͨ����¼�ֵ��
//
//����ֵ:
// 1   ����
// -1  �쳣
//����/����: ����/20170406

int CBmDUTSocketLocation::VisConnCom(unsigned char *src1, int srcHeight, int srcWidth, unsigned char *src, int &ccNum,
	IppiPoint &cen, int &w, int &h, int &left, int &right, int &up, int &down,int &val)
{
	_try{
	//cout << (int)src1[0] << endl;
	/*for (int i = 0; i < srcHeight*srcWidth; i++)
	{
	src[i] = src1[i];
	}*/
	memcpy(src, src1, srcHeight*srcWidth * sizeof(unsigned char));
	//ofstream cc("srcc.txt");

	//for (int i = 0; i<srcHeight; i++)
	//{
	//	for (int j = 0; j < srcWidth; j++)
	//	{
	//		cc << (int)src1[j + i*(srcWidth)] << " ";
	//	}
	//	cc << endl;
	//}
	//cc.close();
	//��ֵ�ָ��ͨ��
	/*int srcStep = srcWidth*sizeof(unsigned char);
	IppiSize roiSize = { srcWidth,srcHeight };
	Ipp8u *pThreshold;
	pThreshold = (Ipp8u*)malloc(srcHeight*srcWidth*sizeof(Ipp8u));
	ippiComputeThreshold_Otsu_8u_C1R(src, srcStep, roiSize, pThreshold);*/

	int threNum = 0;//ͳ����ͨ����ܵ����ظ���
	/*int max = 0;
	for (int i = 0; i < srcHeight*srcWidth; i++)
	{
		if (src[i] > max)
		{
			max = src[i];
		}
	}*/

	int threshold = 0;
	//IMG_UBBUF ss;// = { src ,{ srcWidth ,srcHeight },srcWidth };
	//ss.ptr = (unsigned char*)malloc(srcWidth*srcHeight * sizeof(unsigned char));
	//memcpy(ss.ptr, src, srcHeight*srcWidth * sizeof(unsigned char));
	//ss.size.height = srcHeight;
	//ss.size.width = srcWidth;
	//ss.linestep = srcWidth;
	//float percent = 0.08;
	//VisHistogram(ss, percent, threshold);
	unsigned char* pDst = (unsigned char*)malloc(srcWidth*srcHeight*sizeof(unsigned char));
	threshold = VisSegmentCntDots(src, srcWidth, srcHeight,pDst, InputPara.m_CorasePointPercent);
	free(pDst);
	//cout << threshold << endl;
	//threshold = (int)(max*0.9);
	for (int i = 0; i < srcHeight*srcWidth; i++)
	{
		if (src[i] < threshold)//pThreshold[0])
		{
			src[i] = 0;
		}
		else
		{
			src[i] = 1;
			threNum++;
		}
	}

	if (threNum < 1)
	{
		return -1;
	}
	


	//��
	unsigned char* DstOpen;
	DstOpen = (unsigned char*)malloc(srcWidth*srcHeight * sizeof(unsigned char));
	int status11 = VisOpen(src, srcWidth, srcHeight, DstOpen);
	if (status11 != 0)
	{
		free(DstOpen);
		return -1;
	}

	
	threNum = 0;
	for (int i = 0; i < srcHeight*srcWidth; i++)
	{
		if (DstOpen[i] == 1)
		{
			threNum++;
		}
	}
	IppiConnectedComp *pRegion = NULL;
	//int ccNum = 0;
	connComFloodFilling(DstOpen, srcHeight, srcWidth, threNum, pRegion, ccNum);
	//cout << ccNum << endl;
	memcpy(src, DstOpen, srcHeight*srcWidth * sizeof(unsigned char));
	//ofstream cc("srcc.txt");

	//for (int i = 0; i<srcHeight; i++)
	//{
	//	for (int j = 0; j < srcWidth; j++)
	//	{
	//		cc << (int)DstOpen[j + i*(srcWidth)] << " ";
	//	}
	//	cc << endl;
	//}

	//FILE *cc;
	//cc = fopen("E:\\ProjectBlob\\srcc.txt", "w");
	//for (int i = 0; i<srcHeight; i++)
	//{
	//	for (int j = 0; j < srcWidth; j++)
	//	{
	//		fprintf(cc, "%d   ", DstOpen[j + i*(srcWidth)]);
	//	}
	//	fprintf(cc, "\n");
	//}
	//fclose(cc);


	IppiSize roiSize = { srcWidth,srcHeight };
	int *regionArea = (int*)malloc((ccNum + 2) * sizeof(int));
	RegionPeakPoint *regionPeakPoint = (RegionPeakPoint *)malloc((ccNum + 2) * sizeof(RegionPeakPoint));
	RegionWidthHeight *regionWH = (RegionWidthHeight*)malloc((ccNum + 2) * sizeof(RegionWidthHeight));
	IppiPoint *regionGraCenPoint = (IppiPoint*)malloc((ccNum + 2) * sizeof(IppiPoint));
	for (int i = 0; i < ccNum + 2; i++)
	{
		regionArea[i] = 0;// (int)(pRegion[i - 2].area);
	}

	//double t1 = (double)cvGetTickCount();
	int status = getRegionInfo(DstOpen, roiSize, ccNum, regionArea, regionPeakPoint, regionWH, regionGraCenPoint);

	//t1 = (double)cvGetTickCount() - t1;
	//printf("getregion = %gms\n", t1 / (cvGetTickFrequency() * 1000));

	if (status != 1)
	{
		free(DstOpen);
		free(regionArea);
		free(regionPeakPoint);
		free(regionWH);
		free(regionGraCenPoint);
		free(pRegion);
		return -1;
	}


	//cout << ccNum << endl;
	int goa = 0;
	int maxArea = 0;
	for (int i = 2; i < ccNum + 2; i++)
	{
	//	cout << regionArea[i] << endl;
		if (regionArea[i] > maxArea)
		{
			maxArea = regionArea[i];
			goa = i;
		}
	}
	//cout << goa << endl;
	val = goa;
	if (maxArea < InputPara.m_CoraseMinArea)
	{
		free(DstOpen);
		free(regionArea);
		free(regionPeakPoint);
		free(regionWH);
		free(regionGraCenPoint);
		free(pRegion);
		return -1;
	}
	//����޶�,800-1500


	//����޶�
	//λ���޶�

	//��ֹ�������
	if (regionWH[goa].width > InputPara.m_CoraseWidth1)
	{
		cen.y = regionGraCenPoint[goa].y;
		cen.x = regionPeakPoint[goa].xMax - InputPara.m_CoraseWidth2/2;
		w = InputPara.m_CoraseWidth2;
		h = regionWH[goa].height;

		left = regionPeakPoint[goa].xMax - InputPara.m_CoraseWidth2;
		right = regionPeakPoint[goa].xMax;
		up = regionPeakPoint[goa].yMin;
		down = regionPeakPoint[goa].yMax;

		free(DstOpen);
		free(regionArea);
		free(regionPeakPoint);
		free(regionWH);
		free(regionGraCenPoint);
		free(pRegion);

		return 1;
	}


	cen = regionGraCenPoint[goa];
	w = regionWH[goa].width;
	h = regionWH[goa].height;

	left = regionPeakPoint[goa].xMin;
	right = regionPeakPoint[goa].xMax;
	up = regionPeakPoint[goa].yMin;
	down = regionPeakPoint[goa].yMax;

	free(DstOpen);
	free(regionArea);
	free(regionPeakPoint);
	free(regionWH);
	free(regionGraCenPoint);
	free(pRegion);

	return 1;
	 }
	 __except (EXCEPTION_EXECUTE_HANDLER)
	 {
		 return _CODE_THROW;
	 }
}


int CBmDUTSocketLocation::Line_LeastSquare(double x[], double y[], int num, double &k, double &b)
{
	_try{
	double sum1 = 0.0f;
	double sum2 = 0.0f;

	double sumx = 0.0f;
	double sumy = 0.0f;

	double meanx = 0.0f;
	double meany = 0.0f;

	//////
	for (int i = 0; i < num; i++)
	{
		sumx += x[i];
		sumy += y[i];
	}
	meanx = sumx / num;
	meany = sumy / num;

	////////
	for (int i = 0; i < num; i++)
	{
		sum1 += (x[i] - meanx)*(x[i] - meanx);
		sum2 += (x[i] - meanx)*(y[i] - meany);
	}

	////////
	if (sum1 == 0)
	{
		b = meanx;
		return -1;
	}

	k = sum2 / sum1;
	b = meany - k*meanx;

	return 0;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

//Input:
//double x[]: ������ϵ�x��������
//double y[]: ������ϵ�y��������
//int num : ������ϵ���Ŀ
//double inlierPercent  ѡ��ı���������70%�����ѡȡ70%�ĵ�����ϣ�
//int iter : �������������һ��Ϊ20
//Output:
//double &k,double &b: ���ֱ��б�ʺͽؾ�
//Return
//0  ����
//-1 �쳣
int CBmDUTSocketLocation::FitLine_Ransac(double x[], double y[], int num, double inlierPercent, int iter, double &k, double &b, double &bestError)
{
	_try{
	double k1 = 0.0f;
	double b1 = 0.0f;
	double thisError = 0.0f;
	bestError = 10000.0f;

	//���ѡ��
	int inlier = (int)(num*inlierPercent);

	double *cx;
	double *cy;
	cx = (double*)malloc(inlier * sizeof(double));
	cy = (double*)malloc(inlier * sizeof(double));

	int *cn;
	cn = (int*)malloc(inlier * sizeof(int));

	for (int m = 0; m < iter; m++)
	{
		srand(time(0));
		for (int w = 0; w<inlier; w++)//��0 ~ num֮�䣬���ѡȡinlier���������ѡȡ����������±�
		{
			cn[w] = (int)((num*rand()) / (RAND_MAX + 1.0));
		}

		for (int i = 0; i < inlier; i++)
		{
			cx[i] = x[cn[i]];
			cy[i] = y[cn[i]];
		}

		Line_LeastSquare(cx, cy, inlier, k1, b1);

		for (int n = 0; n<inlier; n++)
		{
			thisError += (k1*cx[n] + b1 - cy[n])*(k1*cx[n] + b1 - cy[n]);
		}

		if (thisError<bestError)
		{
			bestError = thisError;

			k = k1;
			b = b1;
		}
	}

	free(cx);
	free(cy);
	free(cn);
	return 0;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}
int  CBmDUTSocketLocation::GNERAL_LINE_FITTING(vector<IMG_RCOORD>PointCor, IMG_REAL &Slope, IMG_REAL &B, IMG_REAL Sigma, IMG_INT FLAG)
{
	//_try{
	if (PointCor.size() == 0)
		return -1;
	IMG_REAL *MatA, *MatA_Copy, *MatB, *MatC, *MatC_Copy, *Weight, *ErrorRecord;
	IMG_INT ErrorMaxTimes = 20, ErrorTimes, Length = PointCor.size();//length means efficient point numbers subject to sigma

	MatA = (IMG_REAL*)malloc(sizeof(IMG_REAL) * 2 * PointCor.size()); //column: 2, rows: number of points
	MatB = (IMG_REAL*)malloc(sizeof(IMG_REAL) * 2);
	MatC = (IMG_REAL*)malloc(sizeof(IMG_REAL) * PointCor.size());
	MatA_Copy = (IMG_REAL*)malloc(sizeof(IMG_REAL) * 2 * PointCor.size()); //column: 2, rows: number of points
	MatC_Copy = (IMG_REAL*)malloc(sizeof(IMG_REAL) * PointCor.size());
	ErrorRecord = (IMG_REAL*)malloc(sizeof(IMG_REAL) * ErrorMaxTimes * 3);
	Weight = (IMG_REAL*)malloc(sizeof(IMG_REAL) * PointCor.size());
	for (IMG_INT j = 0; j < (IMG_INT)PointCor.size(); j++)
	{
		MatA[j * 2 + 0] = PointCor[j].x;
		MatA[j * 2 + 1] = 1;

		MatA_Copy[j * 2 + 0] = MatA[j * 2 + 0];
		MatA_Copy[j * 2 + 1] = MatA[j * 2 + 1];

		MatC[j] = PointCor[j].y;
		MatC_Copy[j] = MatC[j];
		Weight[j] = 1;
	}

	switch (FLAG)
	{
	case 0:
		MatB[0] = Slope;
		MatB[1] = B;
		break;
	case 1:
		//step: initial fitting
		LAPACKE_sgels(LAPACK_ROW_MAJOR, 'N', PointCor.size(), 2, 1, MatA_Copy, 2, MatC_Copy, 1);//fitting slope and b
		MatB[0] = MatC_Copy[0];
		MatB[1] = MatC_Copy[1];
	default:
		break;
	}

	////step: choose points subject to sigma
	for (IMG_INT j = 0; j < Length; j++)
	{
		MatA_Copy[j * 2 + 0] = MatA[j * 2 + 0];
		MatA_Copy[j * 2 + 1] = MatA[j * 2 + 1];
		MatC_Copy[j] = MatC[j];
	}



	//step: fitting 
	ErrorTimes = 0;
	while (1)
	{
		cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, Length, 1, 2, 1, MatA, 2, MatB, 1, -1, MatC_Copy, 1); //calculate error
		ErrorRecord[ErrorTimes * 3 + 1] = MatB[0];
		ErrorRecord[ErrorTimes * 3 + 2] = MatB[1];
		for (IMG_INT j = 0; j < Length; j++)
		{
			ErrorRecord[ErrorTimes * 3] += MatC_Copy[j];
		}
		ErrorRecord[ErrorTimes * 3] /= (IMG_REAL)Length;
		if (ErrorTimes >= 1 && abs(ErrorRecord[ErrorTimes * 3] - ErrorRecord[(ErrorTimes - 1) * 3]) <= 0.001) //error sum covergence to 0.001
		{
			Slope = MatB[0];
			B = MatB[1];
			break;
		}
		if (ErrorTimes == ErrorMaxTimes - 1) // runing time > max time, choose lowest error sum to return
		{
			IMG_INT FLAG = 0;
			for (IMG_INT i = 0; i < ErrorMaxTimes - 1; i++)
			{
				if (ErrorRecord[3 * i] < ErrorRecord[3 * (i + 1)])
					ErrorRecord[3 * (i + 1)] = ErrorRecord[3 * i];
				else
					FLAG = i + 1;
			}

			Slope = ErrorRecord[FLAG * 3 + 1];
			B = ErrorRecord[FLAG * 3 + 2];
			break;
		}
		ErrorTimes++;
		for (IMG_INT j = 0; j < Length; j++)
		{
			MatA_Copy[j * 2 + 0] = MatA[j * 2 + 0];
			MatA_Copy[j * 2 + 1] = MatA[j * 2 + 1];

			Weight[j] = (IMG_REAL)1.0 / (1.f + exp(MatC_Copy[j] * MatC_Copy[j] / (Sigma * Sigma)));

			MatA_Copy[j * 2 + 0] = Weight[j] * MatA_Copy[j * 2 + 0];
			MatA_Copy[j * 2 + 1] = Weight[j] * MatA_Copy[j * 2 + 1];


			MatC_Copy[j] = MatC[j] * Weight[j]; //reset MatCCopy after calculating error
		}
		LAPACKE_sgels(LAPACK_ROW_MAJOR, 'N', Length, 2, 1, MatA_Copy, 2, MatC_Copy, 1);//fitting slope and b
		MatB[0] = MatC_Copy[0];
		MatB[1] = MatC_Copy[1];
		for (IMG_INT j = 0; j < Length; j++) //reset MatCCopy after fitting line
		{
			MatC_Copy[j] = MatC[j];
		}
	}
	free(MatC_Copy);
	free(Weight);
	free(MatA_Copy);
	free(MatA);
	free(MatB);
	free(MatC);
	free(ErrorRecord);
	return 0;
	 //}
	 //__except (EXCEPTION_EXECUTE_HANDLER)
	 //{
		// return _CODE_THROW;
	 //}
}
int  CBmDUTSocketLocation::GNERAL_LINE_FITTING_NO_WEIGHT(vector<IMG_RCOORD>PointCor, IMG_REAL &Slope, IMG_REAL &B, IMG_REAL &ErrorRecord)
{
//	_try{
	if (PointCor.size() == 0)
		return -1;
	IMG_REAL *MatA, *MatA_Copy, *MatB, *MatC, *MatC_Copy;
	MatA = (IMG_REAL*)malloc(sizeof(IMG_REAL) * 2 * PointCor.size()); //column: 2, rows: number of points
	MatB = (IMG_REAL*)malloc(sizeof(IMG_REAL) * 2);
	MatC = (IMG_REAL*)malloc(sizeof(IMG_REAL) * PointCor.size());
	MatA_Copy = (IMG_REAL*)malloc(sizeof(IMG_REAL) * 2 * PointCor.size()); //column: 2, rows: number of points
	MatC_Copy = (IMG_REAL*)malloc(sizeof(IMG_REAL) * PointCor.size());
	for (IMG_INT j = 0; j < (IMG_INT)PointCor.size(); j++)
	{
		MatA_Copy[j * 2 + 0] = MatA[j * 2 + 0] = PointCor[j].x;
		MatA_Copy[j * 2 + 1] = MatA[j * 2 + 1] = 1;
		MatC_Copy[j] = MatC[j] = PointCor[j].y;
	}
	LAPACKE_sgels(LAPACK_ROW_MAJOR, 'N', PointCor.size(), 2, 1, MatA_Copy, 2, MatC_Copy, 1);//fitting slope and b
	Slope = MatB[0] = MatC_Copy[0];
	B = MatB[1] = MatC_Copy[1];
	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, PointCor.size(), 1, 2, 1, MatA, 2, MatB, 1, -1, MatC, 1); //calculate error
	ErrorRecord = 0;
	for (IMG_INT j = 0; j < PointCor.size(); j++)
	{
		ErrorRecord += MatC[j];
	}	
	free(MatC_Copy);
	free(MatA_Copy);
	free(MatA);
	free(MatB);
	free(MatC);
	return 0;
	//}
	//	__except (EXCEPTION_EXECUTE_HANDLER)
	//{
	//	return _CODE_THROW;
	//}
}

//DownLookCorasePosition_down����˵��
//Input
//sour      ����ͼ��
//
//Output
//leftPoint  Ŀ������Ե��
//rightPoint Ŀ����ұ�Ե��
//upPoint    Ŀ����ϱ�Ե��
//downPoint  Ŀ����±�Ե��
//Return 
//  1  ����
//  -1  �쳣
//DownLookCorasePosition_down����˵��
//Input
//sour      ����ͼ��
//
//Output
//upleft  Ŀ������Ե��
//upright Ŀ����ұ�Ե��
//downleft    Ŀ����ϱ�Ե��
//downright  Ŀ����±�Ե��
//ang   �Ƕȣ�0~2PI�����ȣ�
//Return 
//  1  ����
//  -1  �쳣
int CBmDUTSocketLocation::DownLookCorasePosition_down(IMG_UBBUF sour,//unsigned char *sour, int sourRows, int sourCols,
	IMG_RCOORD &upleft, IMG_RCOORD &upright, IMG_RCOORD &downleft, IMG_RCOORD &downright, double &ang)
{
	//_try{
	CBmSocketLocatingCoarse tool;

	//1��ֱ��ͼ�涨��
	int sourCols = sour.size.width;
	int sourRows = sour.size.height;

	//VisHistogSpeci(sour, 60, 40, sour);
	IMG_UBBUF sou;// = { sour.ptr,{ sourCols, sourRows }, sourCols };
	sou.ptr = (unsigned char*)malloc(sourCols*sourRows * sizeof(unsigned char));
	sou.linestep = sourCols;
	sou.size.height = sourRows;
	sou.size.width = sourCols;

	int statushs = VisHistogSpeci(sour, InputPara.m_CoraseHisSpeMean, InputPara.m_CoraseHisSpeVarian, sou);
	if (statushs != 0)
	{
		free(sou.ptr);
		return -1;
	}

	//FILE *sr2;
	//sr2 = fopen("E:\\ProjectBlob\\HisSp.txt", "w");
	//for (int i = 0; i<sourRows; i++)
	//{
	//for (int j = 0; j < sourCols; j++)
	//{
	//fprintf(sr2, "%d   ", sou.ptr[j + i*(sourCols)]);
	//}
	//fprintf(sr2, "\n");
	//}
	//fclose(sr2);



	//2����ֵ�˲�
	unsigned char *meaSrc = (unsigned char*)malloc(sourCols*sourRows * sizeof(unsigned char));
	//int status21 = VisFilterMean(sou.ptr, sourRows, sourCols, meaSrc, 5, 25);  //��ֵ�˲�
	int status21 = VisFilterMedian(sou.ptr, sourRows, sourCols, meaSrc, InputPara.m_CoraseFilterKern);
	if (status21 == -1)
	{
		//	cout << "coarse damage median" << endl;
		free(sou.ptr);
		free(meaSrc);
		return -1;
	}


	//FILE *s2;
	//s2 = fopen("E:\\ProjectBlob\\sr2.txt", "w");
	//for (int i = 0; i<sourRows; i++)
	//{
	//	for (int j = 0; j < sourCols; j++)
	//	{
	//		fprintf(s2, "%d   ", meaSrc[j + i*(sourCols)]);
	//	}
	//	fprintf(s2, "\n");
	//}
	//fclose(s2);


	//#ifdef DEBUG_VisCP
	/*FILE *sr2;
	sr2 = fopen("E:\\ProjectBlob\\HisSp.txt", "w");
	for (int i = 0; i<srcHeight; i++)
	{
	for (int j = 0; j < srcWidth; j++)
	{
	fprintf(sr2, "%d   ", pDst[j + i*(srcWidth)]);
	}
	fprintf(sr2, "\n");
	}
	fclose(sr2);*/
	//#endif
	
	_BACK_CODE  s = tool.SocketLocatingCoarse(sour, 0);
	/////



	if (s != 0)
	{
		//cout << "coarse damage huang" << endl;
		free(sou.ptr);
		free(meaSrc);
		return -1;
	}

	standardPoint NorPoint;
	NorPoint.leftPoint.max_x = 0;
	NorPoint.leftPoint.min_x = 0;
	NorPoint.leftPoint.max_y = 0;
	NorPoint.leftPoint.min_y = 0;
	NorPoint.rightPoint.max_x = 0;
	NorPoint.rightPoint.min_x = 0;
	NorPoint.rightPoint.max_y = 0;
	NorPoint.rightPoint.min_y = 0;
	tool.VisSS_GetPoint(NorPoint);

	IMG_COORD PP1, PP2;

	if (NorPoint.leftPoint.min_x < NorPoint.rightPoint.min_x)
	{
		PP1.x = pow(2, InputPara.m_CorasePyraLevel) * NorPoint.leftPoint.CenterX;
		PP1.y = pow(2, InputPara.m_CorasePyraLevel) * NorPoint.leftPoint.CenterY;
		//PP1 = { NorPoint.leftPoint.min_x ,NorPoint.leftPoint.max_y };
		//PP2 = { NorPoint.rightPoint.min_x ,NorPoint.rightPoint.max_y };
	}
	else
	{
		PP1.x = pow(2, InputPara.m_CorasePyraLevel) * NorPoint.rightPoint.CenterX;
		PP1.y = pow(2, InputPara.m_CorasePyraLevel) * NorPoint.rightPoint.CenterY;
		//PP2 = { NorPoint.leftPoint.min_x ,NorPoint.leftPoint.max_y };
		//PP1 = { NorPoint.rightPoint.min_x ,NorPoint.rightPoint.max_y };
	}

	//�趨ROI
	//IMG_COORD P1, P2;
	IMG_COORD startPoint = { 0,0 };
	startPoint.x = PP1.x - InputPara.DutCorase_RoiX;
	startPoint.y = PP1.y + InputPara.DutCorase_RoiY;
	int roiWidth = InputPara.DutCorase_RoiWidth;
	int roiHeight = InputPara.DutCorase_RoiHeight;
	if (startPoint.y + roiHeight > sourRows - 1)
	{
		roiHeight = sourRows - startPoint.y - 1;
	}
	unsigned char *roi;
	roi = (unsigned char*)malloc(roiHeight*roiWidth * sizeof(unsigned char));
	for (int i = 0; i < roiHeight; i++)
	{
		for (int j = 0; j < roiWidth; j++)
		{
			roi[j + i*roiWidth] = meaSrc[j + startPoint.x + (i + startPoint.y)*sourCols];
		}
	}

	/*unsigned char* DstOpen;
	DstOpen = (unsigned char*)malloc(roiWidth*roiHeight*sizeof(unsigned char));
	status21 = VisOpen(roi, roiWidth, roiHeight, DstOpen);
	if (status21 != 0)
	{
	free(sou.ptr);
	free(meaSrc);
	free(DstOpen);
	return -1;
	}*/

	/*FILE *sr2;
	sr2 = fopen("E:\\ProjectBlob\\roi.txt", "w");
	for (int i = 0; i<roiHeight; i++)
	{
	for (int j = 0; j < roiWidth; j++)
	{
	fprintf(sr2, "%d   ", roi[j + i*(roiWidth)]);
	}
	fprintf(sr2, "\n");
	}
	fclose(sr2);*/



	unsigned char *ccSrc;
	ccSrc = (unsigned char*)malloc(roiHeight*roiWidth * sizeof(unsigned char));
	int ccNum = 0;

	IppiPoint cen = { 0,0 };
	int w = 0;
	int h = 0;
	int left = 0;
	int right = 0;
	int up = 0;
	int down = 0;
	int val = 0;
	int status41 = VisConnCom(roi, roiHeight, roiWidth, ccSrc, ccNum, cen, w, h, left, right, up, down, val);
	if (status41 == -1)
	{
		//	cout << "coarse damage cc" << endl;
		free(sou.ptr);
		free(meaSrc);
		free(ccSrc);
		free(roi);
		return -1;
	}






	//4����Ŀ����ͨ��ı�Ե(��)

	/*for (int i = up - 30; i < up + 30; i++)
	{
	for (int j = cen.x - 30; j < cen.x + 30; j++)
	{
	if (ccSrc[j + i*sourCols] == 0 && ccSrc[j + (i + 1)*sourCols] != 0)
	{
	pp.x = j;
	pp.y = i;
	ep.push_back(pp);
	}
	}
	}
	*/

	//4����Ŀ����ͨ��ı�Ե
	//////��
	vector<IMG_COORD> ep1;
	IMG_COORD pp1;
	/*if (down - 20 < 0 || down + 20 > roiHeight ||
	cen.x - 60 < 0 || cen.x + 60 > roiWidth)*/
	if (cen.y + 75 - 30 < 0 || cen.y + 75 + 30 > roiHeight ||
		cen.x - 60 < 0 || cen.x + 60 > roiWidth)
	{

		free(sou.ptr);
		free(meaSrc);
		free(ccSrc);
		free(roi);
		return -1;
	}

	for (int i = cen.y + 75 - 30; i < cen.y + 75 + 30; i++)
	{
		for (int j = cen.x - 60; j < cen.x + 60; j++)
		{
			if (ccSrc[j + i*roiWidth] == 0 && ccSrc[j + (i - 1)*roiWidth] == val)
			{
				pp1.x = j;
				pp1.y = i;
				ep1.push_back(pp1);
			}
		}
	}
	//////��
	vector<IMG_COORD> ep2;
	IMG_COORD pp2;
	/*if (up - 20 < 0 || up + 20 > roiHeight ||
	cen.x - 60 < 0 || cen.x + 60 > roiWidth)*/
	if (cen.y - 75 - 30 < 0 || cen.y - 75 + 30 > roiHeight ||
		cen.x - 60 < 0 || cen.x + 60 > roiWidth)
	{

		free(sou.ptr);
		free(meaSrc);
		free(ccSrc);
		free(roi);
		return -1;
	}

	for (int i = cen.y - 75 - 30; i < cen.y - 75 + 30; i++)
	{
		for (int j = cen.x - 60; j < cen.x + 60; j++)
		{
			if (ccSrc[j + i*roiWidth] == val && ccSrc[j + (i - 1)*roiWidth] == 0)
			{
				pp2.x = j;
				pp2.y = i;
				ep2.push_back(pp2);
			}
		}
	}
	//cout << ep1.size() << ep2.size() << endl;
	if (ep1.size() < 10 || ep2.size() < 10)
	{
		//	cout << "coarse damage 75" << endl;
		free(sou.ptr);
		free(meaSrc);
		free(ccSrc);
		free(roi);
		return -1;
	}
	/////////////
	/*int size1 = ep1.size();
	double k1 = 0;
	double b1 = 0;
	double err1 = 0;
	double *X1;
	X1 = (double *)malloc(size1 * sizeof(double));
	double *Y1;
	Y1 = (double *)malloc(size1 * sizeof(double));
	for (int i = 0; i < size1; i++)
	{
		X1[i] = ep1[i].x;
		Y1[i] = ep1[i].y;
	}
	FitLine_Ransac(X1, Y1, size1, 0.7, 20, k1, b1, err1);

	int size2 = ep2.size();
	double k2 = 0;
	double b2 = 0;
	double err2 = 0;
	double *X2;
	X2 = (double *)malloc(size2 * sizeof(double));
	double *Y2;
	Y2 = (double *)malloc(size2 * sizeof(double));
	for (int i = 0; i < size2; i++)
	{
		X2[i] = ep2[i].x;
		Y2[i] = ep2[i].y;
	}
	FitLine_Ransac(X2, Y2, size2, 0.7, 20, k2, b2, err2);*/

	float k1 = 0;
	float b1 = 0;
	float err1 = 0;
	vector<IMG_RCOORD>PointCor1;
	PointCor1.clear();
	IMG_RCOORD p;
	for (int i = 0; i < ep1.size(); i++)
	{
		p.x = (float)ep1[i].x;
		p.y = (float)ep1[i].y;
		PointCor1.push_back(p);
	}
	/*ofstream fp("point1.txt");
	for (int i = 0; i < PointCor1.size(); i++)
	{
		fp << PointCor1[i].x << "," << PointCor1[i].y << endl;
	}
	fp.close();*/

	int sta51 = GNERAL_LINE_FITTING_NO_WEIGHT(PointCor1, k1, b1, err1);


	if (sta51 != 0)
	{
		free(sou.ptr);
		free(meaSrc);
		free(ccSrc);
		return -1;
	}


	float k2 = 0;
	float b2 = 0;
	float err2 = 0;
	vector<IMG_RCOORD>PointCor2;
	PointCor2.clear();
	IMG_RCOORD pp;
	for (int i = 0; i < ep2.size(); i++)
	{
		pp.x = (float)ep2[i].x;
		pp.y = (float)ep2[i].y;
		PointCor2.push_back(pp);
	}
	sta51 = GNERAL_LINE_FITTING_NO_WEIGHT(PointCor2, k2, b2, err2);
	/*ofstream fp2("point2.txt");
	for (int i = 0; i < PointCor2.size(); i++)
	{
		fp2<< PointCor2[i].x << "," << PointCor2[i].y << endl;
	}
	fp2.close();*/

	if (err1 > err2)
	{
		ang = atan(k2);
	}
	else
	{
		ang = atan(k1);
	}

	if (ang < 0)
	{
		ang = ang + 2 * 3.1415926;
	}

	//��ԭͼ
	cen.x = cen.x + startPoint.x;
	cen.y = cen.y + startPoint.y;

	//������ת��������ĸ���������
	IMG_RCOORD P1, P2, P3, P4;
	P1.x = cen.x - InputPara.m_CoraseLeftOff;
	P1.y = cen.y - InputPara.m_CoraseUpOff;
	P2.x = cen.x + InputPara.m_CoraseRightOff;
	P2.y = cen.y - InputPara.m_CoraseUpOff;
	P3.x = cen.x - InputPara.m_CoraseLeftOff;
	P3.y = cen.y + InputPara.m_CoraseDownOff;
	P4.x = cen.x + InputPara.m_CoraseRightOff;
	P4.y = cen.y + InputPara.m_CoraseDownOff;

	/*upleft = P1;
	upright = P2;
	downleft = P3;
	downright = P4;*/

	upleft.x = cen.x + cos(ang)*(P1.x - cen.x) - sin(ang)*(P1.y - cen.y);
	upleft.y = cen.y + sin(ang)*(P1.x - cen.x) + cos(ang)*(P1.y - cen.y);

	upright.x = cen.x + cos(ang)*(P2.x - cen.x) - sin(ang)*(P2.y - cen.y);
	upright.y = cen.y + sin(ang)*(P2.x - cen.x) + cos(ang)*(P2.y - cen.y);

	downleft.x = cen.x + cos(ang)*(P3.x - cen.x) - sin(ang)*(P3.y - cen.y);
	downleft.y = cen.y + sin(ang)*(P3.x - cen.x) + cos(ang)*(P3.y - cen.y);

	downright.x = cen.x + cos(ang)*(P4.x - cen.x) - sin(ang)*(P4.y - cen.y);
	downright.y = cen.y + sin(ang)*(P4.x - cen.x) + cos(ang)*(P4.y - cen.y);

	if (upleft.x<1 || upleft.x>sourCols - 2 || upleft.y<1 || upleft.y>sourRows - 2
		|| upright.x<1 || upright.x>sourCols - 2 || upright.y<1 || upright.y>sourRows - 2
		|| downleft.x<1 || downleft.x>sourCols - 2 || downleft.y<1 || downleft.y>sourRows - 2
		|| downright.x<1 || downright.x>sourCols - 2 || downright.y<1 || downright.y>sourRows - 2)
	{

		free(roi);
		free(sou.ptr);
		free(meaSrc);
		free(ccSrc);
		return -1;
	}


	free(roi);
	free(sou.ptr);
	free(meaSrc);
	free(ccSrc);
	return 1;
}
	 /*}
	 __except (EXCEPTION_EXECUTE_HANDLER)
	 {
		 return _CODE_THROW;
	 }
}*/


/**********************************************/
// EDGEPOINT_FILTER, ����˵��,��Ե�����
// Input:
//     IMG_RCOORD PointStart, �û��������
//     IMG_RCOORD PointEnd,  �û������յ�
//     ROTATIONOPTIONS Rotation, ��ת����
//     RectScanDirection scanDirect, ɨ�跽��
//     EdgeDirection edDirectint, ��Ե����
//     edgeInformation *EdgePoint, ��Ե��
//     IMG_INT EdgePointNumber, ��Ե�����
// Output:
//     vector<edgeInformation> &PointFilterResult, ���˺��ļ���
// Return:
//     0 - ��ת����NOROTAION ���߱�Ե����BOTH
//     1 - ����
// Author: Ling Tan/3/6/2017
/**********************************************/
IMG_INT CBmDUTSocketLocation::EDGEPOINT_FILTER(IMG_RCOORD PointStart, IMG_RCOORD PointEnd, RectScanDirection scanDirect, EdgeDirection edDirectint, edgeInformation *EdgePoint, IMG_INT EdgePointNumber, vector<edgeInformation> &PointFilterResult)
{
	_try{


	//edgeInformation temp;
	IMG_INT LinePhase;//0,lefthand 180,righthand
	IMG_REAL AngleTol = InputPara.PointFilterAngleTolerance;
	if (edDirectint == BOTH)
	{
		for (IMG_INT i = 0; i < EdgePointNumber; i++)
		{
			if ((EdgePoint[i].angle >(270 - AngleTol) && EdgePoint[i].angle < (270 + AngleTol))
				|| (EdgePoint[i].angle < (90 + AngleTol) && EdgePoint[i].angle >(90 - AngleTol)))
				PointFilterResult.push_back(EdgePoint[i]);
		}
		return 0;
	}
	//get linephase subject to rotation direction

	//if (PointEnd.x > PointStart.x)
	//{
	//	if ((scanDirect == LEFT && edDirectint == BLACKWHITE) || (scanDirect == RIGHT && edDirectint == WHITEBLACK))
	//		LinePhase = 3;//lefthand
	//	else
	//		LinePhase = 2;//righthand
	//}
	//else
	//{
		if ((scanDirect == LEFT && edDirectint == BLACKWHITE) || (scanDirect == RIGHT && edDirectint == WHITEBLACK))
			LinePhase = 2;
		else
			LinePhase = 3;
	//}


	//0-vertical lefthand, 1-v r, 2-Horizontal lefthand, 3-H r
	switch (LinePhase)
	{
	case 2:
		for (IMG_INT i = 0; i < EdgePointNumber; i++)
		{
			if (EdgePoint[i].angle >(270 - AngleTol) && EdgePoint[i].angle < (270 + AngleTol))
				PointFilterResult.push_back(EdgePoint[i]);
		}
		break;
	case 3:
		for (IMG_INT i = 0; i < EdgePointNumber; i++)
		{
			if (EdgePoint[i].angle < (90 + AngleTol) && EdgePoint[i].angle >(90 - AngleTol))
				PointFilterResult.push_back(EdgePoint[i]);
		}
		break;
	default:
		break;
	}
	return 1;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}
/**********************************************/
// EDGEPOINT_FILTER, ����˵��,����ģ�壬��Ե�����
// Input:
//     IMG_RCOORD PointStart, �û��������
//     IMG_RCOORD PointEnd,  �û������յ�
//     IMG_RCOORD MaskEnd, ģ���յ�
//     IMG_RCOORD MaskStart, ģ�����
//     ROTATIONOPTIONS Rotation, ��ת����
//     RectScanDirection scanDirect, ɨ�跽��
//     EdgeDirection edDirectint, ��Ե����
//     edgeInformation *EdgePoint, ��Ե��
//     IMG_INT EdgePointNumber, ��Ե�����
// Output:
//     vector<edgeInformation> &PointFilterResult, ���˺��ļ���
// Return:
//     0 - ��ת����NOROTAION ���߱�Ե����BOTH
//     1 - ����
// Author: Ling Tan/4/6/2017
/**********************************************/
IMG_INT CBmDUTSocketLocation::EDGEPOINT_FILTER_MASK(IMG_RCOORD PointStart, IMG_RCOORD PointEnd, IMG_RCOORD MaskEnd, IMG_RCOORD MaskStart, RectScanDirection scanDirect, EdgeDirection edDirectint, edgeInformation *EdgePoint, IMG_INT EdgePointNumber, vector<edgeInformation> &PointFilterResult)
{


	_try{
	//	edgeInformation temp;
	IMG_INT LinePhase;//0,lefthand 180,righthand
	IMG_REAL AngleTol = InputPara.PointFilterAngleTolerance;
	if (edDirectint == BOTH)
	{
		for (IMG_INT i = 0; i < EdgePointNumber; i++)
		{
			if (EdgePoint[i].xyDecimal.x > MaskEnd.x && EdgePoint[i].xyDecimal.x < MaskStart.x)
			{
				if ((EdgePoint[i].angle >(270 - AngleTol) && EdgePoint[i].angle < (270 + AngleTol))
					|| (EdgePoint[i].angle < (90 + AngleTol) && EdgePoint[i].angle >(90 - AngleTol)))
					PointFilterResult.push_back(EdgePoint[i]);
			}
		}
		return 0;
	}
	//get linephase subject to rotation direction

	//if (PointEnd.x > PointStart.x)
	//{
	//	if ((scanDirect == LEFT && edDirectint == BLACKWHITE) || (scanDirect == RIGHT && edDirectint == WHITEBLACK))
	//		LinePhase = 3;//lefthand
	//	else
	//		LinePhase = 2;//righthand
	//}
	//else
	///{
		if ((scanDirect == LEFT && edDirectint == BLACKWHITE) || (scanDirect == RIGHT && edDirectint == WHITEBLACK))
			LinePhase = 2;
		else
			LinePhase = 3;
//	}


	//0-vertical lefthand, 1-v r, 2-Horizontal lefthand, 3-H r
	switch (LinePhase)
	{
	case 2:
		for (IMG_INT i = 0; i < EdgePointNumber; i++)
		{
			if (EdgePoint[i].xyDecimal.x > MaskEnd.x || EdgePoint[i].xyDecimal.x < MaskStart.x)
			{
				if (EdgePoint[i].angle >(270 - AngleTol) && EdgePoint[i].angle < (270 + AngleTol))
					PointFilterResult.push_back(EdgePoint[i]);
			}
		}
		break;
	case 3:
		for (IMG_INT i = 0; i < EdgePointNumber; i++)
		{
			if (EdgePoint[i].xyDecimal.x > MaskEnd.x || EdgePoint[i].xyDecimal.x < MaskStart.x)
			{
				if (EdgePoint[i].angle < (90 + AngleTol) && EdgePoint[i].angle >(90 - AngleTol))
					PointFilterResult.push_back(EdgePoint[i]);
			}
		}
		break;
	default:
		break;
	}
	return 1;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}
IMG_INT CBmDUTSocketLocation::EDGEPOINT_FILTER_MASK(IMG_RCOORD PointStart, IMG_RCOORD PointEnd, IMG_RCOORD MaskEnd, IMG_RCOORD MaskStart, vector<edgeInformation>InitialCo, vector<edgeInformation> &PointFilterResult)
{
	//_try{
	for (IMG_INT i = 0; i < InitialCo.size(); i++)
		{
			if (InitialCo[i].xyDecimal.x > MaskEnd.x || InitialCo[i].xyDecimal.x < MaskStart.x)
			{
				PointFilterResult.push_back(InitialCo[i]);
			}
		}
	return 1;
	//}
	//	__except (EXCEPTION_EXECUTE_HANDLER)
	//{
	//	return _CODE_THROW;
	//}
}


/**********************************************/
// HOUGH_LINE, ���������ֱ��
// Input:
//     IMG_REAL rAngle, ��ת�Ƕ�
//     IMG_SIZE pSrc, ROIͼƬ��С
//     vector<edgeInformation> EdgeDetecResult, ��Ե�����
//     IMG_REAL HoughAcceptScore, ��������ֵ�ٷְ�
//     IMG_REAL AngleTol, �����Ƕȷ�Χ
// Output:
//     vector<LINE_INFO> &HoughResult_Slope; ��⵽��ֱ�߼���
// Return:
//     No Return
// Author: Ling Tan/3/6/2017
/**********************************************/
IMG_VVOID CBmDUTSocketLocation::HOUGH_LINE(IMG_REAL rAngle, IMG_SIZE pSrc, vector<LINE_INFO> &HoughResult_Slope, vector<edgeInformation> EdgeDetecResult, IMG_REAL HoughAcceptScore, IMG_REAL AngleTol)
{
	
	LINE_INFO Line;
	IMG_RCOORD pSrcCenter;
	IMG_REAL AngleIncre, Sin, Cos, VoteRo, PointThd, Angle, AngleSearchStart, EdgeMax;
	IMG_REAL *VoteNum;
	IMG_INT RoMax, Width, Height, Length, TempCo, FloorCo, CeilCo;


	AngleSearchStart = 90.f;

	PointThd = HoughAcceptScore / 100.f * pSrc.width;
	if (pSrc.width > pSrc.height)
		EdgeMax = pSrc.width;
	else
		EdgeMax = pSrc.height;

	RoMax = (pow(pow(pSrc.height - 1, 2) + pow(pSrc.width - 1, 2), 0.5)) / 2;
	AngleIncre = atan2(2.f, RoMax) / PI * 180.f;
	Width = RoMax * 2 + 2 + 1;
	Height = AngleTol * 2 / AngleIncre + 1 + 2;
	Length = Width * Height;
	pSrcCenter.x = (IMG_REAL)(pSrc.width - 1) / 2;
	pSrcCenter.y = (IMG_REAL)(pSrc.height - 1) / 2;

	//VoteNum = (IMG_REAL*)malloc(Length * sizeof(IMG_REAL));
	VoteNum = new IMG_REAL[Length];
	//step: initialing votespace
	for (IMG_INT i = 0; i < Length; i++)
	{
		VoteNum[i] = 0;
	}

	//step: vote
	for (IMG_INT i = 1; i < Height - 1; i++)
	{
		Sin = sin((IMG_REAL)(AngleSearchStart - AngleTol + (i - 1) * AngleIncre) / 180 * PI);
		Cos = cos((IMG_REAL)(AngleSearchStart - AngleTol + (i - 1) * AngleIncre) / 180 * PI);
		for (IMG_INT j = 0; j < EdgeDetecResult.size(); j++)
		{
			VoteRo = ((IMG_REAL)EdgeDetecResult[j].xyDecimal.x - pSrcCenter.x) * Cos + ((IMG_REAL)EdgeDetecResult[j].xyDecimal.y - pSrcCenter.y) * Sin;
			FloorCo = floor(VoteRo), CeilCo = ceil(VoteRo);
			VoteNum[Width * i + RoMax + 1 + FloorCo] += CeilCo - VoteRo;
			VoteNum[Width * i + RoMax + 1 + CeilCo] += VoteRo - FloorCo;
		}
	}

	// step: find local maximum
	for (IMG_INT i = 1; i < Height - 1; i++)
	{
		for (IMG_INT j = 1; j < Width - 1; j++)
		{
			TempCo = i * Width + j;
			if (VoteNum[TempCo] >= PointThd && VoteNum[TempCo] > VoteNum[TempCo - 1] &&
				VoteNum[TempCo] >= VoteNum[TempCo + 1] && VoteNum[TempCo] > VoteNum[TempCo - Width] &&
				VoteNum[TempCo] >= VoteNum[TempCo + Width])
			{
				Angle = (AngleSearchStart - AngleTol + (i - 1.f) * AngleIncre) / 180 * PI;
				Line.Slope = -cos(Angle) / sin(Angle);
				Line.b = (IMG_REAL)(j - RoMax - 1) / sin(Angle) + 1.f / tan(Angle) * pSrcCenter.x + pSrcCenter.y; //move the coordinate origin from the center of Image to the left of Image
				HoughResult_Slope.push_back(Line);
			}
		}
	}

	delete[] VoteNum;
	///////////////////////////////test///////////////////////////////////////////////////////////
	//IplImage *Img1 = cvCreateImage({ pSrc->width, pSrc->height }, IPL_DEPTH_8U, 1); //����1ͨ��ͼ��  
	//for (int i = 0; i < pSrc->height * pSrc->width; i++)
	//{
	//	*(Img1->imageData + i) = 0;
	//}
	//for (int i = 0; i < EdgeDetecResult.size(); i++)
	//{
	//	*(Img1->imageData + EdgeDetecResult[i].xyInteger.y * Img1->widthStep + EdgeDetecResult[i].xyInteger.x) = EdgeDetecResult[i].gradient;
	//}
	//IplImage *Img3 = cvCreateImage(cvGetSize(Img1), IPL_DEPTH_8U, 3); //����3ͨ��ͼ��  

	//cvCvtColor(Img1, Img3, CV_GRAY2BGR);//��ͨ��ת3ͨ��  
	//for (int i = 0; i < HoughResult.size(); i++)
	//{
	//	float x, x1;
	//	int y, y1;

	//	x = -pSrc->width / 2 + 1, x1 = pSrc->width / 2 - 1;
	//	y = cvRound(HoughResult[i].roRadius - x * cos(HoughResult[i].theta / 180 * PI)) / sin(HoughResult[i].theta / 180 * PI);
	//	y1 = cvRound(HoughResult[i].roRadius - x1 * cos(HoughResult[i].theta / 180 * PI)) / sin(HoughResult[i].theta / 180 * PI);


	//	cvLine(Img3, cvPoint(x + pSrcCenter.x, y + pSrcCenter.y), cvPoint(x1 + pSrcCenter.x, y1 + pSrcCenter.y), CV_RGB(255, 0, 0), 1);

	//}
	////cvLine(Img2, cvPoint(20, 20), cvPoint(250, 250), CV_RGB(255, 0, 0), 1);
	//cvShowImage("kuku", Img3);

	//cvSaveImage("fittingResultHough.bmp", Img3);

	////////////////////////////////////////////////////////////////////////////////////////

}
/**********************************************/
// OVERLAPLINE_REJECT, �ų��ظ�ֱ��
// Input:
//     vector<LINE_INFO> &FittingResult��ֱ����Ͻ��
// Output:
//     vector<LINE_INFO> &FittingResult��ֱ����Ͻ��
// Return:
//     No Return
// Author: Ling Tan/3/6/2017
/**********************************************/
IMG_VVOID CBmDUTSocketLocation::OVERLAPLINE_REJECT(vector<LINE_INFO> &FittingResult)
{
	for (int i = 0; i < FittingResult.size(); i++)
	{
		for (int j = i + 1; j < FittingResult.size(); j++)
		{
			if (abs(FittingResult[i].Slope - FittingResult[j].Slope) <= 0.01 && abs(FittingResult[i].b - FittingResult[j].b) <= 0.1)
			{
				FittingResult.erase(FittingResult.begin() + j);
				j--;
			}
		}
	}
}

/**********************************************/
// LINE_FITTING, ֱ�����  
// Input:
//     vector<LINE_INFO> OriginLine, ֱ�߼����
//     vector<edgeInformation>   EdgeDetecResult, ��Ե�㼯�� 
//     IMG_SIZE SrcSize, ԭͼ��С
//     IMG_REAL Sigma, ���Ȳ���
// Output:
//     vector<LINE_INFO> &FittingResult,ֱ����Ͻ��
// Return:
//     NONE
// Author: Ling Tan/4/6/2017
/**********************************************/
IMG_INT CBmDUTSocketLocation::LINE_FITTING(vector<LINE_INFO> &FittingResult, vector<LINE_INFO> OriginLine, vector<edgeInformation>   EdgeDetecResult, IMG_SIZE SrcSize, IMG_REAL Sigma)
{
	//_try{
	IMG_REAL *MatA, *MatACopy, *MatB, *MatC, *MatCCopy, *Weight, *ErrorRecord;
	IMG_REAL Denominator;
	IMG_INT ErrorMaxTimes = 20, ErrorTimes, Length;
	LINE_INFO TempLine;
	IMG_INT status;

	if (EdgeDetecResult.size() <= 5)
		return 0;

	MatA = new IMG_REAL[2 * EdgeDetecResult.size()];
	MatB = new IMG_REAL[2];
	MatC = new IMG_REAL[EdgeDetecResult.size()];
	MatACopy = new IMG_REAL[2 * EdgeDetecResult.size()];
	MatCCopy = new IMG_REAL[EdgeDetecResult.size()];
	Weight = new IMG_REAL[EdgeDetecResult.size()];
	ErrorRecord = new IMG_REAL[ErrorMaxTimes * 3];

	for (IMG_INT i = 0; i < OriginLine.size(); i++)
	{
		Length = 0;
		Denominator = pow(OriginLine[i].Slope * OriginLine[i].Slope + 1, 0.5);

		MatB[0] = OriginLine[i].Slope;
		MatB[1] = OriginLine[i].b;

		//step: choose points

		for (IMG_INT j = 0; j < EdgeDetecResult.size(); j++)
		{

			if (abs(OriginLine[i].Slope * EdgeDetecResult[j].xyDecimal.x - EdgeDetecResult[j].xyDecimal.y + OriginLine[i].b) / Denominator <= Sigma)
			{
				MatA[Length * 2 + 0] = EdgeDetecResult[j].xyDecimal.x;
				MatA[Length * 2 + 1] = 1;
				MatC[Length] = EdgeDetecResult[j].xyDecimal.y;

				Length++;
			}

		}
		if (Length <= 5)
		{
			status = 0;
			goto END;
		}
		for (IMG_INT j = 0; j < Length; j++)
		{
			Weight[j] = 1;
			MatCCopy[j] = MatC[j];

		}

		//step: fitting 
		ErrorTimes = 0;
		while (1)
		{
			cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, Length, 1, 2, 1, MatA, 2, MatB, 1, -1, MatCCopy, 1); //calculate error
			ErrorRecord[ErrorTimes * 3 + 1] = MatB[0];
			ErrorRecord[ErrorTimes * 3 + 2] = MatB[1];
			for (IMG_INT j = 0; j < Length; j++)
			{
				ErrorRecord[ErrorTimes * 3] += MatCCopy[j];
			}
			ErrorRecord[ErrorTimes * 3] /= (IMG_REAL)Length;
			if (ErrorTimes >= 1 && abs(ErrorRecord[ErrorTimes * 3] - ErrorRecord[(ErrorTimes - 1) * 3]) <= 0.001)
			{
				TempLine.Slope = MatB[0];
				TempLine.b = MatB[1];
				TempLine.PointStart.x = 0;
				TempLine.PointStart.y = TempLine.PointStart.x * TempLine.Slope + TempLine.b;
				TempLine.FittingError = ErrorRecord[ErrorTimes * 3];
				
				TempLine.PointEnd.x = SrcSize.width - 1;
				TempLine.PointEnd.y = TempLine.PointEnd.x * TempLine.Slope + TempLine.b;
				
				FittingResult.push_back(TempLine);
				break;
			}
			if (ErrorTimes == ErrorMaxTimes - 1)
			{
				IMG_INT FLAG = 0;
				for (IMG_INT i = 0; i < ErrorMaxTimes - 1; i++)
				{
					if (ErrorRecord[3 * i] < ErrorRecord[3 * (i + 1)])
						ErrorRecord[3 * (i + 1)] = ErrorRecord[3 * i];
					else
						FLAG = i + 1;
				}

				TempLine.Slope = ErrorRecord[FLAG * 3 + 1];
				TempLine.b = ErrorRecord[FLAG * 3 + 2];
				TempLine.FittingError = ErrorRecord[FLAG * 3];
				TempLine.PointStart.x = 0;
				TempLine.PointStart.y = TempLine.PointStart.x * TempLine.Slope + TempLine.b;
				TempLine.PointEnd.x = SrcSize.width - 1;
				TempLine.PointEnd.y = TempLine.PointEnd.x * TempLine.Slope + TempLine.b;
				FittingResult.push_back(TempLine);
				break;
			}
			ErrorTimes++;
			for (IMG_INT j = 0; j < Length; j++)
			{
				MatACopy[j * 2 + 0] = MatA[j * 2 + 0];
				MatACopy[j * 2 + 1] = MatA[j * 2 + 1];

				Weight[j] = 1.0 / (1.f + exp(MatCCopy[j] * MatCCopy[j] / (Sigma * Sigma)));

				MatACopy[j * 2 + 0] = Weight[j] * MatACopy[j * 2 + 0];
				MatACopy[j * 2 + 1] = Weight[j] * MatACopy[j * 2 + 1];


				MatCCopy[j] = MatC[j] * Weight[j]; //reset MatCCopy after calculating error
			}
			LAPACKE_sgels(LAPACK_ROW_MAJOR, 'N', Length, 2, 1, MatACopy, 2, MatCCopy, 1);//fitting slope and b
			MatB[0] = MatCCopy[0];
			MatB[1] = MatCCopy[1];
			for (IMG_INT j = 0; j < Length; j++) //reset MatCCopy after fitting line
			{
				MatCCopy[j] = MatC[j];
			}
		}

	}
	//step: reject overlap
	OVERLAPLINE_REJECT(FittingResult);
	status = 1;
	END:
	delete[] MatCCopy;
	delete[] Weight;
	delete[] MatACopy;
	delete[] MatA;
	delete[] MatB;
	delete[] MatC;
	delete[] ErrorRecord;
	return status;


	 //}
	 //__except (EXCEPTION_EXECUTE_HANDLER)
	 //{
		// return _CODE_THROW;
	 //}





}


/**********************************************/
// VisOmmETEdgeLine_Scale, ����roi��С��ֱ�߼���ٻ�ԭͼ��ֱ�߾�ȷ���
// Input:
//     IMG_UBBUF src, ����ͼƬ
//     GenerateRectRoi roi, roi�����Ϣ
//     RectScanDirection scanDirect, ɨ�跽��
//     EdgeDirection edDirectint, ��Ե����
//     IMG_INT threshold, �ݶ�ǿ����ֵ
// Output:
//      LINE_INFO &FinalResult; ��⵽��ֱ��
//     IMG_REAL &B, ��Ϻ�Ľؾ�
// Return:
//     1 - ���� 
//     ���� - �쳣
// Author: Ling Tan/6/4/2017
/**********************************************/

IMG_INT CBmDUTSocketLocation::VisOmmETEdgeLine_Scale(IMG_UBBUF src, GenerateRectRoi roi, RectScanDirection scanDirect, EdgeDirection edDirectint, IMG_INT threshold, LINE_INFO &FinalResult)
{
//	_try{
	//edge
	IMG_INT status;
	vector<LINE_INFO> FirstFittingResult;
	vector<LINE_INFO> FinalFittingResult;
	vector<HOUGH_TRANS_INFO> HoughResult;
	vector<LINE_INFO> HoughResult_Slope;
	IMG_UBBUF pubDst, pubDst_Scale;
	RecRoiStruct RoiStruct;
	IMG_REAL HoughAcceptScore = InputPara.HoughAcceptScore;
	IMG_INT Flag = VisRoiGnerate_StraightLine(src, roi, pubDst, RoiStruct);
	if (Flag == 1)
	{
		status = 3;
		return 3;
	}
	//ofstream f2("ROI.txt");
	//for (int i = 0; i < pubDst.size.height; i++)
	//{
	//	for (int j = 0; j < pubDst.size.width; j++)
	//	{
	//		f2 << (int)pubDst.ptr[i * pubDst.size.width + j] << " ";
	//	}
	//	f2 << endl;
	//}
	//f2.close();


	IMG_RCOORD pdst_scale_center_co;
	IMG_SIZE pdst_scale_size;
	pubDst_Scale.size.width = round(pubDst.size.width / 2);
	pubDst_Scale.size.height = round(pubDst.size.height / 2);
	pdst_scale_center_co.x = (pubDst_Scale.size.width - 1) / 2;
	pdst_scale_center_co.y = (pubDst_Scale.size.height - 1) / 2;
	pdst_scale_size = pubDst_Scale.size;
	pubDst_Scale.linestep = pubDst_Scale.size.width;
	pubDst_Scale.ptr = new IMG_UBYTE[pubDst_Scale.size.width * pubDst_Scale.size.height];
	RTS_R_8uC1(pubDst, { ((IMG_REAL)pubDst.size.width - 1) / 2,((IMG_REAL)pubDst.size.height - 1) / 2 }, pubDst_Scale, pdst_scale_center_co, 0, pdst_scale_size, 2);
	/*ofstream f1("edgetection.txt");
	for (int i = 0; i < pubDst_Scale.size.height; i++)
	{
	for (int j = 0; j < pubDst_Scale.size.width; j++)
	{
	f1 << (int)pubDst_Scale.ptr[i * pubDst_Scale.size.width + j] << " ";
	}
	f1 << endl;
	}
	f1.close();*/
	//ofstream f1("edgetection.txt");
	//for (int i = 0; i < pubDst.size.height; i++)
	//{
	//	for (int j = 0; j < pubDst.size.width; j++)
	//	{
	//		f1 << (int)pubDst.ptr[i * pubDst.size.width + j] << " ";
	//	}
	//	f1 << endl;
	//}
	//f1.close();


	
	
	
	vector<edgeInformation> InitialCo;
	IMG_INT *binaryPsrc;

	binaryPsrc = new IMG_INT[pubDst_Scale.size.width * pubDst_Scale.size.height];
	IMG_INT GraComReturn = GradientCompute(pubDst_Scale.ptr, pubDst_Scale.size.width , pubDst_Scale.size.height, InputPara.edgeMaskRowSize, InputPara.edgeMaskColSize, threshold, binaryPsrc, InitialCo);
	if (GraComReturn == -1)
	{
		status = 0;
		delete[] binaryPsrc;
		delete[] pubDst_Scale.ptr;
		delete[] pubDst.ptr;
		return status;
	}
	
	/*for (IMG_INT i = 0; i < pubDst.size.height * pubDst.size.width; i++)
	{
	RoiBinary[i] = 0;
	}
	for (IMG_INT i = 0; i < EdgeDetecFilter.size(); i++)
	{
	RoiBinary[EdgeDetecFilter[i].xyInteger.x + EdgeDetecFilter[i].xyInteger.y * pubDst.size.width] = 255;
	}
	ofstream f1("edgetection.txt");
	for (int i = 0; i < pubDst.size.height; i++)
	{
	for (int j = 0; j < pubDst.size.width; j++)
	{
	f1 << (int)RoiBinary[i * pubDst.size.width + j] << " ";
	}
	f1 << endl;
	}
	f1.close();*/



	//ofstream f1("edgetection.txt");
	//for (int i = 0; i < pubDst_Scale.size.height; i++)
	//{
	//for (int j = 0; j < pubDst_Scale.size.width; j++)
	//{
	//f1 << (int)RoiBinary_scale[i * pubDst_Scale.size.width + j] << " ";
	//}
	//f1 << endl;
	//}
	//f1.close();




	//for (IMG_INT i = 0; i < pubDst_Scale.size.height * pubDst_Scale.size.width; i++)
	//{
	//RoiBinary_scale[i] = 0;
	//}
	//for (IMG_INT i = 0; i < EdgeDetecFilter_scale.size(); i++)
	//{
	//RoiBinary_scale[EdgeDetecFilter_scale[i].xyInteger.x + EdgeDetecFilter_scale[i].xyInteger.y * pubDst_Scale.size.width] = 255;
	//}
	//ofstream f3("edgetectionf.txt");
	//for (int i = 0; i < pubDst_Scale.size.height; i++)
	//{
	//for (int j = 0; j < pubDst_Scale.size.width; j++)
	//{
	//f3 << (int)RoiBinary_scale[i * pubDst_Scale.size.width + j] << " ";
	//}
	//f3 << endl;
	//}
	//f3.close();


	//	HOUGH_LINE(RoiStruct.RotationAngle, { pubDst.size.width,pubDst.size.height }, HoughResult_Slope, EdgeDetecFilter, HoughAcceptScore, 10);
	while (1)
	{
		HOUGH_LINE(RoiStruct.RotationAngle, { pubDst_Scale.size.width,pubDst_Scale.size.height }, HoughResult_Slope, InitialCo, HoughAcceptScore, InputPara.HoughAngelTolerance);
		if (HoughResult_Slope.size() == 0)
		{
			if (HoughAcceptScore < InputPara.HoughScoreGap)
			{
				status = 0;
				delete[] binaryPsrc;
				delete[] pubDst_Scale.ptr;
				delete[] pubDst.ptr;
				return status;
			}
			else
				HoughAcceptScore = HoughAcceptScore - InputPara.HoughScoreGap;
		}
		else
			break;
	}
	if(HoughResult_Slope.size()<1)
	{
		status = 0;


		delete[] pubDst_Scale.ptr;
		delete[] binaryPsrc;
		delete[] pubDst.ptr;
		return status;

	}
	IMG_INT lineFitReturn = LINE_FITTING(FirstFittingResult, HoughResult_Slope, InitialCo, { pubDst_Scale.size.width,pubDst_Scale.size.height }, InputPara.FittingSigmaFirst);
	if(lineFitReturn == 0)
	{
		status = 0;
		delete[] pubDst_Scale.ptr;
		delete[] binaryPsrc;
		delete[] pubDst.ptr;
		return status;

	}
	lineFitReturn = LINE_FITTING(FinalFittingResult, FirstFittingResult, InitialCo, { pubDst_Scale.size.width,pubDst_Scale.size.height }, InputPara.FittingSigmaSecond);
	if (lineFitReturn == 0)
	{
		status = 0;
		delete[] pubDst_Scale.ptr;
		delete[] binaryPsrc;
		delete[] pubDst.ptr;
		return status;

	}

	IMG_INT ScoreReturn = Score(FinalFittingResult, InitialCo, InputPara.ScoreAccptDis);
	if (ScoreReturn == 0)
	{
		status = 0;

	
		delete[] pubDst_Scale.ptr;
		delete[] binaryPsrc;
		delete[] pubDst.ptr;
		return status;

	}
	if (FinalFittingResult.size() > 1)
	{
		for (IMG_INT i = 0; i < FinalFittingResult.size(); i++)
		{
			if (FinalFittingResult[i].Score >= FinalFittingResult[i + 1].Score)
			{
				FinalFittingResult.erase(FinalFittingResult.begin() + i + 1);
				i--;
			}
			else
			{
				FinalFittingResult.erase(FinalFittingResult.begin() + i);
				i--;
			}
			if (FinalFittingResult.size() == 1)
				break;
		}
	}

	InitialCo.clear();
	int *binaryNew;
	binaryNew = new IMG_INT[pubDst.size.width * pubDst.size.height];
	GraComReturn = GradientCompute(pubDst.ptr,pubDst.size.width, pubDst.size.height, InputPara.edgeMaskRowSize, InputPara.edgeMaskColSize, threshold, binaryNew, InitialCo);
	if (GraComReturn == -1)
	{
		status = 0;
		delete[] binaryPsrc;
		delete[] binaryNew;
		delete[] pubDst_Scale.ptr;
		delete[] pubDst.ptr;
		return status;
	}

	

	vector<LINE_INFO> Src_FittingResult;
	FinalFittingResult[0].b = FinalFittingResult[0].b * 2;
	lineFitReturn = LINE_FITTING(Src_FittingResult, FinalFittingResult, InitialCo, { pubDst.size.width,pubDst.size.height }, InputPara.FittingSigmaFirst/1.5);
	if (lineFitReturn == 0)
	{
		status = 0;
		delete[] binaryPsrc;
		delete[] binaryNew;
		delete[] pubDst_Scale.ptr;
		delete[] pubDst.ptr;
		return status;

	}
	FinalFittingResult.erase(FinalFittingResult.begin());
	lineFitReturn = LINE_FITTING(FinalFittingResult, Src_FittingResult, InitialCo, { pubDst.size.width,pubDst.size.height }, InputPara.FittingSigmaSecond/1.5);
	if (lineFitReturn == 0)
	{
		status = 0;
		delete[] binaryPsrc;
		delete[] binaryNew;
		delete[] pubDst_Scale.ptr;
		delete[] pubDst.ptr;
		return status;

	}
	ROI_ROTATION_COORDINATE_COMPUTE_IN_SOURCEIMAG_SL(RoiStruct.RotationAngle, RoiStruct.SrcCenterCo, RoiStruct.DstCenterCo, FinalFittingResult[0].PointStart, FinalResult.PointStart);
	ROI_ROTATION_COORDINATE_COMPUTE_IN_SOURCEIMAG_SL(RoiStruct.RotationAngle, RoiStruct.SrcCenterCo, RoiStruct.DstCenterCo, FinalFittingResult[0].PointEnd, FinalResult.PointEnd);
	FinalResult.Slope = (FinalResult.PointStart.y - FinalResult.PointEnd.y) / (FinalResult.PointStart.x - FinalResult.PointEnd.x);
	FinalResult.b = FinalResult.PointStart.y - FinalResult.PointStart.x * FinalResult.Slope;

	status = 1;
	delete[] binaryPsrc;
	delete[] binaryNew;
	delete[] pubDst_Scale.ptr;
	delete[] pubDst.ptr;
	return status;
	/* }
	 __except (EXCEPTION_EXECUTE_HANDLER)
	 {
		 return _CODE_THROW;
	 }*/

}

/**********************************************/
// Score, ֱ�ߴ��
// Input:
//    std::vector<LINE_INFO> &ScoreResult, ֱ�߼����
//    std::vector<edgeInformation> EdgeDetecResult, ��Ե�����
//    IMG_REAL ScoreAcceptDistance, ��Ч����ܾ���
// Output:
//    std::vector<LINE_INFO> &ScoreResult, ��ֺ�Ľ��
// Return:
//     1 - ���� 
//     ���� - �쳣
// Author: Ling Tan/6/4/2017
/**********************************************/

IMG_INT CBmDUTSocketLocation::Score(std::vector<LINE_INFO> &ScoreResult, std::vector<edgeInformation> EdgeDetecResult, IMG_REAL ScoreAcceptDistance)
{
	//_try{
	if (ScoreResult.size() < 1)
		return 0;
	IMG_REAL LineDis, Denominator, KBrecord, DenoSquare, ScoreSum = 0;
	IMG_INT PointCount = 0;
	IMG_REAL XDiff, YDiff;
	vector<IMG_INT> VoteNum;
	for (IMG_INT i = 0; i < ScoreResult.size(); i++)
	{
		
		ScoreSum = 0;
		PointCount = 0;
		XDiff = ScoreResult[i].PointStart.x - ScoreResult[i].PointEnd.x;
		YDiff = ScoreResult[i].PointStart.y - ScoreResult[i].PointEnd.y;
		LineDis = pow(XDiff * XDiff + YDiff * YDiff, 0.5);
		
	//	VoteNum = new IMG_INT[ceil(LineDis)];
		Denominator = pow(ScoreResult[i].Slope * ScoreResult[i].Slope + 1, 0.5);
		KBrecord = ScoreResult[i].Slope * ScoreResult[i].b;
		DenoSquare = Denominator * Denominator;
		for (IMG_INT j = 0; j < ceil(LineDis); j++)
			VoteNum.push_back(0);
		for (IMG_INT j = 0; j < EdgeDetecResult.size(); j++)
		{
			if (abs(ScoreResult[i].Slope * EdgeDetecResult[j].xyDecimal.x - EdgeDetecResult[j].xyDecimal.y + ScoreResult[i].b) / Denominator <= ScoreAcceptDistance)
			{
				IMG_REAL temp = (EdgeDetecResult[j].xyDecimal.x + ScoreResult[i].Slope * EdgeDetecResult[j].xyDecimal.y - KBrecord) / DenoSquare - ScoreResult[i].PointStart.x;
				if (temp < 0 || temp > ceil(LineDis))
					continue;
				else 
					VoteNum[(IMG_INT)temp] = 1;
			}
		}
		for (IMG_INT j = 0; j < ceil(LineDis); j++)
			ScoreSum += (IMG_REAL)VoteNum[j];
		ScoreResult[i].Score = (IMG_REAL)ScoreSum / ceil(LineDis) * 100;
		VoteNum.clear();
	}
	return 1;
	/*}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}*/
}
//1.2 ����ROI����

/**********************************************/
// ROI_ROTATION_COORDINATE_COMPUTE_IN_SOURCEIMAG_SL, ����˵��,Ӧ����ͼƬ��ת������ROI�ϵĵ����귵��ԭʼͼƬ��Ӧ�������
// Input:
//     IMG_REAL rAngle,��ת�Ƕ�
//     IMG_RCOORD SrcCenterCo,ԭʼͼƬroi�����Ӧ�����ĵ�����
//     IMG_RCOORD DstCenterCo,roi��������ĵ�����
//     IMG_RCOORD RoiCo, roi���������
// Output:
//     IMG_RCOORD &RealCo, ԭʼͼƬ������
// Return:
//     0 - ����
// Author: Ling Tan/3/1/2017
/**********************************************/
IMG_INT CBmDUTSocketLocation::ROI_ROTATION_COORDINATE_COMPUTE_IN_SOURCEIMAG_SL(IMG_REAL rAngle, IMG_RCOORD SrcCenterCo, IMG_RCOORD DstCenterCo, IMG_RCOORD RoiCo, IMG_RCOORD &RealCo)
{
	_try{
	RealCo.x = RoiCo.x * cos(rAngle) - sin(rAngle) * RoiCo.y - (cos(rAngle) * DstCenterCo.x - sin(rAngle)* DstCenterCo.y - SrcCenterCo.x);
	RealCo.y = RoiCo.x * sin(rAngle) + cos(rAngle) * RoiCo.y + (-sin(rAngle) * DstCenterCo.x - cos(rAngle)* DstCenterCo.y + SrcCenterCo.y);
	return 0;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}
/**********************************************/
// DSP_RTS_R_8uC1, ����˵��,����תͼƬ��һ��������ˮƽ��ԭͼ��
// Input:
//     IMG_UBBUF prbSrc,����ͼ����ת�����ͼ��
//     IMG_RCOORD *prcoSrcCenter,ԭͼ�����Ȥ���������
//     IMG_RCOORD *prcoDstCenter,Ŀ��ͼ������ģ�δ����ת��ͼ��
//     IMG_REAL rAngle, ��ת��
//     IMG_SIZE *pszOp,Ŀ��ͼ���С
// Output:
//     IMG_UBBUF &prbDst, ��ת�����ͼƬ
// Return:
//     0 - ������������Խ�����⣬��ԭͼû���ҵ�������λ�ú�ɫ��䣩
// Author: Ling Tan/3/1/2017
/**********************************************/
IMG_WORD CBmDUTSocketLocation::DSP_RTS_R_8uC1(IMG_UBBUF prbSrc, IMG_RCOORD prcoSrcCenter, IMG_UBBUF &prbDst, IMG_RCOORD prcoDstCenter, IMG_REAL rAngle, IMG_SIZE pszOp)
{
	_try{
	//CVisMemPool pool;
	IMG_UBYTE *prSrc, *prDst, *pBuffer;
	IppiSize szSrc;
	IppiRect srcROI, dstROI;
	IppStatus sts;
	IMG_WORD wState = 0;//IMG_WORD wState = OK;
	int buffersize, srclinestep, dstlinestep;
	double coeff[2][4];



	coeff[0][0] = 0;
	coeff[0][1] = cos(rAngle);
	coeff[0][2] = -sin(rAngle);
	coeff[0][3] = -(cos(rAngle) * prcoDstCenter.x - sin(rAngle) * prcoDstCenter.y - prcoSrcCenter.x);

	coeff[1][0] = 0;
	coeff[1][1] = sin(rAngle);
	coeff[1][2] = cos(rAngle);
	coeff[1][3] = (-sin(rAngle) * prcoDstCenter.x - cos(rAngle) * prcoDstCenter.y + prcoSrcCenter.y);

	prSrc = prbSrc.ptr;
	szSrc.width = prbSrc.size.width;
	szSrc.height = prbSrc.size.height;

	srcROI.x = 0, srcROI.y = 0;
	srcROI.width = szSrc.width, srcROI.height = szSrc.height;

	prDst = prbDst.ptr;
	dstROI.x = 0;
	dstROI.y = 0;
	dstROI.width = pszOp.width;
	dstROI.height = pszOp.height;

	srclinestep = prbSrc.linestep * sizeof(IMG_UBYTE);
	dstlinestep = prbDst.linestep * sizeof(IMG_UBYTE);

	sts = ippiWarpBilinearGetBufferSize(szSrc, srcROI, dstROI, ippWarpBackward, coeff, IPPI_INTER_NN, &buffersize);
	pBuffer = (IMG_UBYTE*)malloc(sizeof(IMG_UBYTE)*buffersize);
	sts = ippiWarpBilinearBack_8u_C1R(prSrc, szSrc, srclinestep, srcROI, prbDst.ptr, dstlinestep, dstROI, coeff, IPPI_INTER_LINEAR, pBuffer);
	free(pBuffer);

	if (sts != ippStsOk)
	{
		wState = 1;
		//wState = DSP_ERR_FAIL_IN_IPPI;
		goto end;
	}
end:
	return wState;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
	}
/**********************************************/
// RTS_R_8uC1, ����˵��,��ת/�Ŵ���Сͼ��
// Input:
//     IMG_UBBUF prbSrc,����ͼ����ת�����ͼ��
//     IMG_RCOORD *prcoSrcCenter,ԭͼ�����Ȥ���������
//     IMG_RCOORD *prcoDstCenter,Ŀ��ͼ������ģ�δ����ת��ͼ��
//     IMG_REAL rAngle, ��ת��
//     IMG_SIZE *pszOp,Ŀ��ͼ���С
//     IMG_REAL scale, ���ű���
// Output:
//     IMG_UBBUF &prbDst, ��ת�����ͼƬ
// Return:
//     0 - ������������Խ�����⣬��ԭͼû���ҵ�������λ�ú�ɫ��䣩
// Author: Ling Tan/3/1/2017
/**********************************************/
IMG_WORD CBmDUTSocketLocation::RTS_R_8uC1(IMG_UBBUF prbSrc, IMG_RCOORD prcoSrcCenter, IMG_UBBUF &prbDst, IMG_RCOORD prcoDstCenter, IMG_REAL rAngle, IMG_SIZE pszOp, IMG_REAL scale)
{
	_try{
	//CVisMemPool pool;
	IMG_UBYTE *prSrc, *prDst, *pBuffer;
	IppiSize szSrc;
	IppiRect srcROI, dstROI;
	IppStatus sts;
	IMG_WORD wState = 0;//IMG_WORD wState = OK;
	int buffersize, srclinestep, dstlinestep;
	double coeff[2][4];

	coeff[0][0] = 0;
	coeff[0][1] = cos(rAngle) * scale;
	coeff[0][2] = -sin(rAngle) * scale;
	coeff[0][3] = -(cos(rAngle) * prcoDstCenter.x - sin(rAngle) * prcoDstCenter.y) * scale + prcoSrcCenter.x;

	coeff[1][0] = 0;
	coeff[1][1] = sin(rAngle) * scale;
	coeff[1][2] = cos(rAngle) * scale;
	coeff[1][3] = (-sin(rAngle) * prcoDstCenter.x - cos(rAngle) * prcoDstCenter.y) * scale + prcoSrcCenter.y;

	prSrc = prbSrc.ptr;
	szSrc.width = prbSrc.size.width;
	szSrc.height = prbSrc.size.height;

	srcROI.x = 0, srcROI.y = 0;
	srcROI.width = szSrc.width, srcROI.height = szSrc.height;

	prDst = prbDst.ptr;
	dstROI.x = 0;
	dstROI.y = 0;
	dstROI.width = pszOp.width;
	dstROI.height = pszOp.height;

	srclinestep = prbSrc.linestep * sizeof(IMG_UBYTE);
	dstlinestep = prbDst.linestep * sizeof(IMG_UBYTE);

	sts = ippiWarpBilinearGetBufferSize(szSrc, srcROI, dstROI, ippWarpBackward, coeff, IPPI_INTER_NN, &buffersize);
	pBuffer = new IMG_UBYTE[buffersize];
	sts = ippiWarpBilinearBack_8u_C1R(prSrc, szSrc, srclinestep, srcROI, prbDst.ptr, dstlinestep, dstROI, coeff, IPPI_INTER_LINEAR, pBuffer);


	delete[]pBuffer;

	if (sts != ippStsOk)
	{
		wState = 1;
		//wState = DSP_ERR_FAIL_IN_IPPI;
		goto end;
	}
end:
	return wState;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}



/**********************************************/
// ROI_HORIZONTAL_ROTATION, ����˵��,��ROI������ת��ˮƽ���������յ㹹�ɵķ���Ϊˮƽ���򣩣����ؾ���ͼƬ
// Input:
//     IMG_UBBUF pSrc,����ͼ��
//     IMG_RCOORD PointStart,����ˮƽ����ֱ�ߵ����
//     IMG_RCOORD PointEnd,����ˮƽ����ֱ�ߵ��յ�
//     IMG_REAL NormalVari, ���߷������������ROI����Ŀ�ȵ�һ�룩
// Output:
//     IMG_UBBUF &pubDst, ѡȡ�����ROIͼƬ
// Return:
//     0 - ����
//     1 - �û�ѡȡ��������Խ��
// Author: Ling Tan/3/1/2017
/**********************************************/
IMG_INT CBmDUTSocketLocation::ROI_HORIZONTAL_ROTATION(IMG_UBBUF pSrc, IMG_RCOORD PointStart, IMG_RCOORD PointEnd, IMG_REAL NormalVari, RecRoiStruct &RoiStruct, IMG_UBBUF &pubDst)
{
	_try{
	IMG_REAL rAngle;
	IMG_RCOORD prcoSrcCenter, prcoDstCenter;
	IMG_RCOORD PointCor[4];
	IMG_SIZE pszOp;
	IMG_INT DisPoint;

	rAngle = atan2((IMG_REAL)(PointEnd.y - PointStart.y), (IMG_REAL)(PointEnd.x - PointStart.x));
	prcoSrcCenter.x = (IMG_REAL)(PointEnd.x + PointStart.x) / 2;
	prcoSrcCenter.y = (IMG_REAL)(PointEnd.y + PointStart.y) / 2;
	DisPoint = ceil(sqrt(pow(PointEnd.y - PointStart.y, 2) + pow(PointEnd.x - PointStart.x, 2)));
	pszOp.width = DisPoint;
	pszOp.height = 1 + NormalVari * 2;
	prcoDstCenter.x = (IMG_REAL)(pszOp.width - 1) / 2;
	prcoDstCenter.y = (IMG_REAL)(pszOp.height - 1) / 2;


	IMG_REAL coeff[6];
	coeff[0] = cos(rAngle);
	coeff[1] = -sin(rAngle);
	coeff[2] = -(cos(rAngle) * prcoDstCenter.x - sin(rAngle) * prcoDstCenter.y - prcoSrcCenter.x);
	
	coeff[3] = sin(rAngle);
	coeff[4] = cos(rAngle);
	coeff[5] = (-sin(rAngle) * prcoDstCenter.x - cos(rAngle) * prcoDstCenter.y + prcoSrcCenter.y);

	PointCor[0].x = 0 * coeff[0] + 0 * coeff[1] + coeff[2];
	PointCor[0].y = 0 * coeff[3] + 0 * coeff[4] + coeff[5];

	PointCor[1].x = 0 * coeff[0] + (pszOp.height - 1) * coeff[1] + coeff[2];
	PointCor[1].y = 0 * coeff[3] + (pszOp.height - 1) * coeff[4] + coeff[5];

	PointCor[2].x = (pszOp.width - 1) * coeff[0] + 0 * coeff[1] + coeff[2];
	PointCor[2].y = (pszOp.width - 1) * coeff[3] + 0 * coeff[4] + coeff[5];

	PointCor[3].x = (pszOp.width - 1) * coeff[0] + (pszOp.height - 1) * coeff[1] + coeff[2];
	PointCor[3].y = (pszOp.width - 1) * coeff[3] + (pszOp.height - 1) * coeff[4] + coeff[5];

	if (PointCor[0].y > pSrc.size.height)
	{
		if (PointCor[2].y - pSrc.size.height > PointCor[0].y - pSrc.size.height)
			NormalVari = NormalVari - (IMG_INT)abs(cos(rAngle) * (pSrc.size.height - PointCor[2].y));
		else
			NormalVari = NormalVari - (IMG_INT)abs(cos(rAngle) * (pSrc.size.height - PointCor[0].y));
		pszOp.width = DisPoint;
		pszOp.height = 1 + NormalVari * 2;
		prcoDstCenter.x = (IMG_REAL)(pszOp.width - 1) / 2;
		prcoDstCenter.y = (IMG_REAL)(pszOp.height - 1) / 2;
	}
	else if (PointCor[2].y > pSrc.size.height)
	{
		NormalVari = NormalVari - (IMG_INT)abs(cos(rAngle) * (pSrc.size.height - PointCor[2].y));
		pszOp.width = DisPoint;
		pszOp.height = 1 + NormalVari * 2;
		prcoDstCenter.x = (IMG_REAL)(pszOp.width - 1) / 2;
		prcoDstCenter.y = (IMG_REAL)(pszOp.height - 1) / 2;
	}
	if (PointCor[1].y > pSrc.size.height || PointCor[1].y < 0 || PointCor[1].x > pSrc.size.width || PointCor[1].x < 0
		|| PointCor[3].y > pSrc.size.height || PointCor[3].y < 0 || PointCor[3].x > pSrc.size.width || PointCor[3].x < 0
		|| PointCor[0].x > pSrc.size.width || PointCor[0].x < 0|| PointCor[2].x > pSrc.size.width || PointCor[2].x < 0)
		return 1;




	//step: RTS ����תƽ��
	pubDst.ptr = new IMG_UBYTE[pszOp.width * pszOp.height];
	pubDst.size.width = pszOp.width;
	pubDst.size.height = pszOp.height;
	pubDst.linestep = pszOp.width;

	DSP_RTS_R_8uC1(pSrc, prcoSrcCenter, pubDst, prcoDstCenter, rAngle, pszOp);
	RoiStruct.SrcCenterCo.x = prcoSrcCenter.x;
	RoiStruct.SrcCenterCo.y = prcoSrcCenter.y;
	RoiStruct.DstCenterCo.x = prcoDstCenter.x;
	RoiStruct.DstCenterCo.y = prcoDstCenter.y;
	RoiStruct.RectangleSize.width = pszOp.width;
	RoiStruct.RectangleSize.height = pszOp.height;
	RoiStruct.RotationAngle = rAngle;
	ROI_ROTATION_COORDINATE_COMPUTE_IN_SOURCEIMAG_SL(rAngle, prcoSrcCenter, prcoDstCenter, { 0.f,0.f }, RoiStruct.LeftVertex);
	return 0;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// VisRoiGnerate_StraightLine, ����˵��,����ֱ��ѡȡROI
// Input:
//     IMG_UBBUF pSrc,����ͼ��
//     GenerateRectRoi Input, ��������ṹ��
// Output:
//     IMG_UBBUF &pubDst, ѡȡ�����ROIͼƬ
// Return:
//     0 - ����
//     1 - �û�ѡȡ��������Խ��
// Author: Ling Tan/3/1/2017
/**********************************************/
IMG_INT CBmDUTSocketLocation::VisRoiGnerate_StraightLine(IMG_UBBUF pSrc, GenerateRectRoi Input, IMG_UBBUF &pubDst, RecRoiStruct &RoiStruct)
{
	_try{
	IMG_INT Count;
	//	IMG_UBBUF pubDst;

	Count = ROI_HORIZONTAL_ROTATION(pSrc, Input.PointStart, Input.PointEnd, Input.NormalVari, RoiStruct, pubDst);
	if (Count == 1)
		return 1;

	//ofstream f1("edgetection.txt");
	//for (int i = 0; i < medianDst.size.height; i++)
	//{
	//	for (int j = 0; j < medianDst.size.width; j++)
	//	{
	//		f1 << (int)medianDst.ptr[i * medianDst.size.width + j] << " ";
	//	}
	//	f1 << endl;
	//}
	//f1.close();

	//for (int i = 0; i < pubDst.size.height; i++)
	//{
	//	for (int j = 0; j < pubDst.size.width; j++)
	//	{
	//		if (pubDst.ptr[j + i * pubDst.size.width]>100)
	//			pubDst.ptr[j + i * pubDst.size.width] = 255;

	//		else if (pubDst.ptr[j + i * pubDst.size.width]<20)
	//			pubDst.ptr[j + i * pubDst.size.width] = 0;

	//		else
	//			pubDst.ptr[j + i * pubDst.size.width] = (int)((pubDst.ptr[j + i * pubDst.size.width] - 20) * 255.0 / 80);
	//	}
	//}



	return 0;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


IMG_VVOID CBmDUTSocketLocation::SobelFilter_8u16s_C1_3x3(IMG_UBYTE * pSrc, IppiSize roiSize, Ipp16s *&pDst, Ipp32f *&pAngle)
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

	pVert = new Ipp16s[bufLen];
	pHoriz = new Ipp16s[bufLen];
	//pAngle = new Ipp32f [bufLen];
	//	pDst = new Ipp16s[bufLen];

	ippiGradientVectorGetBufferSize(roiSize, mask, ipp16s, 1, &bufferSize);
	pBuffer = new Ipp8u[bufferSize];
	ippiGradientVectorSobel_8u16s_C1R(pSrc, srcStep, pVert, dstStep, pHoriz, dstStep, pDst, dstStep, pAngle, angleStep, roiSize, mask, normType, bordertype, NULL, pBuffer);
	delete[] pBuffer;
	delete[] pVert;
	delete[] pHoriz;
}

//����һ��ֵthreshold�����������䷽��
IMG_REAL CBmDUTSocketLocation::getIntraClassVariance(Ipp16s* src, int srcRows, int srcCols, int &varTh)//int &varian)
{
	_try{//intra-class variance
	float varian = 0;

	int sumPixel = srcRows*srcCols;
	int sumGrayValue = 0;
	int average = 0;

	int sumApixel = 0;
	double PA = 0;
	int sumAgray = 0;
	int averageA = 0;

	int sumBpixel = 0;
	double PB = 0;
	int sumBgray = 0;
	int averageB = 0;

	for (int i = 0; i < sumPixel; i++)
	{
		sumGrayValue = sumGrayValue + src[i];
		if (src[i] < varTh)
		{
			sumApixel++;
			sumAgray = sumAgray + src[i];
		}
	}

	average = sumGrayValue / sumPixel;
	PA = (double)sumApixel / sumPixel;
	if (sumApixel > 0)
	{
		averageA = sumAgray / sumApixel;
	}
	else
	{
		averageA = 0;
	}

	sumBpixel = sumPixel - sumApixel;
	PB = 1.0 - PA;
	sumBgray = sumGrayValue - sumAgray;
	if (sumBpixel > 0)
	{
		averageB = sumBgray / sumBpixel;
	}
	else
	{
		averageB = 0;
	}

	//ICV = PA?(MA?M)2 + PB?(MB?M)2
	varian = PA * (averageA - average) * (averageA - average) + PB * (averageB - average) * (averageB - average);

	return varian;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

IMG_INT CBmDUTSocketLocation::VisEdge_detection(IMG_UBYTE *srcRoi, IMG_SIZE roiSize, int threshold, IMG_WORD *&dstRoi, IMG_UBYTE *dstRoiE, edgeInformation *&edgeArray, IMG_INT &eNum)
{
//	_try{
	//�����ֵС��0������ֱ�ӷ���-1
	if (threshold < 0)
	{
		return -1;
	}

	int roiRows = roiSize.height;
	int roiCols = roiSize.width;

	//�Ƕ���Ϣ
	Ipp32f *angAll;
	angAll = new Ipp32f[roiRows*roiCols];

	std::vector<edgeInformation> edgeInfor;
	edgeInformation edInf;

	int k = 0;//��¼��Ե��ĸ���
	Ipp16u k1;//��������ϵ�������֪��
	Ipp16u k2;
	Ipp16u k3;
	float deci;//��������϶����С�����֣�����Ӧ��������
	float sumx = 0;//��Ե���x����֮��
	float sumy = 0;
	int numberChannels = 1; //the source image is single channel

	IppiSize dstRoiSize = { roiCols,roiRows };

	SobelFilter_8u16s_C1_3x3(srcRoi, dstRoiSize, dstRoi, angAll);


	//�ѽǶ���[-PI��PI]��Ϊ[0��360]
	/*for (int i = 0; i < roiRows; i++)
	{
	for (int j = 0; j < roiCols; j++)
	{
	angAll[j + i * roiCols] = (float)(180 - angAll[j + i * roiCols] / PI * 180);
	}
	}*/

	/*
	FILE *sx;
	sx = fopen("E:\\ProjectMy\\a1sobel.txt", "w");
	FILE *ang;
	ang = fopen("E:\\ProjectMy\\a1ang.txt", "w");
	for (int i = 0; i<roiRows; i++)
	{
	for (int j = 0; j < roiCols; j++)
	{
	fprintf(sx, "%d   ", dstRoi[j+i*roiCols]);
	fprintf(ang, "%f   ", angAll[j+i*roiCols]);
	}
	fprintf(sx,"\n");
	fprintf(ang, "\n");
	}
	fclose(sx);
	fclose(ang);
	*/
	//�Զ���ȡ�ݶ�ǿ�ȵ���ֵ
	//ʲô���������û��������ֵ�أ�
	if (threshold == 0)
	{
		//Otsu�����������еĻҶ�ֵ����1��255��ʹintra-class invariance�����Ǹ�ֵ����ΪҪ�����ֵ
		int varian = 0;
		int temp = 0;
		for (int p = 1; p < 1250; p++)
		{
			temp = getIntraClassVariance(dstRoi, roiRows, roiCols, p);
			if (varian < temp)
			{
				varian = temp;
				threshold = p;
			}
		}
	}
	//printf("%d\n",threshold);

	//��������
	for (int i = 1; i<roiRows - 1; i++)
	{
		for (int j = 1; j<roiCols - 1; j++)
		{
			if (dstRoi[j + i*roiCols] > threshold)
			{
				angAll[j + i * roiCols] = (float)180 - angAll[j + i * roiCols] / PI * 180;
				if ((angAll[j + i*roiCols]>22.5) && (angAll[j + i*roiCols] < 67.5))
				{
					if ((dstRoi[j + i*roiCols] > dstRoi[j - 1 + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j + 1) + (i + 1)*roiCols]))
					{
						k1 = dstRoi[j - 1 + (i - 1)*roiCols];
						k2 = dstRoi[j + i*roiCols];
						k3 = dstRoi[(j + 1) + (i + 1)*roiCols];
						deci = (k3 - k1) / ((float)2.0*(2.0*k2 - k1 - k3));


						edInf.xyInteger.x = j;
						edInf.xyInteger.y = i;
						edInf.xyDecimal.x = j + deci;
						edInf.xyDecimal.y = i + deci;
						edInf.gradient = dstRoi[j + i*roiCols];
						edInf.angle = angAll[j + i*roiCols];
						edgeInfor.push_back(edInf);
						k++;
					}
				}
				else
				{
					if ((angAll[j + i*roiCols] > 202.5) && (angAll[j + i*roiCols] < 247.5))
					{
						if ((dstRoi[j + i*roiCols] > dstRoi[j - 1 + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j + 1) + (i + 1)*roiCols]))
						{
							k3 = dstRoi[j - 1 + (i - 1)*roiCols];
							k2 = dstRoi[j + i*roiCols];
							k1 = dstRoi[(j + 1) + (i + 1)*roiCols];
							deci = (k3 - k1) / ((float)2.0*(2.0*k2 - k1 - k3));

							edInf.xyInteger.x = j;
							edInf.xyInteger.y = i;
							edInf.xyDecimal.x = j - deci;
							edInf.xyDecimal.y = i - deci;
							edInf.gradient = dstRoi[j + i*roiCols];
							edInf.angle = angAll[j + i*roiCols];
							edgeInfor.push_back(edInf);
							k++;
						}
					}
					else
					{
						if ((angAll[j + i*roiCols] > 112.5) && (angAll[j + i*roiCols] < 157.5))
						{

							if ((dstRoi[j + i*roiCols] > dstRoi[(j + 1) + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j - 1) + (i + 1)*roiCols]))
							{
								k1 = dstRoi[(j + 1) + (i - 1)*roiCols];
								k2 = dstRoi[j + i*roiCols];
								k3 = dstRoi[(j - 1) + (i + 1)*roiCols];
								deci = (k3 - k1) / ((float)2.0*(2.0*k2 - k1 - k3));

								edInf.xyInteger.x = j;
								edInf.xyInteger.y = i;
								edInf.xyDecimal.x = j - deci;
								edInf.xyDecimal.y = i + deci;
								edInf.gradient = dstRoi[j + i*roiCols];
								edInf.angle = angAll[j + i*roiCols];
								edgeInfor.push_back(edInf);
								k++;
							}
						}
						else
						{
							if ((angAll[j + i*roiCols] > 292.5) && (angAll[j + i*roiCols] < 337.5))
							{
								if ((dstRoi[j + i*roiCols] > dstRoi[(j + 1) + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j - 1) + (i + 1)*roiCols]))
								{
									k3 = dstRoi[(j + 1) + (i - 1)*roiCols];
									k2 = dstRoi[j + i*roiCols];
									k1 = dstRoi[(j - 1) + (i + 1)*roiCols];
									deci = (k3 - k1) / ((float)2.0*(2.0*k2 - k1 - k3));

									edInf.xyInteger.x = j;
									edInf.xyInteger.y = i;
									edInf.xyDecimal.x = j + deci;
									edInf.xyDecimal.y = i - deci;
									edInf.gradient = dstRoi[j + i*roiCols];
									edInf.angle = angAll[j + i*roiCols];
									edgeInfor.push_back(edInf);
									k++;
								}
							}
							else
							{
								if (((angAll[j + i*roiCols] >= -1) && (angAll[j + i*roiCols] <= 22.5)) || ((angAll[j + i*roiCols] >= 337.5) && (angAll[j + i*roiCols] <= 361)))
								{
									if ((dstRoi[j + i*roiCols] > dstRoi[(j - 1) + i*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j + 1) + i*roiCols]))
									{
										k1 = dstRoi[(j - 1) + i*roiCols];
										k2 = dstRoi[j + i*roiCols];
										k3 = dstRoi[(j + 1) + i*roiCols];
										deci = (k3 - k1) / ((float)2.0*(2.0*k2 - k1 - k3));

										edInf.xyInteger.x = j;
										edInf.xyInteger.y = i;
										edInf.xyDecimal.x = j + deci;
										edInf.xyDecimal.y = i;
										edInf.gradient = dstRoi[j + i*roiCols];
										edInf.angle = angAll[j + i*roiCols];
										edgeInfor.push_back(edInf);
										k++;
									}
								}
								else
								{
									if ((angAll[j + i*roiCols] <= 202.5) && (angAll[j + i*roiCols] >= 157.5))
									{
										if ((dstRoi[j + i*roiCols] > dstRoi[(j - 1) + i*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j + 1) + i*roiCols]))
										{
											k3 = dstRoi[(j - 1) + i*roiCols];
											k2 = dstRoi[j + i*roiCols];
											k1 = dstRoi[(j + 1) + i*roiCols];
											deci = (k3 - k1) / ((float)2.0*(2.0*k2 - k1 - k3));

											edInf.xyInteger.x = j;
											edInf.xyInteger.y = i;
											edInf.xyDecimal.x = j - deci;
											edInf.xyDecimal.y = i;
											edInf.gradient = dstRoi[j + i*roiCols];
											edInf.angle = angAll[j + i*roiCols];
											edgeInfor.push_back(edInf);
											k++;
										}
									}
									else
									{
										if ((angAll[j + i*roiCols] >= 67.5) && (angAll[j + i*roiCols] <= 112.5))
										{

											if ((dstRoi[j + i*roiCols] > dstRoi[j + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[j + (i + 1)*roiCols]))
											{
												k1 = dstRoi[j + (i - 1)*roiCols];
												k2 = dstRoi[j + i*roiCols];
												k3 = dstRoi[j + (i + 1)*roiCols];
												deci = (k3 - k1) / ((float)2.0*(2.0*k2 - k1 - k3));

												edInf.xyInteger.x = j;
												edInf.xyInteger.y = i;
												edInf.xyDecimal.x = j;
												edInf.xyDecimal.y = i + deci;
												edInf.gradient = dstRoi[j + i*roiCols];
												edInf.angle = angAll[j + i*roiCols];
												edgeInfor.push_back(edInf);
												k++;
											}
										}
										else
										{
											if ((angAll[j + i*roiCols] >= 247.5) && (angAll[j + i*roiCols] <= 292.5))
											{
												if ((dstRoi[j + i*roiCols] > dstRoi[j + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[j + (i + 1)*roiCols]))
												{
													k3 = dstRoi[j + (i - 1)*roiCols];
													k2 = dstRoi[j + i*roiCols];
													k1 = dstRoi[j + (i + 1)*roiCols];
													deci = (k3 - k1) / ((float)2.0*(2.0*k2 - k1 - k3));

													edInf.xyInteger.x = j;
													edInf.xyInteger.y = i;
													edInf.xyDecimal.x = j;
													edInf.xyDecimal.y = i - deci;
													edInf.gradient = dstRoi[j + i*roiCols];
													edInf.angle = angAll[j + i*roiCols];
													edgeInfor.push_back(edInf);
													k++;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}


	eNum = k;
	//��ֵͼ
	for (int t = 0; t < roiCols*roiRows; t++)//��ֵͼ�����������ȶ���ֵΪ0����Ե�㸳ֵ255
	{
		dstRoiE[t] = 0;
	}
	for (int q = 0; q < k; q++)
	{
		dstRoiE[edgeInfor[q].xyInteger.x + edgeInfor[q].xyInteger.y * roiCols] = 255;
	}
	/*
	FILE *Binary;
	Binary = fopen("E:\\ProjectMy\\Binary.txt", "w");
	for (int i = 0; i<roiRows; i++)
	{
	for (int j = 0; j < roiCols; j++)
	{
	fprintf(Binary, "%d   ", dstRoiE[j + i*roiCols]);
	}
	fprintf(Binary, "\n");
	}
	fclose(Binary);
	*/
	//������ķ�ʽ������Ե��Ϣ
	edgeArray = new edgeInformation[k];
	for (int i = 0; i < k; i++)
	{
		edgeArray[i] = edgeInfor[i];
	}


	/*FILE *e;
	e = fopen("E:\\project03\\e.txt", "w");
	for (int i = 0; i < k; i++)
	{
	fprintf(e,"%d   %d   \n", edgeInfor[i].xInteger, edgeInfor[i].yInteger);
	}
	*/

	delete[] angAll;
	return 1;
 //}
 //__except (EXCEPTION_EXECUTE_HANDLER)
 //{
	// return _CODE_THROW;
 //}
}
/**********************************************/
// VisDoubleRoiLineFit, ��������roi��Ϣ���ֱ��
// Input:
//     IMG_UBBUF pSrc,����ͼ��
//     IMG_UWORD width, ԭʼͼƬ���
//     LineIn lineInside1�� roi��Ϣ
//     IMG_REAL NormalVari, ���߷������������ROI����Ŀ�ȵ�һ�룩
// Output:
//     LINE_INFO &LineOutside�����ֱ��
// Return:
//     0 - �쳣
//     1 - ����
// Author: Ling Tan/4/6/2017
/**********************************************/
IMG_INT CBmDUTSocketLocation::VisDoubleRoiLineFit(IMG_UBBUF src, IMG_UWORD width, LineIn lineInside1, LINE_INFO &LineOutside)
{
	//_try{
	IMG_INT status;
	IMG_UBBUF dst_1;// dst_2;
	RecRoiStruct RoiStruct_1;// RoiStruct_2;
	dst_1.ptr = NULL;
	IMG_INT RoiReturn_1 = VisRoiGnerate_StraightLine(src, lineInside1.roiLine, dst_1, RoiStruct_1);

	//ofstream f2("fffffedgetection.txt");
	//for (int i = 0; i < dst_1.size.height; i++)
	//{
	//	for (int j = 0; j < dst_1.size.width; j++)
	//	{
	//		f2 << (int)dst_1.ptr[i * dst_1.size.width + j] << " ";
	//	}
	//	f2 << endl;
	//}
	//f2.close();
	if (RoiReturn_1 == 1)
	{
		free(dst_1.ptr);
		return 3;
	}
	vector<LINE_INFO> FirstFittingResult;
	vector<LINE_INFO> FinalFittingResult;
	IMG_REAL HoughAcceptScore;

	vector<HOUGH_TRANS_INFO> HoughResult;
	vector<LINE_INFO> HoughResult_Slope;
	vector<edgeInformation> PointFiltered_1;// PointFiltered_2;
	vector<edgeInformation> InitialCo;
	IMG_INT *binaryPsrc;

	binaryPsrc = new IMG_INT[dst_1.size.width * dst_1.size.height];
	IMG_INT GraComReturn = GradientCompute(dst_1.ptr,dst_1.size.width,dst_1.size.height,InputPara.edgeMaskRowSize,InputPara.edgeMaskColSize, lineInside1.threshold, binaryPsrc, InitialCo);
	/*ofstream f2("fffffedgetection.txt");
	for (int i = 0; i < dst_1.size.height; i++)
	{
		for (int j = 0; j < dst_1.size.width; j++)
		{
			f2 << (int)dst_1.ptr[i * dst_1.size.width + j] << " ";
		}
		f2 << endl;

	}
	f2.close();*/
	if (GraComReturn == -1)
	{
		status = 0;
		goto END;
	}
	
	//ofstream f2("fffffedgetection.txt");
	//for (int i = 0; i < dst_1.size.height; i++)
	//{
	//	for (int j = 0; j < dst_1.size.width; j++)
	//	{
	//		f2 << (int)RoiBinary_1[i * dst_1.size.width + j] << " ";
	//	}
	//	f2 << endl;
	//}
	//f2.close();
	lineInside1.MaskEnd.x = abs(lineInside1.MaskEnd.x - RoiStruct_1.LeftVertex.x);
	lineInside1.MaskStart.x = abs(lineInside1.MaskStart.x - RoiStruct_1.LeftVertex.x);
	IMG_REAL scale, gap;
	scale = (width - abs(lineInside1.MaskEnd.x - lineInside1.MaskStart.x)) / width;
	HoughAcceptScore = InputPara.HoughAcceptScore * scale;
	gap = InputPara.HoughScoreGap * scale;

	if (lineInside1.MaskEnd.x > lineInside1.MaskStart.x)
		EDGEPOINT_FILTER_MASK(lineInside1.roiLine.PointStart, lineInside1.roiLine.PointEnd, lineInside1.MaskEnd, lineInside1.MaskStart, InitialCo, PointFiltered_1);
	else
		EDGEPOINT_FILTER_MASK(lineInside1.roiLine.PointStart, lineInside1.roiLine.PointEnd, lineInside1.MaskStart, lineInside1.MaskEnd, InitialCo, PointFiltered_1);

	/*for (IMG_INT i = 0; i < dst_1.size.height * dst_1.size.width; i++)
	{
	RoiBinary_1[i] = 0;
	}
	for (IMG_INT i = 0; i < PointFiltered_1.size(); i++)
	{
	RoiBinary_1[PointFiltered_1[i].xyInteger.x + PointFiltered_1[i].xyInteger.y * dst_1.size.width] = 255;
	}
	ofstream f1("edgetection.txt");
	for (int i = 0; i < dst_1.size.height; i++)
	{
	for (int j = 0; j < dst_1.size.width; j++)
	{
	f1 << (int)RoiBinary_1[i * dst_1.size.width + j] << " ";
	}
	f1 << endl;
	}
	f1.close();*/

	while (1)
	{


		HOUGH_LINE(RoiStruct_1.RotationAngle, dst_1.size, HoughResult_Slope, PointFiltered_1, HoughAcceptScore, InputPara.HoughAngelTolerance);
		//ofstream f1("pointco.txt");
		///*for (int i = 0; i < PointFiltered_1.size(); i++)
		//{
		//	f1 << PointFiltered_1[i].xyDecimal.x << " " << PointFiltered_1[i].xyDecimal.y << endl;
		//}
		//f1.close();*/


		if (HoughResult_Slope.size() == 0)
		{
			if (HoughAcceptScore <= gap)
			{
				status = 0;
				goto END;
			}
			else
				HoughAcceptScore = HoughAcceptScore - gap;
		}
		else
			break;
	}
	LINE_FITTING(FirstFittingResult, HoughResult_Slope, PointFiltered_1, dst_1.size, InputPara.FittingSigmaFirst);
	LINE_FITTING(FinalFittingResult, FirstFittingResult, PointFiltered_1, dst_1.size, InputPara.FittingSigmaSecond);
	IMG_INT ScoreReturn = Score(FinalFittingResult, PointFiltered_1, InputPara.ScoreAccptDis);
	if (ScoreReturn == 0)
	{
		status = 0;
		goto END;
	}
	if (FinalFittingResult.size() > 1)
	{
		for (IMG_INT i = 0; i < FinalFittingResult.size(); i++)
		{
			if (FinalFittingResult[i].Score >= FinalFittingResult[i + 1].Score)
			{
				FinalFittingResult.erase(FinalFittingResult.begin() + i + 1);
				i--;
			}
			else
			{
				FinalFittingResult.erase(FinalFittingResult.begin() + i);
				i--;
			}
			if (FinalFittingResult.size() == 1)
				break;
		}
	}
	ROI_ROTATION_COORDINATE_COMPUTE_IN_SOURCEIMAG_SL(RoiStruct_1.RotationAngle, RoiStruct_1.SrcCenterCo, RoiStruct_1.DstCenterCo, FinalFittingResult[0].PointEnd, LineOutside.PointEnd);
	ROI_ROTATION_COORDINATE_COMPUTE_IN_SOURCEIMAG_SL(RoiStruct_1.RotationAngle, RoiStruct_1.SrcCenterCo, RoiStruct_1.DstCenterCo, FinalFittingResult[0].PointStart, LineOutside.PointStart);

	LineOutside.Slope = (LineOutside.PointEnd.y - LineOutside.PointStart.y) / (LineOutside.PointEnd.x - LineOutside.PointStart.x);
	LineOutside.b = LineOutside.PointEnd.y - LineOutside.Slope * LineOutside.PointEnd.x;
	status = 1;
END:
	
	delete[] binaryPsrc;
	delete[] dst_1.ptr;
	//	free(dst_2.ptr);
	return status;
	/* }
	 __except (EXCEPTION_EXECUTE_HANDLER)
	 {
		 return _CODE_THROW;
	 }*/
}

/**********************************************/
// CBmDUTTrayLocationLineDetection, ����˵��: ��ⷽ���ĸ����㣬�󶥵��߽���㵽һ���ߵ��е㡣
// Input:
// 	IMG_UBBUF src,����ͼƬ
//  IMG_UWORD Width,ԭͼ���
//  LineIn lineInside1, �ױߣ���Ҫ�����߱ߣ���roi��Ϣ
//  LineIn lineInside2, ȱ������roi��Ϣ
//  LineIn lineInside3, ��ֱ�ڵױ���roi��Ϣ
//  LineIn lineInside4, ��ֱ�ڵױ���roi��Ϣ
// Output:
//  IMG_REAL &RotationAngle, ��ת�Ƕ�
//  IMG_RCOORD PointCo[], �ĸ���������
//  IMG_RCOORD &CenterPoint, �ɶ���ȷ����ͼƬ��������
//  IMG_RCOORD &FinalPoint, �����е㵽ָ���ױߵ��е�
//     ...
// Return:
//     0 - ���ֱ��ʧ��
//     1 - ��������
//     2 - ֱ�߼нǳ�����׼��Χ
//     ...
/**********************************************/

int CBmDUTSocketLocation::CBmDUTTrayLocationLineDetection(IMG_UBBUF src, IMG_UWORD Width, LineIn lineInside1, LineIn lineInside2, LineIn lineInside3, LineIn lineInside4, IMG_RCOORD PointCo[], IMG_RCOORD &CenterPoint, IMG_RCOORD &FinalPoint, IMG_REAL &RotationAngle)
{
_try{
	LINE_INFO lineInfo[4];
	IMG_REAL Slope1, Slope2, B_1, B_2; //across line's slope and b 
	IMG_REAL xScale, yScale, DutWidth, DutHeigth;;
	IMG_REAL Angle[4], PointDis[4];
	xScale = src.size.width / 1624;
	yScale = src.size.height / 1228;
	DutWidth = xScale * 665.f;
	DutHeigth = yScale * 720.f;

	IMG_INT line1Return = VisDoubleRoiLineFit(src, Width, lineInside1, lineInfo[0]);


#ifdef DEBUG_SL
	ofstream fp("IA978_SL_DEBUG.txt", ios::app);

	fp << "LINE_1 FITTED BY DOUBLE ROI:" << endl;
	if (line1Return != 1)
		fp << "NO LINE DETECTED" << endl;
	else
		fp << "Equation:" << "y = " << lineInfo[0].Slope << "* x + " << lineInfo[0].b << endl;
	fp << endl;
#endif // DEBUG_SL
	IMG_INT line2Return = VisDoubleRoiLineFit(src, Width, lineInside2, lineInfo[1]);
	/*ofstream fp("c.txt", ios::app);
	fp << lineInfo[1].Slope << endl;
	fp << lineInfo[1].b << endl;
	fp.close();*/
#ifdef DEBUG_SL

	fp << "SECOND ROI PARALLEL TO FIRST LINE:" << endl;
	if (line2Return != 1)
		fp << "NO LINE DETECTED" << endl;
	else
		fp << "Equation:" << "y = " << lineInfo[1].Slope << "* x + " << lineInfo[1].b << endl;
	fp << endl;
#endif // DEBUG_SL



	IMG_INT line3Return = VisOmmETEdgeLine_Scale(src, lineInside3.roiLine, lineInside3.scanDirect, lineInside3.edDirectint, lineInside3.threshold, lineInfo[2]);
#ifdef DEBUG_SL

	fp << "THIRD ROI:" << endl;
	if (line3Return != 1)
		fp << "NO LINE DETECTED" << endl;
	else
		fp << "Equation:" << "y = " << lineInfo[2].Slope << "* x + " << lineInfo[2].b << endl;
	fp << endl;
#endif // DEBUG_SL
	IMG_INT line4Return = VisOmmETEdgeLine_Scale(src, lineInside4.roiLine, lineInside4.scanDirect, lineInside4.edDirectint, lineInside4.threshold, lineInfo[3]);
#ifdef DEBUG_SL

	fp << "LAST ROI PARALLEL TO THE THIRD LINE:" << endl;
	if (line4Return != 1)
		fp << "NO LINE DETECTED" << endl;
	else
		fp << "Equation:" << "y = " << lineInfo[3].Slope << "* x + " << lineInfo[3].b << endl;
	fp << endl;
	fp.close();
#endif // DEBUG_SL

	if (line2Return != 1 || line1Return != 1 || line3Return != 1 || line4Return != 1)
		return 0;







	Angle[0] = atan2(lineInfo[0].Slope,1) * 180 / PI;

	Angle[1] = atan2(lineInfo[1].Slope,1) * 180 / PI;

	Angle[2] = atan2(lineInfo[2].Slope,1) * 180 / PI;
	if (Angle[2] < 0)
		Angle[2] = Angle[2] + 180;

	Angle[3] = atan2(lineInfo[3].Slope,1) * 180 / PI;
	if (Angle[3] < 0)
		Angle[3] = Angle[3] + 180;
	
	//cout << Angle[0] << endl;
	//cout << Angle[1] << endl;
	//cout << Angle[2] << endl;
	//cout << Angle[3] << endl;






	if (abs(Angle[0] - Angle[1])> InputPara.angleControl)
		return 2;
	if (abs(Angle[2] - Angle[3])> InputPara.angleControl)
		return 2;
	if (abs(abs(Angle[0] - Angle[2]) - 90)> InputPara.angleControl)
		return 2;
	if (abs(abs(Angle[0] - Angle[3]) - 90)> InputPara.angleControl)
		return 2;
	if (abs(abs(Angle[1] - Angle[2]) - 90)> InputPara.angleControl)
		return 2;
	if (abs(abs(Angle[1] - Angle[3]) - 90)> InputPara.angleControl)
		return 2;



	if (lineInfo[2].PointStart.x == lineInfo[2].PointEnd.x)
	{
		PointCo[2].x = lineInfo[2].PointStart.x;
		PointCo[0].x = lineInfo[2].PointStart.x;
	}
	else
	{
		PointCo[0].x = (lineInfo[2].b - lineInfo[0].b) / (lineInfo[0].Slope - lineInfo[2].Slope);
		PointCo[2].x = (lineInfo[2].b - lineInfo[1].b) / (lineInfo[1].Slope - lineInfo[2].Slope);
	}
	PointCo[0].y = lineInfo[0].Slope * PointCo[0].x + lineInfo[0].b;
	PointCo[2].y = lineInfo[1].Slope * PointCo[2].x + lineInfo[1].b;

	if (lineInfo[3].PointStart.x == lineInfo[3].PointEnd.x)
	{
		PointCo[3].x = lineInfo[3].PointStart.x;
		PointCo[1].x = lineInfo[3].PointStart.x;
	}
	else
	{
		PointCo[1].x = (lineInfo[3].b - lineInfo[0].b) / (lineInfo[0].Slope - lineInfo[3].Slope);
		PointCo[3].x = (lineInfo[3].b - lineInfo[1].b) / (lineInfo[1].Slope - lineInfo[3].Slope);
	}
	PointCo[1].y = lineInfo[0].Slope * PointCo[1].x + lineInfo[0].b;
	PointCo[3].y = lineInfo[1].Slope * PointCo[3].x + lineInfo[1].b;

	PointDis[0] = pow(pow(PointCo[0].x - PointCo[1].x, 2) + pow(PointCo[0].y - PointCo[1].y, 2), 0.5);//0-3
	PointDis[1] = pow(pow(PointCo[2].x - PointCo[3].x, 2) + pow(PointCo[2].y - PointCo[3].y, 2), 0.5);//1-2
	PointDis[2] = pow(pow(PointCo[1].x - PointCo[3].x, 2) + pow(PointCo[1].y - PointCo[3].y, 2), 0.5);//0-1
	PointDis[3] = pow(pow(PointCo[2].x - PointCo[0].x, 2) + pow(PointCo[2].y - PointCo[0].y, 2), 0.5);//1-3



	if (PointDis[0] > InputPara.UpSafeRatio * DutWidth || PointDis[0] < InputPara.lowSafeRatio * DutWidth ||
		PointDis[1] > InputPara.UpSafeRatio * DutWidth || PointDis[1] < InputPara.lowSafeRatio * DutWidth ||
		PointDis[2] > InputPara.UpSafeRatio * DutHeigth || PointDis[2] < InputPara.lowSafeRatio * DutHeigth ||
		PointDis[3] > InputPara.UpSafeRatio * DutHeigth || PointDis[3] < InputPara.lowSafeRatio * DutHeigth)
		return 2;



	Slope1 = (PointCo[0].y - PointCo[3].y) / (PointCo[0].x - PointCo[3].x);
	Slope2 = (PointCo[1].y - PointCo[2].y) / (PointCo[1].x - PointCo[2].x);
	B_1 = PointCo[0].y - Slope1 * PointCo[0].x;
	B_2 = PointCo[1].y - Slope2 * PointCo[1].x;
	CenterPoint.x = (B_1 - B_2) / (Slope2 - Slope1);
	CenterPoint.y = Slope1 * CenterPoint.x + B_1;
	FinalPoint.x = (CenterPoint.y + 1 / lineInfo[0].Slope * CenterPoint.x - lineInfo[0].b) / (lineInfo[0].Slope + 1 / lineInfo[0].Slope);
	FinalPoint.y = lineInfo[0].Slope * FinalPoint.x + lineInfo[0].b;
	FinalPoint.x = (FinalPoint.x + CenterPoint.x) / 2;
	FinalPoint.y = (FinalPoint.y + CenterPoint.y) / 2;
	RotationAngle = atan((-1 / lineInfo[0].Slope));
	//if (UpOrDown == 0)
	//{
	if (RotationAngle >= 0)
		FinalPoint.x = FinalPoint.x + xScale * 11 / 2 * cos(RotationAngle) - 1.07;
	else
		FinalPoint.x = FinalPoint.x - xScale * 11 / 2 * cos(RotationAngle) - 1.07;
	FinalPoint.y = FinalPoint.y - abs(xScale * 11 / 2 * sin(RotationAngle)) + 4.56;
	//}
	/*else
	{
	if (RotationAngle >= 0)
	FinalPoint.x = FinalPoint.x + xScale * 11 / 2 * cos(RotationAngle);
	else
	FinalPoint.x = FinalPoint.x - xScale * 11 / 2 * cos(RotationAngle);
	FinalPoint.y = FinalPoint.y + abs(xScale * 11 / 2 * sin(RotationAngle));
	}*/

	RotationAngle = atan(-1 / lineInfo[0].Slope) * 180 / PI;
	if (RotationAngle >= 0)
		RotationAngle = 180.f -  RotationAngle;
	else
		RotationAngle = - RotationAngle;

	return 1;
	 }
	 __except (EXCEPTION_EXECUTE_HANDLER)
	 {
		 return _CODE_THROW;
	 }
}
IMG_VVOID CBmDUTSocketLocation::SetSrcImage(IMG_UBBUF srcImage)
{

	InputPara.prcImage.ptr = (IMG_UBYTE*)malloc(sizeof(IMG_UBYTE) * srcImage.size.width * srcImage.size.height);
	InputPara.prcImage.size = srcImage.size;
	InputPara.prcImage.linestep = srcImage.size.width;
	memcpy(InputPara.prcImage.ptr, srcImage.ptr, srcImage.size.width * srcImage.size.height);
}

_BACK_CODE CBmDUTSocketLocation::DUTLocation(IMG_UBBUF pImg, const int angleFlg, DUTOutPutParam &param)
{
	_try{
	IMG_RCOORD RightUpCo, LeftUpCo, LeftDownCo, RightDownCo, CenterPoint, FinalCo;
	IMG_RCOORD vertex[4], PointCo[4];
	IMG_INT coarseReturn;
	IMG_UWORD width;
	LineIn Line1, Line2, Line3, Line4;
	IMG_REAL RotationAngle;
	double CoarseAngle;
	IMG_RCOORD pSrcCenCo, pDstCenCo;
	_BACK_CODE status;
	switch (angleFlg)
	{
	case 180:
		
		pSrcCenCo.x = (pImg.size.width - 1) / 2;
		pSrcCenCo.y = (pImg.size.height - 1) / 2;
		pDstCenCo = pSrcCenCo;
		InputPara.prcImage.linestep = pImg.linestep;
		InputPara.prcImage.size = pImg.size;
		InputPara.prcImage.ptr = (IMG_UBYTE*)malloc(sizeof(IMG_UBYTE) * InputPara.prcImage.size.height * InputPara.prcImage.size.width);
		
		for (int i = 0; i < pImg.size.height; i++)
		{
			for (int j = 0; j < pImg.size.width; j++)
			{//���Խ���������ת
				InputPara.prcImage.ptr[i*pImg.size.width + j] = pImg.ptr[(pImg.size.height - i - 1)*pImg.size.width + pImg.size.width - j - 1];
			}
		}
		
//	RTS_R_8uC1(pImg, pSrcCenCo, InputPara.prcImage,pDstCenCo,PI,pImg.size,1);

		break;
	case 0:
		SetSrcImage(pImg);
		break;
	default:
		status = _CODE_ANGLE_FAIL;
		goto END;
		
	}
	

	//double t1 = (double)cvGetTickCount();
	coarseReturn = DownLookCorasePosition_down(InputPara.prcImage, LeftUpCo, RightUpCo, LeftDownCo,RightDownCo,CoarseAngle);
	//t1 = (double)cvGetTickCount() - t1;
//	printf("coarse exec time = %gms\n", t1 / (cvGetTickFrequency() * 1000));
	if (coarseReturn == -1)
	{
	//	cout << "coarse damage" << endl;
		status = _CODE_LOCATE_FAIL;
		goto END;
	}
	//

	//vertex[3] = { (IMG_REAL)LeftCo.x,  (IMG_REAL)UpCo.y };
	//vertex[2] = { (IMG_REAL)LeftCo.x,  (IMG_REAL)DownCo.y };
	//vertex[1] = { (IMG_REAL)RightCo.x, (IMG_REAL)UpCo.y };
	//vertex[0] = { (IMG_REAL)RightCo.x, (IMG_REAL)DownCo.y };
	width = pow(pow(LeftUpCo.x - RightUpCo.x, 2) + pow(LeftUpCo.y - RightUpCo.y, 2), 0.5);

	Line1.roiLine.PointEnd = RightUpCo;
	Line1.roiLine.PointStart = LeftUpCo;
	Line1.MaskEnd.x = (IMG_REAL)(RightUpCo.x + LeftUpCo.x) * 0.75;
	Line1.MaskStart.x = (IMG_REAL)(RightUpCo.x + LeftUpCo.x) * 0.50;
	Line1.MaskEnd.y = Line1.MaskStart.y = 0;

	Line2.roiLine.PointEnd = LeftDownCo;
	Line2.roiLine.PointStart = RightDownCo;
	Line2.MaskEnd.x = (IMG_REAL)(RightDownCo.x - 136.f);
	Line2.MaskStart.x = (IMG_REAL)(LeftDownCo.x + 140.f) ;
	Line2.MaskEnd.y = Line2.MaskStart.y = 0;

	Line3.roiLine.PointEnd = RightDownCo;
	Line3.roiLine.PointStart = RightUpCo ;
	Line4.roiLine.PointEnd = LeftUpCo;
	Line4.roiLine.PointStart = LeftDownCo;

	Line1.edDirectint = InputPara.edDirectint[0];
	Line2.edDirectint = InputPara.edDirectint[1];
	Line3.edDirectint = InputPara.edDirectint[2];
	Line4.edDirectint = InputPara.edDirectint[3];
	//	break;
	//default:
	//	return 0;
	//	break;
	//}


	Line1.roiLine.NormalVari = InputPara.roiNormalvari[0];
	Line1.scanDirect = LEFT;
	Line1.threshold = InputPara.threshold[0];
	Line2.roiLine.NormalVari = InputPara.roiNormalvari[1];
	Line2.scanDirect = LEFT;
	Line2.threshold = InputPara.threshold[1];
	Line3.roiLine.NormalVari = InputPara.roiNormalvari[2];
	Line3.scanDirect = LEFT;
	Line3.threshold = InputPara.threshold[2];
	Line4.roiLine.NormalVari = InputPara.roiNormalvari[3];
	Line4.scanDirect = LEFT;
	Line4.threshold = InputPara.threshold[3];

	IMG_UBBUF medianPrc;
	medianPrc.size = InputPara.prcImage.size;
	medianPrc.linestep = InputPara.prcImage.linestep;
	medianPrc.ptr = new IMG_UBYTE[InputPara.prcImage.size.width * InputPara.prcImage.size.height];
	//VisMeanFilter(prcImage.ptr, prcImage.size.height, prcImage.size.width, medianPrc.ptr);

	VisFilterMedian(InputPara.prcImage.ptr, InputPara.prcImage.size.height, InputPara.prcImage.size.width, medianPrc.ptr,InputPara.medianFilterMasksize);
	//ofstream f2("MedianPic.txt");
	//for (int i = 0; i < medianPrc.size.height; i++)
	//{
	//	for (int j = 0; j < medianPrc.size.width; j++)
	//	{
	//		f2 << (int)medianPrc.ptr[i * medianPrc.size.width + j] << " ";
	//	}
	//	f2 << endl;
	//}
	//f2.close();






	int returnLine = CBmDUTTrayLocationLineDetection(medianPrc, width, Line1, Line2, Line3, Line4, PointCo, CenterPoint, FinalCo, RotationAngle);
	delete[] medianPrc.ptr;
	if (returnLine != 1)
	{
		status = _CODE_LOCATE_FAIL;
		goto END;
	}

	switch (angleFlg)
	{
	case 180:
		IMG_REAL coeff[2][3];
		coeff[0][0] = cos(PI);
		coeff[0][1] = -sin(PI);
		coeff[0][2] = -(cos(PI) * pSrcCenCo.x - sin(PI) * pSrcCenCo.y - pSrcCenCo.x);
		coeff[1][0] = sin(PI);
		coeff[1][1] = cos(PI);
		coeff[1][2] = (-sin(PI) * pSrcCenCo.x - cos(PI) * pSrcCenCo.y + pSrcCenCo.y);

		param.x1 = (double)PointCo[1].x * coeff[0][0] + PointCo[1].y * coeff[0][1] + coeff[0][2];
		param.x2 = (double)PointCo[0].x * coeff[0][0] + PointCo[0].y * coeff[0][1] + coeff[0][2];
		param.x3 = (double)PointCo[2].x * coeff[0][0] + PointCo[2].y * coeff[0][1] + coeff[0][2];
		param.x4 = (double)PointCo[3].x * coeff[0][0] + PointCo[3].y * coeff[0][1] + coeff[0][2];
		param.y1 = (double)PointCo[1].x * coeff[1][0] + PointCo[1].y * coeff[1][1] + coeff[1][2];
		param.y2 = (double)PointCo[0].x * coeff[1][0] + PointCo[0].y * coeff[1][1] + coeff[1][2];
		param.y3 = (double)PointCo[2].x * coeff[1][0] + PointCo[2].y * coeff[1][1] + coeff[1][2];
		param.y4 = (double)PointCo[3].x * coeff[1][0] + PointCo[3].y * coeff[1][1] + coeff[1][2];
		param.cx = (double)FinalCo.x * coeff[0][0] + FinalCo.y * coeff[0][1] + coeff[0][2];
		param.cy = (double)FinalCo.x * coeff[1][0] + FinalCo.y * coeff[1][1] + coeff[1][2];
		break;
	case 0:
		param.x1 = (double)PointCo[1].x;
		param.x2 = (double)PointCo[0].x;
		param.x3 = (double)PointCo[2].x;
		param.x4 = (double)PointCo[3].x;
		param.y1 = (double)PointCo[1].y;
		param.y2 = (double)PointCo[0].y;
		param.y3 = (double)PointCo[2].y;
		param.y4 = (double)PointCo[3].y;
		param.cx = (double)FinalCo.x;
		param.cy = (double)FinalCo.y;
	default:
		break;
	}
	
	param.dist = pow(pow(FinalCo.x - CenterPoint.x, 2) + pow(FinalCo.y - CenterPoint.y, 2), 0.5) - 5.65;
	param.angle = (double)RotationAngle;
	
	status = _CODE_OK;
END:
	if (InputPara.prcImage.ptr != NULL)
		free(InputPara.prcImage.ptr);
	return status;
	 }
	 __except (EXCEPTION_EXECUTE_HANDLER)
	 {
		 return _CODE_THROW;
	 }

}
bool CBmDUTSocketLocation::LoadParameter(char path[])
{
	_try{
	LPSTR temp = (LPSTR)new char[MAX_PATH];
	bool status;
	GetPrivateProfileStringA("BmDUTSocketLocation", "edDirectint1 ", "FAULT", temp, MAX_PATH, path);
	if (strcmp(temp, "FAULT") == 0)
	{
		status = FALSE;
		goto END;
	}
	else if (0 == strcmp(temp, "BLACKWHITE"))
		InputPara.edDirectint[0] = BLACKWHITE;
	else if (0 == strcmp(temp, "WHITEBLACK"))
		InputPara.edDirectint[0] = WHITEBLACK;
	else if (0 == strcmp(temp, "BOTH"))
		InputPara.edDirectint[0] = BOTH;

	GetPrivateProfileStringA("BmDUTSocketLocation", "edDirectint2 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	else if (0 == strcmp(temp, "BLACKWHITE"))
		InputPara.edDirectint[1] = BLACKWHITE;
	else if (0 == strcmp(temp, "WHITEBLACK"))
		InputPara.edDirectint[1] = WHITEBLACK;
	else if (0 == strcmp(temp, "BOTH"))
		InputPara.edDirectint[1] = BOTH;
	GetPrivateProfileStringA("BmDUTSocketLocation", "edDirectint3", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	else if (0 == strcmp(temp, "BLACKWHITE"))
		InputPara.edDirectint[2] = BLACKWHITE;
	else if (0 == strcmp(temp, "WHITEBLACK"))
		InputPara.edDirectint[2] = WHITEBLACK;
	else if (0 == strcmp(temp, "BOTH"))
		InputPara.edDirectint[2] = BOTH;

	GetPrivateProfileStringA("BmDUTSocketLocation", "edDirectint4", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	else if (0 == strcmp(temp, "BLACKWHITE"))
		InputPara.edDirectint[3] = BLACKWHITE;
	else if (0 == strcmp(temp, "WHITEBLACK"))
		InputPara.edDirectint[3] = WHITEBLACK;
	else if (0 == strcmp(temp, "BOTH"))
		InputPara.edDirectint[3] = BOTH;

	GetPrivateProfileStringA("BmDUTSocketLocation", "roiNormalvari1", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.roiNormalvari[0] = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "roiNormalvari2", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.roiNormalvari[1] = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "roiNormalvari3", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.roiNormalvari[2] = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "roiNormalvari4", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.roiNormalvari[3] = atoi((char*)temp);

	GetPrivateProfileStringA("BmDUTSocketLocation", "threshold1", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.threshold[0] = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "threshold2", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.threshold[1] = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "threshold3", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.threshold[2] = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "threshold4", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.threshold[3] = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "edgeMaskColSize", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.edgeMaskColSize = (int)atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "edgeMaskRowSize", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.edgeMaskRowSize = (int)atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "medianFilterMasksize", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.medianFilterMasksize = (int)atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "PointFilterAngleTolerance", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.PointFilterAngleTolerance = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "HoughAcceptScore", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.HoughAcceptScore = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "HoughScoreGap", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.HoughScoreGap = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "FittingSigmaFirst", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.FittingSigmaFirst = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "FittingSigmaSecond", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.FittingSigmaSecond = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "ScoreAccptDis", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.ScoreAccptDis = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "angleControl", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.angleControl = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "lowSafeRatio", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.lowSafeRatio = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "UpSafeRatio", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.UpSafeRatio = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "HoughAngelTolerance", "0", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.HoughAngelTolerance = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CorasePyraLevel", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CorasePyraLevel = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseHisSpeMean", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseHisSpeMean = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseHisSpeVarian", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseHisSpeVarian = (float)atof((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseFilterKern", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseFilterKern = (IMG_UINT)atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseFilterDivisor", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseFilterDivisor = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "medianFilterMasksize", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.medianFilterMasksize = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1AreaMin ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1AreaMin = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1AreaMax ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1AreaMax = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1X1 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1X1 = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1X2", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1X2 = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1Y1 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1Y1 = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1Y2", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1Y2 = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1WidthMin", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1WidthMin = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1WidthMax", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1WidthMax = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1HeightMin", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1HeightMin = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1HeightMax ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1HeightMax = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1Width1 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1Width1 = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC1Width2 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC1Width2 = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseROIX ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseROIX = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseROIY ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseROIY = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseROIWidth ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseROIWidth = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseROIHeight ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseROIHeight = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC2Width1 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC2Width1 = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseCC2Width2 ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseCC2Width2 = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseFitNormWidth  ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseFitNormWidth = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseFitNormHeight  ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseFitNormHeight = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseLeftOff  ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseLeftOff = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseUpOff  ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseUpOff = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseRightOff   ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseRightOff = atoi((char*)temp);
	GetPrivateProfileStringA("BmDUTSocketLocation", "m_CoraseDownOff   ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT"))
		if (strcmp(temp, "FAULT") == 0)
		{
			status = FALSE;
			goto END;
		}
	InputPara.m_CoraseDownOff = atoi((char*)temp);
	status = TRUE;
END:

	delete temp;

	return status;
	 }
	 __except (EXCEPTION_EXECUTE_HANDLER)
	 {
		 return _CODE_THROW;
	 }
}
//int CBmDUTSocketLocation::GradientCompute(unsigned char *src, int srcWidth, int srcHeight, int rowNum, int colNum,float threshold, int *dst, vector<edgeInformation> &edgePoint)
//{
//	//////0���쳣�ж�
//	edgeInformation temp;
//	int w = 2 * rowNum;
//	int h = colNum;
//	if (src == NULL || srcWidth < w || srcHeight < h)
//	{
//		return -1;
//	}
//
//	//////1���߽紦����ԭͼ��ȣ�
//	for (int i = 0; i < srcHeight*srcWidth; i++)
//	{
//		dst[i] = 0;
//	}
//	//memcpy(dst,src,srcWidth*srcHeight*sizeof(unsigned char));
//
//	//////3������gradient
//	int len = colNum / 2;
//	int a = 0;
//	int tempGray;
//
//	IMG_COORD point = { 0,0 };
//	for (int j = len; j < srcWidth - len; j++)
//	{
//		for (int i = rowNum ; i < srcHeight - rowNum; i++)
//		{
//			int sum1 = 0;
//			int sum2 = 0;
//			for (int t = 0; t < rowNum; t++)
//			{
//				for (int r = 0; r < colNum; r++)
//				{
//					sum1 = sum1 + src[(i - t - 1)*srcWidth + j - len + r];
//					sum2 = sum2 + src[(i + t + 1)*srcWidth + j - len + r];
//				}
//			}
//
//			tempGray = sum2 - sum1;
//			if (tempGray > threshold)
//			{
//				dst[j + i*srcWidth] = tempGray;
//				temp.xyDecimal = { (float)j,(float)i };
//				edgePoint.push_back(temp);
//			}
//		}
//	}
//	return 0;
//}

/**********************************************  Edge *******************************************/
int CBmDUTSocketLocation::GradientCompute(unsigned char *src, int srcWidth, int srcHeight, int rowNum, int colNum, float threshold, int *dst, vector<edgeInformation> &edgePoint)
{
	_try{
	//////0���쳣�ж�
	int h = 2 * rowNum;
	int w = colNum;
	if (src == NULL || srcWidth < w || srcHeight < h)
	{
		return -1;
	}

	//////1���߽紦����ԭͼ��ȣ�
	for (int i = 0; i < srcHeight*srcWidth; i++)
	{
		dst[i] = 0;
	}
	//memcpy(dst,src,srcWidth*srcHeight*sizeof(unsigned char));

	//////3������gradient
	int len = colNum / 2;
	int a = 0;
	int tempGray;

	IMG_COORD point = { 0,0 };
	for (int j = len; j < srcWidth - len; j++)
	{
		for (int i = rowNum; i < srcHeight - rowNum; i++)
		{
			int sum1 = 0;
			int sum2 = 0;
			for (int t = 0; t < rowNum; t++)
			{
				for (int r = 0; r < colNum; r++)
				{
					sum1 = sum1 + src[(i - t - 1)*srcWidth + j - len + r];
					sum2 = sum2 + src[(i + t + 1)*srcWidth + j - len + r];
				}
			}
			tempGray = sum2 - sum1;
			dst[j + i*srcWidth] = tempGray;
			/*if (tempGray > threshold)
			{
			
			temp.xyDecimal = { (float)j,(float)i };
			edgePoint.push_back(temp);
			}*/
		}
	}

	/////////
	edgeInformation temp;
	int k1 = 0;
	int k2 = 0;
	int k3 = 0;
	float deci = 0;
	for (int j = 0; j < srcWidth; j++)
	{
		for (int i = 1; i < srcHeight - 1; i++)
		{
			if (dst[j + i*srcWidth]>threshold && dst[j + i*srcWidth]>dst[j + (i - 1)*srcWidth] &&
				dst[j + i*srcWidth] >= dst[j + (i + 1)*srcWidth])
			{
				if (dst[j + (i - 1)*srcWidth]>0 && dst[j + (i + 1)*srcWidth] > 0)
				{
					k3 = dst[j + (i + 1)*srcWidth];
					k2 = dst[j + i*srcWidth];
					k1 = dst[j + (i - 1)*srcWidth];
					deci = (k3 - k1) / ((float)2.0*(2.0*k2 - k1 - k3));
					temp.angle = 0;
					temp.gradient = dst[j + i*srcWidth];
					temp.xyInteger.x = j;
					temp.xyInteger.y = i;
					temp.xyDecimal.x = j;
					temp.xyDecimal.y = i + deci;
					edgePoint.push_back(temp);
				}
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