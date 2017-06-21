#include "stdafx.h"
#include"BmSocketLocation.h"

/**********************************************/
// VisBC_Pyramid, 功能说明:金字塔，将图像进行缩小。
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
IMG_VVOID CBmSocketLocation::VisBC_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma)
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

	Roi.width = pPyrStruct->pRoi[int(bmsocket.PyramidLevel)].width;
	Roi.height = pPyrStruct->pRoi[int(bmsocket.PyramidLevel)].height;

	//srcRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));


	memcpy(srcRoi, pPyrImage[int(bmsocket.PyramidLevel)], Roi.width*Roi.height * sizeof(Ipp8u));

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
//	int threshold,梯度强度的阈值（理论上是大于0小于1250的整数）。如果用户不知道阈值设为多少合适，可以输入0（算法自动获取合适的阈值）。
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
IMG_VVOID CBmSocketLocation::VisBC_Binaryzation(Ipp8u *srcRoi, IppiSize Roi, IMG_INT threshold)
{

	int srcDstStep = Roi.width * sizeof(Ipp8u);

	ippiThreshold_LTValGTVal_8u_C1IR(srcRoi, srcDstStep, Roi, threshold, int(bmsocket.BinaryzationMin), threshold, int(bmsocket.BinaryzationMax));

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
// Author: Huang Yige/03/18/2017
/**********************************************/
IMG_VVOID CBmSocketLocation::VisBC_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, IMG_INT &markersNum)
{
	//IppiSize Roi = { src.size.width,src.size.height };
	int markerStep = Roi.width * sizeof(Ipp8u);
	int minLabel = int(bmsocket.LabelMarkerMin);
	int maxLabel = int(bmsocket.LabelMarkerMax);
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
// VisBC_OpenClose, 功能说明:对图像进行开运算，消除小的Blob。
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
IMG_VVOID CBmSocketLocation::VisBC_OpenClose(Ipp8u *srcRoi, IppiSize Roi)
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
// VisBC_Moment, 功能说明:计算图像的矩。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
// 	double &hu, 图像的矩
//     ...
// Return:
//     0 - 正常
//     -1 - 暂无待定
//     -2 - 暂无待定
// Author: Huang Yige/03/18/2017
/**********************************************/
IMG_VVOID CBmSocketLocation::VisBC_Moment(Ipp8u *srcRoi, IppiSize Roi, IMG_LREAL &hu)
{
	//IppiSize Roi = { src.size.width,src.size.height };
	int sizeState = 0;
	IppiHuMoment_64f hmH = { 0 };
	IppiMomentState_64f* pState = NULL;

	ippiMomentGetStateSize_64f(ippAlgHintNone, &sizeState);

	pState = (IppiMomentState_64f*)ippMalloc(sizeState);

	ippiMomentInit_64f(pState, ippAlgHintNone);

	ippiMoments64f_8u_C1R(srcRoi, Roi.width * sizeof(Ipp8u), Roi, pState);

	ippiGetHuMoments_64f(pState, 0, hmH);

	hu = hmH[0];

#ifdef DEBUG_BlobCircle
	printf("M1=%f,M2=%f\n", hmH[0], hmH[1]);
#endif // !1

	ippsFree(pState);
}

/**********************************************/
// VisBC_Screen, 功能说明:对选中的Blob块进行筛选。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//     ...
// Output:
// 	TempStructCircle &circle1, 图像中的第一个特征圆的圆心及半径
//	TempStructCircle &circle2，图像中的第二个特征圆的圆心及半径
//     ...
// Return:
//     0 - 正常
//     -1 - 返回错误，无法检测到特征圆
//     -2 - 暂无待定
// Author: Huang Yige/03/18/2017
/**********************************************/
IMG_INT CBmSocketLocation::VisBC_Screen(Ipp8u *&srcRoi, IppiSize Roi, TempStructCircle &circle1, TempStructCircle &circle2)
{

	//IppiSize Roi = { src.size.width,src.size.height };

	int markersNum = 0;

	VisBC_LabelMarker(srcRoi, Roi, markersNum);

	if (markersNum < 2)
	{
		//printf("the picture has not feature circle\n");
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
			if (NumMar[srcRoi[i*Roi.width + j] - 1] > (float)Roi.width*(float)Roi.height*bmsocket.AreaProportionMax || NumMar[srcRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*bmsocket.AreaProportionMin)
				//if (NumMar[srcRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*0.005 || NumMar[srcRoi[i*Roi.width + j] - 1] > (float)Roi.width*(float)Roi.height*0.025)
			{
				srcRoi[i*Roi.width + j] = 0;
			}
		}
	}

	VisBC_LabelMarker(srcRoi, Roi, markersNum);

	if (markersNum < 2)
	{
		//printf("the picture has not feature circle\n");
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

	/*featureCircle *feature;
	feature = (featureCircle*)malloc(markersNum * sizeof(featureCircle));*/

	vector<int>sign1;

	float *area = (float*)malloc(markersNum * sizeof(TempStructCircle));

	float disNx = 0.0, disNy = 0.0, disNrx = 0.0, disNry = 0.0;
	for (int k = 0; k < markersNum; k++)
	{
		area[k] = 0;
	}

	for (int k = 0; k < markersNum; k++)
	{
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				if (srcRoi[i*Roi.width + j] - 1 == k)
				{
					area[k]++;
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

		if (min_y[k] < bmsocket.BoundaryConstraint || fabs(max_x[k] - Roi.width) < bmsocket.BoundaryConstraint)
		{
			point[k].Radius = float(fabs(max_y[k] - min_y[k] - bmsocket.ReducedImageRadiusConstraint) < fabs(max_x[k] - min_x[k] - bmsocket.ReducedImageRadiusConstraint) ? (float)(max_y[k] - min_y[k]) / 2.0 : (float)(max_x[k] - min_x[k]) / 2.0);
		}
		else
		{
			point[k].Radius = (float)((max_x[k] - min_x[k]) / 2.0);
		}

		/*if (area[k] < bmsocket.ExcludedHole*fabs(max_y[k] - min_y[k])*fabs(max_x[k] - min_x[k]))
		{
			point[k].Radius = 0;
		}*/

		point[k].miny = min_y[k];
		point[k].minx = min_x[k];
		point[k].maxx = max_x[k];
		point[k].maxy = max_y[k];
		if (fabs(point[k].Radius * 2 - bmsocket.ReducedImageRadiusConstraint) < bmsocket.RoiRadiusFaultTolerant)
			sign1.push_back(k);

#ifdef DEBUG_BlobCircle
		printf("min_x%d=%d min_y%d=%d max_x%d=%d max_y%d=%d\n", k, min_x[k], k, min_y[k], k, max_x[k], k, max_y[k]);
#endif // !1

	}


	if (sign1.size() < 2)
	{
		//printf("the picture has not feature circle\n");
		free(area);
		free(point);
		free(sum_x);
		free(sum_y);
		free(num);
		free(min_x);
		free(min_y);
		free(max_x);
		free(max_y);
		free(NumMar);
		//free(feature);
		vector<int>().swap(sign1);
		return -1;
	}
	else
	{
		markersNum = sign1.size();
		JudgeStruct *TwoBlob;
		TwoBlob = (JudgeStruct*)malloc((markersNum*(markersNum - 1) / 2) * sizeof(JudgeStruct));
		double minDis = 0.0;
		int sign = 0;

		float scalex = 0.0;
		float scaley = 0.0;

		int comparison = 0;
		//判断角度
		for (int i = 0; i < markersNum; i++)
		{
			for (int j = i + 1; j < markersNum; j++)
			{
				scalex = (point[sign1[i]].CirCen.x - point[sign1[j]].CirCen.x);
				scaley = (point[sign1[i]].CirCen.y - point[sign1[j]].CirCen.y);

				//scalex = fabs(point[sign1[i]].CirCen.x - point[sign1[j]].CirCen.x);
				//scaley = fabs(point[sign1[i]].CirCen.y - point[sign1[j]].CirCen.y);

				TwoBlob[comparison].dis = atan2(scaley, scalex);//abs(scaley / scalex - 0.503378);//角度
				TwoBlob[comparison].i = sign1[i];//bolb标号
				TwoBlob[comparison].j = sign1[j];//
				TwoBlob[comparison].length = sqrt((scalex)*(scalex)+(scaley)*(scaley));
				if (fabs(TwoBlob[comparison].length - bmsocket.DistanceConstraint) > bmsocket.RoiCircleDistanceFaultTolerant)
				{
					TwoBlob[comparison].dis = 0;
				}
				comparison++;
			}
		}
		//cout << tan(50 * PI / 180) << endl;
		//cout << tan(50 * PI / 180) << endl;
		//cout << atan2(700, -580) << endl;

		minDis = fabs(TwoBlob[0].dis) - bmsocket.AngleConstraint;
		sign = 0;
		float kkdis = float(fabs(TwoBlob[0].length - bmsocket.DistanceConstraint));
		for (int w = 1; w < (markersNum*(markersNum - 1) / 2); w++)
		{

			{
				if ((fabs(fabs(TwoBlob[w].dis) - bmsocket.AngleConstraint) < minDis && kkdis > fabs(TwoBlob[w].length - bmsocket.DistanceConstraint)) ||
					(fabs(fabs(TwoBlob[w].dis) - bmsocket.AngleConstraint) < 0.05 && 10 > fabs(TwoBlob[w].length - bmsocket.DistanceConstraint)))
				{
					minDis = (fabs(TwoBlob[w].dis) - bmsocket.AngleConstraint);
					kkdis = float(fabs(TwoBlob[w].length - bmsocket.DistanceConstraint));
					sign = w;

				}
			}
		}

		if (fabs(minDis) < 0.2)
		{
			if (sign == 0 && fabs(TwoBlob[sign].length - bmsocket.DistanceConstraint) < (bmsocket.RoiCircleDistanceFaultTolerant + 5))
			{
				circle1 = point[TwoBlob[sign].i];
				circle2 = point[TwoBlob[sign].j];
			}
			else
			{
				circle1 = point[TwoBlob[sign].i];
				circle2 = point[TwoBlob[sign].j];
			}

		}
		else
		{
			//printf("the picture has not feature circle\n");
			free(area);
			free(point);
			free(sum_x);
			free(sum_y);
			free(num);
			free(min_x);
			free(min_y);
			free(max_x);
			free(max_y);
			free(NumMar);
			//free(feature);
			free(TwoBlob);
			vector<int>().swap(sign1);
			return -1;
		}
		free(TwoBlob);
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

	free(area);
	free(point);
	free(sum_x);
	free(sum_y);
	free(num);
	free(min_x);
	free(min_y);
	free(max_x);
	free(max_y);
	free(NumMar);
	//free(feature);
	vector<int>().swap(sign1);
	return 0;

}



IMG_INT CBmSocketLocation::VisNewFindCircle(IMG_UBBUF src, IMG_WINDOW imgtop, IMG_WINDOW imgdown, TempStructCircle &circle1, TempStructCircle &circle2, IMG_INT threshold)
{
	
	int status = 0;

	IMG_UBBUF src1;
	src1.size = imgtop.szWindowSize;
	src1.linestep = imgtop.szWindowSize.width;
	src1.ptr = (IMG_UBYTE*)malloc(src1.size.height*src1.size.width * sizeof(IMG_UBYTE));
	//ofstream gh("1.txt");
	if ((imgtop.coWindowOff.x + src1.size.width) > src.size.width ||
		(imgtop.coWindowOff.x) < 0 ||
		(imgtop.coWindowOff.y + src1.size.height) > src.size.height ||
		(imgtop.coWindowOff.y) < 0
		)
	{
		free(src1.ptr);
		return -1;
	}
	for (int i = 0;i < src1.size.height;i++)
	{
		for (int j = 0;j < src1.size.width;j++)
		{
			src1.ptr[i*src1.size.width + j] = src.ptr[(imgtop.coWindowOff.x + j) + (imgtop.coWindowOff.y + i)*src.size.width];

		}

	}
	IppiSize Roi;
	Roi.width = src1.size.width;
	Roi.height = src1.size.height;

	Ipp8u *srcRoitop = (Ipp8u*)malloc(src1.size.height*src1.size.width * sizeof(Ipp8u));


	status = VisNewCoarse(src1, Roi, srcRoitop, threshold);
	if (status != 0)
	{
		free(src1.ptr);
		free(srcRoitop);
		return status;
	}

	/*for (int i = 0;i < Roi.height;i++)
	{
		for (int j = 0;j < Roi.width;j++)
		{
			gh << (double)srcRoitop[i* Roi.width + j] << "  ";
		}
		gh << endl;
	}
	gh.close();*/

	IMG_UBBUF src2;
	src2.size = imgdown.szWindowSize;
	src2.linestep = imgdown.szWindowSize.width;
	src2.ptr = (IMG_UBYTE*)malloc(src2.size.height*src2.size.width * sizeof(IMG_UBYTE));
	if ((imgdown.coWindowOff.x + src2.size.width) > src.size.width ||
		(imgdown.coWindowOff.x) < 0 ||
		(imgdown.coWindowOff.y + src2.size.height) > src.size.height ||
		(imgdown.coWindowOff.y) < 0
		)
	{
		free(src1.ptr);
		free(srcRoitop);
		free(src2.ptr);
		return -1;
	}

	for (int i = 0;i < src2.size.height;i++)
	{
		for (int j = 0;j < src2.size.width;j++)
		{
			src2.ptr[i*src2.size.width + j] = src.ptr[(imgdown.coWindowOff.x + j) + (imgdown.coWindowOff.y + i)*src.size.width];
			//gh << double(src2.ptr[i*src2.size.width + j]) << "   ";
		}
		//gh << endl;
	}
	//gh.close();
	IppiSize Roi2;
	Roi2.width = src2.size.width;
	Roi2.height = src2.size.height;
	Ipp8u *srcRoidown = (Ipp8u*)malloc(src2.size.height*src2.size.width * sizeof(Ipp8u));
	status = VisNewCoarse(src2, Roi2, srcRoidown, threshold);
	if (status != 0)
	{
		free(src2.ptr);
		free(srcRoidown);
		free(src1.ptr);
		free(srcRoitop);
		return status;
	}

	/*for (int i = 0;i < Roi2.height;i++)
	{
		for (int j = 0;j < Roi2.width;j++)
		{
			gh << (double)srcRoidown[i* Roi2.width + j] << "  ";
		}
		gh << endl;
	}
	gh.close();*/

	/*IppiSize Roitop;
	Roitop.width = src1.size.width;
	Roitop.height = src1.size.height;
	IppiSize Roidown;
	Roidown.width = src1.size.width;
	Roidown.height = src1.size.height;*/
	status = VisNewScree(srcRoitop, srcRoidown, imgtop, imgdown, Roi, Roi2, circle1, circle2);
	if (status != 0)
	{
		free(src2.ptr);
		free(srcRoidown);
		free(src1.ptr);
		free(srcRoitop);
		return status;
	}

	circle1.CirCen.x = circle1.CirCen.x * 2 * int(bmsocket.PyramidLevel) + imgtop.coWindowOff.x;
	circle1.CirCen.y = circle1.CirCen.y * 2 * int(bmsocket.PyramidLevel) + imgtop.coWindowOff.y;
	circle1.Radius = circle1.Radius * 2 * int(bmsocket.PyramidLevel);


	circle2.CirCen.x = circle2.CirCen.x * 2 * int(bmsocket.PyramidLevel) + imgdown.coWindowOff.x;
	circle2.CirCen.y = circle2.CirCen.y * 2 * int(bmsocket.PyramidLevel) + imgdown.coWindowOff.y;
	circle2.Radius = circle2.Radius * 2 * int(bmsocket.PyramidLevel);


	free(src2.ptr);
	free(srcRoidown);
	free(src1.ptr);
	free(srcRoitop);


	//int *p = (int*)malloc(2 * sizeof(int));
	//p[2] = 0;

	//int a = 0;
	//int ksdhfa = 1 / a;
	return status;
	
	//return status;
}


IMG_INT CBmSocketLocation::VisNewScree(Ipp8u*srcRoitop, Ipp8u*srcRoidown, IMG_WINDOW imgtop, IMG_WINDOW imgdown, IppiSize Roitop,
	IppiSize Roidown, TempStructCircle &circle1, TempStructCircle &circle2)
{
	int status = 0;

	int markersNum = 0;

	VisBC_LabelMarker(srcRoitop, Roitop, markersNum);
	//_CrtDumpMemoryLeaks();
	if (markersNum < 1)
	{
		//printf("the picture has not feature circle\n");
		return -1;
	}

	int *NumMar;//area
	NumMar = (int*)malloc(markersNum * sizeof(int));

	for (int i = 0; i < markersNum; i++)
	{
		NumMar[i] = 0;
	}

	int grade = 0;

	for (int i = 0; i < Roitop.height; i++)
	{
		for (int j = 0; j < Roitop.width; j++)
		{
			grade = srcRoitop[i*Roitop.width + j];
			if (grade > 0)
			{
				NumMar[grade - 1]++;
			}
		}
	}
	/*FILE *hp;
	hp = fopen("marker12.txt", "w");
	for (int i = 0; i < Roitop.height; i++)
	{
		for (int j = 0; j < Roitop.width; j++)
		{
			fprintf(hp, "%d ", 20 * srcRoitop[i*Roitop.width + j]);
		}
		fprintf(hp, "\n");
	}
	fclose(hp);*/
	//根据面积进行筛选
	for (int i = 0; i < Roitop.height; i++)
	{
		for (int j = 0; j < Roitop.width; j++)
		{
			if (NumMar[srcRoitop[i*Roitop.width + j] - 1] > (float)Roitop.width*(float)Roitop.height*bmsocket.AreaProportionMax || NumMar[srcRoitop[i*Roitop.width + j] - 1] < (float)Roitop.width*(float)Roitop.height*bmsocket.AreaProportionMin)
				//if (NumMar[srcRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*0.005 || NumMar[srcRoi[i*Roi.width + j] - 1] > (float)Roi.width*(float)Roi.height*0.025)
			{
				srcRoitop[i*Roitop.width + j] = 0;
			}
		}
	}

	VisBC_LabelMarker(srcRoitop, Roitop, markersNum);

	if (markersNum < 1)
	{
		//printf("the picture has not feature circle\n");
		free(NumMar);
		return -1;
	}

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

	/*featureCircle *feature;
	feature = (featureCircle*)malloc(markersNum * sizeof(featureCircle));*/

	vector<int>sign1;

	float *area = (float*)malloc(markersNum * sizeof(TempStructCircle));

	float disNx = 0.0, disNy = 0.0, disNrx = 0.0, disNry = 0.0;
	for (int k = 0; k < markersNum; k++)
	{
		area[k] = 0;
	}

	for (int k = 0; k < markersNum; k++)
	{
		for (int i = 0; i < Roitop.height; i++)
		{
			for (int j = 0; j < Roitop.width; j++)
			{
				if (srcRoitop[i*Roitop.width + j] - 1 == k)
				{
					area[k]++;
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

		if (min_y[k] < bmsocket.BoundaryConstraint || fabs(max_x[k] - Roitop.width) < bmsocket.BoundaryConstraint)
		{
			point[k].Radius = float(fabs(max_y[k] - min_y[k] - bmsocket.ReducedImageRadiusConstraint) < fabs(max_x[k] - min_x[k] - bmsocket.ReducedImageRadiusConstraint) ? (float)(max_y[k] - min_y[k]) / 2.0 : (float)(max_x[k] - min_x[k]) / 2.0);
		}
		else
		{
			point[k].Radius = (float)((max_x[k] - min_x[k]) / 2.0);
		}

		/*if (area[k] < bmsocket.ExcludedHole*fabs(max_y[k] - min_y[k])*fabs(max_x[k] - min_x[k]))
		{
			point[k].Radius = 0;
		}*/

		point[k].miny = min_y[k];
		point[k].minx = min_x[k];
		point[k].maxx = max_x[k];
		point[k].maxy = max_y[k];
		//if (fabs(point[k].Radius * 2 - bmsocket.ReducedImageRadiusConstraint) < bmsocket.RoiRadiusFaultTolerant)
		sign1.push_back(k);

#ifdef DEBUG_BlobCircle
		printf("min_x%d=%d min_y%d=%d max_x%d=%d max_y%d=%d\n", k, min_x[k], k, min_y[k], k, max_x[k], k, max_y[k]);
#endif // !1

	}


	if (sign1.size() < 1)
	{
		//printf("the picture has not feature circle\n");
		free(area);
		free(point);
		free(sum_x);
		free(sum_y);
		free(num);
		free(min_x);
		free(min_y);
		free(max_x);
		free(max_y);
		free(NumMar);
		//free(feature);
		vector<int>().swap(sign1);
		return -1;
	}

	////////////////////////////////////////////////////


	int markersNumdown = 0;

	VisBC_LabelMarker(srcRoidown, Roidown, markersNumdown);

	if (markersNumdown < 1)
	{
		free(area);
		free(point);
		free(sum_x);
		free(sum_y);
		free(num);
		free(min_x);
		free(min_y);
		free(max_x);
		free(max_y);
		free(NumMar);
		//free(feature);
		vector<int>().swap(sign1);
		//printf("the picture has not feature circle\n");
		return -1;
	}

	int *NumMardown;//area
	NumMardown = (int*)malloc(markersNumdown * sizeof(int));

	for (int i = 0; i < markersNumdown; i++)
	{
		NumMardown[i] = 0;
	}

	int gradedown = 0;

	for (int i = 0; i < Roidown.height; i++)
	{
		for (int j = 0; j < Roidown.width; j++)
		{
			gradedown = srcRoidown[i*Roidown.width + j];
			if (gradedown > 0)
			{
				NumMardown[gradedown - 1]++;
			}
		}
	}
	/*FILE *hpo;
	hpo = fopen("marker121.txt", "w");
	for (int i = 0; i < Roidown.height; i++)
	{
		for (int j = 0; j < Roidown.width; j++)
		{
			fprintf(hpo, "%d ", 20 * srcRoidown[i*Roidown.width + j]);
		}
		fprintf(hpo, "\n");
	}
	fclose(hpo);*/
	//根据面积进行筛选
	for (int i = 0; i < Roidown.height; i++)
	{
		for (int j = 0; j < Roidown.width; j++)
		{
			if (NumMardown[srcRoidown[i*Roidown.width + j] - 1] > (float)Roidown.width*(float)Roidown.height*bmsocket.AreaProportionMax || NumMardown[srcRoidown[i*Roidown.width + j] - 1] < (float)Roidown.width*(float)Roidown.height*bmsocket.AreaProportionMin)
				//if (NumMar[srcRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*0.005 || NumMar[srcRoi[i*Roi.width + j] - 1] > (float)Roi.width*(float)Roi.height*0.025)
			{
				srcRoidown[i*Roidown.width + j] = 0;
			}
		}
	}

	VisBC_LabelMarker(srcRoidown, Roidown, markersNumdown);

	if (markersNumdown < 1)
	{
		//printf("the picture has not feature circle\n");
		free(area);
		free(point);
		free(sum_x);
		free(sum_y);
		free(num);
		free(min_x);
		free(min_y);
		free(max_x);
		free(max_y);
		free(NumMar);
		//free(feature);
		vector<int>().swap(sign1);
		free(NumMardown);
		return -1;
	}

	//计算每个Blob块的左右上下边界点，中心点
	//int *sum_x, *sum_y, *num, *min_x, *max_x, *min_y, *max_y;
	free(sum_x);
	free(sum_y);
	free(num);
	free(min_x);
	free(min_y);
	free(max_x);
	free(max_y);

	sum_x = (int*)malloc(markersNumdown * sizeof(int));
	sum_y = (int*)malloc(markersNumdown * sizeof(int));
	num = (int*)malloc(markersNumdown * sizeof(int));
	min_x = (int*)malloc(markersNumdown * sizeof(int));
	max_x = (int*)malloc(markersNumdown * sizeof(int));
	min_y = (int*)malloc(markersNumdown * sizeof(int));
	max_y = (int*)malloc(markersNumdown * sizeof(int));
	for (int i = 0; i < markersNumdown; i++)
	{
		sum_x[i] = 0;
		sum_y[i] = 0;
		num[i] = 0;
		min_x[i] = 0;
		max_x[i] = 0;
		min_y[i] = 0;
		max_y[i] = 0;
	}

	TempStructCircle *pointdown;
	pointdown = (TempStructCircle*)malloc(markersNumdown * sizeof(TempStructCircle));

	/*featureCircle *feature;
	feature = (featureCircle*)malloc(markersNum * sizeof(featureCircle));*/

	vector<int>sign1down;

	float *areadown = (float*)malloc(markersNumdown * sizeof(TempStructCircle));

	//float 
	disNx = 0.0, disNy = 0.0, disNrx = 0.0, disNry = 0.0;
	for (int k = 0; k < markersNumdown; k++)
	{
		areadown[k] = 0;
	}

	for (int k = 0; k < markersNumdown; k++)
	{
		for (int i = 0; i < Roidown.height; i++)
		{
			for (int j = 0; j < Roidown.width; j++)
			{
				if (srcRoidown[i*Roidown.width + j] - 1 == k)
				{
					areadown[k]++;
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
		pointdown[k].CirCen.x = (float)sum_x[k] / (float)num[k];
		pointdown[k].CirCen.y = (float)sum_y[k] / (float)num[k];

		if (min_y[k] < bmsocket.BoundaryConstraint || fabs(max_x[k] - Roidown.width) < bmsocket.BoundaryConstraint)
		{
			pointdown[k].Radius = float(fabs(max_y[k] - min_y[k] - bmsocket.ReducedImageRadiusConstraint) < fabs(max_x[k] - min_x[k] - bmsocket.ReducedImageRadiusConstraint) ? (float)(max_y[k] - min_y[k]) / 2.0 : (float)(max_x[k] - min_x[k]) / 2.0);
		}
		else
		{
			pointdown[k].Radius = (float)((max_x[k] - min_x[k]) / 2.0);
		}

		/*if (areadown[k] < bmsocket.ExcludedHole*fabs(max_y[k] - min_y[k])*fabs(max_x[k] - min_x[k]))
		{
			pointdown[k].Radius = 0;
		}*/

		pointdown[k].miny = min_y[k];
		pointdown[k].minx = min_x[k];
		pointdown[k].maxx = max_x[k];
		pointdown[k].maxy = max_y[k];
		//if (fabs(pointdown[k].Radius * 2 - bmsocket.ReducedImageRadiusConstraint) < bmsocket.RoiRadiusFaultTolerant)
		sign1down.push_back(k);

#ifdef DEBUG_BlobCircle
		printf("min_x%d=%d min_y%d=%d max_x%d=%d max_y%d=%d\n", k, min_x[k], k, min_y[k], k, max_x[k], k, max_y[k]);
#endif // !1

	}


	if (sign1down.size() < 1)
	{
		//printf("the picture has not feature circle\n");

		free(NumMar);
		//free(feature);
		vector<int>().swap(sign1);

		free(area);
		free(point);
		free(sum_x);
		free(sum_y);
		free(num);
		free(min_x);
		free(min_y);
		free(max_x);
		free(max_y);

		free(areadown);
		free(pointdown);
		free(NumMardown);
		//free(feature);
		vector<int>().swap(sign1down);
		return -1;
	}



	//else
	{
		//markersNum = sign1.size()+sign1down.size();
		JudgeStruct *TwoBlob;
		TwoBlob = (JudgeStruct*)malloc((sign1.size()*sign1down.size()) * sizeof(JudgeStruct));
		double minDis = 0.0;
		int sign = 0;

		float scalex = 0.0;
		float scaley = 0.0;

		int comparison = 0;
		//判断角度
		for (unsigned int i = 0; i < sign1.size(); i++)
		{
			for (unsigned int j = 0; j < sign1down.size(); j++)
			{
				scalex = (point[sign1[i]].CirCen.x * 2 * int(bmsocket.PyramidLevel) + imgtop.coWindowOff.x - pointdown[sign1down[j]].CirCen.x * 2 * int(bmsocket.PyramidLevel) - imgdown.coWindowOff.x);
				scaley = (point[sign1[i]].CirCen.y * 2 * int(bmsocket.PyramidLevel) + imgtop.coWindowOff.y - pointdown[sign1down[j]].CirCen.y * 2 * int(bmsocket.PyramidLevel) - imgdown.coWindowOff.y);

				//scalex = fabs(point[sign1[i]].CirCen.x - point[sign1[j]].CirCen.x);
				//scaley = fabs(point[sign1[i]].CirCen.y - point[sign1[j]].CirCen.y);

				TwoBlob[comparison].dis = atan2(scaley, scalex);//abs(scaley / scalex - 0.503378);//角度
				TwoBlob[comparison].i = sign1[i];//bolb标号
				TwoBlob[comparison].j = sign1down[j];//
				TwoBlob[comparison].length = sqrt((scalex)*(scalex)+(scaley)*(scaley));
				if (fabs(TwoBlob[comparison].length - bmsocket.DistanceConstraint) > bmsocket.RoiCircleDistanceFaultTolerant)
				{
					TwoBlob[comparison].dis = 0;
				}
				comparison++;
			}
		}
		//cout << tan(50 * PI / 180) << endl;
		//cout << tan(50 * PI / 180) << endl;
		//cout << atan2(700, -580) << endl;

		minDis = fabs(TwoBlob[0].dis) - bmsocket.AngleConstraint;
		sign = 0;
		float kkdis = float(fabs(TwoBlob[0].length - bmsocket.DistanceConstraint));
		for (unsigned int w = 1; w < (sign1.size()*(sign1down.size())); w++)
		{

			{
				if ((fabs(fabs(TwoBlob[w].dis) - bmsocket.AngleConstraint) < minDis && kkdis > fabs(TwoBlob[w].length - bmsocket.DistanceConstraint)) ||
					(fabs(fabs(TwoBlob[w].dis) - bmsocket.AngleConstraint) < 0.05 && 10 > fabs(TwoBlob[w].length - bmsocket.DistanceConstraint)))
				{
					minDis = (fabs(TwoBlob[w].dis) - bmsocket.AngleConstraint);
					kkdis = float(fabs(TwoBlob[w].length - bmsocket.DistanceConstraint));
					sign = w;

				}
			}
		}

		if (fabs(minDis) < 0.4)
		{
			if (sign == 0 && fabs(TwoBlob[sign].length - bmsocket.DistanceConstraint) < (bmsocket.RoiCircleDistanceFaultTolerant + 5))
			{
				circle1 = point[TwoBlob[sign].i];
				circle2 = pointdown[TwoBlob[sign].j];
			}
			else
			{
				circle1 = point[TwoBlob[sign].i];
				circle2 = pointdown[TwoBlob[sign].j];
			}

		}
		else
		{
			//printf("the picture has not feature circle\n");
			free(area);
			free(point);
			free(sum_x);
			free(sum_y);
			free(num);
			free(min_x);
			free(min_y);
			free(max_x);
			free(max_y);
			free(NumMar);
			//free(feature);
			free(TwoBlob);
			vector<int>().swap(sign1);
			free(areadown);
			free(pointdown);
			free(NumMardown);
			//free(feature);
			vector<int>().swap(sign1down);
			return -1;
		}
		free(TwoBlob);
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

	free(area);
	free(point);
	free(sum_x);
	free(sum_y);
	free(num);
	free(min_x);
	free(min_y);
	free(max_x);
	free(max_y);
	free(NumMar);
	//free(feature);
	vector<int>().swap(sign1);
	free(areadown);
	free(pointdown);
	free(NumMardown);
	//free(feature);
	vector<int>().swap(sign1down);



	return status;

}

IMG_INT  CBmSocketLocation::VisNewCoarse(IMG_UBBUF src, IppiSize &Roi, Ipp8u* srcRoi, IMG_INT threshold)
{
	_try{
	int status = 0;

	int level = int(bmsocket.PyramidLevel);
	double sigma = 3.0;



	//Ipp8u* srcRoi;

	VisBC_Pyramid(src, srcRoi, Roi, level, sigma);

#ifdef DEBUG_BlobCircle
	FILE *hp111;
	hp111 = fopen("pyramid.txt", "w");
	for (int i = 0;i < Roi.height;i++)
	{
		for (int j = 0;j < Roi.width;j++)
		{
			fprintf(hp111, "%d ", srcRoi[i*Roi.width + j]);
		}
		fprintf(hp111, "\n");
	}
	fclose(hp111);
#endif

	VisBC_Binaryzation(srcRoi, Roi, threshold);



	VisBC_OpenClose(srcRoi, Roi);

	return status;
}
__except (EXCEPTION_EXECUTE_HANDLER)
{
	return _CODE_THROW;
}
}

_BACK_CODE CBmSocketLocation::SocketLocation(const IMG_UBBUF src,
	const int angleFlg, //0 or 180
	const double dist,	//DUT locating 输出的dist
	double &x, double &y, double &angle)	//输出角度逆时针为正
{

	_BACK_CODE status = _CODE_OK;

	TempStructCircle CirRa1;
	TempStructCircle CirRa2;
	CirRa1.CirCen = { 0,0 };
	CirRa1.Radius = 0;
	CirRa1.maxx = 0;
	CirRa1.maxy = 0;
	CirRa1.minx = 0;
	CirRa1.miny = 0;
	CirRa2.CirCen = { 0,0 };
	CirRa2.Radius = 0;
	CirRa2.maxx = 0;
	CirRa2.maxy = 0;
	CirRa2.minx = 0;
	CirRa2.miny = 0;

	IMG_WINDOW imgtop;
	imgtop.coWindowOff.x = (IMG_WORD)bmsocket.IMG_WINDOWimgtopcoWindowOffx;
	imgtop.coWindowOff.y = (IMG_WORD)bmsocket.IMG_WINDOWimgtopcoWindowOffy;
	imgtop.szWindowSize.width = (IMG_UWORD)bmsocket.IMG_WINDOWimgtopszWindowSizewidth;
	imgtop.szWindowSize.height = (IMG_UWORD)bmsocket.IMG_WINDOWimgtopszWindowSizeheight;
	IMG_WINDOW imgdown;
	imgdown.coWindowOff.x = (IMG_WORD)bmsocket.IMG_WINDOWimgdowncoWindowOffx;
	imgdown.coWindowOff.y = (IMG_WORD)bmsocket.IMG_WINDOWimgdowncoWindowOffy;
	imgdown.szWindowSize.width = (IMG_UWORD)bmsocket.IMG_WINDOWimgdownszWindowSizewidth;
	imgdown.szWindowSize.height = (IMG_UWORD)bmsocket.IMG_WINDOWimgdownszWindowSizeheight;
	IMG_INT threshold = (int)bmsocket.ThresholdImage;

	if (0 == angleFlg)
	{
		int status = VisNewFindCircle(src, imgtop, imgdown, CirRa1, CirRa2, threshold);
		if (status == -4)
		{
			return _CODE_PROERROR;
		}
		else if (status != -4 && status != 0)
		{
			return _CODE_LOCATE_FAIL;
		}

		double topX = CirRa1.CirCen.x;
		double topY = CirRa1.CirCen.y;
		double downX = CirRa2.CirCen.x;
		double downY = CirRa2.CirCen.y;
		//double dist = 180;
		status = GetSunctionPosition(topX, topY, downX, downY, dist, angle, x, y);
		if (status != 0)
		{
			return _CODE_LOCATE_FAIL;
		}

		return _CODE_OK;
	}
	else if (180 == angleFlg)
	{
		IMG_UBBUF src1;
		src1.linestep = src.linestep;
		src1.size = src.size;
		src1.ptr = (IMG_UBYTE*)malloc(sizeof(IMG_UBYTE)*src.size.width*src.size.height);
		//ofstream kkkkk("180.txt");
		for (int i = 0;i < src.size.height;i++)
		{
			for (int j = 0;j < src.size.width;j++)
			{//可以金字塔上旋转
				src1.ptr[i*src.size.width + j] = src.ptr[(src.size.height - i - 1)*src.size.width + src.size.width - j - 1];
				//kkkkk <<(double) src1.ptr[i*src.size.width + j] << "  ";
			}
			//kkkkk << endl;
		}
		//imgtop.coWindowOff.x = (IMG_WORD)(src.size.width - bmsocket.IMG_WINDOWimgtopcoWindowOffx - 1);
		//imgtop.coWindowOff.y = (IMG_WORD)(src.size.height - bmsocket.IMG_WINDOWimgtopcoWindowOffy - 1);
		//imgtop.szWindowSize.width = bmsocket.IMG_WINDOWimgtopszWindowSizewidth;
		//imgtop.szWindowSize.height = bmsocket.IMG_WINDOWimgtopszWindowSizeheight;

		//imgdown.coWindowOff.x = (IMG_WORD)(src.size.width - bmsocket.IMG_WINDOWimgdowncoWindowOffx - 1);
		//imgdown.coWindowOff.y = (IMG_WORD)(src.size.height - bmsocket.IMG_WINDOWimgdowncoWindowOffy - 1);
		//imgdown.szWindowSize.width = bmsocket.IMG_WINDOWimgdownszWindowSizewidth;
		//imgdown.szWindowSize.height = bmsocket.IMG_WINDOWimgdownszWindowSizeheight;

		int status = VisNewFindCircle(src1, imgtop, imgdown, CirRa1, CirRa2, threshold);

		if (status == -4)
		{
			free(src1.ptr);
			return _CODE_PROERROR;
		}
		else if (status != -4 && status != 0)
		{
			free(src1.ptr);
			return _CODE_LOCATE_FAIL;
		}

		double topX = CirRa1.CirCen.x;
		double topY = CirRa1.CirCen.y;
		double downX = CirRa2.CirCen.x;
		double downY = CirRa2.CirCen.y;
		//double dist = 180;
		double Angle = 0;
		double xx = 0;
		double yy = 0;
		status = GetSunctionPosition(topX, topY, downX, downY, dist, Angle, xx, yy);
		if (status != 0)
		{
			free(src1.ptr);
			return _CODE_LOCATE_FAIL;
		}

		angle = 180 - Angle;//PI - Angle;
		x = src.size.width - xx - 1;
		y = src.size.height - yy - 1;

		if (src1.ptr != nullptr)
			free(src1.ptr);
		return _CODE_OK;
	}
	else
	{
		return _CODE_ANGLE_FAIL;
	}
	return status;
	
}

/**********************************************/
// VisBlobCircle, 功能说明:对图像中的特征圆进行粗定位。
// Input:
// 	IMG_UBBUF src, 输入图片
//	int threshold	梯度强度的阈值（理论上是大于0小于1250的整数）.
//     ...
// Output:
// 	TempStructCircle &circle1, 图像中的第一个特征圆的圆心及半径
//	TempStructCircle &circle2，图像中的第二个特征圆的圆心及半径
//     ...
// Return:
//     0 - 正常
//     -1 - 返回错误，无法检测到特征圆
// Author: 04/27/2017
/**********************************************/
IMG_INT CBmSocketLocation::VisBC_BlobCircle_coarse(IMG_UBBUF src, TempStructCircle &circle1, TempStructCircle &circle2, IMG_INT threshold)
{
	_try{
	int level = int(bmsocket.PyramidLevel);
	double sigma = 3.0;

	IppiSize Roi;
	Roi.width = src.size.width;
	Roi.height = src.size.height;

	Ipp8u* srcRoi;
	srcRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

	VisBC_Pyramid(src, srcRoi, Roi, level, sigma);

#ifdef DEBUG_BlobCircle
	FILE *hp111;
	hp111 = fopen("pyramid.txt", "w");
	for (int i = 0;i < Roi.height;i++)
	{
		for (int j = 0;j < Roi.width;j++)
		{
			fprintf(hp111, "%d ", srcRoi[i*Roi.width + j]);
		}
		fprintf(hp111, "\n");
	}
	fclose(hp111);
#endif

	VisBC_Binaryzation(srcRoi, Roi, threshold);



	VisBC_OpenClose(srcRoi, Roi);




	//LabelMarker(dstRoi, Roi);

	int value = 0;

	value = VisBC_Screen(srcRoi, Roi, circle1, circle2);
	if (value == -1)
	{
		//printf("can not find the feature circle!\n");
		free(srcRoi);
		return -1;
	}
	free(srcRoi);
	return 0;
}
__except (EXCEPTION_EXECUTE_HANDLER)
{
	return _CODE_THROW;
}
}

/**********************************************/
//VisEdge_detection功能说明
//Input
//srcRoi   输入图像
//roiSize  输入图像的尺寸
//threshold  梯度强度的阈值（理论上是大于0小于1250的整数）。如果用户不知道阈值设为多少合适，可以输入0（算法自动获取合适的阈值）。
//
//output
//dstRoi  梯度强度
//edgeInformation *&edgeArray  边缘点信息，包括像素坐标、亚像素坐标、梯度强度、角度
//
//函数返回
//正常情况下返回0；
//如果用户输入阈值小于0，函数返回-1；
//如果输入参数不正确，包括图像尺寸不正确、srcRoi大小与尺寸不符合，函数返回-1。
/**********************************************/
IMG_VVOID CBmSocketLocation::VisBC_Fix_Sobel(IMG_UBYTE * pSrc, IppiSize roiSize, Ipp16s *&pDst, Ipp32f *&pAngle)
{

	IppiMaskSize mask = ippMskSize3x3;
	IppiBorderType bordertype = ippBorderRepl; //Border is replicated from the edge pixels
	Ipp16s *pHoriz, *pVert;

	int srcStep = roiSize.width * sizeof(Ipp8u);

	int dstStep = roiSize.width * sizeof(Ipp16s);
	int angleStep = roiSize.width * sizeof(Ipp32f);
	int bufferSize;
	int bufLen = roiSize.width * roiSize.height;
	//IppStatus statusVert, statusHoriz, status;
	Ipp8u *pBuffer;
	IppNormType normType = ippNormL2;//input gradient magnitude

	pVert = (Ipp16s *)malloc(sizeof(Ipp16s)*bufLen);
	pHoriz = (Ipp16s *)malloc(sizeof(Ipp16s)*bufLen);
	//pAngle = (Ipp32f *)malloc(sizeof(Ipp32f)*bufLen);
	//pDst = (Ipp16s *)malloc(sizeof(Ipp16s)*bufLen);

	ippiGradientVectorGetBufferSize(roiSize, mask, ipp16s, 1, &bufferSize);
	pBuffer = (Ipp8u *)malloc(bufferSize);
	ippiGradientVectorSobel_8u16s_C1R(pSrc, srcStep, pVert, dstStep, pHoriz, dstStep, pDst, dstStep, pAngle, angleStep, roiSize, mask, normType, bordertype, NULL, pBuffer);

	if (pVert != nullptr)
		free(pVert);
	if (pHoriz != nullptr)
		free(pHoriz);
	if (pBuffer != nullptr)
		free(pBuffer);


}
//给定一个值threshold，计算最大类间方差
IMG_REAL CBmSocketLocation::getIntraClassVariance(Ipp16s* src, IMG_INT srcRows, IMG_INT srcCols, IMG_INT &varTh)//int &varian)
{
	_try{
		//intra-class variance
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
		varian = (float)(PA * (averageA - average) * (averageA - average)) + (float)(PB * (averageB - average) * (averageB - average));

		return varian;
	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}
//抛物线边缘检测
IMG_INT CBmSocketLocation::VisEdge_detection(IMG_UBYTE *srcRoi, IMG_SIZE roiSize, IMG_INT threshold, IMG_WORD *&dstRoi, IMG_UBYTE *dstRoiE, edgeInformation *&edgeArray, IMG_INT &eNum)
{

	//如果阈值小于0，函数直接返回-1
	if (threshold < 0)
	{
		return -1;
	}

	int roiRows = roiSize.height;
	int roiCols = roiSize.width;

	//角度信息
	Ipp32f *angAll;
	angAll = (Ipp32f*)malloc(roiRows*roiCols * sizeof(Ipp32f));

	std::vector<edgeInformation> edgeInfor;
	edgeInformation edInf;

	int k = 0;//记录边缘点的个数
	float k1;//抛物线拟合的三个已知点
	float k2;
	float k3;
	float deci;//抛物线拟合顶点的小数部分，即对应的亚像素
	float sumx = 0;//边缘点的x坐标之和
	float sumy = 0;
	int numberChannels = 1; //the source image is single channel

	IppiSize dstRoiSize = { roiCols,roiRows };

	//dstRoi = (Ipp16s *)malloc(sizeof(Ipp16s)*roiRows*roiCols);
	VisBC_Fix_Sobel(srcRoi, dstRoiSize, dstRoi, angAll);

	if (threshold == 0)
	{
		//Otsu法，遍历所有的灰度值，从1到255，使intra-class invariance最大的那个值，即为要求的阈值
		int varian = 0;
		int temp = 0;
		for (int p = 1; p < 1250; p++)
		{
			temp = (int)getIntraClassVariance(dstRoi, roiRows, roiCols, p);
			if (varian < temp)
			{
				varian = temp;
				threshold = p;
			}
		}
	}
	//printf("%d\n",threshold);

	//到亚像素
	for (int i = 1; i < roiRows - 1; i++)
	{
		for (int j = 1; j < roiCols - 1; j++)
		{
			if (dstRoi[j + i*roiCols] > threshold)
			{
				angAll[j + i * roiCols] = (float)180 - (float)angAll[j + i * roiCols] / (float)(PI * 180);
				if ((angAll[j + i*roiCols] > 22.5) && (angAll[j + i*roiCols] < 67.5))
				{
					if ((dstRoi[j + i*roiCols] > dstRoi[j - 1 + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j + 1) + (i + 1)*roiCols]))
					{
						k1 = (float)dstRoi[j - 1 + (i - 1)*roiCols];
						k2 = (float)dstRoi[j + i*roiCols];
						k3 = (float)dstRoi[(j + 1) + (i + 1)*roiCols];
						deci = (float)(k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));


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
							k3 = (float)dstRoi[j - 1 + (i - 1)*roiCols];
							k2 = (float)dstRoi[j + i*roiCols];
							k1 = (float)dstRoi[(j + 1) + (i + 1)*roiCols];
							deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

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
								k1 = (float)dstRoi[(j + 1) + (i - 1)*roiCols];
								k2 = (float)dstRoi[j + i*roiCols];
								k3 = (float)dstRoi[(j - 1) + (i + 1)*roiCols];
								deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

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
									k3 = (float)dstRoi[(j + 1) + (i - 1)*roiCols];
									k2 = (float)dstRoi[j + i*roiCols];
									k1 = (float)dstRoi[(j - 1) + (i + 1)*roiCols];
									deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

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
										k1 = (float)dstRoi[(j - 1) + i*roiCols];
										k2 = (float)dstRoi[j + i*roiCols];
										k3 = (float)dstRoi[(j + 1) + i*roiCols];
										deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

										edInf.xyInteger.x = j;
										edInf.xyInteger.y = i;
										edInf.xyDecimal.x = j + deci;
										edInf.xyDecimal.y = (float)i;
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
											k3 = (float)dstRoi[(j - 1) + i*roiCols];
											k2 = (float)dstRoi[j + i*roiCols];
											k1 = (float)dstRoi[(j + 1) + i*roiCols];
											deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

											edInf.xyInteger.x = j;
											edInf.xyInteger.y = i;
											edInf.xyDecimal.x = j - deci;
											edInf.xyDecimal.y = (float)i;
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
												k1 = (float)dstRoi[j + (i - 1)*roiCols];
												k2 = (float)dstRoi[j + i*roiCols];
												k3 = (float)dstRoi[j + (i + 1)*roiCols];
												deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

												edInf.xyInteger.x = j;
												edInf.xyInteger.y = i;
												edInf.xyDecimal.x = (float)j;
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
													deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

													edInf.xyInteger.x = j;
													edInf.xyInteger.y = i;
													edInf.xyDecimal.x = (float)j;
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
	//二值图
	for (int t = 0; t < roiCols*roiRows; t++)//二值图像，所有像素先都赋值为0，边缘点赋值255
	{
		dstRoiE[t] = 0;
	}
	for (int q = 0; q < k; q++)
	{
		dstRoiE[edgeInfor[q].xyInteger.x + edgeInfor[q].xyInteger.y * roiCols] = 255;
	}
	edgeArray = (edgeInformation*)malloc(k * sizeof(edgeInformation));
	for (int i = 0; i < k; i++)
	{
		edgeArray[i] = edgeInfor[i];
	}

	vector<edgeInformation>().swap(edgeInfor);

	free(angAll);
	return 0;


}
/**********************************************/
// VisBC_RoiGnerate_Circular_Fitting, 拟合圆
// Input:
// 	IMG_REAL *point_pos, 输入点的坐标
//	IMG_INT m,输入点的数量
//  IMG_INT iteration_times，控制迭代次数
// Output:
// 	CIRCULAR_FITT circular_fit，拟合圆中心点(a,b)和半径输出r;
//     ...
// Return:
//     0 - 正常
//     1 - 点数太少
//     2 - 直线
// Author: 03/18/2017
/**********************************************/
IMG_INT CBmSocketLocation::VisRoiGnerate_Circular_Fitting(IMG_REAL *point_pos, IMG_INT m, CIRCULAR_FITT &circular_fit)
{

	_try{
	int status = 0;
	IMG_INT iteration_times = 4;
	if (m < 3)
	{
		//printf("point number <3,can't fitting circular\n");
		return 1;
	}
	int tempxnum = 0, tempynum = 0;
	for (int i = 0;i < m * 2;i = i + 2)
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

		for (int i = 0, j = 0, k = 0;i < 3 * m;i = i + 3, j = j + 2, k++)
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
			error_weight[i] = (float)1.0 / (float)(1 + exp((error[i]) / (float)(bmsocket.FittingSigma*bmsocket.FittingSigma)));
		}
		temp_time++;
	} while (abs(nowsum - lastsum) > 0.0002 && temp_time < iteration_times);
	/*if (nowsum >= 1000000)
	{
	printf("Too big error\n");
	return 3;
	}*/
	circular_fit.a = C[0];
	circular_fit.b = C[1];
	circular_fit.r = sqrt(circular_fit.a*circular_fit.a + circular_fit.b*circular_fit.b - C[2]);

end:
	if (error_weight != nullptr) free(error_weight);
	if (A != nullptr) free(A);
	if (B != nullptr) free(B);
	if (C != nullptr) free(C);
	if (recordB != nullptr) free(recordB);
	if (recordC != nullptr) free(recordC);
	if (error != nullptr) free(error);
	return status;

	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

/**********************************************/
// Vis_Calculate_Circle_CompleteDegree_seach_match,实现自定义方式圆度和误差计算
// Input:
// 	CIRCULAR_FITT circular_fit，圆的中心及半径（x-a)*(x-a)+(y-b)*(y-b)=r*r;
// 	IMG_INT m,点的个数；
//	IMG_REAL *point_pos；点的坐标；
//  IMG_INT Score_width；打分宽度，如：5那么点到圆上距离为五时得1分，点到圆上距离为四时得2分
//										    点到圆上距离为三时得3分，点到圆上距离为二时得4分；依次类推
// Output:
// 	IMG_REAL &degreescore;圆度打分结果
//	
//     ...
// Return:
//     0 - 正常
//	   -1 - 打分宽度小于0或点数小于1
// Author: 03/23/2017
/**********************************************/
IMG_INT CBmSocketLocation::Vis_Calculate_Circle_CompleteDegree_seach_match(CIRCULAR_FITT circular_fit, IMG_INT m, IMG_REAL *point_pos, IMG_INT Score_width, IMG_REAL &degreescore)
{

	_try{
	int status = 0;

	//double error_score = 0;

	degreescore = 0;
	if (m <= 0 || Score_width < 0)
	{
		status = -1;
		return status;
	}

	float a = circular_fit.a;
	float b = circular_fit.b;
	float r = circular_fit.r;

	float *edge_point = (float*)malloc(m * 2 * sizeof(float));
	for (int i = 0;i < m * 2;i = i + 2)
	{
		edge_point[i] = point_pos[i] - a;
		edge_point[i + 1] = point_pos[i + 1] - b;
	}

	//float angle_initial = 0;
	float angular_increment = 2;
	float x = 0;
	float y = 0;
	double dis = Score_width + 1;
	//逆时针旋转匹配
	for (float i = 0;i <= 360;i = i + angular_increment)
	{
		//i = i / 180 * PI;
		x = float(r*cos(double(i / 180 * PI)));
		y = float(r*sin(double(i / 180 * PI)));
		float x1 = x - Score_width;
		float x2 = x + Score_width;
		float y1 = y - Score_width;
		float y2 = y + Score_width;

		for (int j = 0;j < m * 2;j = j + 2)
		{
			if (edge_point[j] >= x1&&
				edge_point[j] <= x2&&
				edge_point[j + 1] >= y1&&
				edge_point[j + 1] <= y
				)
			{
				double temp_d = sqrt((edge_point[j] - x)*(edge_point[j] - x) +
					(edge_point[j + 1] - y)*(edge_point[j + 1] - y));
				if (temp_d < dis)
				{
					dis = temp_d;
				}
			}
		}
		if (dis <= Score_width)
			for (int k = 0;k <= Score_width;k++)
			{
				if (dis <= k)
				{
					degreescore += (Score_width - k);//整数打分
													 //degreescore += (Score_width-dis);//浮点打分
					dis = Score_width + 1;
					break;
				}
			}

	}

	if (Score_width != 0)
		degreescore = float(degreescore / (Score_width*360.0 / angular_increment));

	if (edge_point != nullptr)
		free(edge_point);

	return status;

	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}
/**********************************************/
// Assist_VisBC_BlobCircleAccurate，针对每一个圆进行定位计算
// Input:
// 	IMG_UBBUF src, 输入图像roi
// 	TempStructCircle *CirRa, 图像中特征圆的圆心及半径
//	IMG_INT time,半径误差大小，宽度
//	
// Output:
// 	TempStructCircle *CirRA, 图像中特征圆的圆心及半径结果
//     ...
// Return:
//     1 - 点数太少
//     2 - 直线
//	   -3 - 拟合圆误差太大
//	   -4 - 园完整度太小
// Author: 03/18/2017
/**********************************************/
IMG_INT CBmSocketLocation::Assist_VisBC_BlobCircleAccurate_order(IMG_UBBUF drc, TempStructCircle &CirRa, IMG_INT time)
{


	IMG_INT status = 0;

	IMG_BOOL flag = true;
	IMG_INT threshold = 150;
	IMG_UBYTE *dstRoiE;// = NULL;
	edgeInformation *edgeArray_roi;
	IMG_INT numroi = 0;
	IMG_WORD *dstRoi;
	CIRCULAR_FITT circular_fit;
	circular_fit.a = CirRa.CirCen.x;
	circular_fit.b = CirRa.CirCen.y;
	circular_fit.r = CirRa.Radius;

	IMG_REAL *point_pos;
	IMG_REAL score = 0;
	vector<edgeInformation> selectpoint;
	edgeInformation selectpointtemp;

	dstRoiE = (IMG_UBYTE*)malloc(drc.size.height*drc.size.width * sizeof(IMG_UBYTE));

	float temp_width = 36.0;
	int temp_width2 = 36;
	IMG_INT *point_num = (IMG_INT*)malloc(sizeof(IMG_INT) * temp_width2);
	for (IMG_INT i = 0;i < temp_width2;i++)
	{
		point_num[i] = 0;
	}
	dstRoi = (Ipp16s *)malloc(sizeof(Ipp16s)*drc.size.height*drc.size.width);
	status = VisEdge_detection(drc.ptr, drc.size, threshold, dstRoi, dstRoiE, edgeArray_roi, numroi);

	IMG_REAL re_deviation_x = CirRa.CirCen.x - CirRa.Radius - time;
	IMG_REAL re_deviation_y = CirRa.CirCen.y - CirRa.Radius - time;

	for (IMG_INT i = 0;i < numroi;i++)
	{
		float sqrtx = sqrt((edgeArray_roi[i].xyDecimal.x + re_deviation_x - circular_fit.a)*(edgeArray_roi[i].xyDecimal.x + re_deviation_x - circular_fit.a) +
			(edgeArray_roi[i].xyDecimal.y + re_deviation_y - circular_fit.b)*(edgeArray_roi[i].xyDecimal.y + re_deviation_y - circular_fit.b));
		float j = float(0.9);
		for (int k = 0;k < temp_width2;k++)
		{
			if ((sqrtx < ((j + (1.3 - 0.9) / temp_width)*circular_fit.r)) && (sqrtx > (j*circular_fit.r)))
			{
				point_num[k]++;
			}
			j = float(j + (1.3 - 0.9) / temp_width);
		}
	}

	IMG_INT postion_point = 0;
	for (IMG_INT i = 1;i < temp_width2 - 1;i++)
	{
		if (time < 10)
		{
			if (point_num[i] > point_num[postion_point])
			{
				postion_point = i;
			}
		}
		else
		{
			if (point_num[i] > point_num[postion_point])
			{
				postion_point = i;
			}
		}
	}

	for (IMG_INT i = 0;i < numroi;i++)
	{

		float sqrtx = sqrt((edgeArray_roi[i].xyDecimal.x + re_deviation_x - circular_fit.a)*(edgeArray_roi[i].xyDecimal.x + re_deviation_x - circular_fit.a) +
			(edgeArray_roi[i].xyDecimal.y + re_deviation_y - circular_fit.b)*(edgeArray_roi[i].xyDecimal.y + re_deviation_y - circular_fit.b));

		if ((sqrtx < ((0.9 + (postion_point + 4) * (1.3 - 0.9) / temp_width)*circular_fit.r)) && ((0.9 + (postion_point - 2) *  (1.3 - 0.9) / temp_width)*circular_fit.r) < sqrtx)
		{
			selectpointtemp.angle = edgeArray_roi[i].angle;
			selectpointtemp.gradient = edgeArray_roi[i].gradient;
			selectpointtemp.xyDecimal = edgeArray_roi[i].xyDecimal;
			selectpointtemp.xyInteger = edgeArray_roi[i].xyInteger;

			selectpoint.push_back(selectpointtemp);
		}

	}

	/*for (int i = 0;i < selectpoint.size();i++)
	{
	if (selectpoint[i].gradient < 200)
	{
	selectpoint.erase(selectpoint.begin() + i);
	i--;
	}
	}*/

	numroi = selectpoint.size();
	point_pos = (IMG_REAL*)malloc(numroi * 2 * sizeof(IMG_REAL));
	for (IMG_INT i = 0, j = 0;i < numroi;i++, j = j + 2)
	{
		point_pos[j] = selectpoint[i].xyDecimal.x;
		point_pos[j + 1] = selectpoint[i].xyDecimal.y;
	}

	status = VisRoiGnerate_Circular_Fitting(point_pos, numroi, circular_fit);

	//acceptance 50% and score 
	IMG_INT temp_num = 0;
	for (IMG_INT i = 0;i < numroi;i++)
	{
		IMG_LREAL temp_accept = fabs(sqrt((edgeArray_roi[i].xyDecimal.x - circular_fit.a)*(edgeArray_roi[i].xyDecimal.x - circular_fit.a) +
			(edgeArray_roi[i].xyDecimal.y - circular_fit.b)*(edgeArray_roi[i].xyDecimal.y - circular_fit.b)) - circular_fit.r);
		score += (float)temp_accept;
		// if (temp_accept >= 1)
		// {
		// temp_num++;
		// }
	}
	if ((score / numroi) > 100)
	{
#ifdef DEBUG_BlobCircle
		printf("image maybe error,this result error is bigger than normal\n");
#endif
		status = -4;
}

	IMG_REAL degreescore = 0;
	Vis_Calculate_Circle_CompleteDegree_seach_match(circular_fit, numroi, point_pos, int(0.245*circular_fit.r), degreescore);
	if (degreescore <= 0.10)
	{
#ifdef DEBUG_BlobCircle
		printf("Circle complete dgree is too small the target mayn't be round!\n");
#endif
		status = -3;
	}
	/*if (temp_num > (numroi)*1.0 / 2.0)
	{

	printf("can't reach acceptability!\n");
	vector<edgeInformation>().swap(selectpoint);

	if (dstRoiE != nullptr)
	free(dstRoiE);
	if (point_pos != nullptr)
	free(point_pos);
	if (point_num != nullptr)
	free(point_num);
	if (dstRoi != nullptr)
	free(dstRoi);
	if (edgeArray_roi != nullptr)
	free(edgeArray_roi);
	return -2;
	}*/

	CirRa.CirCen.x = circular_fit.a;
	CirRa.CirCen.y = circular_fit.b;
	CirRa.Radius = circular_fit.r;

	vector<edgeInformation>().swap(selectpoint);

	if (dstRoiE != nullptr)
		free(dstRoiE);
	if (point_pos != nullptr)
		free(point_pos);
	if (point_num != nullptr)
		free(point_num);
	if (dstRoi != nullptr)
		free(dstRoi);
	if (edgeArray_roi != nullptr)
		free(edgeArray_roi);
	return status;


	}



/**********************************************/
// Assist_VisBC_BlobCircleAccurate, 实现由白到黑找最大值抛物线拟合，回退到原图位置
// Input:
// 	IMG_UBBUF src, 输入图像
// 	TempStructCircle *&CirRa_in, 图像中的第一个特征圆的圆心及半径，图像中的第二个特征圆的圆心及半径
//	
// Output:
// 	TempStructCircle *&CirRa_out, 图像中的第一个特征圆的圆心及半径，图像中的第二个特征圆的圆心及半径
//     ...
// Return:
//     0 - 正常
//     1 - 点数太少
//     2 - 直线
//	   -3 - 拟合圆误差太大
//	   -4 - 园完整度太小
// Author: 03/18/2017
/**********************************************/
IMG_INT CBmSocketLocation::Assist_VisBC_BlobCircleAccurate(IMG_UBBUF src, TempStructCircle *&CirRa_in, TempStructCircle *&CirRa_out)
{

	_try{
	IMG_INT status = 0;

	IMG_UBBUF drc;
	drc.size.height = (IMG_UWORD)(2 * CirRa_in[0].Radius) + (IMG_UWORD)10;
	drc.size.width = (IMG_UWORD)(2 * CirRa_in[0].Radius) + (IMG_UWORD)10;
	drc.linestep = (IMG_UWORD)(2 * CirRa_in[0].Radius) + (IMG_UWORD)10;
	IMG_REAL re_deviation_x = CirRa_in[0].CirCen.x - CirRa_in[0].Radius - 5;
	IMG_REAL re_deviation_y = CirRa_in[0].CirCen.y - CirRa_in[0].Radius - 5;

	if ((int)(drc.size.height + re_deviation_y)*src.size.width + (int)(drc.size.width + re_deviation_x) > src.size.width*src.size.height ||
		(int)(re_deviation_y)*src.size.width + (int)(re_deviation_x) < 0)
	{
		//printf("border image one!\n");
		//if (drc.ptr != nullptr)
		//free(drc.ptr);

		drc.size.height = (IMG_UWORD)(CirRa_in[0].CirCen.y + CirRa_in[0].Radius) + (IMG_UWORD)5;
		drc.size.width = (IMG_UWORD)(2 * CirRa_in[0].Radius) + (IMG_UWORD)10;
		drc.linestep = (IMG_UWORD)(2 * CirRa_in[0].Radius) + (IMG_UWORD)10;

		re_deviation_x = CirRa_in[0].CirCen.x - CirRa_in[0].Radius - 5;
		re_deviation_y = 0;

		//return -1;
	}

	drc.ptr = (IMG_UBYTE*)malloc(sizeof(IMG_UBYTE)*drc.size.height*drc.size.width);
	for (IMG_INT i = 0;i < drc.size.height;i++)
	{
		for (IMG_INT j = 0;j < drc.size.width;j++)
		{
			drc.ptr[i*drc.size.width + j] = src.ptr[(int)(i + re_deviation_y)*src.size.width + (int)(j + re_deviation_x)];

		}
	}


	TempStructCircle CirRa_1 = CirRa_in[0];
	status = Assist_VisBC_BlobCircleAccurate_order(drc, CirRa_1, 5);
	if (status != 0)
	{
		if (drc.ptr != nullptr)
			free(drc.ptr);

		return status;
	}

	CirRa_1.CirCen.x += re_deviation_x;
	CirRa_1.CirCen.y += re_deviation_y;
	if (drc.ptr != nullptr)
		free(drc.ptr);

	drc.size.height = (IMG_UWORD)(2 * CirRa_in[1].Radius) + (IMG_UWORD)20;
	drc.size.width = (IMG_UWORD)(2 * CirRa_in[1].Radius) + (IMG_UWORD)20;
	drc.linestep = (IMG_UWORD)(2 * CirRa_in[1].Radius) + (IMG_UWORD)20;
	//drc.ptr = (IMG_UBYTE*)malloc(sizeof(IMG_UBYTE)*drc.size.height*drc.size.width);
	re_deviation_x = CirRa_in[1].CirCen.x - CirRa_in[1].Radius - 10;
	re_deviation_y = CirRa_in[1].CirCen.y - CirRa_in[1].Radius - 10;
	if ((int)(drc.size.height + re_deviation_y)*src.size.width + (int)(drc.size.width + re_deviation_x) > src.size.width*src.size.height ||
		(int)(re_deviation_y)*src.size.width + (int)(re_deviation_x) < 0
		|| (drc.size.width + CirRa_in[1].CirCen.x) > src.size.width
		)
	{
		//printf("border image two!\n");
		//if (drc.ptr != nullptr)
		//free(drc.ptr);

		drc.size.width = (IMG_UWORD)(src.size.width - CirRa_in[1].CirCen.x + CirRa_in[1].Radius) + (IMG_UWORD)10;

	}
	drc.ptr = (IMG_UBYTE*)malloc(sizeof(IMG_UBYTE)*drc.size.height*drc.size.width);
	for (IMG_INT i = 0;i < drc.size.height;i++)
	{
		for (IMG_INT j = 0;j < drc.size.width;j++)
		{
			drc.ptr[i*drc.size.width + j] = src.ptr[(int)(i + re_deviation_y)*src.size.width + (int)(j + re_deviation_x)];

		}
	}


	TempStructCircle CirRa_2 = CirRa_in[1];
	status = Assist_VisBC_BlobCircleAccurate_order(drc, CirRa_2, 10);
	CirRa_2.CirCen.x += re_deviation_x;
	CirRa_2.CirCen.y += re_deviation_y;



	CirRa_out[0] = CirRa_1;
	CirRa_out[1] = CirRa_2;


	if (drc.ptr != nullptr)
		free(drc.ptr);

	return status;

	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}
/**********************************************/
// VisBC_BlobCircleAccurate,实现圆心半径精确计算
// Input:
// 	IMG_UBBUF src, 输入图像
// 	TempStructCircle &circle1, 图像中的第一个特征圆的圆心及半径
//	TempStructCircle &circle2，图像中的第二个特征圆的圆心及半径
// Output:
// 	TempStructCircle &circle1, 图像中的第一个特征圆的圆心及半径
//	TempStructCircle &circle2，图像中的第二个特征圆的圆心及半径
//     ...
// Return:
//     0 - 正常
//	   -1 - 返回错误，无法检测到特征圆
//	   -4 - 圆完整度太低
// Author: 04/27/2017
/**********************************************/
IMG_INT CBmSocketLocation::VisBC_BlobCircleAccurate(IMG_UBBUF src, TempStructCircle &CirRa1, TempStructCircle &CirRa2)
{

	_try{
	int status = 0;

	TempStructCircle circle1, circle2;
	circle1.CirCen = { 0.0,0.0 };
	circle1.Radius = 0.0;
	circle2.CirCen = { 0.0,0.0 };
	circle2.Radius = 0.0;

	int threshold = (int)bmsocket.ThresholdImage;
	//double t = (double)getTickCount();
	status = VisBC_BlobCircle_coarse(src, circle1, circle2, threshold);
	//t = 1000 * ((double)getTickCount() - t) / getTickFrequency();
	//cout << t << endl;
	if (status != 0)
	{
		//printf("error in VisBC_BlobCircle\n");
		return status;
	}
	TempStructCircle *CirRa_in;
	CirRa_in = (TempStructCircle*)malloc(sizeof(TempStructCircle) * 2);
	CirRa_in[0].CirCen = { circle1.CirCen.x * 2 * int(bmsocket.PyramidLevel),circle1.CirCen.y * 2 * int(bmsocket.PyramidLevel) };
	CirRa_in[0].Radius = circle1.Radius * 2 * int(bmsocket.PyramidLevel);
	CirRa_in[1].CirCen = { circle2.CirCen.x * 2 * int(bmsocket.PyramidLevel),circle2.CirCen.y * 2 * int(bmsocket.PyramidLevel) };
	CirRa_in[1].Radius = circle2.Radius * 2 * int(bmsocket.PyramidLevel);
	//CirRa1 = CirRa_in[0];//判断下半圆是否完整
	//CirRa2 = CirRa_in[1];//判断左半圆是否完整

	int miny = circle1.miny * 4;
	if (miny == 0)
	{
		CirRa_in[0].CirCen.y = (circle1.maxy - circle1.Radius) * 2 * int(bmsocket.PyramidLevel);
	}
	float angular_increment = float(PI / CirRa_in[0].Radius);
	int x1 = 0, y1 = 0;
	int temp_defect = (int)bmsocket.ScoreWidthTemp_defect;
	int defect = 0, defect1 = 0;

	for (float i = float(PI);i <= 2 * PI;i = i + angular_increment)
	{
		x1 = int(CirRa_in[0].Radius*cos(i) + CirRa_in[0].CirCen.x);
		y1 = int(CirRa_in[0].CirCen.y - CirRa_in[0].Radius*sin(i));
		if (y1 <= 0)
		{
			continue;
		}
		int re_te = 0;

		for (int k = -temp_defect + 1;k < temp_defect;k++)
		{
			if ((y1 - k) <= 0)
			{
				continue;
			}
			for (int j = -temp_defect + 1;j < temp_defect;j++)
			{
				if (src.ptr[(y1 - k)*src.size.width + x1 + j] >= bmsocket.ImageCircleThreshold)
				{
					re_te++;
				}
			}
		}
		if ((re_te*1.0 / (2 * temp_defect - 1) / (2 * temp_defect - 1)) > bmsocket.SatisfactionRatio)
		{
			defect++;
		}
		defect1++;
	}
	if (defect == 0)//((defect*1.0 / defect1) < 0.4)
	{
		if (CirRa_in != nullptr)
			free(CirRa_in);
		return -4;
	}

	defect = 0, defect1 = 0;
	angular_increment = float(PI / CirRa_in[1].Radius);
	circle2.minx = 2 * int(bmsocket.PyramidLevel) * circle2.minx;
	circle2.maxx = 2 * int(bmsocket.PyramidLevel) * circle2.maxx;
	circle2.miny = 2 * int(bmsocket.PyramidLevel) * circle2.miny;
	circle2.maxy = 2 * int(bmsocket.PyramidLevel) * circle2.maxy;

	int *socket_re = (int*)malloc(sizeof(int)*(circle2.maxx - circle2.minx + 1));
	for (int i = 0;i < (circle2.maxx - circle2.minx);i++)
	{
		socket_re[i] = 0;
	}
	//ofstream fo("src22.txt");
	for (int i = circle2.minx;i <= circle2.maxx;i++)
	{
		for (int j = circle2.miny;j <= circle2.maxy;j++)
		{
			//fo << (double)src.ptr[j*src.size.width + i] << "  ";
			if ((double)src.ptr[j*src.size.width + i] - bmsocket.ThresholdImage > 0)
			{
				socket_re[i - circle2.minx] ++;
				//socket_re[i] = socket_re[i] + 1;
			}
		}
		//fo << endl;
	}
	//fo.close();

	if (socket_re[0] > bmsocket.TruncatedBoundaryDetermination && socket_re[1] > bmsocket.TruncatedBoundaryDetermination)
	{
		if (socket_re[circle2.maxx - circle2.minx - 2] > bmsocket.TruncatedBoundaryDetermination && socket_re[circle2.maxx - circle2.minx - 1] > bmsocket.TruncatedBoundaryDetermination)
		{
			CirRa_in[1].CirCen.x = circle2.CirCen.x * 2 * int(bmsocket.PyramidLevel);
		}
		else
		{
			CirRa_in[1].CirCen.x = (circle2.maxx - CirRa_in[1].Radius);
		}
	}
	else
	{
		//cout << socket_re[circle2.maxx - circle2.minx-2] << endl;
		//cout << socket_re[circle2.maxx - circle2.minx - 1] << endl;
		if (socket_re[circle2.maxx - circle2.minx - 2] > bmsocket.TruncatedBoundaryDetermination && socket_re[circle2.maxx - circle2.minx - 1] > bmsocket.TruncatedBoundaryDetermination)
		{
			CirRa_in[1].CirCen.x = circle2.minx + CirRa_in[1].Radius;
		}
		else
		{
			CirRa_in[1].CirCen.x = circle2.CirCen.x * 2 * int(bmsocket.PyramidLevel);
		}
	}

	for (float i = float(PI*0.5);i <= 1.5 * PI;i = i + angular_increment)
	{
		x1 = int(CirRa_in[1].Radius*cos(i) + CirRa_in[1].CirCen.x);
		y1 = int(CirRa_in[1].CirCen.y - CirRa_in[1].Radius*sin(i));
		int re_te = 0;
		if (x1 >= src.size.width)
		{
			continue;
		}
		for (int k = -temp_defect + 1;k < temp_defect;k++)
		{
			for (int j = -temp_defect + 1;j < temp_defect;j++)
			{
				if ((x1 + j) >= src.size.width)
				{
					continue;
				}
				if (src.ptr[(y1 - k)*src.size.width + x1 + j] >= bmsocket.ImageCircleThreshold)
				{
					re_te++;
				}
			}
		}
		if ((re_te*1.0 / (2 * temp_defect - 1) / (2 * temp_defect - 1)) > bmsocket.SatisfactionRatio)
		{
			defect++;
		}
		defect1++;
	}
	if (defect == 0) //((defect*1.0 / defect1) < 0.4)
	{
		free(socket_re);
		if (CirRa_in != nullptr)
			free(CirRa_in);
		return -4;
	}

	//TempStructCircle *CirRa_out;
	//CirRa_out = (TempStructCircle *)malloc(sizeof(TempStructCircle) * 2);
	//status = Assist_VisBC_BlobCircleAccurate(src, CirRa_in, CirRa_out);
	//if (status != 0)
	//{
	//if (CirRa_out != nullptr)
	//free(CirRa_out);
	//free(socket_re);
	//if (CirRa_in != nullptr)
	//free(CirRa_in);
	//return status;
	//}
	CirRa1 = CirRa_in[0];
	CirRa2 = CirRa_in[1];
	/*CirRa1 = CirRa_out[0];
	CirRa2 = CirRa_out[1];*/

	//if (CirRa_out != nullptr)
	//free(CirRa_out);
	free(socket_re);
	if (CirRa_in != nullptr)
		free(CirRa_in);
	return status;

	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}
/**********************************************/
// SocketLocation,实现特征圆圆心半径定位
// Input:
// 	   IMG_UBBUF src, 输入图像;
// 	   angleFlg, //0 or 180
//	   const double dist,	//DUT locating 输出的dist
// Output:
// 	   TempStructCircle &circle1, 图像中的第一个特征圆的圆心及半径
//	   TempStructCircle &circle2，图像中的第二个特征圆的圆心及半径
//     double &angle,//输出角度逆时针为正
// Return:
//    _BACK_CODE,参照 enum _BACK_CODE;
// Author: 04/27/2017
/**********************************************/

/*
输入：topX, topY --上方圆点中心坐标
downX,downY -- 下方圆点中心坐标
dist --- 中心点到吸嘴间的图像距离（可以设置为180 ）

输出：Angle --- 中心点到吸嘴所形成直线在图像中的角度
cx,cy ---- 吸嘴图像坐标
*/

//旋转求点  
IMG_INT CBmSocketLocation::GetSunctionRotationPosition(double cx, double cy, double ix, double iy, double &x, double &y, double &angle)
{
	_try{
	double r = sqrt((ix - cx)*(ix - cx) + (iy - cy)*(iy - cy));

	double m_offsetAngle = -bmsocket.m_offsetAngle;//-2.01;//度数

	double angle2 = m_offsetAngle *PI / 180;

	double angle1 = atan2((cy - iy), (ix - cx));


	x = r*cos(angle1 + angle2);
	y = r*sin(angle1 + angle2);

	x = cx + x;
	y = cy - y;

	angle = atan2((cy - y), (x - cx)) * 180 / PI;//比acos精度好
	if (fabs(angle - 90) > bmsocket.m_angletole)
	{
		return -1;
	}
												 //angle = acos(fabs(cx - x) / sqrt((cx - x)*(cx - x) + (cy - y)*(cy - y)));// *180 / PI;
	return 0;

	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

IMG_INT CBmSocketLocation::VisOmmIntersection_oflineandcir(FloatLine line, StructCircle cir, IMG_RCOORD *intersectionpoint, IMG_INT &m)
{
	_try{
	int status = 0;
	//缺少图像宽 高 越界判断
	if (line.startPoint.x < 0 || line.startPoint.y < 0 ||
		line.endPoint.x < 0 || line.endPoint.y < 0 ||
		//cir.CirCen.x<0||cir.CirCen.y<0||
		cir.Radius < 0)
	{
		status = -1;
		return status;
	}//越界

	double temp_d = 0;
	double temp_k = 0;
	if (line.startPoint.x == line.endPoint.x)
	{
		temp_d = fabs(line.startPoint.x - cir.CirCen.x);

		if (temp_d > cir.Radius)
		{
			status = -2;
			m = 0;
			return status;
		}
		else if (temp_d == cir.Radius)
		{
			m = 1;
			intersectionpoint[0].x = line.startPoint.x;
			intersectionpoint[0].y = cir.CirCen.y;
			return status;
		}
		else
		{
			m = 2;
			intersectionpoint[0].x = line.startPoint.x;
			intersectionpoint[0].y = cir.CirCen.y + sqrt(cir.Radius*cir.Radius - (intersectionpoint[0].x - cir.CirCen.x)*(intersectionpoint[0].x - cir.CirCen.x));
			intersectionpoint[1].x = line.startPoint.x;
			intersectionpoint[1].y = cir.CirCen.y - sqrt(cir.Radius*cir.Radius - (intersectionpoint[0].x - cir.CirCen.x)*(intersectionpoint[0].x - cir.CirCen.x));
			return status;
		}

	}
	else
	{
		temp_k = 1.0*(line.startPoint.y - line.endPoint.y) / (line.startPoint.x - line.endPoint.x);
		temp_d = fabs(temp_k*(cir.CirCen.x - line.endPoint.x) + line.endPoint.y - cir.CirCen.y)*1.0 / sqrt(1 + temp_k*temp_k);

		double temp_B = line.endPoint.y - temp_k*line.endPoint.x - cir.CirCen.y;

		double temp_a = 1 + temp_k*temp_k;
		double temp_b = (2 * temp_k*temp_B - 2 * cir.CirCen.x);
		double temp_c = cir.CirCen.x*cir.CirCen.x + temp_B*temp_B - cir.Radius*cir.Radius;

		if (temp_d > cir.Radius + 0.1)
		{
			status = -2;
			m = 0;
			return status;
		}
		else if (temp_d >= cir.Radius - 0.1)
		{
			m = 1;
			intersectionpoint[0].x = float(-1.0*temp_b / (2 * temp_a));
			intersectionpoint[0].y = float(line.endPoint.y + temp_k*(intersectionpoint[0].x - line.endPoint.x));
			return status;
		}
		else
		{
			m = 2;
			intersectionpoint[0].x = float((-1.0*temp_b + sqrt(temp_b*temp_b - 4 * temp_a*temp_c)) / (2 * temp_a));
			intersectionpoint[0].y = float(line.endPoint.y + temp_k*(intersectionpoint[0].x - line.endPoint.x));
			intersectionpoint[1].x = float((-1.0*temp_b - sqrt(temp_b*temp_b - 4 * temp_a*temp_c)) / (2 * temp_a));
			intersectionpoint[1].y = float(line.endPoint.y + temp_k*(intersectionpoint[1].x - line.endPoint.x));
			return status;
		}

	}

	return status;

	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

IMG_INT CBmSocketLocation::VisOmmIntersection_ofcirandcir(StructCircle cir1, StructCircle cir2, IMG_RCOORD *intersectionpoint, IMG_INT &m)
{
	_try{
	int status = 0;

	if (cir1.CirCen.x < 0 || cir1.CirCen.y < 0 || cir1.Radius < 0 || cir2.CirCen.x < 0 || cir2.CirCen.y < 0 || cir2.Radius < 0)
	{
		status = -1;
		return status;
	}

	if (cir1.CirCen.x == cir2.CirCen.x&&cir1.CirCen.y == cir2.CirCen.y&&cir1.Radius == cir2.Radius)
	{
		status = -9999;
		return status;//全是交点
	}

	IMG_RCOORD point1 = cir1.CirCen;
	IMG_RCOORD point2 = cir2.CirCen;

	float d = sqrt((point1.x - point2.x)*(point1.x - point2.x) + (point1.y - point2.y)*(point1.y - point2.y));
	if (d > (cir1.Radius + cir2.Radius) || d < fabs(cir1.Radius - cir2.Radius))
	{
		status = -1;
		return status;
	}

	float temp_min_r = cir1.Radius > cir2.Radius ? cir2.Radius : cir1.Radius;
	if (point1.y == point2.y)//针对此用例 返回失败
	{
		return -1;
		//float temp_x = float((point1.x + point2.x)*1.0 / 2);
		FloatLine line1;
		line1.startPoint = point1;
		line1.endPoint = point2;

		float L = sqrt((cir1.CirCen.x - cir2.CirCen.x)*(cir1.CirCen.x - cir2.CirCen.x) + (cir1.CirCen.y - cir2.CirCen.y)*(cir1.CirCen.y - cir2.CirCen.y));

		float temp_k = (line1.endPoint.y - line1.startPoint.y) / (line1.endPoint.x - line1.startPoint.x);

		float temp_x = cir1.CirCen.x + (cir2.CirCen.x - cir1.CirCen.x)*(cir1.Radius*cir1.Radius - cir2.Radius*cir2.Radius + L*L) / 2 / L / L;
		float temp_y = cir1.CirCen.y;
		//float temp_dev = -1.0 / temp_k;
		//float temp_devb = temp_x;


		line1.startPoint.x = temp_x;
		line1.startPoint.y = temp_y;
		line1.endPoint.x = temp_x;
		line1.endPoint.y = temp_y + temp_min_r;

		/*line1.startPoint.x = temp_x;
		line1.startPoint.y = point1.y + temp_min_r;
		line1.endPoint.x = temp_x;
		line1.endPoint.y = point1.y - temp_min_r;*/
		status = VisOmmIntersection_oflineandcir(line1, cir1, intersectionpoint, m);
		if (status != 0)
		{
			return status;
		}
	}
	else if (point1.x == point2.x)//针对此用例 返回失败
	{
		return -1;
		float temp_y = (cir1.Radius*cir1.Radius - cir2.Radius*cir2.Radius - cir1.CirCen.y*cir1.CirCen.y + cir2.CirCen.y + cir2.CirCen.y) / 2 / (cir2.CirCen.y - cir1.CirCen.y);

		FloatLine line1;
		line1.startPoint.x = point1.x + temp_min_r;
		line1.startPoint.y = temp_y;
		line1.endPoint.x = point1.x - temp_min_r;
		line1.endPoint.y = temp_y;
		status = VisOmmIntersection_oflineandcir(line1, cir1, intersectionpoint, m);
		if (status != 0)
		{
			return status;
		}
	}
	else
	{
		FloatLine line1;
		line1.startPoint = point1;
		line1.endPoint = point2;


		float L = sqrt((cir1.CirCen.x - cir2.CirCen.x)*(cir1.CirCen.x - cir2.CirCen.x) + (cir1.CirCen.y - cir2.CirCen.y)*(cir1.CirCen.y - cir2.CirCen.y));


		float temp_k = (line1.endPoint.y - line1.startPoint.y) / (line1.endPoint.x - line1.startPoint.x);


		float temp_x = cir1.CirCen.x + (cir2.CirCen.x - cir1.CirCen.x)*(cir1.Radius*cir1.Radius - cir2.Radius*cir2.Radius + L*L) / 2 / L / L;
		float temp_y = cir1.CirCen.y - temp_k*cir1.CirCen.x + temp_k*temp_x;
		float temp_dev = (float)-1.0 / temp_k;
		float temp_devb = temp_y - temp_dev*temp_x;


		if (temp_k > 0)
		{
			line1.startPoint.x = temp_x;
			line1.startPoint.y = temp_y;
			line1.endPoint.x = temp_x + temp_dev;
			line1.endPoint.y = temp_dev*line1.endPoint.x + temp_devb;
		}
		else
		{
			line1.startPoint.x = temp_x;
			line1.startPoint.y = temp_y;
			line1.endPoint.x = temp_x + temp_dev;
			line1.endPoint.y = temp_dev*line1.endPoint.x + temp_devb;
		}

		status = VisOmmIntersection_oflineandcir(line1, cir1, intersectionpoint, m);
		if (status != 0)
		{
			return status;
		}
	}

	return status;

	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}

//dist=180
IMG_INT CBmSocketLocation::GetSunctionPosition(double topX, double topY, double downX, double downY, double dist, double &Angle, double &x, double &y)
{
	/*try
	{*/
	_try{
	double m_topRadius = bmsocket.m_topRadius;
	double m_downRadius = bmsocket.m_downRadius;
	double m_pixSize = bmsocket.m_pixSize;

	double topRadius = m_topRadius / m_pixSize;
	double downRadius = m_downRadius / m_pixSize;

	int status = 0;
	StructCircle cir1;
	StructCircle cir2;
	cir1.CirCen.x = (float)topX;
	cir1.CirCen.y = (float)topY;
	cir1.Radius = (float)topRadius;
	cir2.CirCen.x = (float)downX;
	cir2.CirCen.y = (float)downY;
	cir2.Radius = (float)downRadius;
	IMG_RCOORD intersectionpoint[2];
	intersectionpoint[0] = { 0,0 };
	intersectionpoint[1] = { 0,0 };
	IMG_INT m = 0;

	status = VisOmmIntersection_ofcirandcir(cir1, cir2, intersectionpoint, m);
	if (status != 0)
	{
		return status;
	}
	double cx = 0, cy = 0;
	if (intersectionpoint[1].y > intersectionpoint[0].y)
	{
		cx = intersectionpoint[1].x;
		cy = intersectionpoint[1].y;
	}
	else
	{
		cx = intersectionpoint[0].x;
		cy = intersectionpoint[0].y;
	}


	intersectionpoint[0] = { 0,0 };
	intersectionpoint[1] = { 0,0 };
	FloatLine line;
	StructCircle cir;
	line.startPoint = { (float)cx,(float)cy };
	line.endPoint = { (float)topX,(float)topY };
	cir.CirCen = { (float)cx,(float)cy };
	cir.Radius = (float)dist;

	status = VisOmmIntersection_oflineandcir(line, cir, intersectionpoint, m);
	if (status != 0)
	{
		return status;
	}

	double ix = 0, iy = 0;
	if (intersectionpoint[1].y < intersectionpoint[0].y)
	{
		ix = intersectionpoint[1].x;
		iy = intersectionpoint[1].y;
	}
	else
	{
		ix = intersectionpoint[0].x;
		iy = intersectionpoint[0].y;
	}

	status = GetSunctionRotationPosition(cx, cy, ix, iy, x, y, Angle);
	if (status != 0)
	{
		return status;
	}
	return status;
	/*}
	catch ()
	{
	return;
	}*/

	}
		__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;
	}
}



bool CBmSocketLocation::LoadParameter(char path[])
{
	bookSocket ok;
	//ok.Write(".\\BmSocketFileName.ini");
	//ok.Read(".\\BmSocketFileName.ini", bmsocket);
	int status = ok.Read(path, bmsocket);
	if (status != 0)
	{
		return false;
	}
	return true;
}
CBmSocketLocation::CBmSocketLocation()
{};
CBmSocketLocation::~CBmSocketLocation()
{};




int bookSocket::Read(char *ConfigFileName, BMSOCKET &bmsocket)
{
	int status = 0;
	LPSTR temp = (LPSTR)new char[MAX_PATH];
	GetPrivateProfileStringA("BmSocket", "IMG_WINDOWimgtopcoWindowOffx", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.IMG_WINDOWimgtopcoWindowOffx = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "IMG_WINDOWimgtopcoWindowOffy", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.IMG_WINDOWimgtopcoWindowOffy = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "IMG_WINDOWimgtopszWindowSizewidth", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.IMG_WINDOWimgtopszWindowSizewidth = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "IMG_WINDOWimgtopszWindowSizeheight", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.IMG_WINDOWimgtopszWindowSizeheight = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "IMG_WINDOWimgdowncoWindowOffx", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.IMG_WINDOWimgdowncoWindowOffx = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "IMG_WINDOWimgdowncoWindowOffy", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.IMG_WINDOWimgdowncoWindowOffy = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "IMG_WINDOWimgdownszWindowSizewidth", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.IMG_WINDOWimgdownszWindowSizewidth = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "IMG_WINDOWimgdownszWindowSizeheight", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.IMG_WINDOWimgdownszWindowSizeheight = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "FittingSigma", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.FittingSigma = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "PyramidLevel", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.PyramidLevel = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "LabelMarkerMin", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.LabelMarkerMin = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "LabelMarkerMax", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.LabelMarkerMax = atof((char*)temp);

	GetPrivateProfileStringA("BmSocket", "AreaProportionMax", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.AreaProportionMax = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "AreaProportionMin", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.AreaProportionMin = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "BoundaryConstraint", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.BoundaryConstraint = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "ReducedImageRadiusConstraint", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.ReducedImageRadiusConstraint = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "AngleConstraint", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.AngleConstraint = atof((char*)temp);

	GetPrivateProfileStringA("BmSocket", "DistanceConstraint", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.DistanceConstraint = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "RadiusFaultTolerant", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.RadiusFaultTolerant = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "ThresholdImage", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.ThresholdImage = atof((char*)temp);

	GetPrivateProfileStringA("BmSocket", "m_offsetAngle", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.m_offsetAngle = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "m_topRadius", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.m_topRadius = atof((char*)temp);

	GetPrivateProfileStringA("BmSocket", "m_downRadius", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.m_downRadius = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "m_pixSize", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.m_pixSize = atof((char*)temp);
	
	GetPrivateProfileStringA("BmSocket", "m_angletole", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.m_angletole = atof((char*)temp);

	GetPrivateProfileStringA("BmSocket", "ScoreWidthTemp_defect", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.ScoreWidthTemp_defect = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "TruncatedBoundaryDetermination", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.TruncatedBoundaryDetermination = atof((char*)temp);

	GetPrivateProfileStringA("BmSocket", "SatisfactionRatio", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.SatisfactionRatio = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "ImageCircleThreshold", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.ImageCircleThreshold = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "StatisticalNumber", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.StatisticalNumber = atof((char*)temp);

	GetPrivateProfileStringA("BmSocket", "BinaryzationMin", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.BinaryzationMin = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "BinaryzationMax", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.BinaryzationMax = atof((char*)temp);

	GetPrivateProfileStringA("BmSocket", "ExcludedHole", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.ExcludedHole = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "RoiRadiusFaultTolerant", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.RoiRadiusFaultTolerant = atof((char*)temp);
	GetPrivateProfileStringA("BmSocket", "RoiCircleDistanceFaultTolerant", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return -1;
	}
	bmsocket.RoiCircleDistanceFaultTolerant = atof((char*)temp);

	delete temp;
	return status;
}

int bookSocket::Write(char *ConfigFileName)
{
	int status = 0;

	WritePrivateProfileStringA("BmSocket", "IMG_WINDOWimgtopcoWindowOffx", "500", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "IMG_WINDOWimgtopcoWindowOffy", "0", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "IMG_WINDOWimgtopszWindowSizewidth", "800", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "IMG_WINDOWimgtopszWindowSizeheight", "400", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "IMG_WINDOWimgdowncoWindowOffx", "1208", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "IMG_WINDOWimgdowncoWindowOffy", "430", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "IMG_WINDOWimgdownszWindowSizewidth", "400", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "IMG_WINDOWimgdownszWindowSizeheight", "700", ConfigFileName);

	WritePrivateProfileStringA("BmSocket", "FittingSigma", "10", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "PyramidLevel", "2", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "LabelMarkerMin", "1", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "LabelMarkerMax", "200", ConfigFileName);

	WritePrivateProfileStringA("BmSocket", "AreaProportionMax", "0.025", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "AreaProportionMin", "0.0033", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "BoundaryConstraint", "4", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "ReducedImageRadiusConstraint", "45", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "AngleConstraint", "2.26", ConfigFileName);

	WritePrivateProfileStringA("BmSocket", "DistanceConstraint", "226", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "RadiusFaultTolerant", "20", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "ThresholdImage", "200", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "m_angletole", "20", ConfigFileName);

	WritePrivateProfileStringA("BmSocket", "m_offsetAngle", "0.1714", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "m_topRadius", "3.5", ConfigFileName);

	WritePrivateProfileStringA("BmSocket", "m_downRadius", "3.05", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "m_pixSize", "0.0055", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "ScoreWidthTemp_defect", "10", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "TruncatedBoundaryDetermination", "90", ConfigFileName);

	WritePrivateProfileStringA("BmSocket", "SatisfactionRatio", "0.3", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "ImageCircleThreshold", "100", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "StatisticalNumber", "0", ConfigFileName);

	WritePrivateProfileStringA("BmSocket", "BinaryzationMin", "0", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "BinaryzationMax", "255", ConfigFileName);

	WritePrivateProfileStringA("BmSocket", "ExcludedHole", "0.7", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "RoiRadiusFaultTolerant", "10", ConfigFileName);
	WritePrivateProfileStringA("BmSocket", "RoiCircleDistanceFaultTolerant", "15", ConfigFileName);

	return status;
}


bookSocket::bookSocket()
{

}

bookSocket::~bookSocket()
{

}