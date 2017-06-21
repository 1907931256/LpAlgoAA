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
//���Բ��Ȩ�ص�������
using namespace std;

//#define DEBUG_BlobCircle

//typedef struct
//{
//	IMG_RCOORD CirCen;//Բ��
//	IMG_REAL Radius;//�뾶
//	IMG_INT miny;
//	IMG_INT minx;
//	IMG_INT maxy;
//	IMG_INT maxx;
//}TempStructCircle;//Բ

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
	// TrayLocation, ����˵��:��ͼ���е�����Բ���ж�λ��
	// Input:
	// 	   IMG_UBBUF src, ����ͼƬ
	//     ...
	// Output:
	// 	   x, ͼ���еĵ�����Բ��Բ��x����
	// 	   y, ͼ���еĵ�����Բ��Բ��y����
	//     ...
	// Return:
	//    _BACK_CODE,���� enum _BACK_CODE;
	/**********************************************/
	_BACK_CODE TrayLocation(const IMG_UBBUF src, double &x, double &y);

private:

	BMTRAY bmtray;
	/**********************************************/
	// VisBC_BlobCircleAccurate, ����˵��:��ͼ���е�����Բ���ж�λ��
	// Input:
	// 	IMG_UBBUF src, ����ͼƬ
	//	int threshold	�ݶ�ǿ�ȵ���ֵ���������Ǵ���0С��1250�����������������200����
	//     ...
	// Output:
	// 	TempStructCircle &circle1, ͼ���еĵ�һ������Բ��Բ�ļ��뾶
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���ش����޷���⵽����Բ
	//     ��������
	// Author:/03/18/2017
	/**********************************************/
	IMG_INT VisBC_BlobCircleAccurate(IMG_UBBUF src, TempStructCircle &circle1);

	/**********************************************/
	// VisBC_Pyramid, ����˵��:����������ͼ�������С��Ȼ������С��ͼ���Ͻ���ͼ��ת,�ױ��,�ڱ��
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
	//	int threshold,�ݶ�ǿ�ȵ���ֵ���������Ǵ���0С��1250����������һ���ڴ�������200���ұȽϺ���
	//     ...
	// Output:
	// 	IMG_UBBUF src,���ͼ��
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���޴���
	//     -2 - ���޴���
	// Author: /04/27/2017
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
	// Author: /04/27/2017
	/**********************************************/
	IMG_VVOID VisBC_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, IMG_INT &markersNum);

	/**********************************************/
	// VisBC_OpenClose, ����˵��:��ͼ����п����㣬����С��Blob��������С�Ŀ׶�����ͨ��
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
	// Author: /04/27/2017
	/**********************************************/
	IMG_VVOID VisBC_OpenClose(Ipp8u *srcRoi, IppiSize Roi);

	/**********************************************/
	// VisBC_Screen, ����˵��:��ѡ�е�Blob�����ɸѡ��
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//     ...
	// Output:
	// 	TempStructCircle &circle1, ͼ���е�����Բ��Բ�ļ��뾶
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���ش����޷���⵽����Բ
	//     -2 - ���޴���
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

	int Read(char *ConfigFileName, BMTRAY &bmtray);//�������ļ�
	int Write(char *ConfigFileName);//д�������ļ�
};

