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

using namespace std;
//#define DEBUG_BlobCircle


typedef struct {
	double dis;
	int i;
	int j;
	float length;
}JudgeStruct;

//typedef struct
//{
//	IMG_RCOORD CirCen;//圆心
//	IMG_REAL Radius;//半径
//	IMG_INT miny;
//	IMG_INT minx;
//	IMG_INT maxy;
//	IMG_INT maxx;
//}TempStructCircle;//圆

typedef struct Circular_Fit
{
	IMG_REAL a = 0;
	IMG_REAL b = 0;
	IMG_REAL r = 0;
}CIRCULAR_FITT;//圆拟合

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

//typedef struct
//{
//	IMG_RCOORD startPoint;
//	IMG_RCOORD endPoint;
//}FloatLine;//直线，方向为起点到终点

typedef struct BmSocket
{

	double IMG_WINDOWimgtopcoWindowOffx = 500;
	double 	IMG_WINDOWimgtopcoWindowOffy = 0;
	double 	IMG_WINDOWimgtopszWindowSizewidth = 800;
	double IMG_WINDOWimgtopszWindowSizeheight = 400;
	double 	IMG_WINDOWimgdowncoWindowOffx = 1208;
	double 	IMG_WINDOWimgdowncoWindowOffy = 430;
	double IMG_WINDOWimgdownszWindowSizewidth = 400;
	double IMG_WINDOWimgdownszWindowSizeheight = 700;
	double FittingSigma = 10;
	double PyramidLevel = 2;
	double LabelMarkerMin = 1;
	double LabelMarkerMax = 200;
	double AreaProportionMax = 0.20;//0.025;
	double AreaProportionMin = 0.0325;//0.0033;
	double BoundaryConstraint = 4;
	double ReducedImageRadiusConstraint = 45;
	double AngleConstraint = 2.26;
	double DistanceConstraint = 905;//226;
	double RadiusFaultTolerant = 20;
	double ThresholdImage = 200;
	double m_offsetAngle = 0.1714;//1.01;
	double m_topRadius = 3.5;
	double m_downRadius = 3.05;
	double m_pixSize = 0.0055;
	double m_angletole = 20;
	double ScoreWidthTemp_defect = 10;
	double TruncatedBoundaryDetermination = 90;
	double SatisfactionRatio = 0.3;
	double ImageCircleThreshold = 100;
	double StatisticalNumber = 0;
	double BinaryzationMin = 0;
	double BinaryzationMax = 255;
	double ExcludedHole = 0.5;
	double RoiRadiusFaultTolerant = 10;
	double RoiCircleDistanceFaultTolerant = 80;//15;
}BMSOCKET;


class CBmSocketLocation
{
public:
	CBmSocketLocation();
	~CBmSocketLocation();

//public:
	bool LoadParameter(char path[]);
	
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

	_BACK_CODE SocketLocation(const IMG_UBBUF src,
		const int angleFlg, //0 or 180
		const double dist,	//DUT locating 输出的dist
		double &x, double &y, double &angle);	//输出角度逆时针为正


private:


	IMG_INT CBmSocketLocation::VisNewFindCircle(IMG_UBBUF src,IMG_WINDOW imgtop, IMG_WINDOW imgdown, TempStructCircle &circle1, TempStructCircle &circle2, IMG_INT threshold);
	IMG_INT  CBmSocketLocation::VisNewCoarse(IMG_UBBUF src, IppiSize &Roi, Ipp8u* srcRoi, IMG_INT threshold);
	IMG_INT CBmSocketLocation::VisNewScree(Ipp8u*srcRoitop, Ipp8u*srcRoidown, IMG_WINDOW imgtop, IMG_WINDOW imgdown, IppiSize Roitop,
	IppiSize Roidown, TempStructCircle &circle1, TempStructCircle &circle2);

	BMSOCKET bmsocket;
	/*
	输入：topX, topY --上方圆点中心坐标
	downX,downY -- 下方圆点中心坐标
	dist --- 中心点到吸嘴间的图像距离（可以设置为180 ）

	输出：Angle --- 中心点到吸嘴所形成直线在图像中的角度
	cx,cy ---- 吸嘴图像坐标
	*/
	//主要是通过俩个圆心坐标求得Angle --- 中心点到吸嘴所形成直线在图像中的角度cx, cy----吸嘴图像坐标
	
	/**********************************************/
	// GetSunctionPosition, 功能说明:主要是通过俩个圆心坐标求得Angle --- 中心点到吸嘴所形成直线在图像中的角度cx, cy----吸嘴图像坐标
	// Input:
	//		topX,圆心1的横坐标
	//		topY,圆心1的纵坐标
	//		downX,圆心1的横坐标
	//		downY,圆心1
	//		dist,//DUT locating 输出的dist 一般为180
	//		downY,圆心1的纵坐标
	//		 ...
	// Output:
	// 		Angle --- 中心点到吸嘴所形成直线在图像中的角度
	//		cx, cy----吸嘴图像坐标
	//		 ...
	// Return:
	//     0 - 正常
	//     其他 - 待定
	// Author: 04/28/2017
	/**********************************************/
	int GetSunctionPosition(double topX, double topY, double downX, double downY, double dist, double &Angle, double &x, double &y);
	
	/**********************************************/
	// VisOmmIntersection_ofcirandcir, 功能说明:求取圆和圆的交点
	// Input:
	// 		TempStructCircle &circle1, 图像中的第一个特征圆的圆心及半径
	//		TempStructCircle &circle2，图像中的第二个特征圆的圆心及半径
	//		 ...
	// Output:
	// 		IMG_RCOORD *intersectionpoint,交点坐标
	//		IMG_INT &m,交点有m个
	//		 ...
	// Return:
	//     0 - 正常
	//     其他 - 待定
	// Author: 04/27/2017
	/**********************************************/
	IMG_INT VisOmmIntersection_ofcirandcir(StructCircle cir1, StructCircle cir2, IMG_RCOORD *intersectionpoint, IMG_INT &m);
	/**********************************************/
	// VisOmmIntersection_oflineandcir, 功能说明:求取直线和圆的交点
	// Input:
	// 		FloatLine line, 直线俩点式,避免斜率不存在
	//		TempStructCircle &cir，图像中的特征圆的圆心及半径
	//		 ...
	// Output:
	// 		IMG_RCOORD *intersectionpoint,交点坐标
	//		IMG_INT &m,交点有m个
	//		 ...
	// Return:
	//     0 - 正常
	//     其他 - 待定
	// Author: 04/27/2017
	/**********************************************/
	IMG_INT VisOmmIntersection_oflineandcir(FloatLine line, StructCircle cir, IMG_RCOORD *intersectionpoint, IMG_INT &m);
	
	/**********************************************/
	// GetSunctionRotationPosition, 功能说明:求取旋转特定角度的新的坐标和图像坐标系x的负方向角度
	// Input:
	//		cx,旋转坐标轴心点的横坐标
	//		cy,旋转坐标轴心点的纵坐标
	//		ix,旋转前点的横坐标
	//		iy,旋转前点的纵坐标
	//		 ...
	// Output:
	// 		Angle --- 中心点到吸嘴所形成直线在图像中的角度
	//		x, y----坐标旋转以后得到的新的坐标
	//		 ...
	// Return:
	//     0 - 正常
	//     其他 - 待定
	// Author: 04/28/2017
	/**********************************************/
	int GetSunctionRotationPosition(double cx, double cy, double ix, double iy, double &x, double &y, double &angle);
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
	IMG_INT VisBC_BlobCircle_coarse(IMG_UBBUF src, TempStructCircle &circle1, TempStructCircle &circle2, IMG_INT threshold = 200);
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
	IMG_INT VisBC_BlobCircleAccurate(IMG_UBBUF src, TempStructCircle &CirRa1, TempStructCircle &CirRa2);

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
	IMG_VVOID VisBC_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma);

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
	//  Ipp8u *srcRoi, 输出图片
	//     ...
	// Return:
	//     0 - 正常
	//     -1 - 暂无待定
	//     -2 - 暂无待定
	// Author: Huang Yige/03/18/2017
	/**********************************************/
	IMG_VVOID VisBC_OpenClose(Ipp8u *srcRoi, IppiSize Roi);

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
	IMG_VVOID VisBC_Moment(Ipp8u *srcRoi, IppiSize Roi, IMG_LREAL &hu);

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

	IMG_INT VisBC_Screen(Ipp8u *&srcRoi, IppiSize Roi, TempStructCircle &circle1, TempStructCircle &circle2);
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
	IMG_INT VisRoiGnerate_Circular_Fitting(IMG_REAL *point_pos, IMG_INT m, CIRCULAR_FITT &circular_fit);
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
	//	   其他 - 出错（测试中总结）
	// Author: 03/23/2017
	/**********************************************/
	IMG_INT Vis_Calculate_Circle_CompleteDegree_seach_match(CIRCULAR_FITT circular_fit, IMG_INT m, IMG_REAL *point_pos, IMG_INT Score_width, IMG_REAL &degreescore);
	/**********************************************/
	// Assist_VisBC_BlobCircleAccurate，进行定位计算
	// Input:
	// 	IMG_UBBUF src, 输入图像
	// 	TempStructCircle *&CirRa_in, 图像中的第一个特征圆的圆心及半径，图像中的第二个特征圆的圆心及半径
	//	
	// Output:
	// 	TempStructCircle *&CirRa_out, 图像中的第一个特征圆的圆心及半径，图像中的第二个特征圆的圆心及半径
	//     ...
	// Return:
	//     0 - 正常
	//	   其他 - 出错
	// Author: 03/18/2017
	/**********************************************/
	IMG_INT Assist_VisBC_BlobCircleAccurate(IMG_UBBUF src, TempStructCircle *&CirRa_in, TempStructCircle *&CirRa_out);
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
	//     0 - 正常
	//	   其他 - 出错
	// Author: 03/18/2017
	/**********************************************/
	IMG_INT Assist_VisBC_BlobCircleAccurate_order(IMG_UBBUF drc, TempStructCircle &CirRa, IMG_INT time);

};

class bookSocket
{
public:
	bookSocket();
	~bookSocket();

	int Read(char *ConfigFileName, BMSOCKET &bmsocket);//读配置文件
	int Write(char *ConfigFileName);//写入配置文件

private:

};

