#pragma once
#include "BmErrorCode.h"
#include "BmPocketLocation.h"
#include "BmDUTTrayLocation.h"
#include "Dut.h"

/***			memory leak					****/
#ifdef _DEBUG  
#define new  new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 
#include <crtdbg.h>  
//在入口函数中包含 _CrtDumpMemoryLeaks();    
//即可检测到内存泄露
/**********************************************/

class CBmPocketStatus
{
public:
	CBmPocketStatus();
	~CBmPocketStatus();
public:
	//Load Parameter
	bool LoadParameter(char path[]);

	/**********************************************/
	// GetPocketStatus, pocket状态判断
	// Input:
	// IMG_UBBUF src; 输入图片
	//
	// Return:
	//		_CODE_EMPTY,//空料；	14
	//		_CODE_PROERROR, //物料异常	15
	//		_CODE_DUT_NORMAL , //物料正常   16
	//		_CODE_LOCATE_FAIL,//定位失败 4  
	//		_CODE_THROW//无图-1
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	_BACK_CODE GetPocketStatus(const IMG_UBBUF src);


private:
	//阈值
	int threshold_OTSU(unsigned char * ucpSrc, IppiSize imgSize, unsigned char * ucpDst);
	int threshold_OTSU_inv(unsigned char * ucpSrc, IppiSize imgSize, unsigned char * ucpDst);
	int threshold_manual_inv(unsigned char * ucpSrc, IppiSize imgSize, unsigned char * ucpDst);


	//膨胀，腐蚀
	int morfDilateBorder_notInPlace_5by5(IMG_UBBUF src, unsigned char * pDst);
	int morfErodeBorder_notInPlace_7by7(IMG_UBBUF src, unsigned char * pDst);

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
	
	/**********************************************/
	// getRegionInfo, blob分析
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
	//	0 ：成功
	//  
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	int getRegionInfo(int * intSrc, IppiSize imgSize, int regionNum, int * regionArea, IppiPoint * regionGraCenPoint, regionDef::RegionPeakPoint * regionPeakPoint, regionDef::RegionWidthHeight * regionWH, ProjectScore * projectScore);
	
	//sobel
	void SobelFilter_8u16s_C1_5x5(IMG_UBYTE * pSrc, IppiSize roiSize, Ipp16s *pDst, Ipp32f *pAngle);

	/**********************************************/
	// eraseEdgeRegion, 排除边界连通域
	// Return:
	//	true : 排除
	//  false:不排除
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	bool eraseEdgeRegion(regionDef::RegionPeakPoint regionPeakPoint, int width, int height);

	/**********************************************/
	// judgeNormalDUT, 判断异物是否为DUT且摆放正确
	// Input :
	// const IMG_UBBUF src, 输入图像
	//
	// Return:
	//	true : 有DUT且摆放正常
	//  false: 异物或者DUT摆放不正
	// Author: Jimmy Zhan  3/5/2017
	/**********************************************/
	bool judgeNormalDUT(const IMG_UBBUF src, int trayX, int trayY);
	bool judgeNormalDUT2(const IMG_UBBUF src, int trayX, int trayY);

private:
	CBmPocketLocation m_pocketLocate;
	CBmDUTTrayLocation m_DUTTrayLocate;
	DUT m_judgeDUT;
	VisBuf m_setVisBuf;

	int m_outHalfLength;	//pocket半宽
	int m_halfLength;	//tray半宽

	int m_grayInside;	//分割时hole里的梯度阈值
	int m_grayInsideMax;	//分割时hole里的最大梯度阈值
	int m_grayOutside;		//分割tray里hole外区域的梯度阈值
	int m_grayCountMax;		//对hole里超过最大梯度阈值的区域进行计数，超过此值则认为是异物

	int m_holeAreaMin;		//hole黑洞最小面积
	int m_trayHalfWidth;	//hole半宽

	int m_foreignBody;		//外边缘异物面积判定

	int m_grayMean;		//图像平均灰度
};

