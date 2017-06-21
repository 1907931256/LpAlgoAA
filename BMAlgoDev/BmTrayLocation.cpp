#include "stdafx.h"
#include"BmTrayLocation.h"

/**********************************************/
// VisBC_Pyramid, 功能说明:金字塔，将图像进行缩小。然后在缩小的图像上进行图像反转,白变黑,黑变白
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
IMG_VVOID CBmTrayLocation::VisBC_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma)
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

	Roi.width = pPyrStruct->pRoi[(int)bmtray.PyramidLevel].width;
	Roi.height = pPyrStruct->pRoi[(int)bmtray.PyramidLevel].height;

	//srcRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));


	memcpy(srcRoi, pPyrImage[2], Roi.width*Roi.height * sizeof(Ipp8u));


	ippiAbsDiffC_8u_C1R(srcRoi, Roi.width, srcRoi, Roi.width, Roi, 255);
	//for (int i = 0; i < Roi.height; i++)
	//{
	//	for (int j = 0; j < Roi.width; j++)
	//	{
	//		srcRoi[i*Roi.width + j] = pPyrImage[2][i*Roi.width + j];
	//	}
	//}

	for (int i = 1; i <= level; i++)
		free(pPyrImage[i]);
	free(pPyrStrBuffer);
	free(pPyrBuffer);
	free(pPyrLBuffer);
	free(pPyrLStateBuf);
}


/**********************************************/
// VisBC_Binaryzation, 功能说明:对图像进行二值化。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//	int threshold,梯度强度的阈值（理论上是大于0小于1250的整数）。一般在此用例中200左右比较合适
//     ...
// Output:
// 	IMG_UBBUF src,输出图像
//     ...
// Return:
//     0 - 正常
//     -1 - 暂无待定
//     -2 - 暂无待定
// Author: /04/27/2017
/**********************************************/
IMG_VVOID CBmTrayLocation::VisBC_Binaryzation(Ipp8u *srcRoi, IppiSize Roi, IMG_INT threshold)
{

	int srcDstStep = Roi.width * sizeof(Ipp8u);

	ippiThreshold_LTValGTVal_8u_C1IR(srcRoi, srcDstStep, Roi, threshold, (int)bmtray.BinaryzationMin, threshold, (int)bmtray.BinaryzationMax);

#ifdef DEBUG_BlobCircle

	FILE *hp;
	hp = fopen("threshold.txt", "w");
	for (int i = 0; i < Roi.height; i++)
	{
		for (int j = 0; j < Roi.width; j++)
		{
			fprintf(hp, "%d ", srcRoi[i*Roi.width + j]);
		}
		fprintf(hp, "\n");
	}
	fclose(hp);

#endif // DEBUG
}


/**********************************************/
// VisBC_LabelMarker, 功能说明:对图像中的Blob块进行提取。
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
// Author: /04/27/2017
/**********************************************/
IMG_VVOID CBmTrayLocation::VisBC_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, IMG_INT &markersNum)
{
	//IppiSize Roi = { src.size.width,src.size.height };
	int markerStep = Roi.width * sizeof(Ipp8u);
	int minLabel = (int)bmtray.LabelMarkerMin;
	int maxLabel = (int)bmtray.LabelMarkerMax;
	//int markersNum = 0;
	Ipp8u *pBuffer = NULL;
	int bufferSize = 0;

	ippiLabelMarkersGetBufferSize_8u_C1R(Roi, &bufferSize);

	pBuffer = ippsMalloc_8u(bufferSize);

	ippiLabelMarkers_8u_C1IR(srcRoi, markerStep, Roi, minLabel, maxLabel, ippiNormInf, &markersNum, pBuffer);

	//printf("markersNum=%d\n", markersNum);

	ippsFree(pBuffer);
}

/**********************************************/
// VisBC_OpenClose, 功能说明:对图像进行开运算，消除小的Blob。连接有小的孔洞的连通域
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
//  Ipp8u *srcRoi, 输出图片
//     ...
// Return:
//     0 - 正常
//     -1 - 暂无待定
//     -2 - 暂无待定
// Author: /04/27/2017
/**********************************************/
IMG_VVOID CBmTrayLocation::VisBC_OpenClose(Ipp8u *srcRoi, IppiSize Roi)
{
	//IppiSize Roi = { src.size.width,src.size.height };
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

	ippiMorphOpenBorder_8u_C1R(dstRoi, srcStep, srcRoi, dstStep, Roi, borderType, borderValue, pSpec, pBuffer);

#ifdef DEBUG_BlobCircle
	FILE *hp1;
	hp1 = fopen("Open.txt", "w");
	for (int i = 0; i < Roi.height; i++)
	{
		for (int j = 0; j < Roi.width; j++)
		{
			fprintf(hp1, "%d ", dstRoi[i*Roi.width + j]);
		}
		fprintf(hp1, "\n");
	}
	fclose(hp1);
	FILE *hp2;
	hp2 = fopen("Close.txt", "w");
	for (int i = 0; i < Roi.height; i++)
	{
		for (int j = 0; j < Roi.width; j++)
		{
			fprintf(hp2, "%d ", srcRoi[i*Roi.width + j]);
		}
		fprintf(hp2, "\n");
	}
	fclose(hp2);
#endif // !1

	ippsFree(pBuffer);
	ippsFree(pSpec);
	free(dstRoi);

}


/**********************************************/
// VisBC_Screen, 功能说明:对选中的Blob块进行筛选。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
// 	TempStructCircle &circle1, 图像中的特征圆的圆心及半径
//     ...
// Return:
//     0 - 正常
//     -1 - 返回错误，无法检测到特征圆
//     -2 - 暂无待定
// Author: /04/27/2017
/**********************************************/

IMG_INT CBmTrayLocation::VisBC_Screen(Ipp8u *&srcRoi, IppiSize Roi, TempStructCircle &circle1)
{
	int markersNum = 0;

	VisBC_LabelMarker(srcRoi, Roi, markersNum);

	if (markersNum < 1)
	{
		return -1;
	}

	int *NumMar;//area
	NumMar = (int*)malloc(markersNum * sizeof(int));

	for (int i = 0; i < markersNum; i++)
	{
		NumMar[i] = 0;
	}

	int grade = 0;

	for (int i = 0; i < Roi.height; i++)
	{
		for (int j = 0; j < Roi.width; j++)
		{
			grade = srcRoi[i*Roi.width + j];
			if (grade > 0)
			{
				NumMar[grade - 1]++;
			}
		}
	}
	/*FILE *hp;
	hp = fopen("marker12.txt", "w");
	for (int i = 0; i < Roi.height; i++)
	{
		for (int j = 0; j < Roi.width; j++)
		{
			fprintf(hp, "%d ", 20 * srcRoi[i*Roi.width + j]);
		}
		fprintf(hp, "\n");
	}
	fclose(hp);*/
	//根据面积进行筛选
	for (int i = 0; i < Roi.height; i++)
	{
		for (int j = 0; j < Roi.width; j++)
		{
			if (NumMar[srcRoi[i*Roi.width + j] - 1] > (float)Roi.width*(float)Roi.height*bmtray.AreaProportionMax || NumMar[srcRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*bmtray.AreaProportionMin)
				//if (NumMar[srcRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*0.005 || NumMar[srcRoi[i*Roi.width + j] - 1] > (float)Roi.width*(float)Roi.height*0.025)
			{
				srcRoi[i*Roi.width + j] = 0;
			}
		}
	}

	VisBC_LabelMarker(srcRoi, Roi, markersNum);

	if (markersNum < 1)
	{
		free(NumMar);
		return -1;
	}

	//#ifdef DEBUG_BlobCircle
	/*FILE *hp1;
	hp1 = fopen("marker1.txt", "w");
	for (int i = 0; i < Roi.height; i++)
	{
	for (int j = 0; j < Roi.width; j++)
	{
	fprintf(hp1, "%d ", 20 * srcRoi[i*Roi.width + j]);
	}
	fprintf(hp1, "\n");
	}
	fclose(hp1);*/
	//#endif // !1

	//计算每个Blob块的左右上下边界点，中心点
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

	TempStructCircle *point;
	point = (TempStructCircle*)malloc(markersNum * sizeof(TempStructCircle));

	vector<int>sign1;

	float disNx = 0.0, disNy = 0.0, disNrx = 0.0, disNry = 0.0;

	for (int k = 0; k < markersNum; k++)
	{
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				if (srcRoi[i*Roi.width + j] - 1 == k)
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
		point[k].CirCen.x = (float)sum_x[k] / (float)num[k];
		point[k].CirCen.y = (float)sum_y[k] / (float)num[k];

		if (min_y[k] < bmtray.BoudaryConstraint || fabs(max_x[k] - Roi.width) < bmtray.BoudaryConstraint)
		{
			point[k].Radius = float(fabs(max_y[k] - min_y[k] - bmtray.DoubleReducedImageRadiusConstraint) < fabs(max_x[k] - min_x[k] - bmtray.DoubleReducedImageRadiusConstraint) ? (float)(max_y[k] - min_y[k]) / 2.0 : (float)(max_x[k] - min_x[k]) / 2.0);
		}
		else
		{
			point[k].Radius = (float)((max_x[k] - min_x[k]) / 2.0);
		}

		point[k].miny = min_y[k];
		point[k].minx = min_x[k];
		point[k].maxx = max_x[k];
		point[k].maxy = max_y[k];

		if (fabs(point[k].Radius - bmtray.VoteIntegrity) < bmtray.ImageDiameterFaultTolerant)
			sign1.push_back(k);

#ifdef DEBUG_BlobCircle
		printf("min_x%d=%d min_y%d=%d max_x%d=%d max_y%d=%d\n", k, min_x[k], k, min_y[k], k, max_x[k], k, max_y[k]);
#endif // !1

	}

	if (sign1.size() < 1)
	{
		free(point);
		free(sum_x);
		free(sum_y);
		free(num);
		free(min_x);
		free(min_y);
		free(max_x);
		free(max_y);
		free(NumMar);
		vector<int>().swap(sign1);
		return -1;
	}
	else
	{
		markersNum = sign1.size();

		int sign = sign1[0];
		double minDis = fabs(point[sign].Radius - bmtray.VoteIntegrity);

		for (unsigned int i = 0;i < sign1.size();i++)
		{
			if (minDis > fabs(point[i].Radius - bmtray.VoteIntegrity))
			{
				minDis = fabs(point[i].Radius - bmtray.VoteIntegrity);
				sign = sign1[i];
			}
		}

		circle1 = point[sign];

	}




#ifdef DEBUG_BlobCircle
	FILE *hp;
	hp = fopen("markerNumber.txt", "w");
	for (int i = 1; i < markersNum + 1; i++)
	{
		fprintf(hp, "NumMar=%d  Num=%d\n", NumMar[i], i);
	}
	fclose(hp);
#endif // !1
#ifdef DEBUG_BlobCircle
	FILE *hp12;
	hp12 = fopen("markerEnd.txt", "w");
	for (int i = 0; i < Roi.height; i++)
	{
		for (int j = 0; j < Roi.width; j++)
		{
			fprintf(hp12, "%d ", 20 * srcRoi[i*Roi.width + j]);
		}
		fprintf(hp12, "\n");
	}
	fclose(hp12);
#endif // !1

	free(point);
	free(sum_x);
	free(sum_y);
	free(num);
	free(min_x);
	free(min_y);
	free(max_x);
	free(max_y);
	free(NumMar);
	vector<int>().swap(sign1);
	return 0;

}

/**********************************************/
// VisBC_BlobCircleAccurate, 功能说明:对图像中的特征圆进行定位。
// Input:
// 	IMG_UBBUF src, 输入图片
//	int threshold	梯度强度的阈值（理论上是大于0小于1250的整数）。最好输入200左右
//     ...
// Output:
// 	TempStructCircle &circle1, 图像中的第一个特征圆的圆心及半径
//     ...
// Return:
//     0 - 正常
//     -1 - 返回错误，无法检测到特征圆
//     其他待定
// Author:/03/18/2017
/**********************************************/
IMG_INT CBmTrayLocation::VisBC_BlobCircleAccurate(IMG_UBBUF src, TempStructCircle &circle1)
{
	//_try{
	int level = (int)bmtray.PyramidLevel;
	double sigma = 3.0;

	IppiSize Roi;
	Roi.width = src.size.width;
	Roi.height = src.size.height;

	Ipp8u* srcRoi;
	srcRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

	VisBC_Pyramid(src, srcRoi, Roi, level, sigma);
	//Roi = { pPyrStruct->pRoi[2].width,pPyrStruct->pRoi[2].height };
	/*ofstream fp("srcroi.txt");
	for (int i = 0;i < Roi.height;i++)
	{
		for (int j = 0;j < Roi.width;j++)
		{
			fp << (double)srcRoi[i*Roi.width + j] << "  ";
		}
		fp << endl;
	}
	fp.close();*/
	Ipp8u*dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

	IMG_INT threshold = 200;
	Histogram(srcRoi, Roi, dstRoi, threshold);

	VisBC_Binaryzation(srcRoi, Roi, threshold);
	/*ofstream fp("srcroi.txt");
	for (int i = 0;i < Roi.height;i++)
	{
		for (int j = 0;j < Roi.width;j++)
		{
			fp << (double)srcRoi[i*Roi.width + j] << "  ";
		}
		fp << endl;
	}
	fp.close();*/
	VisBC_OpenClose(srcRoi, Roi);

	int value = 0;
	TempStructCircle circle2;
	value = VisBC_Screen(srcRoi, Roi, circle2);
	if (value == -1)
	{
		free(dstRoi);
		free(srcRoi);
		return -1;
	}
	TempStructCircle *CirRa_in;
	CirRa_in = (TempStructCircle*)malloc(sizeof(TempStructCircle));
	CirRa_in[0].CirCen = { circle2.CirCen.x * 2 * level,circle2.CirCen.y * 2 * level };
	CirRa_in[0].Radius = circle2.Radius * 2 * level;

	//CirRa1 = CirRa_in[0];//判断下半圆是否完整
	//CirRa2 = CirRa_in[1];//判断左半圆是否完整


	circle2.minx = 2 * level * circle2.minx;
	circle2.maxx = 2 * level * circle2.maxx;
	circle2.miny = 2 * level * circle2.miny;
	circle2.maxy = 2 * level * circle2.maxy;
	double areasum = 0;
	if (fabs(CirRa_in[0].Radius * 2 - bmtray.OriginalDiameterConstraint) > bmtray.ImageDiameterFaultTolerant)
	{
		free(dstRoi);
		if (CirRa_in != nullptr)
			free(CirRa_in);
		if (srcRoi != nullptr)
			free(srcRoi);
		return -4;
	}

	int *socket_re = (int*)malloc(sizeof(int)*(circle2.maxx - circle2.minx + 1));
	for (int i = 0;i < (circle2.maxx - circle2.minx + 1);i++)
	{
		socket_re[i] = 0;
	}
	for (int i = circle2.minx;i <= circle2.maxx;i++)
	{
		for (int j = circle2.miny;j <= circle2.maxy;j++)
		{
			if ((double)src.ptr[j*src.size.width + i] < bmtray.ReducedImageRadiusConstraint)
			{
				socket_re[i - circle2.minx] ++;
			}
		}
		areasum += socket_re[i - circle2.minx];
	}
	if (socket_re[5] < bmtray.VoteTruncation && socket_re[4] < bmtray.VoteTruncation&&
		//socket_re[6] < bmtray.VoteTruncation && socket_re[7] < bmtray.VoteTruncation&&
		socket_re[circle2.maxx - circle2.minx - 5] < bmtray.VoteTruncation && socket_re[circle2.maxx - circle2.minx - 5] < bmtray.VoteTruncation
		//||
		//(socket_re[200] >=30* bmtray.VoteTruncation && socket_re[203] >= 30 * bmtray.VoteTruncation)
		//||
		//(socket_re[5] > 20 * bmtray.VoteTruncation && socket_re[4] > 20 * bmtray.VoteTruncation)
		//(socket_re[circle2.maxx - circle2.minx - 5] >22.5* bmtray.VoteTruncation && socket_re[circle2.maxx - circle2.minx - 5] > 22.5* bmtray.VoteTruncation)

		)
	{
		free(socket_re);
		if (CirRa_in != nullptr)
			free(CirRa_in);
		if (srcRoi != nullptr)
			free(srcRoi);
		free(dstRoi);
		return -4;
	}
	//int temp = 0;
	//for (int i = CirRa_in[0].Radius-20;i < CirRa_in[0].Radius+20;i++)
	//{
	//	//if(socket_re[i]-)
	//}
	if (areasum > fabs((PI*CirRa_in[0].Radius*CirRa_in[0].Radius - bmtray.OriginalAreaTolerant)))
	{
		circle1 = CirRa_in[0];
	}
	else
	{
		free(dstRoi);
		free(socket_re);
		if (CirRa_in != nullptr)
			free(CirRa_in);
		if (srcRoi != nullptr)
			free(srcRoi);
		return -4;
	}
	//cout << areasum << endl;

	free(socket_re);
	if (CirRa_in != nullptr)
		free(CirRa_in);
	if (srcRoi != nullptr)
		free(srcRoi);
	free(dstRoi);
	return 0;
	/*}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}*/
}

/**********************************************/
// TrayLocation, 功能说明:对图像中的特征圆进行定位。
// Input:
// 	   IMG_UBBUF src, 输入图片
//     ...
// Output:
// 	   x, 图像中的第特征圆的圆心x坐标
// 	   y, 图像中的第特征圆的圆心y坐标
//     ...
// Return:
//    _BACK_CODE,参照 enum _BACK_CODE;
/**********************************************/
_BACK_CODE  CBmTrayLocation::TrayLocation(const IMG_UBBUF src, double &x, double &y)
{
	_try
	{
	_BACK_CODE status1 = _CODE_OK;

	TempStructCircle circle;
	IMG_INT threshold = (int)bmtray.ThresholdImage;

	IMG_INT status = VisBC_BlobCircleAccurate(src, circle);
	if (status == 0)
	{
		x = circle.CirCen.x;
		y = circle.CirCen.y;
		return _CODE_OK;
	}
	else if (status == -4)
	{
		return _CODE_PROERROR;
	}
	else
	{
		return _CODE_LOCATE_FAIL;
	}

	/*x = circle.CirCen.x;
	y = circle.CirCen.y;*/

	return status1;
	}
		//catch(...)
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}



int CBmTrayLocation::Histogram(Ipp8u*srcRoi, IppiSize Roi, Ipp8u*dstRoi, IMG_INT &thre)
{

	float *histo = (float*)malloc(256 * sizeof(float));
	float *histopro = (float*)malloc(256 * sizeof(float));
	for (int i = 0;i < 256;i++)
	{
		histo[i] = 0;
		histopro[i] = 0;
	}
	double sumpixeladd = 0;
	for (int i = 0;i < Roi.height;i++)
	{
		for (int j = 0;j < Roi.width;j++)
		{
			sumpixeladd += (double)srcRoi[i*Roi.width + j];
			histo[srcRoi[i*Roi.width + j]]++;
		}
	}
	float sumpixel = (float)Roi.width*Roi.height;

	histopro[0] = histo[0];
	for (int i = 1;i < 256;i++)
	{
		histopro[i] = histopro[i - 1] + histo[i];
	}

	sumpixeladd = sumpixeladd*1.0 / sumpixel;


	thre = int(fabs(sumpixeladd - 220)*0.5 + sumpixeladd);

	if (thre > 230)
	{
		thre = 230;
	}
	if (thre < 150)
	{
		thre = 150;
	}
	//0.7针对不同亮度图片加权处理
	//for (int i = 0;i < Roi.height;i++)
	//{
	//	for (int j = 0;j < Roi.width;j++)
	//	{
	//		/*if (histopro[srcRoi[i*Roi.width + j]] / sumpixel < 0.78)
	//		{
	//			dstRoi[i*Roi.width + j] = 0;
	//		}
	//		else*/
	//		{
	//			dstRoi[i*Roi.width + j] = (Ipp8u)(255 * histopro[srcRoi[i*Roi.width + j]] / sumpixel);
	//		}
	//	}
	//}


	free(histo);
	free(histopro);
	return 0;
}
bool CBmTrayLocation::LoadParameter(char path[])
{

	book ok;
	//ok.Write(".\\BmTrayFileName.ini");
	//ok.Read(".\\BmTrayFileName.ini", bmtray);
	int status = ok.Read(path, bmtray);
	if (status != 0)
	{
		return false;
	}
	return true;

	//catch(...)

}
CBmTrayLocation::CBmTrayLocation()
{}
CBmTrayLocation::~CBmTrayLocation()
{}




int book::Read(char *ConfigFileName, BMTRAY &bmtray)
{
	_try{
	int status = 0;
	LPSTR temp = (LPSTR)new char[MAX_PATH];

	GetPrivateProfileStringA("BmTray", "AreaProportionMax", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.AreaProportionMax = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "AreaProportionMin", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.AreaProportionMin = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "LabelMarkerMax", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.LabelMarkerMax = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "LabelMarkerMin", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.LabelMarkerMin = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "VoteIntegrity", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.VoteIntegrity = atof((char*)temp);

	GetPrivateProfileStringA("BmTray", "VoteTruncation", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.VoteTruncation = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "PyramidLevel", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.PyramidLevel = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "ThresholdImage", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.ThresholdImage = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "BoudaryConstraint", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.BoudaryConstraint = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "OriginalDiameterConstraint", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.OriginalDiameterConstraint = atof((char*)temp);

	GetPrivateProfileStringA("BmTray", "ReducedImageRadiusConstraint", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.ReducedImageRadiusConstraint = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "DoubleReducedImageRadiusConstraint", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.DoubleReducedImageRadiusConstraint = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "ImageDiameterFaultTolerant", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.ImageDiameterFaultTolerant = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "RowPixelStatisticsUpperLimit", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.RowPixelStatisticsUpperLimit = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "RowPixelStatisticsLowerLimit", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.RowPixelStatisticsLowerLimit = atof((char*)temp);

	GetPrivateProfileStringA("BmTray", "OriginalAreaTolerant", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.OriginalAreaTolerant = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "BinaryzationMin", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.BinaryzationMin = atof((char*)temp);
	GetPrivateProfileStringA("BmTray", "BinaryzationMax", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmtray.BinaryzationMax = atof((char*)temp);

	delete temp;
	return status;
	}
		//catch(...)
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

int book::Write(char *ConfigFileName)
{
	_try{
	int status = 0;

	WritePrivateProfileStringA("BmTray", "AreaProportionMax", "0.33", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "AreaProportionMin", "0.083", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "LabelMarkerMax", "200", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "LabelMarkerMin", "1", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "VoteIntegrity", "80", ConfigFileName);

	WritePrivateProfileStringA("BmTray", "VoteTruncation", "20", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "PyramidLevel", "2", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "ThresholdImage", "200", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "BoudaryConstraint", "4", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "OriginalDiameterConstraint", "632", ConfigFileName);

	WritePrivateProfileStringA("BmTray", "ReducedImageRadiusConstraint", "80", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "DoubleReducedImageRadiusConstraint", "158", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "ImageDiameterFaultTolerant", "10", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "RowPixelStatisticsUpperLimit", "49", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "RowPixelStatisticsLowerLimit", "31", ConfigFileName);

	WritePrivateProfileStringA("BmTray", "OriginalAreaTolerant", "10000", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "BinaryzationMin", "0", ConfigFileName);
	WritePrivateProfileStringA("BmTray", "BinaryzationMax", "255", ConfigFileName);

	return status;
	}
		//catch(...)
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}


book::book()
{

}

book::~book()
{

}
