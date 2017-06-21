#pragma once

#include "stdafx.h"
#include "ViType.h"
#include"BmErrorCode.h"
#include <stdio.h>��
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
//	IMG_RCOORD CirCen;//Բ��
//	IMG_REAL Radius;//�뾶
//	IMG_INT miny;
//	IMG_INT minx;
//	IMG_INT maxy;
//	IMG_INT maxx;
//}TempStructCircle;//Բ

typedef struct Circular_Fit
{
	IMG_REAL a = 0;
	IMG_REAL b = 0;
	IMG_REAL r = 0;
}CIRCULAR_FITT;//Բ���

//typedef struct
//{
//	IMG_COORD xyInteger; //���ص�
//	IMG_RCOORD xyDecimal;//�����ص�
//	int gradient;
//	float angle;
//}edgeInformation;//��Ե��

//typedef struct
//{
//	IMG_RCOORD CirCen;//Բ��
//	IMG_REAL Radius;//�뾶
//}StructCircle;//Բ

//typedef struct
//{
//	IMG_RCOORD startPoint;
//	IMG_RCOORD endPoint;
//}FloatLine;//ֱ�ߣ�����Ϊ��㵽�յ�

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
	// SocketLocation,ʵ������ԲԲ�İ뾶��λ
	// Input:
	// 	   IMG_UBBUF src, ����ͼ��;
	// 	   angleFlg, //0 or 180
	//	   const double dist,	//DUT locating �����dist
	// Output:
	// 	   TempStructCircle &circle1, ͼ���еĵ�һ������Բ��Բ�ļ��뾶
	//	   TempStructCircle &circle2��ͼ���еĵڶ�������Բ��Բ�ļ��뾶
	//     double &angle,//����Ƕ���ʱ��Ϊ��
	// Return:
	//    _BACK_CODE,���� enum _BACK_CODE;
	// Author: 04/27/2017
	/**********************************************/

	_BACK_CODE SocketLocation(const IMG_UBBUF src,
		const int angleFlg, //0 or 180
		const double dist,	//DUT locating �����dist
		double &x, double &y, double &angle);	//����Ƕ���ʱ��Ϊ��


private:


	IMG_INT CBmSocketLocation::VisNewFindCircle(IMG_UBBUF src,IMG_WINDOW imgtop, IMG_WINDOW imgdown, TempStructCircle &circle1, TempStructCircle &circle2, IMG_INT threshold);
	IMG_INT  CBmSocketLocation::VisNewCoarse(IMG_UBBUF src, IppiSize &Roi, Ipp8u* srcRoi, IMG_INT threshold);
	IMG_INT CBmSocketLocation::VisNewScree(Ipp8u*srcRoitop, Ipp8u*srcRoidown, IMG_WINDOW imgtop, IMG_WINDOW imgdown, IppiSize Roitop,
	IppiSize Roidown, TempStructCircle &circle1, TempStructCircle &circle2);

	BMSOCKET bmsocket;
	/*
	���룺topX, topY --�Ϸ�Բ����������
	downX,downY -- �·�Բ����������
	dist --- ���ĵ㵽������ͼ����루��������Ϊ180 ��

	�����Angle --- ���ĵ㵽�������γ�ֱ����ͼ���еĽǶ�
	cx,cy ---- ����ͼ������
	*/
	//��Ҫ��ͨ������Բ���������Angle --- ���ĵ㵽�������γ�ֱ����ͼ���еĽǶ�cx, cy----����ͼ������
	
	/**********************************************/
	// GetSunctionPosition, ����˵��:��Ҫ��ͨ������Բ���������Angle --- ���ĵ㵽�������γ�ֱ����ͼ���еĽǶ�cx, cy----����ͼ������
	// Input:
	//		topX,Բ��1�ĺ�����
	//		topY,Բ��1��������
	//		downX,Բ��1�ĺ�����
	//		downY,Բ��1
	//		dist,//DUT locating �����dist һ��Ϊ180
	//		downY,Բ��1��������
	//		 ...
	// Output:
	// 		Angle --- ���ĵ㵽�������γ�ֱ����ͼ���еĽǶ�
	//		cx, cy----����ͼ������
	//		 ...
	// Return:
	//     0 - ����
	//     ���� - ����
	// Author: 04/28/2017
	/**********************************************/
	int GetSunctionPosition(double topX, double topY, double downX, double downY, double dist, double &Angle, double &x, double &y);
	
	/**********************************************/
	// VisOmmIntersection_ofcirandcir, ����˵��:��ȡԲ��Բ�Ľ���
	// Input:
	// 		TempStructCircle &circle1, ͼ���еĵ�һ������Բ��Բ�ļ��뾶
	//		TempStructCircle &circle2��ͼ���еĵڶ�������Բ��Բ�ļ��뾶
	//		 ...
	// Output:
	// 		IMG_RCOORD *intersectionpoint,��������
	//		IMG_INT &m,������m��
	//		 ...
	// Return:
	//     0 - ����
	//     ���� - ����
	// Author: 04/27/2017
	/**********************************************/
	IMG_INT VisOmmIntersection_ofcirandcir(StructCircle cir1, StructCircle cir2, IMG_RCOORD *intersectionpoint, IMG_INT &m);
	/**********************************************/
	// VisOmmIntersection_oflineandcir, ����˵��:��ȡֱ�ߺ�Բ�Ľ���
	// Input:
	// 		FloatLine line, ֱ������ʽ,����б�ʲ�����
	//		TempStructCircle &cir��ͼ���е�����Բ��Բ�ļ��뾶
	//		 ...
	// Output:
	// 		IMG_RCOORD *intersectionpoint,��������
	//		IMG_INT &m,������m��
	//		 ...
	// Return:
	//     0 - ����
	//     ���� - ����
	// Author: 04/27/2017
	/**********************************************/
	IMG_INT VisOmmIntersection_oflineandcir(FloatLine line, StructCircle cir, IMG_RCOORD *intersectionpoint, IMG_INT &m);
	
	/**********************************************/
	// GetSunctionRotationPosition, ����˵��:��ȡ��ת�ض��Ƕȵ��µ������ͼ������ϵx�ĸ�����Ƕ�
	// Input:
	//		cx,��ת�������ĵ�ĺ�����
	//		cy,��ת�������ĵ��������
	//		ix,��תǰ��ĺ�����
	//		iy,��תǰ���������
	//		 ...
	// Output:
	// 		Angle --- ���ĵ㵽�������γ�ֱ����ͼ���еĽǶ�
	//		x, y----������ת�Ժ�õ����µ�����
	//		 ...
	// Return:
	//     0 - ����
	//     ���� - ����
	// Author: 04/28/2017
	/**********************************************/
	int GetSunctionRotationPosition(double cx, double cy, double ix, double iy, double &x, double &y, double &angle);
	/**********************************************/
	// VisBlobCircle, ����˵��:��ͼ���е�����Բ���дֶ�λ��
	// Input:
	// 	IMG_UBBUF src, ����ͼƬ
	//	int threshold	�ݶ�ǿ�ȵ���ֵ���������Ǵ���0С��1250��������.
	//     ...
	// Output:
	// 	TempStructCircle &circle1, ͼ���еĵ�һ������Բ��Բ�ļ��뾶
	//	TempStructCircle &circle2��ͼ���еĵڶ�������Բ��Բ�ļ��뾶
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���ش����޷���⵽����Բ
	// Author: 04/27/2017
	/**********************************************/
	IMG_INT VisBC_BlobCircle_coarse(IMG_UBBUF src, TempStructCircle &circle1, TempStructCircle &circle2, IMG_INT threshold = 200);
	/**********************************************/
	// VisBC_BlobCircleAccurate,ʵ��Բ�İ뾶��ȷ����
	// Input:
	// 	IMG_UBBUF src, ����ͼ��
	// 	TempStructCircle &circle1, ͼ���еĵ�һ������Բ��Բ�ļ��뾶
	//	TempStructCircle &circle2��ͼ���еĵڶ�������Բ��Բ�ļ��뾶
	// Output:
	// 	TempStructCircle &circle1, ͼ���еĵ�һ������Բ��Բ�ļ��뾶
	//	TempStructCircle &circle2��ͼ���еĵڶ�������Բ��Բ�ļ��뾶
	//     ...
	// Return:
	//     0 - ����
	//	   -1 - ���ش����޷���⵽����Բ
	//	   -4 - Բ������̫��
	// Author: 04/27/2017
	/**********************************************/
	IMG_INT VisBC_BlobCircleAccurate(IMG_UBBUF src, TempStructCircle &CirRa1, TempStructCircle &CirRa2);

	/**********************************************/
	// VisBC_Pyramid, ����˵��:����������ͼ�������С��
	// Input:
	// 	IMG_UBBUF src, ����ͼƬ
	//	int level,�û�ָ���Ľ���������
	//	double sigma,����
	//     ...
	// Output:
	// 	IppiPyramid*&pPyrStruct, �������ṹ
	//	Ipp8u ** &pPyrImage ������ÿһ���ж�Ӧ��ͼ��ĵ�ĻҶ�ֵ
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���޴���
	//     -2 - ���޴���
	// Author: Huang Yige/03/18/2017
	/**********************************************/
	IMG_VVOID VisBC_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma);

	/**********************************************/
	// VisBC_Binaryzation, ����˵��:��ͼ����ж�ֵ����
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//	int threshold,�ݶ�ǿ�ȵ���ֵ���������Ǵ���0С��1250��������������û���֪����ֵ��Ϊ���ٺ��ʣ���������0���㷨�Զ���ȡ���ʵ���ֵ����
	//     ...
	// Output:
	// 	IMG_UBBUF src,���ͼ��
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���޴���
	//     -2 - ���޴���
	// Author: Huang Yige/03/18/2017
	/**********************************************/
	IMG_VVOID VisBC_Binaryzation(Ipp8u *srcRoi, IppiSize Roi, IMG_INT threshold);

	/**********************************************/
	// VisBC_LabelMarker, ����˵��:��ͼ���е�Blob�������ȡ��
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//     ...
	// Output:
	// 	int &markersNum, ���Blob��ĸ���
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���޴���
	//     -2 - ���޴���
	// Author: Huang Yige/03/18/2017
	/**********************************************/
	IMG_VVOID VisBC_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, IMG_INT &markersNum);

	/**********************************************/
	// VisBC_OpenClose, ����˵��:��ͼ����п����㣬����С��Blob��
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//     ...
	// Output:
	//  Ipp8u *srcRoi, ���ͼƬ
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���޴���
	//     -2 - ���޴���
	// Author: Huang Yige/03/18/2017
	/**********************************************/
	IMG_VVOID VisBC_OpenClose(Ipp8u *srcRoi, IppiSize Roi);

	/**********************************************/
	// VisBC_Moment, ����˵��:����ͼ��ľء�
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//     ...
	// Output:
	// 	double &hu, ͼ��ľ�
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���޴���
	//     -2 - ���޴���
	// Author: Huang Yige/03/18/2017
	/**********************************************/
	IMG_VVOID VisBC_Moment(Ipp8u *srcRoi, IppiSize Roi, IMG_LREAL &hu);

	/**********************************************/
	// VisBC_Screen, ����˵��:��ѡ�е�Blob�����ɸѡ��
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//     ...
	// Output:
	// 	TempStructCircle &circle1, ͼ���еĵ�һ������Բ��Բ�ļ��뾶
	//	TempStructCircle &circle2��ͼ���еĵڶ�������Բ��Բ�ļ��뾶
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���ش����޷���⵽����Բ
	//     -2 - ���޴���
	// Author: Huang Yige/03/18/2017
	/**********************************************/

	IMG_INT VisBC_Screen(Ipp8u *&srcRoi, IppiSize Roi, TempStructCircle &circle1, TempStructCircle &circle2);
	/**********************************************/
	//VisEdge_detection����˵��
	//Input
	//srcRoi   ����ͼ��
	//roiSize  ����ͼ��ĳߴ�
	//threshold  �ݶ�ǿ�ȵ���ֵ���������Ǵ���0С��1250��������������û���֪����ֵ��Ϊ���ٺ��ʣ���������0���㷨�Զ���ȡ���ʵ���ֵ����
	//
	//output
	//dstRoi  �ݶ�ǿ��
	//edgeInformation *&edgeArray  ��Ե����Ϣ�������������ꡢ���������ꡢ�ݶ�ǿ�ȡ��Ƕ�
	//
	//��������
	//��������·���1��
	//����û�������ֵС��0����������-1��
	//��������������ȷ������ͼ��ߴ粻��ȷ��srcRoi��С��ߴ粻���ϣ���������-1��
	/**********************************************/
	IMG_VVOID VisBC_Fix_Sobel(IMG_UBYTE * pSrc, IppiSize roiSize, Ipp16s *&pDst, Ipp32f *&pAngle);
	IMG_REAL getIntraClassVariance(Ipp16s* src, IMG_INT srcRows, IMG_INT srcCols, IMG_INT &varTh);
	IMG_INT VisEdge_detection(IMG_UBYTE *srcRoi, IMG_SIZE roiSize, IMG_INT threshold, IMG_WORD *&dstRoi, IMG_UBYTE *dstRoiE, edgeInformation *&edgeArray, IMG_INT &eNum);
	/**********************************************/
	// VisBC_RoiGnerate_Circular_Fitting, ���Բ
	// Input:
	// 	IMG_REAL *point_pos, ����������
	//	IMG_INT m,����������
	//  IMG_INT iteration_times�����Ƶ�������
	// Output:
	// 	CIRCULAR_FITT circular_fit�����Բ���ĵ�(a,b)�Ͱ뾶���r;
	//     ...
	// Return:
	//     0 - ����
	//     1 - ����̫��
	//     2 - ֱ��
	// Author: 03/18/2017
	/**********************************************/
	IMG_INT VisRoiGnerate_Circular_Fitting(IMG_REAL *point_pos, IMG_INT m, CIRCULAR_FITT &circular_fit);
	/**********************************************/
	// Vis_Calculate_Circle_CompleteDegree_seach_match,ʵ���Զ��巽ʽԲ�Ⱥ�������
	// Input:
	// 	CIRCULAR_FITT circular_fit��Բ�����ļ��뾶��x-a)*(x-a)+(y-b)*(y-b)=r*r;
	// 	IMG_INT m,��ĸ�����
	//	IMG_REAL *point_pos��������ꣻ
	//  IMG_INT Score_width����ֿ�ȣ��磺5��ô�㵽Բ�Ͼ���Ϊ��ʱ��1�֣��㵽Բ�Ͼ���Ϊ��ʱ��2��
	//										    �㵽Բ�Ͼ���Ϊ��ʱ��3�֣��㵽Բ�Ͼ���Ϊ��ʱ��4�֣���������
	// Output:
	// 	IMG_REAL &degreescore;Բ�ȴ�ֽ��
	//	
	//     ...
	// Return:
	//     0 - ����
	//	   ���� - �����������ܽᣩ
	// Author: 03/23/2017
	/**********************************************/
	IMG_INT Vis_Calculate_Circle_CompleteDegree_seach_match(CIRCULAR_FITT circular_fit, IMG_INT m, IMG_REAL *point_pos, IMG_INT Score_width, IMG_REAL &degreescore);
	/**********************************************/
	// Assist_VisBC_BlobCircleAccurate�����ж�λ����
	// Input:
	// 	IMG_UBBUF src, ����ͼ��
	// 	TempStructCircle *&CirRa_in, ͼ���еĵ�һ������Բ��Բ�ļ��뾶��ͼ���еĵڶ�������Բ��Բ�ļ��뾶
	//	
	// Output:
	// 	TempStructCircle *&CirRa_out, ͼ���еĵ�һ������Բ��Բ�ļ��뾶��ͼ���еĵڶ�������Բ��Բ�ļ��뾶
	//     ...
	// Return:
	//     0 - ����
	//	   ���� - ����
	// Author: 03/18/2017
	/**********************************************/
	IMG_INT Assist_VisBC_BlobCircleAccurate(IMG_UBBUF src, TempStructCircle *&CirRa_in, TempStructCircle *&CirRa_out);
	/**********************************************/
	// Assist_VisBC_BlobCircleAccurate�����ÿһ��Բ���ж�λ����
	// Input:
	// 	IMG_UBBUF src, ����ͼ��roi
	// 	TempStructCircle *CirRa, ͼ��������Բ��Բ�ļ��뾶
	//	IMG_INT time,�뾶����С�����
	//	
	// Output:
	// 	TempStructCircle *CirRA, ͼ��������Բ��Բ�ļ��뾶���
	//     ...
	// Return:
	//     0 - ����
	//	   ���� - ����
	// Author: 03/18/2017
	/**********************************************/
	IMG_INT Assist_VisBC_BlobCircleAccurate_order(IMG_UBBUF drc, TempStructCircle &CirRa, IMG_INT time);

};

class bookSocket
{
public:
	bookSocket();
	~bookSocket();

	int Read(char *ConfigFileName, BMSOCKET &bmsocket);//�������ļ�
	int Write(char *ConfigFileName);//д�������ļ�

private:

};

