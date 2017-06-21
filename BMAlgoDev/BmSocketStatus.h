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
	// GetSocketStatus, 功能说明:判断Socket是否为空
	// Input:
	// 	const IMG_UBBUF srcRoi, 输入图片
	//	const int &angleFlg,输入角度
	//     ...
	// Output:
	//     ...
	// Return:
	//     0 - 正常
	//     _CODE_OK - Socket为有
	//     _CODE_PROERROR - Socket为有异物
	//     _CODE_EMPTY - Socket为空
	//	   _CODE_FILE_LOST - 文件丢失
	//	   _CODE_ANGLE_FAIL - 输入的角度标识既不是0度也不是180度
	//	   _CODE_LOCATE_FAIL - 定位失败
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE  GetSocketStatus(const IMG_UBBUF srcRoi, const int &angleFlg, int &result);
	//返回值：
		//_CODE_EMPTY,//空料；
		//_CODE_PROERROR, //物料异常
		//_CODE_OK //物料正常

private:

	SocketStatusPara InputPara;

	vector<int>m_signNum;
	vector<RoiFeature>m_signEdge;
	vector<IMG_COORD>m_signReal;
	vector<float>m_diff;

	/**********************************************/
	// VisSS_Pyramid, 功能说明:金字塔，将图像进行缩小。
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
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma);
	/**********************************************/
	// VisSS_Binaryzation, 功能说明:对图像进行二值化。
	// Input:
	// 	Ipp8u *srcRoi, 输入图片
	//	IppiSize Roi,感兴趣区域大小
	//     ...
	// Output:
	// 	IMG_UBBUF src,输出图像
	//     ...
	// Return:
	//     0 - 正常
	//     -1 - 暂无待定
	//     -2 - 暂无待定
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_Binaryzation(Ipp8u *srcRoi, IppiSize Roi);
	/**********************************************/
	// VisSS_LabelMarker, 功能说明:对图像中的Blob块进行提取。
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
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, int &markersNum);
	/**********************************************/
	// VisSS_OpenClose, 功能说明:对图像进行开运算，消除小的Blob。
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
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_OpenClose(Ipp8u *srcRoi, IppiSize Roi, int flag);
	/**********************************************/
	// VisSS_Screen, 功能说明:寻找图像中的特征圆。
	// Input:
	// 	Ipp8u *srcRoi, 输入图片
	//	IppiSize Roi,感兴趣区域大小
	//     ...
	// Output:
	// 	IMG_WINDOW JudgeRoi, 选取Roi的特征
	//     ...
	// Return:
	//     0 - 正常
	//     -1 - 特征区域不足两个
	//     -2 - 特征区域超过两个
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_Screen(Ipp8u *srcRoi, IppiSize Roi, standardPoint &twoPoint);
	/**********************************************/
	// VisSS_EmptyJudge, 功能说明:判断socket是否为空。
	// Input:
	// 	Ipp8u *srcRoi, 输入图片
	//	IppiSize Roi,感兴趣区域大小
	//     ...
	// Output:
	// 	int &result, socket是否为空（0为空）
	//     ...
	// Return:
	//     0 - 正常
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_EmptyJudge(Ipp8u *srcRoi, IppiSize Roi,  int &result);
	/**********************************************/
	// VisSS_GetBlobThreshold, 功能说明:获取动态阈值对图像进行二值化。
	// Input:
	// 	Ipp8u *srcRoi, 输入图片
	//	IppiSize Roi,感兴趣区域大小
	//     ...
	// Output:
	//     ...
	// Return:
	//     0 - 正常
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_GetBlobThreshold(Ipp8u *srcRoi, IppiSize Roi);
	/**********************************************/
	// VisSS_GetEdge, 功能说明:寻找图像中的Blob的边缘和中心点。
	// Input:
	// 	Ipp8u *srcRoi, 输入图片
	//	IppiSize Roi,感兴趣区域大小
	//     ...
	// Output:
	// 	int &markersNum, Blob块的个数
	//  IMG_RCOORD *&CenterPoint,Blob块的中心点
	//  RoiEdge *&BlobEdge,Blob块的边缘
	//     ...
	// Return:
	//     0 - 正常
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_GetEdge(Ipp8u *srcRoi, IppiSize Roi,  int &markersNum ,RoiFeature *&BlobEdge);
	/**********************************************/
	// VisSS_GetArea, 功能说明:寻找图像中的Blob块的面积。
	// Input:
	// 	Ipp8u *srcRoi, 输入图片
	//	IppiSize Roi,感兴趣区域大小
	//     ...
	// Output:
	// 	int &markersNum, Blob块的个数
	//  int *&Area，Blob块的面积
	//     ...
	// Return:
	//     0 - 正常
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_GetArea(Ipp8u *srcRoi, IppiSize Roi, int &markersNum, int *&Area);
	/**********************************************/
	// Vis_ForeignJudge, 功能说明:判断Socket中dut摆放是否正确。
	// Input:
	// 	Ipp8u *srcRoi, 输入图片
	//	IppiSize Roi,感兴趣区域大小
	//  DUTOutPutParam param，芯片的特征点
	//  int angle，输入角度
	//     ...
	// Output:
	// 	int &result, socket是否有异物（1为有，2为摆放标准）
	//     ...
	// Return:
	//     0 - 正常
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE Vis_ForeignJudge(Ipp8u *srcRoi, IppiSize Roi, int &result, DUTOutPutParam param, int angle);
	/**********************************************/
	// Vis_ForeignJudge, 功能说明:判断Socket中dut摆放是否正确。
	// Input:
	// 	Ipp8u *srcRoi, 输入图片
	//	IppiSize Roi,感兴趣区域大小
	//  DUTOutPutParam param，芯片的特征点
	//  int angle，输入角度
	//     ...
	// Output:
	// 	int &result, socket是否有异物（1为有，2为摆放标准）
	//     ...
	// Return:
	//     0 - 正常
	// Author: Huang Yige/05/02/2017
	/**********************************************/
	_BACK_CODE VisSS_LocationJudge(DUTOutPutParam param, standardPoint twoPoint, int angle, int &result);

	_BACK_CODE CBmSocketStatus::VisSS_AngleJudge(DUTOutPutParam param, int angle, int &result);

	CBmDUTSocketLocation m_p;
};


