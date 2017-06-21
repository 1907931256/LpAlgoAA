#pragma once
#include "ViType.h"
#include <vector>
#include <ipp.h>
#include <mkl.h>
#include <math.h>
#include <time.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
//#include "BmErrorCode.h"
//#include "DefineData.h"
#include "BmDUTSocketLocation.h"
#define PI 3.14159265386
#define BlobThreshold 5

#define MAX_PATH 260

//#define DEBUG_BmSocketStatus
//#define BmSocketLocatingCoarse

#define DLL_EXPORTS

#ifdef  DLL_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)   
#endif

using namespace std;

//typedef struct {
//	int min_x;
//	int min_y;
//	int max_x;
//	int max_y;
//	float CenterX;
//	float CenterY;
//}RoiFeature;
//typedef struct {
//	RoiFeature leftPoint;
//	RoiFeature rightPoint;
//}standardPoint;
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

struct SocketStatusPara {
	IMG_INT m_threshold[3] = { 230,200,170 };
	IMG_WINDOW m_CircleRoi1;
	IMG_WINDOW m_CircleRoi2;
	/*m_CircleRoi1.coWindowOff = { 123,0 };
	m_CircleRoi1.szWindowSize = { 200,123 };
	m_CircleRoi2.coWindowOff = { 285,139 };
	m_CircleRoi2.szWindowSize = { 120,167 };*/
	IMG_INT m_ScreenAreaMin = 500;
	IMG_INT m_ScreenAreaMax = 2000;
	IMG_INT m_OpenCloseflag[2] = { 0,0 };
	IMG_INT m_featureCircleRadius = 44;
	IMG_COORD m_edgeError = { 20,10 };
	IMG_COORD m_featureCircleDis = { 144,172 };
	IMG_REAL m_errorRoate = 0.15;
	IMG_REAL m_errorDis = 5.60803;
	IMG_WINDOW m_EmptyJudgeRoi;
	/*m_EmptyJudgeRoi.coWindowOff = { 51,27 };
	m_EmptyJudgeRoi.szWindowSize = { 75,40 };*/
	IMG_INT m_EmptyJudgeAreaMin = 200;
	IMG_INT m_EmptyJudgeAreaMax = 800;
	IMG_INT m_EmptyJudgeColumn[4] = { 33,35,27,18 };
	IMG_INT m_EmptyJudgeRow[6] = { 23,3,30,57,84,110 };
	IMG_INT m_EmptyJudgeCircle = 27;
	IMG_INT m_AreaMinIncomplete = 50;
	IMG_INT m_AreaMinOriginal = 170;
	IMG_INT m_AreaMidOriginal = 710;
	IMG_INT m_AreaMaxOriginal = 6000;
	IppiSize m_ForeignJudgeRoiStandard = { 120,80 };
	IMG_INT m_ForeignJudgeStaPointValue = 60;
	IMG_INT m_ForeignJudgeRoiPointValue = 80;
	IMG_REAL m_ForeignAreapMean = 0.6;
	IMG_INT m_LocationJudgeRowLeft = 290;
	IMG_INT	m_LocationJudgeRowRight = 320;
	IMG_INT	m_LocationJudgeColumnUp = 130;
	IMG_INT m_RoiX = 73;
	IMG_INT m_RoiY = 60;
	IMG_INT m_RoiW = 173;
	IMG_INT m_RoiH = 188;
	IMG_REAL m_ForeignAreapMeanAbnormal = 0.53;
	IMG_REAL m_EmptyBlobWhiteRatio = 0.7;
	IMG_INT m_EmptyBlobWhiteLine = 520;
	IMG_REAL m_AngleStandard = 90.0;
	IMG_REAL m_AngleDis = 2.5;
	IMG_REAL m_DisStandard = 711.0;
	IMG_REAL m_DisDis = 10.0;
};

class DLLEXPORT CBmSocketStatus
{
public:
	CBmSocketStatus() 
	{
		InputPara.m_threshold[0] = 230;
		InputPara.m_threshold[1] = 200;
		InputPara.m_threshold[2] = 170;
		InputPara.m_CircleRoi1.coWindowOff = { 123,0 };
		InputPara.m_CircleRoi1.szWindowSize = { 200,123 };
		InputPara.m_CircleRoi2.coWindowOff = { 285,139 };
		InputPara.m_CircleRoi2.szWindowSize = { 120,167 };
		InputPara.m_ScreenAreaMin = 500;
		InputPara.m_ScreenAreaMax = 2000;
		InputPara.m_OpenCloseflag[0] = 0;
		InputPara.m_OpenCloseflag[1] = 5;
		InputPara.m_featureCircleRadius = 44;
		InputPara.m_edgeError = { 20,10 };
		InputPara.m_featureCircleDis = { 144,172 };
		InputPara.m_errorRoate = 0.15;
		InputPara.m_errorDis = 7.2928;
		InputPara.m_EmptyJudgeRoi.coWindowOff = { 51,27 };
		InputPara.m_EmptyJudgeRoi.szWindowSize = { 75,40 };
		InputPara.m_EmptyJudgeAreaMin = 200;
		InputPara.m_EmptyJudgeAreaMax = 800;
		InputPara.m_EmptyJudgeColumn[0] = 33;
		InputPara.m_EmptyJudgeColumn[1] = 35;
		InputPara.m_EmptyJudgeColumn[2] = 24;
		InputPara.m_EmptyJudgeColumn[3] = 18;
		InputPara.m_EmptyJudgeRow[0] = 23;
		InputPara.m_EmptyJudgeRow[1] = 3;
		InputPara.m_EmptyJudgeRow[2] = 30;
		InputPara.m_EmptyJudgeRow[3] = 57;
		InputPara.m_EmptyJudgeRow[4] = 84;
		InputPara.m_EmptyJudgeRow[5] = 110;
		InputPara.m_EmptyJudgeCircle = 23;
		InputPara.m_AreaMinIncomplete = 50;
		InputPara.m_AreaMinOriginal = 170;
		InputPara.m_AreaMidOriginal = 710;
		InputPara.m_AreaMaxOriginal = 6000;
		InputPara.m_ForeignJudgeRoiStandard = { 120,80 };
		InputPara.m_ForeignJudgeStaPointValue = 60;
		InputPara.m_ForeignJudgeRoiPointValue = 80;
		InputPara.m_ForeignAreapMean = 0.6;
		InputPara.m_LocationJudgeRowLeft = 295;
		InputPara.m_LocationJudgeRowRight = 342;
		InputPara.m_LocationJudgeColumnUp = 100;
		InputPara.m_RoiX = 73;
		InputPara.m_RoiY = 65;
		InputPara.m_RoiW = 173;
		InputPara.m_RoiH = 188;
		InputPara.m_ForeignAreapMeanAbnormal = 0.53;
		InputPara.m_EmptyBlobWhiteRatio = 0.7;
		InputPara.m_EmptyBlobWhiteLine = 520;
		InputPara.m_AngleStandard = 90.0;
		InputPara.m_AngleDis = 2.5;
		InputPara.m_DisStandard = 711.0;
		InputPara.m_DisDis = 10.0;
		//m_p = new CBmDUTSocketLocation;
	};
	~CBmSocketStatus() 
	{
		vector<int>().swap(m_signNum);
		vector<RoiFeature>().swap(m_signEdge);
		vector<IMG_COORD>().swap(m_signReal);
		vector<float>().swap(m_diff);
		//delete m_p;
	};
public:

	//Load Parameter
	bool LoadParameter(char path[]);


	/**********************************************/
	// GetSocketStatus, ����˵��:�ж�Socket�Ƿ�Ϊ��
	// Input:
	// 	const IMG_UBBUF srcRoi, ����ͼƬ
	//	const int &angleFlg,����Ƕ�
	//     ...
	// Output:
	//     ...
	// Return:
	//     0 - ����
	//     _CODE_OK - SocketΪ��
	//     _CODE_PROERROR - SocketΪ������
	//     _CODE_EMPTY - SocketΪ��
	//	   _CODE_FILE_LOST - �ļ���ʧ
	//	   _CODE_ANGLE_FAIL - ����ĽǶȱ�ʶ�Ȳ���0��Ҳ����180��
	//	   _CODE_LOCATE_FAIL - ��λʧ��
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE  GetSocketStatus(const IMG_UBBUF srcRoi, const int &angleFlg, int &result);
	//����ֵ��
		//_CODE_EMPTY,//���ϣ�
		//_CODE_PROERROR, //�����쳣
		//_CODE_OK //��������

private:

	SocketStatusPara InputPara;

	vector<int>m_signNum;
	vector<RoiFeature>m_signEdge;
	vector<IMG_COORD>m_signReal;
	vector<float>m_diff;

	/**********************************************/
	// VisSS_Pyramid, ����˵��:����������ͼ�������С��
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
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma);
	/**********************************************/
	// VisSS_Binaryzation, ����˵��:��ͼ����ж�ֵ����
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//     ...
	// Output:
	// 	IMG_UBBUF src,���ͼ��
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ���޴���
	//     -2 - ���޴���
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_Binaryzation(Ipp8u *srcRoi, IppiSize Roi);
	/**********************************************/
	// VisSS_LabelMarker, ����˵��:��ͼ���е�Blob�������ȡ��
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
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, int &markersNum);
	/**********************************************/
	// VisSS_OpenClose, ����˵��:��ͼ����п����㣬����С��Blob��
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
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_OpenClose(Ipp8u *srcRoi, IppiSize Roi, int flag);
	/**********************************************/
	// VisSS_Screen, ����˵��:Ѱ��ͼ���е�����Բ��
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//     ...
	// Output:
	// 	IMG_WINDOW JudgeRoi, ѡȡRoi������
	//     ...
	// Return:
	//     0 - ����
	//     -1 - ��������������
	//     -2 - �������򳬹�����
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_Screen(Ipp8u *srcRoi, IppiSize Roi, standardPoint &twoPoint);
	/**********************************************/
	// VisSS_EmptyJudge, ����˵��:�ж�socket�Ƿ�Ϊ�ա�
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//     ...
	// Output:
	// 	int &result, socket�Ƿ�Ϊ�գ�0Ϊ�գ�
	//     ...
	// Return:
	//     0 - ����
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_EmptyJudge(Ipp8u *srcRoi, IppiSize Roi,  int &result);
	/**********************************************/
	// VisSS_GetBlobThreshold, ����˵��:��ȡ��̬��ֵ��ͼ����ж�ֵ����
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//     ...
	// Output:
	//     ...
	// Return:
	//     0 - ����
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_GetBlobThreshold(Ipp8u *srcRoi, IppiSize Roi);
	/**********************************************/
	// VisSS_GetEdge, ����˵��:Ѱ��ͼ���е�Blob�ı�Ե�����ĵ㡣
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//     ...
	// Output:
	// 	int &markersNum, Blob��ĸ���
	//  IMG_RCOORD *&CenterPoint,Blob������ĵ�
	//  RoiEdge *&BlobEdge,Blob��ı�Ե
	//     ...
	// Return:
	//     0 - ����
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_GetEdge(Ipp8u *srcRoi, IppiSize Roi,  int &markersNum ,RoiFeature *&BlobEdge);
	/**********************************************/
	// VisSS_GetArea, ����˵��:Ѱ��ͼ���е�Blob��������
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//     ...
	// Output:
	// 	int &markersNum, Blob��ĸ���
	//  int *&Area��Blob������
	//     ...
	// Return:
	//     0 - ����
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_GetArea(Ipp8u *srcRoi, IppiSize Roi, int &markersNum, int *&Area);
	/**********************************************/
	// Vis_ForeignJudge, ����˵��:�ж�Socket��dut�ڷ��Ƿ���ȷ��
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//  DUTOutPutParam param��оƬ��������
	//  int angle������Ƕ�
	//     ...
	// Output:
	// 	int &result, socket�Ƿ������1Ϊ�У�2Ϊ�ڷű�׼��
	//     ...
	// Return:
	//     0 - ����
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE Vis_ForeignJudge(Ipp8u *srcRoi, IppiSize Roi, int &result, DUTOutPutParam param, int angle);
	/**********************************************/
	// Vis_ForeignJudge, ����˵��:�ж�Socket��dut�ڷ��Ƿ���ȷ��
	// Input:
	// 	Ipp8u *srcRoi, ����ͼƬ
	//	IppiSize Roi,����Ȥ�����С
	//  DUTOutPutParam param��оƬ��������
	//  int angle������Ƕ�
	//     ...
	// Output:
	// 	int &result, socket�Ƿ������1Ϊ�У�2Ϊ�ڷű�׼��
	//     ...
	// Return:
	//     0 - ����
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_LocationJudge(DUTOutPutParam param, standardPoint twoPoint, int angle, int &result);

	_BACK_CODE CBmSocketStatus::VisSS_AngleJudge(DUTOutPutParam param, int angle, int &result);

	CBmDUTSocketLocation m_p;
};


