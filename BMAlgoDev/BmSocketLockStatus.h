#pragma once
#define _CRTDBG_MAP_ALLOC
//#include<crtdbg.h>
//#include "stdafx.h"
#include<stdlib.h>
#include<crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#include <ipp.h>
#include <ippcv.h>
#include <ippi.h>
#include <iostream>
#include <math.h>
#include <deque>
#include <vector>
#include "ViType.h"
//#include "BmErrorCode.h"
#include "BmSocketLocatingCoarse.h"
//#define DEBUG_BmSocketStatus

#define DLL_EXPORTS

#ifdef  DLL_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)   
#endif

using namespace std;


struct SocketLockPara {
	IMG_INT CircleWidth;
	IMG_INT CircleHeight;
	IMG_INT LockRangeWidth;
	IMG_INT LockRangeHeight;
	IMG_INT circleLockHeight;
	IMG_INT circleLockWidth;
	IMG_INT blackHeight;
	IMG_INT halfBlackHeight;
	IMG_INT whiteHeight;
	IMG_INT lockEffHeight;
	IMG_INT botHeight;
	IMG_INT roiBotVerBgnHeight;
	IMG_REAL avgErrThr;
	IMG_REAL ratioThr;
	IMG_INT diffThr;
	IMG_REAL bigRatioThr;
	IMG_REAL smallRatioThr;
	IMG_INT roiEffThr1;
	IMG_INT roiEffThr2;
};

class DLLEXPORT CBmSocketLockStatus
{
public:
	CBmSocketLockStatus() {
		_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		lockParas.CircleWidth = 579;   //����Բ��֮��Ŀ��
		lockParas.CircleHeight = 685;   //����Բ��֮��ĸ߶�
		lockParas.LockRangeWidth = 40; //����ͬһ��ͼ�������,lock����Ŀ��
		lockParas.LockRangeHeight = 565; //lock����ĸ߶�
		lockParas.circleLockHeight = 239; //��Բ��Բ����Lock�������֮��ĸ߶�
		lockParas.circleLockWidth = 370; //��Բ��Բ����Lock����������ʼλ��֮��ľ���
		lockParas.blackHeight = 258;
		lockParas.halfBlackHeight = 76;
		lockParas.whiteHeight = 240;
		lockParas.lockEffHeight = 136;
		lockParas.botHeight = 135;
		lockParas.roiBotVerBgnHeight = 610;
		lockParas.avgErrThr = 0.2;
		lockParas.ratioThr = 0.8;
		lockParas.diffThr = 30;
		lockParas.bigRatioThr = 0.9;
		lockParas.smallRatioThr = 0.75;
		lockParas.roiEffThr1 = 3;
		lockParas.roiEffThr2 = 1;
		//locating = new CBmSocketLocatingCoarse;
	};
	~CBmSocketLockStatus() {
		//delete locating;
	};
public:

	//Load Parameter
	//bool LoadParameter(char path[]);
	bool LoadParameter(char path[]);
	_BACK_CODE GetSocketOpenStatus(const IMG_UBBUF src,
		const int angleFlg//0 or 180
	);

	//����???�̡�o
		//_CODE_EMPTY,//??��?��?
		//_CODE_PROERROR, //??��?����3��
		//_CODE_OK //??��??y3��

private:
	int curBlackHeight;
	int curHalgBlackHeight;
	int curWhiteHeight;
	int curLockEffHeight;
	int curBotHeight;
	int curRoiBotHeight;
	int threshold;
	SocketLockPara lockParas;
	CBmSocketLocatingCoarse locating;
	_BACK_CODE VisSS_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma);
	_BACK_CODE VisSS_Binaryzation(Ipp8u *srcRoi, IppiSize Roi);
	_BACK_CODE VisSS_OpenClose(Ipp8u *srcRoi, IppiSize Roi);
	_BACK_CODE VisSS_lockStatus(Ipp8u *srcRoi, Ipp8u *roiImg, IppiSize Roi, standardPoint twoPoint, int exceedNum);
	_BACK_CODE lockEffective(Ipp8u *srcRoi, IppiSize Roi, int &offset, bool &effective, standardPoint twoPoint);
	_BACK_CODE lockBinaryzation(Ipp8u *srcRoi, IppiSize Roi);
	_BACK_CODE calcVerInfo(standardPoint twoPoint, IppiSize Roi);
	_BACK_CODE calcThr(Ipp8u *srcRoi, IppiSize Roi, bool &blackWhiteEqual);
	_BACK_CODE VisFilterSobel_8u8u(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst, const unsigned char winWidth);
	_BACK_CODE detectDutAbnormal(const unsigned char *src, const int srcHeight, const int srcWidth, const unsigned char winWidth);
};

