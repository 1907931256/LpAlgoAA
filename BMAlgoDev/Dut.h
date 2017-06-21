#pragma once
#include "stdafx.h"
#include"BmErrorCode.h"
#include"ViType.h"
#include <stdio.h>��
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
//���Բ��Ȩ�ص�������
using namespace std;

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

typedef struct  Newline
{
	IMG_REAL k;//ֱ�߽ؾ�ʽ�С�б��
	IMG_REAL b;//ֱ�߽ؾ�ʽ�С��ؾ�
	IMG_REAL A;//ֱ��һ��ʽ�С�x��ϵ��
	IMG_REAL B;//ֱ��һ��ʽ�С�y��ϵ����
	IMG_REAL C;//ֱ��һ��ʽ�С�������
}NEWLINE;

//struct DUTOutPutParam
//{
//	double x1;	//�ĸ��ǵ�(���ϡ����ϡ����¡�����)
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
//	double cx;	//ԭ�������DUT�����
//	double cy;
//	double angle;	//����Ƕ���ʱ��Ϊ��
//	double dist;	//cx,xy���ױ߾����һ��
//
//};

//typedef struct
//{
//	IMG_RCOORD startPoint;
//	IMG_RCOORD endPoint;
//}FloatLine;//ֱ�ߣ�����Ϊ��㵽�յ�


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
	/**********************************************/
	IMG_VVOID VisBC_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma);


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
	// 	Ipp8u *srcRoi, ���ͼƬ
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���޴���
	//     -2 - ���޴���
	// Author: Huang Yige/03/18/2017
	/**********************************************/
	IMG_VVOID VisBC_OpenClose(Ipp8u *srcRoi, IppiSize Roi);
	int Histogram(Ipp8u*srcRoi, IppiSize Roi, Ipp8u*dstRoi, double &sumpixel);
	IMG_INT VisOmmIntersection_oftwoline(FloatLine line1, FloatLine line2, IMG_RCOORD &intersectionpoint);
	int SrcLine(Ipp8u*srcRoi, IMG_SIZE roiSize, NEWLINE &line);
	IMG_BOOL VisOmmLine_Fitting(IMG_RCOORD* point, NEWLINE &line, IMG_INT m);
};

