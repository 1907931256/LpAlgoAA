#pragma once

#include "stdafx.h"
#include "ViType.h"
#include"BmErrorCode.h"
#include <stdio.h>　
#include <stdlib.h>
#include<math.h>
#include<ipp.h>
#include<mkl.h>
#include <iostream>
#include <fstream>
#include<vector>
#include<windows.h>

#define PI 3.14159265386

#ifdef  DLL_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)   
#endif
//拟合圆，权重调整参数
using namespace std;

//#define DEBUG_BlobCircle

//typedef struct
//{
//	IMG_RCOORD CirCen;//圆心
//	IMG_REAL Radius;//半径
//	IMG_INT miny;
//	IMG_INT minx;
//	IMG_INT maxy;
//	IMG_INT maxx;
//}TempStructCircle;//圆

typedef struct BmTray
{
	double AreaProportionMax = 0.33;
	double AreaProportionMin = 0.083;
	double LabelMarkerMax = 200;
	double LabelMarkerMin = 1;
	double VoteIntegrity = 80;
	double VoteTruncation = 20;
	double PyramidLevel = 2;
	double ThresholdImage = 150;
	double BoudaryConstraint = 4;
	double OriginalDiameterConstraint = 632;
	double ReducedImageRadiusConstraint = 100;
	double DoubleReducedImageRadiusConstraint = 158;
	double ImageDiameterFaultTolerant = 30;
	double RowPixelStatisticsUpperLimit = 49;
	double RowPixelStatisticsLowerLimit = 31;
	//double PI = 3.14159265386;
	double OriginalAreaTolerant = 10000;
	double BinaryzationMin = 0;
	double BinaryzationMax = 255;
}BMTRAY;

class CBmTrayLocation
{
public:
	CBmTrayLocation();
	~CBmTrayLocation();

public:
	bool LoadParameter(char path[]);
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
	_BACK_CODE TrayLocation(const IMG_UBBUF src, double &x, double &y);

private:

	BMTRAY bmtray;
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
	IMG_INT VisBC_BlobCircleAccurate(IMG_UBBUF src, TempStructCircle &circle1);

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
	IMG_VVOID VisBC_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma);

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
	IMG_VVOID VisBC_Binaryzation(Ipp8u *srcRoi, IppiSize Roi, IMG_INT threshold);

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
	IMG_VVOID VisBC_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, IMG_INT &markersNum);

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
	IMG_VVOID VisBC_OpenClose(Ipp8u *srcRoi, IppiSize Roi);

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

	IMG_INT VisBC_Screen(Ipp8u *&srcRoi, IppiSize Roi, TempStructCircle &circle1);



	int Histogram(Ipp8u*srcRoi, IppiSize Roi, Ipp8u*dstRoi, IMG_INT &thre);
};


class book
{
public:
	book();
	~book();

	int Read(char *ConfigFileName, BMTRAY &bmtray);//读配置文件
	int Write(char *ConfigFileName);//写入配置文件
};

