
// BMAlgoDevView.h : CBMAlgoDevView 类的接口
//

#pragma once

#include "MyGdiPlus.h"
#include "MyTimercount.h"
#include "ViType.h"
#include "VisAlg2DBase.h"

#include "BmDUTTrayLocation.h"
#include "BmDUTSocketLocation.h"
#include "BmSocketLocatingCoarse.h"
#include "BmSocketLocation.h"
#include "BmSocketStatus.h"
#include "BmPocketLocation.h"
#include "BmPocketStatus.h"
#include "BmTrayLocation.h"
#include "BmSocketLockStatus.h"
//#include "BmDUTTrayRecognization.h"
#include "Dut.h"

#include "AaAlgorithm.h"

class CBMAlgoDevView : public CScrollView
{
protected: // 仅从序列化创建
	CBMAlgoDevView();
	DECLARE_DYNCREATE(CBMAlgoDevView)

// 特性
public:
	CBMAlgoDevDoc* GetDocument() const;

	// 自定义私有成员变量
private:
	CMyGdiPlus*	m_pMyGDIPlus;
	int		m_imagePos;
	int		m_imageNum;
	CString		m_ImageNameList[Image_Num_Max];
	CString		m_sFileName;
	SGrayBuff	m_ImageVolume[Image_Num_Max];
	SGrayBuff	m_pBmpData;
	IMG_UBBUF	m_src;
	IMG_UBBUF	m_dst;
	int			m_srcW;
	int			m_srcH;

	CPoint	m_showPoint;
	bool	m_bShowMovePos;
	bool	m_bMouseDown;

	bool	m_bAllFolder;
	int		m_nProgressTotal;
	int		m_nProgressCurrent;

	CFileFind				m_FileFinder;
	CList<CString, CString>	m_lstFileList;
	POSITION				m_posFileList;
	bool					m_bIsNext;
	CString					m_szFilePath;

	//Algorithm classes
	CVisAlg2DBase m_myVisAlg2DBase;

	//do folder
	float	m_fTimer;
	bool	m_bDoFolder;

	//Pyramid
	int m_nPrePyramid1;
	bool m_bPrePyramid1;
	int m_nPrePyramid2;
	bool m_bPrePyramid2;
	int m_nPrePyramid3;
	bool m_bPrePyramid3;

	//ROI
	int m_nRoiDraw;
	bool m_bRoiDraw;
	int m_nRoiDelete;
	bool m_bRoiDelete;
	VisRoiRect m_roiRect;

	//Filter menu
	//Mean
	int m_nPreFilterMean3;
	bool m_bPreFilterMean3;
	int m_nPreFilterMean5;
	bool m_bPreFilterMean5;
	int m_nPreFilterMean7;
	bool m_bPreFilterMean7;
	int m_nPreFilterMean9;
	bool m_bPreFilterMean9;
	int m_nPreFilterMean15;
	bool m_bPreFilterMean15;
	//Median
	int m_nPreFilterMedian3;
	bool m_bPreFilterMedian3;
	int m_nPreFilterMedian5;
	bool m_bPreFilterMedian5;
	int m_nPreFilterMedian7;
	bool m_bPreFilterMedian7;
	int m_nPreFilterMedian9;
	bool m_bPreFilterMedian9;
	int m_nPreFilterMedian15;
	bool m_bPreFilterMedian15;
	//Gauss
	int m_nPreFilterGauss3;
	bool m_bPreFilterGauss3;
	int m_nPreFilterGauss5;
	bool m_bPreFilterGauss5;
	int m_nPreFilterGauss7;
	bool m_bPreFilterGauss7;
	int m_nPreFilterGauss9;
	bool m_bPreFilterGauss9;
	int m_nPreFilterGauss15;
	bool m_bPreFilterGauss15;
	//Sobel
	int m_nPreFilterSobel3;
	bool m_bPreFilterSobel3;
	int m_nPreFilterSobel5;
	bool m_bPreFilterSobel5;
	int m_nPreFilterSobel7;
	bool m_bPreFilterSobel7;

	//Segment
	int		m_nSegmentMannual;
	bool	m_bSegmentMannual;
	int		m_nSegmentOtsu;
	bool	m_bSegmentOtsu;
	int		m_nSegmentDynamic;
	bool	m_bSegmentDynamic;
	int		m_nSegmentCntDots;
	bool	m_bSegmentCntDots;

	//Morphology
	int		m_nMorphonogyErode;
	bool	m_bMorphonogyErode;
	int		m_nMorphonogyDilation;
	bool	m_bMorphonogyDilation;
	int		m_nMorphonogyOpen;
	bool	m_bMorphonogyOpen;
	int		m_nMorphonogyClose;
	bool	m_bMorphonogyClose;

	//IA978相关菜单变量
	int m_nIa978DutTrayLocationCoarse;
	bool m_bIa978DutTrayLocationCoarse;
	int m_nIa978DutTrayLocation;
	bool m_bIa978DutTrayLocation;
	int m_nIa978DutSocketLocationCoarse;
	bool m_bIa978DutSocketLocationCoarse;
	int m_nIa978DutSocketLocation;
	bool m_bIa978DutSocketLocation;
	int m_nIa978SocketLocationCoarse;
	bool m_bIa978SocketLocationCoarse;
	int m_nIa978SocketLocation;
	bool m_bIa978SocketLocation;
	int m_nIa978SocketLockStatus;
	bool m_bIa978SocketLockStatus;
	int m_nIa978SocketStatus;
	bool m_bIa978SocketStatus;
	int m_nIa978PocketLocation;
	bool m_bIa978PocketLocation;
	int m_nIa978PocketStatus;
	bool m_bIa978PocketStatus;
	int m_nIa978TrayLocation;
	bool m_bIa978TrayLocation;

	//IA978相关类对象及变量
	CBmDUTTrayLocation m_bmDutTrayLocating;
	CBmDUTSocketLocation m_bmDutSocketLocating;
	DUTOutPutParam m_parDutTrayLocating;

	CBmSocketLocatingCoarse m_bmSocketLocatingCoarse;
	standardPoint m_NorPoint;
	
	CBmSocketLocation m_bmSocketLocation;
	StructCircle m_circle1;
	StructCircle m_circle2;
	PointF m_socketCenter;
	double	m_socketAngle;

	CBmPocketLocation m_bmPocketLocation;
	PointF m_pocketCenter;
	PointF m_pocketArrow;
	double m_pocketAngle;
	CBmPocketStatus m_bmPocketStatus;
	int	m_nStatus;

	CBmTrayLocation m_bmTrayLocation;
	PointF m_trayCircleCenter;

	CBmSocketLockStatus m_bmSocketLockStatus;
	int	m_nLock;

	CBmSocketStatus m_bmSocketStatus;
	CBmDUTTrayRecognization m_bmDutTrayReco;


	//Active Alignment 菜单变量
	bool m_bAaChartArea;
	int	m_nAaChartArea;
	bool m_bAaRectangles;
	int	m_nAaRectangles;
	bool m_bAaSfrMtf;
	int m_nAaSfrMtf;
	bool m_bAaMtfStdDev;
	int m_nAaMtfStdDev;
	bool m_bAaMtfMaxMin;
	int m_nAaMtfMaxMin;
	int m_nAaMtfGradMax;
	bool m_bAaMtfGradMax;
	int m_nAaMtfGradAve;
	bool m_bAaMtfGradAve;

	int m_nAaLightDistribution;
	bool m_bAaLightDistribution;
	int m_nAaLightOpen;
	bool m_bAaLightOpen;
	int m_nAaLightClose;
	bool m_bAaLightClose;

	CAaAlgorithm m_bmAaAlgorithm;
	AaStructChartArea m_sChartArea;
	AaStructChartBlobs m_sChartBlobs;
	AaStructMTF m_sMTF;
	double m_pMTF[CHART_BLOB_NUM];
	

// 操作
public:


// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CBMAlgoDevView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void On_OpenImage();
	afx_msg void On_OpenFolder();
	afx_msg void On_Doing();
	afx_msg void On_Forward();
	afx_msg void On_Backward();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void On_DoFolder();
	afx_msg void OnFilterMean3();
	afx_msg void OnUpdateFilterMean3(CCmdUI *pCmdUI);
	afx_msg void OnFilterMean5();
	afx_msg void OnUpdateFilterMean5(CCmdUI *pCmdUI);
	afx_msg void OnFilterMean7();
	afx_msg void OnUpdateFilterMean7(CCmdUI *pCmdUI);
	afx_msg void OnFilterMean9();
	afx_msg void OnUpdateFilterMean9(CCmdUI *pCmdUI);
	afx_msg void OnFilterMean15();
	afx_msg void OnUpdateFilterMean15(CCmdUI *pCmdUI);
	afx_msg void OnFilterMedian3();
	afx_msg void OnUpdateFilterMedian3(CCmdUI *pCmdUI);
	afx_msg void OnFilterMedian5();
	afx_msg void OnUpdateFilterMedian5(CCmdUI *pCmdUI);
	afx_msg void OnFilterMedian7();
	afx_msg void OnUpdateFilterMedian7(CCmdUI *pCmdUI);
	afx_msg void OnFilterMedian9();
	afx_msg void OnUpdateFilterMedian9(CCmdUI *pCmdUI);
	afx_msg void OnFilterMedian15();
	afx_msg void OnUpdateFilterMedian15(CCmdUI *pCmdUI);
	afx_msg void OnFilterGauss3();
	afx_msg void OnUpdateFilterGauss3(CCmdUI *pCmdUI);
	afx_msg void OnFilterGauss5();
	afx_msg void OnUpdateFilterGauss5(CCmdUI *pCmdUI);
	afx_msg void OnFilterGauss7();
	afx_msg void OnUpdateFilterGauss7(CCmdUI *pCmdUI);
	afx_msg void OnFilterGauss9();
	afx_msg void OnUpdateFilterGauss9(CCmdUI *pCmdUI);
	afx_msg void OnFilterGauss15();
	afx_msg void OnUpdateFilterGauss15(CCmdUI *pCmdUI);
	afx_msg void OnIa978DutTrayLocation();
	afx_msg void OnUpdateIa978DutTrayLocation(CCmdUI *pCmdUI);
	afx_msg void OnIa978DutSocketLocation();
	afx_msg void OnUpdateIa978DutSocketLocation(CCmdUI *pCmdUI);
	afx_msg void OnIa978SocketLocation();
	afx_msg void OnUpdateIa978SocketLocation(CCmdUI *pCmdUI);
	afx_msg void OnIa978SocketLockStatus();
	afx_msg void OnUpdateIa978SocketLockStatus(CCmdUI *pCmdUI);
	afx_msg void OnIa978SocketStatus();
	afx_msg void OnUpdateIa978SocketStatus(CCmdUI *pCmdUI);
	afx_msg void OnIa978PocketLocation();
	afx_msg void OnUpdateIa978PocketLocation(CCmdUI *pCmdUI);
	afx_msg void OnIa978PocketStatus();
	afx_msg void OnUpdateIa978PocketStatus(CCmdUI *pCmdUI);
	afx_msg void OnIa978TrayLocation();
	afx_msg void OnUpdateIa978TrayLocation(CCmdUI *pCmdUI);
	afx_msg void OnIa978DutTrayLocationCoarse();
	afx_msg void OnUpdateIa978DutTrayLocationCoarse(CCmdUI *pCmdUI);
	afx_msg void OnIa978DutSocketLocationCoarse();
	afx_msg void OnUpdateIa978DutSocketLocationCoarse(CCmdUI *pCmdUI);
	afx_msg void OnIa978SocketLocationCoarse();
	afx_msg void OnUpdateIa978SocketLocationCoarse(CCmdUI *pCmdUI);
	afx_msg void OnFilterSobel3();
	afx_msg void OnUpdateFilterSobel3(CCmdUI *pCmdUI);
	afx_msg void OnFilterSobel5();
	afx_msg void OnUpdateFilterSobel5(CCmdUI *pCmdUI);
	afx_msg void OnFilterSobel7();
	afx_msg void OnUpdateFilterSobel7(CCmdUI *pCmdUI);
	afx_msg void OnPyramid2();
	afx_msg void OnUpdatePyramid2(CCmdUI *pCmdUI);
	afx_msg void OnPyramid3();
	afx_msg void OnUpdatePyramid3(CCmdUI *pCmdUI);
	afx_msg void OnPyramid1();
	afx_msg void OnUpdatePyramid1(CCmdUI *pCmdUI);
	afx_msg void OnSegmentMannual();
	afx_msg void OnUpdateSegmentMannual(CCmdUI *pCmdUI);
	afx_msg void OnSegmentOtsu();
	afx_msg void OnUpdateSegmentOtsu(CCmdUI *pCmdUI);
	afx_msg void OnSegmentDynamic();
	afx_msg void OnUpdateSegmentDynamic(CCmdUI *pCmdUI);
	afx_msg void OnSegmentCntDots();
	afx_msg void OnUpdateSegmentCntDots(CCmdUI *pCmdUI);
	afx_msg void OnMorphonogyErode();
	afx_msg void OnUpdateMorphonogyErode(CCmdUI *pCmdUI);
	afx_msg void OnMorphonogyDilation();
	afx_msg void OnUpdateMorphonogyDilation(CCmdUI *pCmdUI);
	afx_msg void OnMorphonogyOpen();
	afx_msg void OnUpdateMorphonogyOpen(CCmdUI *pCmdUI);
	afx_msg void OnMorphonogyClose();
	afx_msg void OnUpdateMorphonogyClose(CCmdUI *pCmdUI);
	afx_msg void OnRoiDraw();
	afx_msg void OnUpdateRoiDraw(CCmdUI *pCmdUI);
	afx_msg void OnRoiDelete();
	afx_msg void OnUpdateRoiDelete(CCmdUI *pCmdUI);
	afx_msg void OnAaAreas();
	afx_msg void OnUpdateAaAreas(CCmdUI *pCmdUI);
	afx_msg void OnAaRectangles();
	afx_msg void OnUpdateAaRectangles(CCmdUI *pCmdUI);
	afx_msg void OnAaSfrMtf();
	afx_msg void OnUpdateAaSfrMtf(CCmdUI *pCmdUI);
	afx_msg void OnAaMtfStdDev();
	afx_msg void OnUpdateAaMtfStdDev(CCmdUI *pCmdUI);
	afx_msg void OnAaMtfMaxMin();
	afx_msg void OnUpdateAaMtfMaxMin(CCmdUI *pCmdUI);
	afx_msg void OnAaMtfGradMax();
	afx_msg void OnUpdateAaMtfGradMax(CCmdUI *pCmdUI);
	afx_msg void OnAaMtfGradAve();
	afx_msg void OnUpdateAaMtfGradAve(CCmdUI *pCmdUI);
	afx_msg void OnAaAll();
	afx_msg void OnAaLightDistribution();
	afx_msg void OnUpdateAaLightDistribution(CCmdUI *pCmdUI);
	afx_msg void OnAaLightOpen();
	afx_msg void OnUpdateAaLightOpen(CCmdUI *pCmdUI);
	afx_msg void OnAaLightClose();
	afx_msg void OnUpdateAaLightClose(CCmdUI *pCmdUI);
	afx_msg void OnLightRangeIncrease();
	afx_msg void OnLightRangeDecrease();
};

#ifndef _DEBUG  // BMAlgoDevView.cpp 中的调试版本
inline CBMAlgoDevDoc* CBMAlgoDevView::GetDocument() const
   { return reinterpret_cast<CBMAlgoDevDoc*>(m_pDocument); }
#endif

