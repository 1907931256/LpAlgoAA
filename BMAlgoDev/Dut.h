#pragma once
#include "stdafx.h"
#include"BmErrorCode.h"
#include"ViType.h"
#include <stdio.h>　
#include <stdlib.h>
#include<math.h>
#include<ipp.h>
#include<mkl.h>
#include <iostream>
#include <fstream>
#include<vector>
#include<string.h>
//#include <opencv2/opencv.hpp>
//#include<highgui\highgui.hpp>
//
//using namespace cv;
//#define DEBUG_BlobCircle

#include "BmDUTTrayLocation.h"

#define PI 3.14159265386					//
#define fitting_sigma 10	
#ifdef  DLL_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)   
#endif
//拟合圆，权重调整参数
using namespace std;

//typedef struct
//{
//	IMG_COORD xyInteger; //像素点
//	IMG_RCOORD xyDecimal;//亚像素点
//	int gradient;
//	float angle;
//}edgeInformation;//边缘点

//typedef struct
//{
//	IMG_RCOORD CirCen;//圆心
//	IMG_REAL Radius;//半径
//}StructCircle;//圆

typedef struct  Newline
{
	IMG_REAL k;//直线截距式中　斜率
	IMG_REAL b;//直线截距式中　截距
	IMG_REAL A;//直线一般式中　x的系数
	IMG_REAL B;//直线一般式中　y的系数　
	IMG_REAL C;//直线一般式中　常数项
}NEWLINE;

//struct DUTOutPutParam
//{
//	double x1;	//四个角点(左上、右上、右下、左下)
//	double y1;
//
//	double x2;
//	double y2;
//
//	double x3;
//	double y3;
//
//	double x4;
//	double y4;
//
//	double cx;	//原来定义的DUT输出点
//	double cy;
//	double angle;	//输出角度逆时针为正
//	double dist;	//cx,xy到底边距离的一半
//
//};

//typedef struct
//{
//	IMG_RCOORD startPoint;
//	IMG_RCOORD endPoint;
//}FloatLine;//直线，方向为起点到终点


typedef struct CBmDUTTrayRecognization
{
	double Weightedproportion = 0.780;
	int linefitintimes = 1;
	double linefiterrorAdjacenterror = 0.0002;
	int intpylevel = 2;
	double firstdutareaminportion = 0.032;
	double firstdutareamaxportion = 0.30;
	double firstdutareamax = 30000;
	double firstdutareamin = 12000;
	double seconddutsummin = 15000;
	double seconddutsummax = 30000;
	int imageedgeintmin = 4;
	int intlinerecoderowdstRoicopy = 130;
	int intsilineangleto = 5;
	int intlinedis = 20;
	int inttwltrdangleto = 10;
	double thirdareaprotionmin = 0.008;
	double thirdareaprotionmax = 0.30;
	int thirdmaxcirclera = 70;
	int thirdmincirclera = 20;
	int thirdcircleedgemaxpro = 3;
	int thirdcircleedgeminpro = 0;
	int thirdcircleedgemax = 40;
	double lastareaprotionmax = 1.20;
	double lastareaprotionmin = 0.5;
	int lastwidthto = 10;
	int lastheightto = 15;
	int lastfindth = 125;
}CBMDUTTRAYRECOGNIZATION;



class DUT
{
public:
	DUT() {};
	~DUT() {};

	_BACK_CODE DUTLocation(const IMG_UBBUF src, DUTOutPutParam &param, int & Angle);


	int DUTMATCH(IMG_UBBUF src, DUTOutPutParam &param, int & Angle);


	CBMDUTTRAYRECOGNIZATION cbmdut;
//private:
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
	/**********************************************/
	IMG_VVOID VisBC_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma);


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
	//正常情况下返回1；
	//如果用户输入阈值小于0，函数返回-1；
	//如果输入参数不正确，包括图像尺寸不正确、srcRoi大小与尺寸不符合，函数返回-1。
	/**********************************************/
	IMG_VVOID VisBC_Fix_Sobel(IMG_UBYTE * pSrc, IppiSize roiSize, Ipp16s *&pDst, Ipp32f *&pAngle);
	IMG_REAL getIntraClassVariance(Ipp16s* src, IMG_INT srcRows, IMG_INT srcCols, IMG_INT &varTh);
	IMG_INT VisEdge_detection(IMG_UBYTE *srcRoi, IMG_SIZE roiSize, IMG_INT threshold, IMG_WORD *&dstRoi, IMG_UBYTE *dstRoiE, edgeInformation *&edgeArray, IMG_INT &eNum);
	/**********************************************/
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
	IMG_VVOID VisBC_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, IMG_INT &markersNum);
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
	IMG_VVOID VisBC_OpenClose(Ipp8u *srcRoi, IppiSize Roi);
	int Histogram(Ipp8u*srcRoi, IppiSize Roi, Ipp8u*dstRoi, double &sumpixel);
	IMG_INT VisOmmIntersection_oftwoline(FloatLine line1, FloatLine line2, IMG_RCOORD &intersectionpoint);
	int SrcLine(Ipp8u*srcRoi, IMG_SIZE roiSize, NEWLINE &line);
	IMG_BOOL VisOmmLine_Fitting(IMG_RCOORD* point, NEWLINE &line, IMG_INT m);
};

