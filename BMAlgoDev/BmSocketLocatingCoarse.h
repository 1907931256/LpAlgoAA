#pragma once
#include <ipp.h>
#include <ippcv.h>
#include <ippi.h>
#include <iostream>
#include <math.h>
#include <deque>
#include <vector>
#include <Windows.h>
#include "ViType.h"
#include "BmErrorCode.h"

//#define DEBUG_BmSocketStatus
#define PI 3.1415926535

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

struct SocketLocatingCoarsePara {
	IMG_INT m_threshold[3] = { 230,200,170 };
	IMG_WINDOW m_CircleRoi1;
	IMG_WINDOW m_CircleRoi2;
	IMG_INT m_ScreenAreaMin = 500;
	IMG_INT m_ScreenAreaMax = 2000;
	IMG_INT m_OpenCloseflag[2] = { 0,0 };
	IMG_INT m_featureCircleRadius = 44;
	IMG_COORD m_edgeError = { 20,10 };
	IMG_COORD m_featureCircleDis = { 144,172 };
	IMG_REAL m_errorRoate = 0.15;
	IMG_REAL m_errorDis = 5.60803;
};

class CBmSocketLocatingCoarse
{
public:
	CBmSocketLocatingCoarse();
	~CBmSocketLocatingCoarse();

	//Load Parameter
	bool LoadParameter(char path[]);

	_BACK_CODE  SocketLocatingCoarse(const IMG_UBBUF srcRoi, const int &angleFlg);
	//返回值：
		//_CODE_EMPTY,//空料；
		//_CODE_PROERROR, //物料异常
		//_CODE_OK //物料正常

	Ipp8u* m_getRoi;
	Ipp8u* m_getRoiZero;
	IppiSize m_GetRoi;
	standardPoint m_normPoint;
	standardPoint m_normPointZero;

	/*int VisSS_GetPicture(Ipp8u* &getRoi1);
	int VisSS_GetRoi(IppiSize &GetRoi1);
	int VisSS_GetPoint(standardPoint &twoPoint);*/
	
	int VisSS_GetPicture(IMG_UBBUF &GetRoi);
	int VisSS_GetPoint(standardPoint &twoPoint);

	int VisSS_GetPictureZero(IMG_UBBUF &GetRoi);
	int VisSS_GetPointZero(standardPoint &twoPoint);

private:
	SocketLocatingCoarsePara InputPara;
	vector<int>m_signNum;
	vector<RoiFeature>m_signEdge;
	vector<IMG_COORD>m_signReal;
	vector<float>m_diff;
	//_BACK_CODE RTS_R_8uC1(IMG_UBBUF prbSrc, IMG_RCOORD prcoSrcCenter, IMG_UBBUF &prbDst, IMG_RCOORD prcoDstCenter, IMG_REAL rAngle, IMG_SIZE pszOp, IMG_REAL scale);
	_BACK_CODE VisSS_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma);
	_BACK_CODE VisSS_Binaryzation(Ipp8u *srcRoi, IppiSize Roi);
	_BACK_CODE VisSS_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, int &markersNum);
	_BACK_CODE VisSS_OpenClose(Ipp8u *srcRoi, IppiSize Roi, int &flag);
	_BACK_CODE VisSS_Screen(Ipp8u *srcRoi, IppiSize Roi, standardPoint &JudgeRoi);
	_BACK_CODE VisSS_GetArea(Ipp8u *srcRoi, IppiSize Roi, int &markersNum, int *&Area);
	_BACK_CODE VisSS_GetEdge(Ipp8u *srcRoi, IppiSize Roi, int &markersNum, RoiFeature *&BlobEdge);
	//_BACK_CODE LocationCenter(Ipp8u *srcRoi, IppiSize Roi, standardPoint &twoPoint);
};

