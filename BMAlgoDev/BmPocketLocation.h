#pragma once
#include "ViType.h"
#include "Vis_assist.h"
#include "BmErrorCode.h"
#include <ipp.h>
#include <mkl.h>

#include <malloc.h>
#include <assert.h>
#include <cstring>
#include <math.h>
#include <vector>

/***			memory leak					****/
#ifdef _DEBUG  
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 
#include <crtdbg.h>  
//在入口函数中包含 _CrtDumpMemoryLeaks();    
//即可检测到内存泄露
/**********************************************/

#define PI 3.14159265386

#define  HALCON_X_BIAS 4.102447
#define  HALCON_Y_BIAS 158.0719
#define  HALCON_ANGLE_BIAS -0.837419952

namespace regionDef
{
	typedef struct tagRegionPeakPoint
	{
		int xMin;
		int xMax;
		int yMin;
		int yMax;
		//tagRegionPeakPoint(int _xMax,int _yMax)
		//{
		//	xMin = 0;
		//	xMax = _xMax;
		//	yMin = 0;
		//	yMax = _yMax;
		//}
		tagRegionPeakPoint()
		{
			xMin = 0;
			xMax = 0;
			yMin = 0;
			yMax = 0;
		}

	}RegionPeakPoint;	//连通域四个极值点

	typedef struct tagRegionWidthHeight
	{
		int width;
		int height;
		tagRegionWidthHeight()
		{
			width = 0;
			height = 0;
		}
	}RegionWidthHeight;	//连通域宽高
}

typedef struct tagProjectScore
{
	int projectX;
	int projectY;

	tagProjectScore()
	{
		projectX = 0;
		projectY = 0;
	}

}ProjectScore;	//连通域投影分数




typedef struct tagLShape
{
	int index;
	IppiPoint pos;

	tagLShape()
	{
		index = 0;
		pos = {0,0};
	}
}LShape;	//T连通域宽高

typedef struct tagTShape
{
	int index;
	IppiPoint pos;

	tagTShape()
	{
		index = 0;
		pos = { 0,0 };
	}
}TShape;	//L连通域宽高

class CBmPocketLocation
{
public:
	CBmPocketLocation();
	~CBmPocketLocation();

	

	std::vector<std::vector <LShape> > getScoreProperIndex() const { return m_LshapeIndex; }

	IppiPoint getCenPoint() const { return m_cenPoint; }

public:
	
	bool LoadParameter(char[]);


	

	/**********************************************/
	// PocketLocation, pocket定位
	// Input:
	//	IMG_UBBUF src; 输入图片
	//	double &x, pocket中心x坐标
	//	double &y, pocket中心y坐标
	//  double &angle, pocket角度
	//
	// Return:
	//
	//		_CODE_OK , //定位成功   0
	//		_CODE_LOCATE_FAIL,//定位失败 4  
	//		_CODE_THROW//无图-1
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	_BACK_CODE PocketLocation_Old(const IMG_UBBUF src,
		double &x, double &y, double &angle);	//输出角度逆时针为正
	_BACK_CODE PocketLocation(const IMG_UBBUF src,
		double &x, double &y, double &angle);	//输出角度逆时针为正
	
private:
	/**********************************************/
	// pyramid, 金字塔
	// Author: Jimmy Zhan  5/5/2017
	/**********************************************/
	int pyramid(IMG_UBBUF src, unsigned char * pDst, int & pyramid_width, int & pyramid_height, int level);

	//直方图均衡化
	//HistogramEqualization直方图均衡化：
	//Input
	//IMG_UBBUF src  输入图像
	//OutPut
	//IMG_UBBUF dst  输出图像
	//Return
	// 0   正常
	//-1   异常
	//作者/日期  姜贺/20170407
	int VisHistogramEqualization(IMG_UBBUF src, IMG_UBBUF dst);

	//直方图规定化
	//HistogramSpecification图像直方图规定化功能说明：给定一幅图和规定的均值和方差，计算规定化的图像
	//Input
	//src  输入图像
	//meanValue  规定的灰度的均值
	//variance   规定的灰度的标准差
	//Output
	//dst   由给定的均值和标准差规定化的图像
	//Return
	// 0   正常
	//-1   异常
	//作者/日期  姜贺/20170407
	int VisHistogramSpecification(IMG_UBBUF src, float meanValue, float stdDevValue, IMG_UBBUF dst);

	/**********************************************/
	// threshold_OTSU, OTSU阈值分割
	// Input:
	// unsigned char * ucpSrc ， 输入图像指针
	// IppiSize imgSize, roi大小
	//
	// Output:
	// unsigned char * ucpDst ， 输出图像指针
	//
	// Return:
	//	ippstatus
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	int threshold_OTSU(unsigned char * ucpSrc, IppiSize imgSize, unsigned char * ucpDst);

	/**********************************************/
	// threshold_manual, 手动阈值分割
	// Input:
	// unsigned char * ucpSrc ， 输入图像指针
	// IppiSize imgSize, roi大小
	//
	// Output:
	// unsigned char * ucpDst ， 输出图像指针
	//
	// Return:
	//	ippstatus
	//
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	int threshold_manual(unsigned char * ucpSrc, IppiSize imgSize, unsigned char * ucpDst);


	/**********************************************/
	// labelMarker, 连通域标记
	// Input:
	// unsigned char * ucpSrc ， 输入图像指针
	// IppiSize imgSize, roi大小
	//
	// Output:
	// int * intpDst, 标记图像
	// int & regionNum， 连通域数目
	//
	// Return:
	//	ippstatus
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	int labelMarker(unsigned char * ucpSrc, IppiSize imgSize, int * intpDst, int & regionNum);

	//腐蚀膨胀函数，3by3表示核是3*3，5by5表示核是5*5
	int morfDilateBorder_notInPlace_3by3(IMG_UBBUF src, unsigned char * pDst);
	int morfErodeBorder_notInPlace_3by3(IMG_UBBUF src, unsigned char *pDst);
	int morfErodeBorder_notInPlace_5by5(IMG_UBBUF src, unsigned char * pDst);

	/**********************************************/
	// eraseEdgeRegion, 排除边界连通域
	// Return:
	//	true : 排除
	//  false:不排除
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	bool eraseEdgeRegion(regionDef::RegionPeakPoint regionPeakPoint, int width, int height);

	/**********************************************/
	// eraseCenterRegion, 排除中间（T型块）间的连通域
	// Return:
	//	true : 排除
	//  false:不排除
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	bool eraseCenterRegion(IppiPoint regionGraCenPoint, regionDef::RegionPeakPoint regionPeakPoint,int margin);

	/**********************************************/
	// judgeRealLShape, 判断连通域是否为真正L型
	// Return:
	//	true : 是要找的L型
	//  false: 不是要找的L型
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	bool judgeRealLShape(int * intSrc, int width, int height, int index, int limit_Lwidth, IppiPoint regionGraCenPoint, regionDef::RegionWidthHeight regionWidthHeight, int regionNum);

	/**********************************************/
	// getRegionInfo, blob分析找L型
	// Input:
	// int * intSrc ， 输入已标记过的图像
	// IppiSize imgSize, roi大小
	// int regionNum， 连通域数量
	// 
	// Output:
	// int * regionArea, 各连通域面积
	// IppiPoint * regionGraCenPoint， 各连通域重心
	// RegionPeakPoint * regionPeakPoint， 各连通域极值点
	// RegionWidthHeight * regionWH, 各连通域宽高
	// ProjectScore *projectScore,各连通域x和y方向的投影分数
	//
	// Return:
	//	0 ：至少找到一个L型
	//  -1: 找不到L型
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	int getRegionInfo(int * intSrc, IppiSize imgSize, int regionNum, int * regionArea, IppiPoint * regionGraCenPoint, regionDef::RegionPeakPoint * regionPeakPoint, regionDef::RegionWidthHeight * regionWH, ProjectScore *projectScore);

	//与getRegionInfo相同，只是删去L型重心离图像中间的距离判断
	int getRegionInfo2(int * intSrc, IppiSize imgSize, int regionNum, int * regionArea, IppiPoint * regionGraCenPoint, regionDef::RegionPeakPoint * regionPeakPoint, regionDef::RegionWidthHeight * regionWH, ProjectScore * projectScore);

	//int linePointSelect(int * intSrc, IppiSize imgSize, int index, int regionRef, IppiPoint regionGraCenPoint, std::vector<IMG_RCOORD> &PointCor);

	/**********************************************/
	// LShapeSelect, 剔除假的L型块
	// Author: Jimmy Zhan  5/5/2017
	/**********************************************/
	int LShapeSelect();

	/**********************************************/
	// Vis_HistogramExample, 统计直方图
	// Author: Yige?
	/**********************************************/
	int Vis_HistogramExample(Ipp8u * srcRoi, IppiSize Roi, Ipp32u pHistVec[256]);

	/**********************************************/
	// PocketCoarseLocate, pocket粗定位
	// Input:
	// const IMG_UBBUF src ， 输入图像
	// 
	// Output:
	// int &xMin, pocket最左位置
	// int &xMax， pocket最右位置
	//
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	void PocketCoarseLocate_old(const IMG_UBBUF src, int & xMin, int & xMax);
	int PocketCoarseLocate(const IMG_UBBUF src, int &xMin, int &xMax);

	/**********************************************/
	// erase_inverseDUT, 将翻转DUT的亮部涂黑，以便能够分割出L型块
	// Input:
	// IMG_UBBUF src ， 输入图像
	// erasePro，去除比例
	//
	// Author: Jimmy Zhan  16/5/2017
	/**********************************************/
	int erase_inverseDUT(IMG_UBBUF src, float erasePro);


	/**********************************************/
	// HistogramFilter, 将DUT上某些较亮的区域涂成设定值，以便能够分割出L型块
	// Input:
	// IMG_UBBUF ubbSrc ， 输入图像
	// Ipp32u * pHistVec，输入直方图
	// float filterPro， 去除比例
	// IMG_UBYTE setValue， 设定的值
	//
	// Author: Jimmy Zhan  16/5/2017
	/**********************************************/
	void HistogramFilter(IMG_UBBUF ubbSrc, Ipp32u * pHistVec, float filterPro, IMG_UBYTE setValue);


private:
	int m_pyramidLevel;	//金字塔等级

	int m_ucThreshold;	//手动阈值
	int m_whiteValue;	//亮白区域阈值（填充亮白区域使OTSU完美分割）

	float m_meanValueMin;	//最低图像平均灰度
	float m_meanValue;	//图像平均灰度
	float m_stdDevValue;	//图像灰度标准差

	int m_areaMin;		//L型块最小面积
	int m_areaMax;		//L型块最大面积
	int m_LwidthMax;	//Lshape max width
	int m_LwidthMin;
	int m_LheightMax;
	int m_LheightMin;
	int m_DUT_areaMin;  //DUT最小面积

	int m_TareaMin;		//T型块最小面积
	int m_TareaMax;		//T型块最大面积
	int m_TwidthMax;	//T型块最大宽度
	int m_TheightMax;	//T型块最大高度

	float m_projectPercent;		//L型块选择投影部分的比例
	float m_regionAreaPercent;		// L型块投影区域面积占总面积比例
	int   m_projectDiff;		//x方向和y方向投影分数的差

	int m_margin;	//L型块重心离T型块边缘的差

	int m_limit_Lwidth;		//L型块最大宽度

	int m_xBias;		//由L型块重心推算tray中心
	int m_yBias;

	//int m_xBias_ROI;		//拟合用
	//int m_yBias_ROI;
	//int m_ROIwidth;
	//int m_ROIheight;

	int m_BlockHeight;		//寻找矩形块（计算角度）
	int m_BlockWidth;
	int m_BlockShift_x;
	int m_BlockShift_y;

	int m_LshapeTurn;		//L型块转角大小
	int m_cenXjudge;		//判断L型块是否是里面用
	int m_cenYjudge;		//判断L型块是否是里面用

	int m_LshapeDiffY;		//水平L型块Y方向偏差
	int m_LshapeDiffX;		//垂直L型块X方向偏差

	float m_hisErasePro;		//直方图过滤比例
	int m_hisEraseMean;

	std::vector <int> m_areaProperIndex;	//保存符合面积范围的连通域ID
	std::vector<std::vector <LShape> >m_LshapeIndex;	//保存L型块ID
	TShape m_Tshape;	//保存T型块ID，位置
	IppiPoint m_cenPoint;	//保存tray中心
	std::vector <int> m_vecDUTindex;

	VisBuf m_setVisBuf;		//assit
};

//typedef struct tagCBmPocketLocationMember
//{
//	int m_pyramidLevel;	//金字塔等级
//
//	int m_ucThreshold;	//手动阈值
//	int m_whiteValue;	//亮白区域阈值（填充亮白区域使OTSU完美分割）
//
//	float m_meanValueMin;	//最低图像平均灰度
//	float m_meanValue;	//图像平均灰度
//	float m_stdDevValue;	//图像灰度标准差
//
//	int m_areaMin;		//L型块最小面积
//	int m_areaMax;		//L型块最大面积
//	int m_DUT_areaMin;  //DUT最小面积
//
//	int m_TareaMin;		//T型块最小面积
//	int m_TareaMax;		//T型块最大面积
//	int m_TwidthMax;	//T型块最大宽度
//	int m_TheightMax;	//T型块最大高度
//
//	float m_projectPercent;		//L型块选择投影部分的比例
//	float m_regionAreaPercent;		// L型块投影区域面积占总面积比例
//	int   m_projectDiff;		//x方向和y方向投影分数的差
//
//	int m_margin;	//L型块重心离T型块边缘的差
//
//	int m_limit_Lwidth;		//L型块最大宽度
//
//	int m_xBias;		//由L型块重心推算tray中心
//	int m_yBias;
//
//	//int m_xBias_ROI;		//拟合用
//	//int m_yBias_ROI;
//	//int m_ROIwidth;
//	//int m_ROIheight;
//
//	int m_BlockHeight;		//寻找矩形块（计算角度）
//	int m_BlockWidth;
//	int m_BlockShift_x;
//	int m_BlockShift_y;
//
//	int m_LshapeTurn;		//L型块转角大小
//	int m_cenXjudge;		//判断L型块是否是里面用
//	int m_cenYjudge;		//判断L型块是否是里面用
//
//	int m_LshapeDiffY;		//水平L型块Y方向偏差
//	int m_LshapeDiffX;		//垂直L型块X方向偏差
//}CBmPocketLocationMember;