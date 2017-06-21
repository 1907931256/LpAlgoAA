
// BMAlgoDevView.cpp : CBMAlgoDevView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "BMAlgoDev.h"
#endif

#include "BMAlgoDevDoc.h"
#include "BMAlgoDevView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _CRTDBG_MAP_ALLOC
#include<crtdbg.h>
#include<stdlib.h>
//#ifdef _DEBUG
//#ifndef DBG_NEW
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ );
//#define new DBG_NEW
//#endif
//#endif


// CBMAlgoDevView

IMPLEMENT_DYNCREATE(CBMAlgoDevView, CScrollView)

BEGIN_MESSAGE_MAP(CBMAlgoDevView, CScrollView)
	ON_COMMAND(ID_FILE_OPEN_IMAGE, &CBMAlgoDevView::On_OpenImage)
	ON_COMMAND(ID_FILE_OPEN_FOLDER, &CBMAlgoDevView::On_OpenFolder)
	ON_COMMAND(ID_DOING, &CBMAlgoDevView::On_Doing)
	ON_COMMAND(ID_FORWARD, &CBMAlgoDevView::On_Forward)
	ON_COMMAND(ID_BACKWARD, &CBMAlgoDevView::On_Backward)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_DO_FOLDER, &CBMAlgoDevView::On_DoFolder)
	ON_COMMAND(ID_FILTER_MEAN3, &CBMAlgoDevView::OnFilterMean3)
	ON_UPDATE_COMMAND_UI(ID_FILTER_MEAN3, &CBMAlgoDevView::OnUpdateFilterMean3)
	ON_COMMAND(ID_FILTER_MEAN5, &CBMAlgoDevView::OnFilterMean5)
	ON_UPDATE_COMMAND_UI(ID_FILTER_MEAN5, &CBMAlgoDevView::OnUpdateFilterMean5)
	ON_COMMAND(ID_FILTER_MEAN7, &CBMAlgoDevView::OnFilterMean7)
	ON_UPDATE_COMMAND_UI(ID_FILTER_MEAN7, &CBMAlgoDevView::OnUpdateFilterMean7)
	ON_COMMAND(ID_FILTER_MEAN9, &CBMAlgoDevView::OnFilterMean9)
	ON_UPDATE_COMMAND_UI(ID_FILTER_MEAN9, &CBMAlgoDevView::OnUpdateFilterMean9)
	ON_COMMAND(ID_FILTER_MEAN15, &CBMAlgoDevView::OnFilterMean15)
	ON_UPDATE_COMMAND_UI(ID_FILTER_MEAN15, &CBMAlgoDevView::OnUpdateFilterMean15)
	ON_COMMAND(ID_FILTER_MEDIAN3, &CBMAlgoDevView::OnFilterMedian3)
	ON_UPDATE_COMMAND_UI(ID_FILTER_MEDIAN3, &CBMAlgoDevView::OnUpdateFilterMedian3)
	ON_COMMAND(ID_FILTER_MEDIAN5, &CBMAlgoDevView::OnFilterMedian5)
	ON_UPDATE_COMMAND_UI(ID_FILTER_MEDIAN5, &CBMAlgoDevView::OnUpdateFilterMedian5)
	ON_COMMAND(ID_FILTER_MEDIAN7, &CBMAlgoDevView::OnFilterMedian7)
	ON_UPDATE_COMMAND_UI(ID_FILTER_MEDIAN7, &CBMAlgoDevView::OnUpdateFilterMedian7)
	ON_COMMAND(ID_FILTER_MEDIAN9, &CBMAlgoDevView::OnFilterMedian9)
	ON_UPDATE_COMMAND_UI(ID_FILTER_MEDIAN9, &CBMAlgoDevView::OnUpdateFilterMedian9)
	ON_COMMAND(ID_FILTER_MEDIAN15, &CBMAlgoDevView::OnFilterMedian15)
	ON_UPDATE_COMMAND_UI(ID_FILTER_MEDIAN15, &CBMAlgoDevView::OnUpdateFilterMedian15)
	ON_COMMAND(ID_FILTER_GAUSS3, &CBMAlgoDevView::OnFilterGauss3)
	ON_UPDATE_COMMAND_UI(ID_FILTER_GAUSS3, &CBMAlgoDevView::OnUpdateFilterGauss3)
	ON_COMMAND(ID_FILTER_GAUSS5, &CBMAlgoDevView::OnFilterGauss5)
	ON_UPDATE_COMMAND_UI(ID_FILTER_GAUSS5, &CBMAlgoDevView::OnUpdateFilterGauss5)
	ON_COMMAND(ID_FILTER_GAUSS7, &CBMAlgoDevView::OnFilterGauss7)
	ON_UPDATE_COMMAND_UI(ID_FILTER_GAUSS7, &CBMAlgoDevView::OnUpdateFilterGauss7)
	ON_COMMAND(ID_FILTER_GAUSS9, &CBMAlgoDevView::OnFilterGauss9)
	ON_UPDATE_COMMAND_UI(ID_FILTER_GAUSS9, &CBMAlgoDevView::OnUpdateFilterGauss9)
	ON_COMMAND(ID_FILTER_GAUSS15, &CBMAlgoDevView::OnFilterGauss15)
	ON_UPDATE_COMMAND_UI(ID_FILTER_GAUSS15, &CBMAlgoDevView::OnUpdateFilterGauss15)
	ON_COMMAND(ID_IA978_DUT_TRAY_LOCATION, &CBMAlgoDevView::OnIa978DutTrayLocation)
	ON_UPDATE_COMMAND_UI(ID_IA978_DUT_TRAY_LOCATION, &CBMAlgoDevView::OnUpdateIa978DutTrayLocation)
	ON_COMMAND(ID_IA978_DUT_SOCKET_LOCATION, &CBMAlgoDevView::OnIa978DutSocketLocation)
	ON_UPDATE_COMMAND_UI(ID_IA978_DUT_SOCKET_LOCATION, &CBMAlgoDevView::OnUpdateIa978DutSocketLocation)
	ON_COMMAND(ID_IA978_SOCKET_LOCATION, &CBMAlgoDevView::OnIa978SocketLocation)
	ON_UPDATE_COMMAND_UI(ID_IA978_SOCKET_LOCATION, &CBMAlgoDevView::OnUpdateIa978SocketLocation)
	ON_COMMAND(ID_IA978_SOCKET_LOCK_STATUS, &CBMAlgoDevView::OnIa978SocketLockStatus)
	ON_UPDATE_COMMAND_UI(ID_IA978_SOCKET_LOCK_STATUS, &CBMAlgoDevView::OnUpdateIa978SocketLockStatus)
	ON_COMMAND(ID_IA978_SOCKET_STATUS, &CBMAlgoDevView::OnIa978SocketStatus)
	ON_UPDATE_COMMAND_UI(ID_IA978_SOCKET_STATUS, &CBMAlgoDevView::OnUpdateIa978SocketStatus)
	ON_COMMAND(ID_IA978_POCKET_LOCATION, &CBMAlgoDevView::OnIa978PocketLocation)
	ON_UPDATE_COMMAND_UI(ID_IA978_POCKET_LOCATION, &CBMAlgoDevView::OnUpdateIa978PocketLocation)
	ON_COMMAND(ID_IA978_POCKET_STATUS, &CBMAlgoDevView::OnIa978PocketStatus)
	ON_UPDATE_COMMAND_UI(ID_IA978_POCKET_STATUS, &CBMAlgoDevView::OnUpdateIa978PocketStatus)
	ON_COMMAND(ID_IA978_TRAY_LOCATION, &CBMAlgoDevView::OnIa978TrayLocation)
	ON_UPDATE_COMMAND_UI(ID_IA978_TRAY_LOCATION, &CBMAlgoDevView::OnUpdateIa978TrayLocation)
	ON_COMMAND(ID_IA978_DUT_TRAY_LOCATION_COARSE, &CBMAlgoDevView::OnIa978DutTrayLocationCoarse)
	ON_UPDATE_COMMAND_UI(ID_IA978_DUT_TRAY_LOCATION_COARSE, &CBMAlgoDevView::OnUpdateIa978DutTrayLocationCoarse)
	ON_COMMAND(ID_IA978_DUT_SOCKET_LOCATION_COARSE, &CBMAlgoDevView::OnIa978DutSocketLocationCoarse)
	ON_UPDATE_COMMAND_UI(ID_IA978_DUT_SOCKET_LOCATION_COARSE, &CBMAlgoDevView::OnUpdateIa978DutSocketLocationCoarse)
	ON_COMMAND(ID_IA978_SOCKET_LOCATION_COARSE, &CBMAlgoDevView::OnIa978SocketLocationCoarse)
	ON_UPDATE_COMMAND_UI(ID_IA978_SOCKET_LOCATION_COARSE, &CBMAlgoDevView::OnUpdateIa978SocketLocationCoarse)
	ON_COMMAND(ID_FILTER_SOBEL3, &CBMAlgoDevView::OnFilterSobel3)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SOBEL3, &CBMAlgoDevView::OnUpdateFilterSobel3)
	ON_COMMAND(ID_FILTER_SOBEL5, &CBMAlgoDevView::OnFilterSobel5)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SOBEL5, &CBMAlgoDevView::OnUpdateFilterSobel5)
	ON_COMMAND(ID_FILTER_SOBEL7, &CBMAlgoDevView::OnFilterSobel7)
	ON_UPDATE_COMMAND_UI(ID_FILTER_SOBEL7, &CBMAlgoDevView::OnUpdateFilterSobel7)
	ON_COMMAND(ID_PYRAMID_2, &CBMAlgoDevView::OnPyramid2)
	ON_UPDATE_COMMAND_UI(ID_PYRAMID_2, &CBMAlgoDevView::OnUpdatePyramid2)
	ON_COMMAND(ID_PYRAMID_3, &CBMAlgoDevView::OnPyramid3)
	ON_UPDATE_COMMAND_UI(ID_PYRAMID_3, &CBMAlgoDevView::OnUpdatePyramid3)
	ON_COMMAND(ID_PYRAMID_1, &CBMAlgoDevView::OnPyramid1)
	ON_UPDATE_COMMAND_UI(ID_PYRAMID_1, &CBMAlgoDevView::OnUpdatePyramid1)
	ON_COMMAND(ID_SEGMENT_MANNUAL, &CBMAlgoDevView::OnSegmentMannual)
	ON_UPDATE_COMMAND_UI(ID_SEGMENT_MANNUAL, &CBMAlgoDevView::OnUpdateSegmentMannual)
	ON_COMMAND(ID_SEGMENT_OTSU, &CBMAlgoDevView::OnSegmentOtsu)
	ON_UPDATE_COMMAND_UI(ID_SEGMENT_OTSU, &CBMAlgoDevView::OnUpdateSegmentOtsu)
	ON_COMMAND(ID_SEGMENT_DYNAMIC, &CBMAlgoDevView::OnSegmentDynamic)
	ON_UPDATE_COMMAND_UI(ID_SEGMENT_DYNAMIC, &CBMAlgoDevView::OnUpdateSegmentDynamic)
	ON_COMMAND(ID_SEGMENT_CNT_DOTS, &CBMAlgoDevView::OnSegmentCntDots)
	ON_UPDATE_COMMAND_UI(ID_SEGMENT_CNT_DOTS, &CBMAlgoDevView::OnUpdateSegmentCntDots)
	ON_COMMAND(ID_MORPHONOGY_ERODE, &CBMAlgoDevView::OnMorphonogyErode)
	ON_UPDATE_COMMAND_UI(ID_MORPHONOGY_ERODE, &CBMAlgoDevView::OnUpdateMorphonogyErode)
	ON_COMMAND(ID_MORPHONOGY_DILATION, &CBMAlgoDevView::OnMorphonogyDilation)
	ON_UPDATE_COMMAND_UI(ID_MORPHONOGY_DILATION, &CBMAlgoDevView::OnUpdateMorphonogyDilation)
	ON_COMMAND(ID_MORPHONOGY_OPEN, &CBMAlgoDevView::OnMorphonogyOpen)
	ON_UPDATE_COMMAND_UI(ID_MORPHONOGY_OPEN, &CBMAlgoDevView::OnUpdateMorphonogyOpen)
	ON_COMMAND(ID_MORPHONOGY_CLOSE, &CBMAlgoDevView::OnMorphonogyClose)
	ON_UPDATE_COMMAND_UI(ID_MORPHONOGY_CLOSE, &CBMAlgoDevView::OnUpdateMorphonogyClose)
	ON_COMMAND(ID_ROI_DRAW, &CBMAlgoDevView::OnRoiDraw)
	ON_UPDATE_COMMAND_UI(ID_ROI_DRAW, &CBMAlgoDevView::OnUpdateRoiDraw)
	ON_COMMAND(ID_ROI_DELETE, &CBMAlgoDevView::OnRoiDelete)
	ON_UPDATE_COMMAND_UI(ID_ROI_DELETE, &CBMAlgoDevView::OnUpdateRoiDelete)
	ON_COMMAND(ID_AA_AREAS, &CBMAlgoDevView::OnAaAreas)
	ON_UPDATE_COMMAND_UI(ID_AA_AREAS, &CBMAlgoDevView::OnUpdateAaAreas)
	ON_COMMAND(ID_AA_RECTANGLES, &CBMAlgoDevView::OnAaRectangles)
	ON_UPDATE_COMMAND_UI(ID_AA_RECTANGLES, &CBMAlgoDevView::OnUpdateAaRectangles)
	ON_COMMAND(ID_AA_SFR_MTF, &CBMAlgoDevView::OnAaSfrMtf)
	ON_UPDATE_COMMAND_UI(ID_AA_SFR_MTF, &CBMAlgoDevView::OnUpdateAaSfrMtf)
	ON_COMMAND(ID_AA_MTF_Std_Dev, &CBMAlgoDevView::OnAaMtfStdDev)
		ON_UPDATE_COMMAND_UI(ID_AA_MTF_Std_Dev, &CBMAlgoDevView::OnUpdateAaMtfStdDev)
		ON_COMMAND(ID_AA_MTF_Max_Min, &CBMAlgoDevView::OnAaMtfMaxMin)
		ON_UPDATE_COMMAND_UI(ID_AA_MTF_Max_Min, &CBMAlgoDevView::OnUpdateAaMtfMaxMin)
		ON_COMMAND(ID_AA_MTF_GRAD_MAX, &CBMAlgoDevView::OnAaMtfGradMax)
		ON_UPDATE_COMMAND_UI(ID_AA_MTF_GRAD_MAX, &CBMAlgoDevView::OnUpdateAaMtfGradMax)
		ON_COMMAND(ID_AA_MTF_GRAD_AVE, &CBMAlgoDevView::OnAaMtfGradAve)
		ON_UPDATE_COMMAND_UI(ID_AA_MTF_GRAD_AVE, &CBMAlgoDevView::OnUpdateAaMtfGradAve)
		ON_COMMAND(ID_AA_ALL, &CBMAlgoDevView::OnAaAll)
		ON_COMMAND(ID_AA_LIGHT_DISTRIBUTION, &CBMAlgoDevView::OnAaLightDistribution)
		ON_UPDATE_COMMAND_UI(ID_AA_LIGHT_DISTRIBUTION, &CBMAlgoDevView::OnUpdateAaLightDistribution)
		ON_COMMAND(ID_AA_LIGHT_OPEN, &CBMAlgoDevView::OnAaLightOpen)
		ON_UPDATE_COMMAND_UI(ID_AA_LIGHT_OPEN, &CBMAlgoDevView::OnUpdateAaLightOpen)
		ON_COMMAND(ID_AA_LIGHT_CLOSE, &CBMAlgoDevView::OnAaLightClose)
		ON_UPDATE_COMMAND_UI(ID_AA_LIGHT_CLOSE, &CBMAlgoDevView::OnUpdateAaLightClose)
		ON_COMMAND(ID_LIGHT_RANGE_Increase, &CBMAlgoDevView::OnLightRangeIncrease)
		ON_COMMAND(ID_LIGHT_RANGE_Decrease, &CBMAlgoDevView::OnLightRangeDecrease)
		END_MESSAGE_MAP()

// CBMAlgoDevView 构造/析构

CBMAlgoDevView::CBMAlgoDevView()
{
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	m_pMyGDIPlus = NULL;
	m_pMyGDIPlus = new CMyGdiPlus;
	m_imageNum = 0;
	m_imagePos = 0;
	m_showPoint.x = 0;
	m_showPoint.y = 0;
	m_bShowMovePos = false;

	m_roiRect.topLeft = { 0,0 };
	m_roiRect.downRight = { 0,0 };

	for (int n = 0; n < Image_Num_Max; n++)
	{
		m_ImageVolume[n].pData = NULL;
		m_ImageVolume[n].width = 0;
		m_ImageVolume[n].height = 0;
	}
	m_pBmpData.pData = NULL;
	m_pBmpData.width = 0;
	m_pBmpData.height = 0;

	m_src.ptr = NULL;
	m_src.size = { 0,0 };
	m_dst.ptr = NULL;
	m_dst.size = { 0,0 };

	m_sMTF.nMTFzpos = 0;

	m_bAllFolder = false;
	m_nProgressTotal = m_nProgressCurrent = 0;
	m_bDoFolder = false;
	m_fTimer = 0.0f;

	//Pyramid
	m_nPrePyramid1 = -1;
	m_bPrePyramid1 = false;
	m_nPrePyramid2 = -1;
	m_bPrePyramid2 = false;
	m_nPrePyramid3 = -1;
	m_bPrePyramid3 = false;

	//ROI
	m_nRoiDraw = -1;
	m_bRoiDraw = false;
	m_nRoiDelete = -1;
	m_bRoiDelete = false;

	//Filter menu initial
	//Mean
	m_nPreFilterMean3 = -1;
	m_bPreFilterMean3 = false;
	m_nPreFilterMean5 = -1;
	m_bPreFilterMean5 = false;
	m_nPreFilterMean7 = -1;
	m_bPreFilterMean7 = false;
	m_nPreFilterMean9 = -1;
	m_bPreFilterMean9 = false;
	m_nPreFilterMean15 = -1;
	m_bPreFilterMean15 = false;
	//Median
	m_nPreFilterMedian3 = -1;
	m_bPreFilterMedian3 = false;
	m_nPreFilterMedian5 = -1;
	m_bPreFilterMedian5 = false;
	m_nPreFilterMedian7 = -1;
	m_bPreFilterMedian7 = false;
	m_nPreFilterMedian9 = -1;
	m_bPreFilterMedian9 = false;
	m_nPreFilterMedian15 = -1;
	m_bPreFilterMedian15 = false;
	//Gauss
	m_nPreFilterGauss3 = -1;
	m_bPreFilterGauss3 = false;
	m_nPreFilterGauss5 = -1;
	m_bPreFilterGauss5 = false;
	m_nPreFilterGauss7 = -1;
	m_bPreFilterGauss7 = false;
	m_nPreFilterGauss9 = -1;
	m_bPreFilterGauss9 = false;
	m_nPreFilterGauss15 = -1;
	m_bPreFilterGauss15 = false;
	//Sobel
	m_nPreFilterSobel3 = -1;
	m_bPreFilterSobel3 = false;
	m_nPreFilterSobel5 = -1;
	m_bPreFilterSobel5 = false;
	m_nPreFilterSobel7 = -1;
	m_bPreFilterSobel7 = false;

	//Segment
	m_nSegmentMannual = -1;
	m_bSegmentMannual = false;
	m_nSegmentOtsu = -1;
	m_bSegmentOtsu = false;
	m_nSegmentDynamic = -1;
	m_bSegmentDynamic = false;
	m_nSegmentCntDots = -1;
	m_bSegmentCntDots = false;

	//Morphology
	m_nMorphonogyErode = -1;
	m_bMorphonogyErode = false;
	m_nMorphonogyClose = -1;
	m_bMorphonogyClose = false;
	m_nMorphonogyDilation = -1;
	m_bMorphonogyDilation = false;
	m_nMorphonogyOpen = -1;
	m_bMorphonogyOpen = false;

	// IA978 MENU
	m_nIa978DutTrayLocationCoarse = -1;
	m_bIa978DutTrayLocationCoarse = false;
	m_nIa978DutTrayLocation = -1;
	m_bIa978DutTrayLocation = false;
	m_nIa978DutSocketLocationCoarse = -1;
	m_bIa978DutSocketLocationCoarse = false;
	m_nIa978DutSocketLocation = -1;
	m_bIa978DutSocketLocation = false;
	m_nIa978SocketLocationCoarse = -1;
	m_bIa978SocketLocationCoarse = false;
	m_nIa978SocketLocation = -1;
	m_bIa978SocketLocation = false;
	m_nIa978SocketLockStatus = -1;
	m_bIa978SocketLockStatus = false;
	m_nIa978SocketStatus = -1;
	m_bIa978SocketStatus = false;
	m_nIa978PocketLocation = -1;
	m_bIa978PocketLocation = false;
	m_nIa978PocketStatus = -1;
	m_bIa978PocketStatus = false;
	m_nIa978TrayLocation = -1;
	m_bIa978TrayLocation = false;

	// AA menu
	m_bAaChartArea = false;
	m_nAaChartArea = -1;
	m_bAaRectangles = false;
	m_nAaRectangles = -1;
	m_bAaSfrMtf = false;
	m_nAaSfrMtf = -1;
	m_bAaMtfStdDev = false;
	m_nAaMtfStdDev = -1;
	m_bAaMtfMaxMin = false;
	m_nAaMtfMaxMin = -1;
	m_nAaMtfGradMax = -1;
	m_bAaMtfGradMax = false;
	m_nAaMtfGradAve = -1;
	m_bAaMtfGradAve = false;
	m_nAaLightDistribution = -1;
	m_bAaLightDistribution = false;
	m_nAaLightOpen = -1;
	m_bAaLightOpen = false;
	m_nAaLightClose = -1;
	m_bAaLightClose = false;
}

CBMAlgoDevView::~CBMAlgoDevView()
{
	if (m_pMyGDIPlus)
		delete m_pMyGDIPlus;
	for (int n = 0; n < Image_Num_Max; n++)
	{
		if(m_ImageVolume[n].pData)
			delete[](m_ImageVolume[n].pData);
	}
	if (m_pBmpData.pData)
		delete[]m_pBmpData.pData;
	//if (m_src.ptr)
	//	delete[]m_src.ptr;
	if (m_dst.ptr)
		delete[]m_dst.ptr;

	//_CrtDumpMemoryLeaks();
}

BOOL CBMAlgoDevView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
}

// CBMAlgoDevView 绘制

void CBMAlgoDevView::OnDraw(CDC* pDC)
{
	CBMAlgoDevDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码

	if (m_bAllFolder)
	{
		return;
	}

	if (!m_pBmpData.pData)
		return;

	m_pMyGDIPlus->InitGdiPlus(pDC->GetSafeHdc());
	if (!m_pMyGDIPlus)
		return;
	if (!(m_pMyGDIPlus->m_pGraphics))
		return;

	// show image
	Point p(0, 0);
	//if (m_bPrePyramid1 || m_bPrePyramid2 || m_bPrePyramid3)
		m_pMyGDIPlus->DrawBmp(m_src.ptr, p, m_src.size.width, m_src.size.height);
	//else
	//	m_pMyGDIPlus->DrawBmp(m_pBmpData.pData, p, m_pBmpData.width, m_pBmpData.height);

	Color cR(255, 255, 0, 0);
	Pen pR(cR, 1);
	Color cG(255, 0, 255, 0);
	Pen pG(cG, 1);
	Color cB(255, 0, 0, 255);
	Pen pB(cB, 1);

	// show click point
	if (m_showPoint.x > 0 && m_showPoint.y > 0)
	{
		m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pG, m_showPoint.x - 3, m_showPoint.y - 3, 7, 7);
	}

	// show ROI
	if (m_bRoiDraw && m_roiRect.downRight.x > 0)
	{
		m_pMyGDIPlus->m_pGraphics->DrawRectangle(&pG, m_roiRect.topLeft.x, m_roiRect.topLeft.y, m_roiRect.downRight.x - m_roiRect.topLeft.x, m_roiRect.downRight.y - m_roiRect.topLeft.y);
	}

	// Show IA978 results

	// show dut location
	if ((m_bIa978DutTrayLocation || m_bIa978DutSocketLocation) && m_parDutTrayLocating.cx > 0 && m_parDutTrayLocating.cy > 0)
	{
		m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pG, INT(m_parDutTrayLocating.cx + 0.5) - 3, INT(m_parDutTrayLocating.cy + 0.5) - 3, 7, 7);
		m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pR, INT(m_parDutTrayLocating.x1 + 0.5) - 3, INT(m_parDutTrayLocating.y1 + 0.5) - 3, 7, 7);
		m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pR, INT(m_parDutTrayLocating.x2 + 0.5) - 3, INT(m_parDutTrayLocating.y2 + 0.5) - 3, 7, 7);
		m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pR, INT(m_parDutTrayLocating.x3 + 0.5) - 3, INT(m_parDutTrayLocating.y3 + 0.5) - 3, 7, 7);
		m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pR, INT(m_parDutTrayLocating.x4 + 0.5) - 3, INT(m_parDutTrayLocating.y4 + 0.5) - 3, 7, 7);
	}
	// show socket location coarse
	if (m_bIa978SocketLocationCoarse && m_NorPoint.leftPoint.max_x > 0)
	{
		m_pMyGDIPlus->m_pGraphics->DrawRectangle(&pR, m_NorPoint.leftPoint.min_x, m_NorPoint.leftPoint.min_y, m_NorPoint.leftPoint.max_x - m_NorPoint.leftPoint.min_x, m_NorPoint.leftPoint.max_y - m_NorPoint.leftPoint.min_y);
		m_pMyGDIPlus->m_pGraphics->DrawRectangle(&pR, m_NorPoint.rightPoint.min_x, m_NorPoint.rightPoint.min_y, m_NorPoint.rightPoint.max_x - m_NorPoint.rightPoint.min_x, m_NorPoint.rightPoint.max_y - m_NorPoint.rightPoint.min_y);
	}
	// show socket location fine
	if (m_bIa978SocketLocation)
	{
		/*if(m_circle1.CirCen.x > 0 && m_circle1.CirCen.y > 0)
			m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pR, REAL(m_circle1.CirCen.x- m_circle1.Radius + 0.5) , REAL(m_circle1.CirCen.y - m_circle1.Radius + 0.5), m_circle1.Radius * 2, m_circle1.Radius * 2);
		if (m_circle1.CirCen.x > 0 && m_circle1.CirCen.y > 0)
			m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pR, REAL(m_circle2.CirCen.x - m_circle2.Radius + 0.5), REAL(m_circle2.CirCen.y - m_circle2.Radius + 0.5), m_circle2.Radius * 2, m_circle2.Radius * 2);*/
		if (m_socketCenter.X > 0 && m_socketCenter.Y > 0)
		{
			m_pMyGDIPlus->m_pGraphics->DrawLine(&pR, m_socketCenter, m_pocketArrow);
			m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pG, INT(m_socketCenter.X + 0.5) - 3, INT(m_socketCenter.Y + 0.5) - 3, 7, 7);
		}
	}
	// show pocket location
	if (m_bIa978PocketLocation)
	{
		if (m_pocketCenter.X > 0 && m_pocketCenter.Y > 0)
		{
			m_pMyGDIPlus->m_pGraphics->DrawLine(&pR, m_pocketCenter, m_pocketArrow);
			m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pG, INT(m_pocketCenter.X + 0.5) - 3, INT(m_pocketCenter.Y + 0.5) - 3, 7, 7);
		}
	}
	// show tray circle center
	if (m_bIa978TrayLocation)
	{
		if (m_trayCircleCenter.X > 0 && m_trayCircleCenter.Y > 0)
		{
			m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pG, INT(m_trayCircleCenter.X + 0.5) - 3, INT(m_trayCircleCenter.Y + 0.5) - 3, 7, 7);
		}
	}

	/******************  show AA *************************/
	if (m_bAaChartArea)
	{
		m_pMyGDIPlus->m_pGraphics->DrawLine(&pR, m_sChartArea.TopLeft.x, m_sChartArea.TopLeft.y, m_sChartArea.TopRight.x, m_sChartArea.TopRight.y);
		m_pMyGDIPlus->m_pGraphics->DrawLine(&pR, m_sChartArea.TopRight.x, m_sChartArea.TopRight.y, m_sChartArea.BottomRight.x, m_sChartArea.BottomRight.y);
		m_pMyGDIPlus->m_pGraphics->DrawLine(&pR, m_sChartArea.BottomRight.x, m_sChartArea.BottomRight.y, m_sChartArea.BottomLeft.x, m_sChartArea.BottomLeft.y);
		m_pMyGDIPlus->m_pGraphics->DrawLine(&pR, m_sChartArea.BottomLeft.x, m_sChartArea.BottomLeft.y, m_sChartArea.TopLeft.x, m_sChartArea.TopLeft.y);
		if (m_bAaRectangles)
		{
			//if (!m_bAaSfrMtf && !m_bAaMtfStdDev && !m_bAaMtfMaxMin)
			{
				for (int n = 0; n < CHART_BLOB_NUM; n++)
				{
					m_pMyGDIPlus->m_pGraphics->DrawEllipse(&pG, m_sChartBlobs.Centers[n].x - 3, m_sChartBlobs.Centers[n].y - 3, 7, 7);
					//m_pMyGDIPlus->m_pGraphics->DrawRectangle(&pG, m_sChartBlobs.Blobs[n].TopLeft.x, m_sChartBlobs.Blobs[n].TopLeft.y, m_sChartBlobs.Blobs[n].BottomRight.x - m_sChartBlobs.Blobs[n].TopLeft.x, m_sChartBlobs.Blobs[n].BottomRight.y - m_sChartBlobs.Blobs[n].TopLeft.y);
					if (m_sChartBlobs.EdgeLeft[n][0].x > 0 || m_sChartBlobs.EdgeLeft[n][1].x > 0)
						m_pMyGDIPlus->m_pGraphics->DrawLine(&pR, m_sChartBlobs.EdgeLeft[n][0].x, m_sChartBlobs.EdgeLeft[n][0].y, m_sChartBlobs.EdgeLeft[n][1].x, m_sChartBlobs.EdgeLeft[n][1].y);
					if (m_sChartBlobs.EdgeRight[n][0].x > 0 || m_sChartBlobs.EdgeRight[n][1].x > 0)
						m_pMyGDIPlus->m_pGraphics->DrawLine(&pR, m_sChartBlobs.EdgeRight[n][0].x, m_sChartBlobs.EdgeRight[n][0].y, m_sChartBlobs.EdgeRight[n][1].x, m_sChartBlobs.EdgeRight[n][1].y);
					if (m_sChartBlobs.EdgeUp[n][0].x > 0 || m_sChartBlobs.EdgeUp[n][1].x > 0)
						m_pMyGDIPlus->m_pGraphics->DrawLine(&pR, m_sChartBlobs.EdgeUp[n][0].x, m_sChartBlobs.EdgeUp[n][0].y, m_sChartBlobs.EdgeUp[n][1].x, m_sChartBlobs.EdgeUp[n][1].y);
					if (m_sChartBlobs.EdgeDown[n][0].x > 0 || m_sChartBlobs.EdgeDown[n][1].x > 0)
						m_pMyGDIPlus->m_pGraphics->DrawLine(&pR, m_sChartBlobs.EdgeDown[n][0].x, m_sChartBlobs.EdgeDown[n][0].y, m_sChartBlobs.EdgeDown[n][1].x, m_sChartBlobs.EdgeDown[n][1].y);
				}
			}
			if (m_bAaSfrMtf || m_bAaMtfStdDev || m_bAaMtfMaxMin || m_bAaMtfGradMax || m_bAaMtfGradAve)
			{
				CPoint pnt;
				pnt = { 150, 550 };
				int x, y;
				x = pnt.x;
				y = pnt.y - 100;
				for (int n = 0; n < CHART_BLOB_NUM; n++)
				{
					if (0 == n % 3)
					{
						x = pnt.x;
						y += 70;
					}
					CString str;
					str.Format(_T("%0.4f"), m_pMTF[n]);
					pDC->TextOutW(x, y, str);
					x += 150;
				}
			}
		}
	}
	if (m_sMTF.nMTFzpos > 0)
	{
		Color c1(255, 255, 0, 0);
		Pen p1(c1, 1);
		Color c2(255, 128, 0, 128);
		Pen p2(c2, 1);
		Color c3(255, 0, 128, 128);
		Pen p3(c3, 1);
		Color c4(255, 0, 255, 0);
		Pen p4(c4, 1);
		Color c5(255, 0, 255, 255);
		Pen p5(c5, 1);
		Color c6(255, 255, 0, 255);
		Pen p6(c6, 1);
		Color c7(255, 0, 0, 255);
		Pen p7(c7, 1);
		Color c8(255, 255, 200, 0);
		Pen p8(c8, 1);
		Color c9(255, 0, 0, 0);
		Pen p9(c9, 1);
		Point pnts1[MTF_MAX_Z_POS];
		Point pnts2[MTF_MAX_Z_POS];
		Point pnts3[MTF_MAX_Z_POS];
		Point pnts4[MTF_MAX_Z_POS];
		Point pnts5[MTF_MAX_Z_POS];
		Point pnts6[MTF_MAX_Z_POS];
		Point pnts7[MTF_MAX_Z_POS];
		Point pnts8[MTF_MAX_Z_POS];
		Point pnts9[MTF_MAX_Z_POS];
		int x = 700, y = 700, d = 40;
		for (int n = 0; n < m_sMTF.nMTFzpos; n++)
		{
			x += d;
			pnts1[n].X = x;
			pnts1[n].Y = y - INT(m_sMTF.ppMTF[n][0] * y + 0.5);
			pnts2[n].X = x ;
			pnts2[n].Y = y - INT(m_sMTF.ppMTF[n][1] * y + 0.5);
			pnts3[n].X = x;
			pnts3[n].Y = y - INT(m_sMTF.ppMTF[n][2] * y + 0.5);
			pnts4[n].X = x;
			pnts4[n].Y = y - INT(m_sMTF.ppMTF[n][3] * y + 0.5);
			pnts5[n].X = x;
			pnts5[n].Y = y - INT(m_sMTF.ppMTF[n][4] * y + 0.5);
			pnts6[n].X = x;
			pnts6[n].Y = y - INT(m_sMTF.ppMTF[n][5] * y + 0.5);
			pnts7[n].X = x;
			pnts7[n].Y = y - INT(m_sMTF.ppMTF[n][6] * y + 0.5);
			pnts8[n].X = x;
			pnts8[n].Y = y - INT(m_sMTF.ppMTF[n][7] * y + 0.5);
			pnts9[n].X = x;
			pnts9[n].Y = y - INT(m_sMTF.ppMTF[n][8] * y + 0.5);
		}
		// draw mtf curves
		m_pMyGDIPlus->m_pGraphics->DrawLines(&p1, pnts1, m_sMTF.nMTFzpos);
		m_pMyGDIPlus->m_pGraphics->DrawLines(&p2, pnts2, m_sMTF.nMTFzpos);
		m_pMyGDIPlus->m_pGraphics->DrawLines(&p3, pnts3, m_sMTF.nMTFzpos);
		m_pMyGDIPlus->m_pGraphics->DrawLines(&p4, pnts4, m_sMTF.nMTFzpos);
		m_pMyGDIPlus->m_pGraphics->DrawLines(&p5, pnts5, m_sMTF.nMTFzpos);
		m_pMyGDIPlus->m_pGraphics->DrawLines(&p6, pnts6, m_sMTF.nMTFzpos);
		m_pMyGDIPlus->m_pGraphics->DrawLines(&p7, pnts7, m_sMTF.nMTFzpos);
		m_pMyGDIPlus->m_pGraphics->DrawLines(&p8, pnts8, m_sMTF.nMTFzpos);
		m_pMyGDIPlus->m_pGraphics->DrawLines(&p9, pnts9, m_sMTF.nMTFzpos);
		// draw demo lines
		m_pMyGDIPlus->m_pGraphics->DrawLine(&p1, 700, 10, 720, 10);
		m_pMyGDIPlus->m_pGraphics->DrawLine(&p2, 730, 10, 750, 10);
		m_pMyGDIPlus->m_pGraphics->DrawLine(&p3, 760, 10, 780, 10);
		m_pMyGDIPlus->m_pGraphics->DrawLine(&p4, 700, 20, 720, 20);
		m_pMyGDIPlus->m_pGraphics->DrawLine(&p5, 730, 20, 750, 20);
		m_pMyGDIPlus->m_pGraphics->DrawLine(&p6, 760, 20, 780, 20);
		m_pMyGDIPlus->m_pGraphics->DrawLine(&p7, 700, 30, 720, 30);
		m_pMyGDIPlus->m_pGraphics->DrawLine(&p8, 730, 30, 750, 30);
		m_pMyGDIPlus->m_pGraphics->DrawLine(&p9, 760, 30, 780, 30);
	}
}

void CBMAlgoDevView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	//CSize sizeTotal;
	//// TODO: 计算此视图的合计大小
	//sizeTotal.cx = sizeTotal.cy = 100;
	//SetScrollSizes(MM_TEXT, sizeTotal);

	SIZE size = { 3000,1500 };
	SetScrollSizes(MM_TEXT, size);//滚动窗口的最大区域/////////////////////////////////////
}


// CBMAlgoDevView 诊断

#ifdef _DEBUG
void CBMAlgoDevView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CBMAlgoDevView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CBMAlgoDevDoc* CBMAlgoDevView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBMAlgoDevDoc)));
	return (CBMAlgoDevDoc*)m_pDocument;
}
#endif //_DEBUG


// CBMAlgoDevView 消息处理程序


void CBMAlgoDevView::On_OpenImage()
{
	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Image Files(*.BMP;*.JPG)|*.BMP;*.JPG|BMP Files(*.BMP)|*.BMP|Jpeg Files(*.JPG)|*.JPG|All Files (*.*)|*.*||"));
	if (OpenFileDlg.DoModal() == IDCANCEL)
		return;

	CString szFilePath = OpenFileDlg.GetPathName();

	m_imagePos = 0;
	m_imageNum = 1;
	m_ImageNameList[0] = szFilePath;

	AfxGetMainWnd()->SetWindowText(szFilePath);

	BYTE* p1;
	WCHAR * wc = szFilePath.AllocSysString();
	CSize size = m_pMyGDIPlus->CreateImageToBuff(wc, &p1);

	int nS = size.cx * size.cy;
	m_ImageVolume[0].width = size.cx;
	m_ImageVolume[0].height = size.cy;
	if (m_ImageVolume[0].pData)
		delete[]m_ImageVolume[0].pData;
	m_ImageVolume[0].pData = new BYTE[nS];
	memcpy(m_ImageVolume[0].pData, p1, nS);

	m_pBmpData.width = size.cx;
	m_pBmpData.height = size.cy;
	if (m_pBmpData.pData)
		delete[]m_pBmpData.pData;
	m_pBmpData.pData = new BYTE[nS];

	SetScrollSizes(MM_TEXT, size);//滚动窗口的最大区域/////////////////////////////////////

	m_imagePos -= 1;

	//Clear:
	m_showPoint.x = 0;
	m_showPoint.y = 0;

	m_bMouseDown = false;

	On_Forward();

	Invalidate(FALSE);

	//_CrtDumpMemoryLeaks();
}


void CBMAlgoDevView::On_OpenFolder()
{
	m_lstFileList.RemoveAll();
	m_posFileList = NULL;

	CFileDialog OpenFileDlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Image Files(*.BMP;*.JPG)|*.BMP;*.JPG|BMP Files(*.BMP)|*.BMP|Jpeg Files(*.JPG)|*.JPG|All Files (*.*)|*.*||"));
	if (OpenFileDlg.DoModal() == IDCANCEL)
		return;

	CString szFileName = OpenFileDlg.GetFileName();
	CString szLoadFile = szFileName;
	CString szFilePath = OpenFileDlg.GetPathName();
	CString szExt = szFileName.Right(4);
	szExt = _T("*") + szExt;

	szFilePath = szFilePath.Left(szFilePath.GetLength() - szFileName.GetLength());
	m_szFilePath = szFilePath;
	szFileName = szFilePath + szExt;//_T("*.jpg");
	m_bIsNext = true;
	m_posFileList = NULL;

	m_imagePos = 0;
	m_imageNum = 0;
	if (m_FileFinder.FindFile(szFileName))
	{
		m_lstFileList.RemoveAll();
		while (m_FileFinder.FindNextFile())
		{
			szFileName = m_FileFinder.GetFileName();
			m_lstFileList.AddTail(szFileName);

			if (m_imageNum < Image_Num_Max)
			{
				if (szLoadFile == szFileName)
				{
					m_imagePos = m_imageNum;
				}
				m_ImageNameList[m_imageNum++] = m_szFilePath + szFileName;
			}
		}
		szFileName = m_FileFinder.GetFileName();
		m_lstFileList.AddTail(szFileName);
		if (m_imageNum < Image_Num_Max)
		{
			if (szLoadFile == szFileName)
			{
				m_imagePos = m_imageNum;
			}
			m_ImageNameList[m_imageNum++] = m_szFilePath + szFileName;
		}

		m_bIsNext = true;
		m_posFileList = m_lstFileList.Find(szLoadFile);
	}
	else
		return;

	if (m_lstFileList.GetCount() > Image_Num_Max)
	{
		MessageBox(_T("Image number exceed 100 in one folder!"));
	}

	AfxGetMainWnd()->SetWindowText(szFilePath + szLoadFile);

	if (1)
	{
		/***************** read images from folder files **************************/
		BYTE* p1;
		for (int n = 0; n < m_imageNum; n++)
		{
			szFileName = m_ImageNameList[n];
			WCHAR * wc = szFileName.AllocSysString();
			CSize size = m_pMyGDIPlus->CreateImageToBuff(wc, &p1);

			int nS = size.cx * size.cy;

			m_ImageVolume[n].width = size.cx;
			m_ImageVolume[n].height = size.cy;
			if (m_ImageVolume[n].pData)
				delete[]m_ImageVolume[n].pData;
			m_ImageVolume[n].pData = new BYTE[nS];
			memcpy(m_ImageVolume[n].pData, p1, nS);

			if(m_imagePos == n)
			{
				m_pBmpData.width = size.cx;
				m_pBmpData.height = size.cy;
				if (m_pBmpData.pData)
					delete[]m_pBmpData.pData;
				m_pBmpData.pData = new BYTE[nS];

				SetScrollSizes(MM_TEXT, size);//滚动窗口的最大区域/////////////////////////////////////
			}
		}
	}
	else
	{
		/***************** read images from raw data **************************/
		//CString filePath = m_szFilePath + _T("RawData_100.raw");
		//CFile pFile;
		//if(pFile.Open(filePath,CFile::modeRead))
		//{
		//	BYTE * pTmp;
		//	CRect * pRect;
		//	CString str;
		//	int ns;
		//	for (int n=0;n<m_imageNum;n++)
		//	{
		//		pFile.Read(&ns, sizeof(int));
		//		TCHAR * buf = new TCHAR[ns];
		//		wmemset(buf,0,ns);
		//		pFile.Read(buf, ns);
		//		m_ImageNameList[n] = (CString)buf;
		//		delete []buf;
		//		pTmp = m_ImageVolume[n];
		//		pFile.Read((BYTE*)pTmp, image_s);
		//		pRect = &(m_EyeRectListLeft[n]);
		//		pFile.Read((CRect*)pRect, sizeof(CRect));
		//		pRect = &(m_EyeRectListRight[n]);
		//		pFile.Read((CRect*)pRect, sizeof(CRect));
		//	}
		//	pFile.Close();
		//}
		//else
		//{
		//	MessageBox(_T("Can not open file RawData_100.raw"));
		//}
	}
	/*********************************************************************/
	m_imagePos -= 1;

	//Clear:
	m_showPoint.x = 0;
	m_showPoint.y = 0;
	m_bMouseDown = false;

	On_Forward();
}


void CBMAlgoDevView::On_Doing()
{
	IMG_UBBUF	m_srcBAK;
	m_srcBAK.ptr = NULL;

	m_srcW = m_ImageVolume[m_imagePos].width;
	m_srcH = m_ImageVolume[m_imagePos].height;
	int nS = m_srcW * m_srcH;
	m_pBmpData.width = m_ImageVolume[m_imagePos].width;
	m_pBmpData.height = m_ImageVolume[m_imagePos].height;
	if (m_pBmpData.pData)
		delete[]m_pBmpData.pData;
	m_pBmpData.pData = new BYTE[nS];
	memcpy(m_pBmpData.pData, m_ImageVolume[m_imagePos].pData, nS);
	CSize size(m_pBmpData.width, m_pBmpData.height);
	SetScrollSizes(MM_TEXT, size);//滚动窗口的最大区域/////////////////////////////////////

	m_src.ptr = m_pBmpData.pData;
	m_src.size.width = m_pBmpData.width;
	m_src.size.height = m_pBmpData.height;
	m_src.linestep = m_pBmpData.width;

	if (m_dst.ptr)
		delete[]m_dst.ptr;
	m_dst.ptr = new IMG_UBYTE[m_src.size.width * m_src.size.height];
	m_dst.size.width = m_pBmpData.width;
	m_dst.size.height = m_pBmpData.height;
	//memcpy(m_dst.ptr, m_src.ptr, m_src.size.width * m_src.size.height);

	CMyTimercount timer;
	timer.GetTimeMilli(true);

	/**************************  Pyramid *************************/
	//Pyramid
	if (m_bPrePyramid1)
	{
		m_myVisAlg2DBase.VisPyramid(m_src, m_src.ptr, m_srcW, m_srcH, 1);
		m_src.size.width = m_srcW;
		m_src.size.height = m_srcH;
		m_src.linestep = m_srcW;
	}
	if (m_bPrePyramid2)
	{
		m_myVisAlg2DBase.VisPyramid(m_src, m_src.ptr, m_srcW, m_srcH, 2);
		m_src.size.width = m_srcW;
		m_src.size.height = m_srcH;
		m_src.linestep = m_srcW;
	}
	if (m_bPrePyramid3)
	{
		m_myVisAlg2DBase.VisPyramid(m_src, m_src.ptr, m_srcW, m_srcH, 3);
		m_src.size.width = m_srcW;
		m_src.size.height = m_srcH;
		m_src.linestep = m_srcW;
	}

	/**************************  ROI *************************/
	if (m_roiRect.downRight.x > 0)
	{
		m_srcBAK.ptr = new IMG_UBYTE[m_srcW*m_srcH];
		m_srcBAK.size.width = m_srcW;
		m_srcBAK.size.height = m_srcH;
		memcpy(m_srcBAK.ptr, m_src.ptr, m_srcW*m_srcH);

		int x, y, n = 0, pos = m_roiRect.topLeft.y * m_srcW;
		for (y = m_roiRect.topLeft.y; y <= m_roiRect.downRight.y; y++)
		{
			for(x = m_roiRect.topLeft.x; x <= m_roiRect.downRight.x; x++)
			{
				m_src.ptr[n++] = m_srcBAK.ptr[pos + x];
			}
			pos += m_srcW;
		}

		m_src.size.width = IMG_UWORD(m_roiRect.downRight.x - m_roiRect.topLeft.x + 1);
		m_src.size.height = IMG_UWORD(m_roiRect.downRight.y - m_roiRect.topLeft.y + 1);
		m_src.linestep = m_src.size.width;
	}

	/**************************  Filter *************************/
	//Filter Mean
	if (m_bPreFilterMean3)
	{
		m_myVisAlg2DBase.VisFilterMean(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 3);
	}
	else if (m_bPreFilterMean5)
	{
		m_myVisAlg2DBase.VisFilterMean(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 5);
	}
	else if (m_bPreFilterMean7)
	{
		m_myVisAlg2DBase.VisFilterMean(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 7);
	}
	else if (m_bPreFilterMean9)
	{
		m_myVisAlg2DBase.VisFilterMean(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 9);
	}
	else if (m_bPreFilterMean15)
	{
		m_myVisAlg2DBase.VisFilterMean(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 15);
	}
	//Filter Median
	if (m_bPreFilterMedian3)
	{
		m_myVisAlg2DBase.VisFilterMedian(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 3);
	}
	else if (m_bPreFilterMedian5)
	{
		m_myVisAlg2DBase.VisFilterMedian(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 5);
	}
	else if (m_bPreFilterMedian7)
	{
		m_myVisAlg2DBase.VisFilterMedian(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 7);
	}
	else if (m_bPreFilterMedian9)
	{
		m_myVisAlg2DBase.VisFilterMedian(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 9);
	}
	else if (m_bPreFilterMedian15)
	{
		m_myVisAlg2DBase.VisFilterMedian(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 15);
	}
	//Filter Gaussian
	if (m_bPreFilterGauss3)
	{
		m_myVisAlg2DBase.VisFilterGaussian(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 3);
	}
	else if (m_bPreFilterGauss5)
	{
		m_myVisAlg2DBase.VisFilterGaussian(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 5);
	}
	else if (m_bPreFilterGauss7)
	{
		m_myVisAlg2DBase.VisFilterGaussian(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 7);
	}
	else if (m_bPreFilterGauss9)
	{
		m_myVisAlg2DBase.VisFilterGaussian(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 9);
	}
	else if (m_bPreFilterGauss15)
	{
		m_myVisAlg2DBase.VisFilterGaussian(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 15);
	}
	//Filter Sobel
	if (m_bPreFilterSobel3)
	{
		m_myVisAlg2DBase.VisFilterSobel_8u8u(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 3);
	}
	if (m_bPreFilterSobel5)
	{
		m_myVisAlg2DBase.VisFilterSobel_8u8u(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 5);
	}
	if (m_bPreFilterSobel7)
	{
		m_myVisAlg2DBase.VisFilterSobel_8u8u(m_src.ptr, m_src.size.height, m_src.size.width, m_src.ptr, 7);
	}

	/**************************  Segmentation *************************/
	if (m_bSegmentMannual)
	{
		m_myVisAlg2DBase.VisSegmentManual(m_src.ptr, m_src.size.width, m_src.size.height, m_src.ptr, 128);
	}
	if (m_bSegmentOtsu)
	{
		m_myVisAlg2DBase.VisSegmentOtsu(m_src.ptr, m_src.size.width, m_src.size.height, m_src.ptr);
	}
	if (m_bSegmentDynamic)
	{
		m_myVisAlg2DBase.VisSegmentDynamic(m_src.ptr, m_src.size.width, m_src.size.height, m_src.ptr, 15);
	}
	if (m_bSegmentCntDots)
	{
		m_myVisAlg2DBase.VisSegmentCntDots(m_src.ptr, m_src.size.width, m_src.size.height, m_src.ptr, 0.1);
	}

	/**************************  Morphology *************************/
	if (m_bMorphonogyErode)
	{
		m_myVisAlg2DBase.VisMorphologyErode(m_src, m_src);
	}
	if (m_bMorphonogyDilation)
	{
		m_myVisAlg2DBase.VisMorphologyDilation(m_src, m_src);
	}
	if (m_bMorphonogyOpen)
	{
		m_myVisAlg2DBase.VisMorphologyOpen(m_src, m_src);
	}
	if (m_bMorphonogyClose)
	{
		m_myVisAlg2DBase.VisMorphologyClose(m_src, m_src);
	}

	/******************* IA978 *******************************/
	_BACK_CODE re;

	//Dut
	if (m_bIa978DutTrayLocation)
	{
		m_parDutTrayLocating.cx = 0;
		m_parDutTrayLocating.cy = 0;
		
		re = m_bmDutTrayLocating.DUTLocation(m_src, 0, m_parDutTrayLocating);

		//Dut On Tray Recognization
		/*m_nStatus = -1;
		m_bmDutTrayReco.DUTLocation(m_src, m_parDutTrayLocating, m_nStatus);
		m_parDutTrayLocating.x1 *= 4;
		m_parDutTrayLocating.y1 *= 4;
		m_parDutTrayLocating.x2 *= 4;
		m_parDutTrayLocating.y2 *= 4;
		m_parDutTrayLocating.x3 *= 4;
		m_parDutTrayLocating.y3 *= 4;
		m_parDutTrayLocating.x4 *= 4;
		m_parDutTrayLocating.y4 *= 4;
		m_parDutTrayLocating.cx *= 4;
		m_parDutTrayLocating.cy *= 4;*/
	}
	if (m_bIa978DutSocketLocation)
	{
		m_parDutTrayLocating.cx = 0;
		m_parDutTrayLocating.cy = 0;
		re = m_bmDutSocketLocating.DUTLocation(m_src, 0, m_parDutTrayLocating);
	}

	//Socket
	if (m_bIa978SocketLocationCoarse)
	{
		m_NorPoint.leftPoint.max_x = 0;
		m_NorPoint.leftPoint.min_x = 0;
		m_NorPoint.leftPoint.max_y = 0;
		m_NorPoint.leftPoint.min_y = 0;
		m_NorPoint.rightPoint.max_x = 0;
		m_NorPoint.rightPoint.min_x = 0;
		m_NorPoint.rightPoint.max_y = 0;
		m_NorPoint.rightPoint.min_y = 0;
		int re = m_bmSocketLocatingCoarse.SocketLocatingCoarse(m_src, 0);
		if (0 == re)
		{
			m_bmSocketLocatingCoarse.VisSS_GetPoint(m_NorPoint);
			m_NorPoint.leftPoint.max_x *= 4;
			m_NorPoint.leftPoint.min_x *= 4;
			m_NorPoint.leftPoint.max_y *= 4;
			m_NorPoint.leftPoint.min_y *= 4;
			m_NorPoint.rightPoint.max_x *= 4;
			m_NorPoint.rightPoint.min_x *= 4;
			m_NorPoint.rightPoint.max_y *= 4;
			m_NorPoint.rightPoint.min_y *= 4;
		}
	}
	if (m_bIa978SocketLocation)
	{
		m_socketCenter = { 0,0 };
		m_pocketArrow = { 0,0 };
		//bool bPar = m_bmSocketLocation.LoadParameter(".\\BmSocketLocation.ini");
		double x, y;
		x = y = 0;
		m_socketAngle = 0;
		re = m_bmSocketLocation.SocketLocation(m_src, 180, 171, x, y, m_socketAngle);
		if (re == _CODE_OK)
		{
			m_socketCenter = { float(x),float(y) };
			m_pocketArrow.Y = y - 100;
			m_pocketArrow.X = x - 100 * tan((m_socketAngle - 90) * PI / 180);
		}
	}
	if (m_bIa978SocketLockStatus)
	{
		m_nLock = -1;
		re = m_bmSocketLockStatus.GetSocketOpenStatus(m_src, 0);
		if (re == _CODE_SOCKETLOCK)
			m_nLock = 1;
		else if (re == _CODE_SOCKETUNLOCK)
			m_nLock = 0;
		else
			m_nLock = -1;
	}
	if (m_bIa978SocketStatus)
	{
		m_nStatus = 0;
		//bool bPar = m_bmSocketStatus.LoadParameter(".\\BmSocketStatus.ini");
		re = m_bmSocketStatus.GetSocketStatus(m_src, 0, m_nStatus);
		if (re == _CODE_EMPTY)
			m_nStatus = 0;
		else if (re == _CODE_OK)
			m_nStatus = 1;
		else if (re == _CODE_PROERROR)
			m_nStatus = 2;
		else
			m_nStatus = -1;
	}
	//Pocket
	if (m_bIa978PocketLocation)
	{
		m_socketCenter = { 0,0 };
		m_pocketArrow = { 0,0 };
		m_pocketAngle = 0;
		double x = 0, y = 0;
		//bool bPar = m_bmPocketLocation.LoadParameter(".\\BmPocketLocation.ini");
		re = m_bmPocketLocation.PocketLocation(m_src, x, y, m_pocketAngle);
		if (re == _CODE_OK)
		{
			m_pocketCenter = { float(x),float(y) };
			m_pocketArrow.Y = m_pocketCenter.Y - 100;
			m_pocketArrow.X = m_pocketCenter.X - 100 * tan((m_pocketAngle - 90) * PI / 180);
		}
	}
	if (m_bIa978PocketStatus)
	{
		m_nStatus = 0;
		//bool bPar = m_bmPocketStatus.LoadParameter(".\\BmPocketStatus.ini");
		re = m_bmPocketStatus.GetPocketStatus(m_src);
		if (re == _CODE_EMPTY)
			m_nStatus = 0;
		else if (re == _CODE_DUT_NORMAL)
			m_nStatus = 1;
		else if (re == _CODE_PROERROR)
			m_nStatus = 2;
		else
			m_nStatus = -1;
	}

	//Tray Circle
	if (m_bIa978TrayLocation)
	{
		//bool bPar = m_bmTrayLocation.LoadParameter(".\\BmTrayLocation.ini");
		double x = 0, y = 0;
		re = m_bmTrayLocation.TrayLocation(m_src, x, y);
		m_trayCircleCenter = { float(x),float(y) };
	}

	
	/*******************   AA   *******************************/
	if (m_bAaChartArea)
	{
		m_bmAaAlgorithm.AaChartArea(m_src, m_dst, m_sChartArea);
		if (m_bAaRectangles)
		{
			m_bmAaAlgorithm.AaChartBlobs(m_src, m_dst, m_sChartBlobs);
			if (m_bAaSfrMtf)
			{
				m_bmAaAlgorithm.AaMtfSfr(m_src, m_pMTF);
			}
			if (m_bAaMtfStdDev)
			{
				m_bmAaAlgorithm.AaMtfStdDev(m_src, m_pMTF);
			}
			if (m_bAaMtfMaxMin)
			{
				m_bmAaAlgorithm.AaMtfMaxMin(m_src, m_pMTF);
			}
			if (m_bAaMtfGradMax)
			{
				m_bmAaAlgorithm.AaMtfGradMax(m_src, m_pMTF);
			}
			if (m_bAaMtfGradAve)
			{
				m_bmAaAlgorithm.AaMtfGradAve(m_src, m_pMTF);
			}
		}
	}
	if (m_bAaLightDistribution)
	{
		m_bmAaAlgorithm.AaLightDistribution(m_src, m_src);
		if (m_bAaLightClose)
			m_bmAaAlgorithm.AaLightClose(m_src, m_src);
		if(m_bAaLightOpen)
			m_bmAaAlgorithm.AaLightOpen(m_src, m_src);
	}
		
	/*************************** end **********************************/

	float fTime = timer.GetTimeMilli(false);
	if (m_bDoFolder)
		m_fTimer += fTime;

	delete[] m_dst.ptr;
	m_dst.ptr = NULL;

	CString str;
	if (m_bAllFolder)
		str.Format(_T("Progress: %d of %d"), m_nProgressCurrent, m_nProgressTotal);
	else
	{
		if (m_bIa978DutTrayLocation)
			str.Format(_T("Algorithm Time: %3.1f ms;    Progress: %d of %d;    angle: %d"), fTime, m_nProgressCurrent, m_nProgressTotal, m_nStatus);
		else if (m_bIa978SocketStatus)
			str.Format(_T("Algorithm Time: %3.1f ms;    Progress: %d of %d;    Socket Status: %d"), fTime, m_nProgressCurrent, m_nProgressTotal, m_nStatus);
		else if (m_bIa978PocketStatus)
			str.Format(_T("Algorithm Time: %3.1f ms;    Progress: %d of %d;    Pocket Status: %d"), fTime, m_nProgressCurrent, m_nProgressTotal, m_nStatus);
		else if (m_bIa978SocketLockStatus)
			str.Format(_T("Algorithm Time: %3.1f ms;    Progress: %d of %d;    Socket Lock: %d"), fTime, m_nProgressCurrent, m_nProgressTotal, m_nLock);
		else
			str.Format(_T("Algorithm Time: %3.1f ms;    Progress: %d of %d"), fTime, m_nProgressCurrent, m_nProgressTotal);

	}
	CStatusBar& statusBar = ((CMainFrame *)(AfxGetApp()->m_pMainWnd))->m_wndStatusBar;
	statusBar.SetPaneText(0, (LPCTSTR)str);

	//将ROI子图拷贝回显示图
	if (m_roiRect.downRight.x > 0 && m_srcBAK.ptr != NULL)
	{
		int x, y, n = 0, pos = m_roiRect.topLeft.y * m_srcW;
		for (y = m_roiRect.topLeft.y; y <= m_roiRect.downRight.y; y++)
		{
			for (x = m_roiRect.topLeft.x; x <= m_roiRect.downRight.x; x++)
			{
				m_srcBAK.ptr[pos + x] = m_src.ptr[n++];
			}
			pos += m_srcW;
		}
		memcpy(m_src.ptr, m_srcBAK.ptr, m_srcW*m_srcH);
		m_src.size.width = m_srcW;
		m_src.size.height = m_srcH;
		m_src.linestep = m_src.size.width;

		delete[] m_srcBAK.ptr;
	}

	//if (m_bPrePyramid1 || m_bPrePyramid2 || m_bPrePyramid3)
		Invalidate(TRUE);
	//else
	//	Invalidate(FALSE);
}


void CBMAlgoDevView::On_Forward()
{
	if (m_imagePos >= m_imageNum - 1)
	{
		MessageBox(_T("No More Image File!"));
		m_bIsNext = false;
		return;
	}
	else
		m_imagePos++;

	if (/*!m_bBestPar &&*/ !m_bAllFolder)
	{
		m_nProgressTotal = m_imageNum;
		m_nProgressCurrent = m_imagePos + 1;
	}

	m_sFileName = m_ImageNameList[m_imagePos];

	AfxGetMainWnd()->SetWindowText(m_sFileName);

	//Clear:
	m_showPoint.x = 0;
	m_showPoint.y = 0;

	m_bShowMovePos = false;
	On_Doing();
}


void CBMAlgoDevView::On_Backward()
{
	if (m_imagePos <= 0)
	{
		MessageBox(_T("No More Image File!"));
		return;
	}
	else
		m_imagePos--;

	if (/*!m_bBestPar &&*/ !m_bAllFolder)
	{
		m_nProgressTotal = m_imageNum;
		m_nProgressCurrent = m_imagePos + 1;
	}

	m_sFileName = m_ImageNameList[m_imagePos];
	AfxGetMainWnd()->SetWindowText(m_sFileName);

	//Clear:
	m_showPoint.x = 0;
	m_showPoint.y = 0;
	m_bShowMovePos = false;

	On_Doing();
}


void CBMAlgoDevView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CClientDC dc(this);//获取客户窗口DC
	OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&point);//将视图坐标转换为文档作标

	if (!m_pBmpData.pData)
		return;

	if (m_pMyGDIPlus)
	{
		int x, y;

		x = point.x;
		y = point.y;
		m_showPoint.x = 0;
		m_showPoint.y = 0;

		if (x > 0 && y > 0 && x < m_pBmpData.width && y < m_pBmpData.height)
		{
			m_showPoint.x = x;
			m_showPoint.y = y;
			
			m_bMouseDown = true;

			if (m_bRoiDraw)
			{
				if (m_roiRect.downRight.x > 0)
				{
					m_roiRect.topLeft = { (IMG_WORD)x,(IMG_WORD)y };
					m_roiRect.downRight = { 0,0 };
				}
				else
				{
					if(0 == m_roiRect.topLeft.x)
						m_roiRect.topLeft = { (IMG_WORD)x,(IMG_WORD)y };
					else
						m_roiRect.downRight = { (IMG_WORD)x,(IMG_WORD)y };
				}
			}
		}
	}

	HCURSOR hCurs = LoadCursor(NULL, IDC_UPARROW);
	SetCursor(hCurs);
	Invalidate(FALSE);

	if (m_bShowMovePos)
		m_bShowMovePos = false;
	else
		m_bShowMovePos = true;


	CScrollView::OnLButtonDown(nFlags, point);
	Invalidate(FALSE);
}


void CBMAlgoDevView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMouseDown = false;
	CClientDC dc(this);//获取客户窗口DC
	OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&point);//将视图坐标转换为文档作标

	CScrollView::OnLButtonUp(nFlags, point);
}


void CBMAlgoDevView::OnMouseMove(UINT nFlags, CPoint point)
{
	CClientDC dc(this);//获取客户窗口DC
	OnPrepareDC(&dc);//进行坐标原点的匹配
	dc.DPtoLP(&point);//将视图坐标转换为文档作标

	if (!m_pBmpData.pData)
		return;

	if (m_pMyGDIPlus && m_bShowMovePos)
	{
		HCURSOR hCurs = LoadCursor(NULL, IDC_UPARROW);
		SetCursor(hCurs);
		Invalidate(FALSE);

		int x, y;
		x = point.x;
		y = point.y;
		BYTE gray = 0, gradV = 0, gradH = 0;

		if (x < m_pBmpData.width && y < m_pBmpData.height)
		{
			gray = m_pBmpData.pData[y*m_pBmpData.width + x];
			if (x > 0)
				gradH = abs(m_pBmpData.pData[y*m_pBmpData.width + x] - m_pBmpData.pData[y*m_pBmpData.width + x - 1]);
			if (y > 0)
				gradV = abs(m_pBmpData.pData[y*m_pBmpData.width + x] - m_pBmpData.pData[(y - 1)*m_pBmpData.width + x]);

			CString str;
			str.Format(_T("Mouse point (%d, %d);   Gray: %d;   GradV: %d;   GradH: %d;"), x, y, gray, gradV, gradH);
			CStatusBar& statusBar = ((CMainFrame *)(AfxGetApp()->m_pMainWnd))->m_wndStatusBar;
			statusBar.SetPaneText(0, (LPCTSTR)str);
		}
	}

	CScrollView::OnMouseMove(nFlags, point);
}


void CBMAlgoDevView::On_DoFolder()
{
	m_bDoFolder = true;
	m_fTimer = 0.0f;

	m_imagePos = -1;
	while (m_imagePos < m_imageNum - 1)
	{
		On_Forward();
		//Sleep(1000);
	}

	m_bDoFolder = false;
	m_fTimer /= m_imageNum;

	CString str;
	str.Format(_T("Average Algorithm Time: %3.1f ms;    Progress: %d of %d"), m_fTimer, m_nProgressCurrent, m_nProgressTotal);
	CStatusBar& statusBar = ((CMainFrame *)(AfxGetApp()->m_pMainWnd))->m_wndStatusBar;
	statusBar.SetPaneText(0, (LPCTSTR)str);

	Invalidate(TRUE);
}


/************************ Pyramid *******************************/
void CBMAlgoDevView::OnPyramid1()
{
	if (m_bPrePyramid1)
	{
		m_nPrePyramid1 = -1;
		m_bPrePyramid1 = false;
	}
	else
	{
		m_nPrePyramid1 = 0;
		m_bPrePyramid1 = true;
		m_nPrePyramid2 = -1;
		m_bPrePyramid2 = false;
		m_nPrePyramid3 = -1;
		m_bPrePyramid3 = false;
	}
}
void CBMAlgoDevView::OnUpdatePyramid1(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPrePyramid1 == 0);
}

void CBMAlgoDevView::OnPyramid2()
{
	if (m_bPrePyramid2)
	{
		m_nPrePyramid2 = -1;
		m_bPrePyramid2 = false;
	}
	else
	{
		m_nPrePyramid2 = 0;
		m_bPrePyramid2 = true;
		m_nPrePyramid1 = -1;
		m_bPrePyramid1 = false;
		m_nPrePyramid3 = -1;
		m_bPrePyramid3 = false;
	}
}
void CBMAlgoDevView::OnUpdatePyramid2(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPrePyramid2 == 0);
}

void CBMAlgoDevView::OnPyramid3()
{
	if (m_bPrePyramid3)
	{
		m_nPrePyramid3 = -1;
		m_bPrePyramid3 = false;
	}
	else
	{
		m_nPrePyramid3 = 0;
		m_bPrePyramid3 = true;
		m_nPrePyramid1 = -1;
		m_bPrePyramid1 = false;
		m_nPrePyramid2 = -1;
		m_bPrePyramid2 = false;
	}
}
void CBMAlgoDevView::OnUpdatePyramid3(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPrePyramid3 == 0);
}

/************************ ROI *******************************/

void CBMAlgoDevView::OnRoiDraw()
{
	if (m_bRoiDraw)
	{
		m_nRoiDraw = -1;
		m_bRoiDraw = false;
	}
	else
	{
		m_nRoiDraw = 0;
		m_bRoiDraw = true;
		m_nRoiDelete = -1;
		m_bRoiDelete = false;
	}
}
void CBMAlgoDevView::OnUpdateRoiDraw(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nRoiDraw == 0);
}

void CBMAlgoDevView::OnRoiDelete()
{
	if (m_bRoiDelete)
	{
		m_nRoiDelete = -1;
		m_bRoiDelete = false;
	}
	else
	{
		m_nRoiDelete = 0;
		m_bRoiDelete = true;
		m_nRoiDraw = -1;
		m_bRoiDraw = false;

		m_roiRect.topLeft = { 0,0 };
		m_roiRect.downRight = { 0,0 };
	}
}
void CBMAlgoDevView::OnUpdateRoiDelete(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nRoiDelete == 0);
}

/************************ Filter *******************************/
//Mean
void CBMAlgoDevView::OnFilterMean3()
{
	if (m_bPreFilterMean3)
	{
		m_nPreFilterMean3 = -1;
		m_bPreFilterMean3 = false;
	}
	else
	{
		m_nPreFilterMean3 = 0;
		m_bPreFilterMean3 = true;
		m_nPreFilterMean7 = -1;
		m_bPreFilterMean7 = false;
		m_nPreFilterMean5 = -1;
		m_bPreFilterMean5 = false;
		m_nPreFilterMean9 = -1;
		m_bPreFilterMean9 = false;
		m_nPreFilterMean15 = -1;
		m_bPreFilterMean15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterMean3(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterMean3 == 0);
}

void CBMAlgoDevView::OnFilterMean5()
{
	if (m_bPreFilterMean5)
	{
		m_nPreFilterMean5 = -1;
		m_bPreFilterMean5 = false;
	}
	else
	{
		m_nPreFilterMean5 = 0;
		m_bPreFilterMean5 = true;
		m_nPreFilterMean3 = -1;
		m_bPreFilterMean3 = false;
		m_nPreFilterMean7 = -1;
		m_bPreFilterMean7 = false;
		m_nPreFilterMean9 = -1;
		m_bPreFilterMean9 = false;
		m_nPreFilterMean15 = -1;
		m_bPreFilterMean15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterMean5(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterMean5 == 0);
}

void CBMAlgoDevView::OnFilterMean7()
{
	if (m_bPreFilterMean7)
	{
		m_nPreFilterMean7 = -1;
		m_bPreFilterMean7 = false;
	}
	else
	{
		m_nPreFilterMean7 = 0;
		m_bPreFilterMean7 = true;
		m_nPreFilterMean3 = -1;
		m_bPreFilterMean3 = false;
		m_nPreFilterMean5 = -1;
		m_bPreFilterMean5 = false;
		m_nPreFilterMean9 = -1;
		m_bPreFilterMean9 = false;
		m_nPreFilterMean15 = -1;
		m_bPreFilterMean15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterMean7(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterMean7 == 0);
}

void CBMAlgoDevView::OnFilterMean9()
{
	if (m_bPreFilterMean9)
	{
		m_nPreFilterMean9 = -1;
		m_bPreFilterMean9 = false;
	}
	else
	{
		m_nPreFilterMean9 = 0;
		m_bPreFilterMean9 = true;
		m_nPreFilterMean3 = -1;
		m_bPreFilterMean3 = false;
		m_nPreFilterMean5 = -1;
		m_bPreFilterMean5 = false;
		m_nPreFilterMean7 = -1;
		m_bPreFilterMean7 = false;
		m_nPreFilterMean15 = -1;
		m_bPreFilterMean15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterMean9(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterMean9 == 0);
}

void CBMAlgoDevView::OnFilterMean15()
{
	if (m_bPreFilterMean15)
	{
		m_nPreFilterMean15 = -1;
		m_bPreFilterMean15 = false;
	}
	else
	{
		m_nPreFilterMean15 = 0;
		m_bPreFilterMean15 = true;
		m_nPreFilterMean3 = -1;
		m_bPreFilterMean3 = false;
		m_nPreFilterMean5 = -1;
		m_bPreFilterMean5 = false;
		m_nPreFilterMean7 = -1;
		m_bPreFilterMean7 = false;
		m_nPreFilterMean9 = -1;
		m_bPreFilterMean9 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterMean15(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterMean15 == 0);
}


//Median
void CBMAlgoDevView::OnFilterMedian3()
{
	if (m_bPreFilterMedian3)
	{
		m_nPreFilterMedian3 = -1;
		m_bPreFilterMedian3 = false;
	}
	else
	{
		m_nPreFilterMedian3 = 0;
		m_bPreFilterMedian3 = true;
		m_nPreFilterMedian5 = -1;
		m_bPreFilterMedian5 = false;
		m_nPreFilterMedian7 = -1;
		m_bPreFilterMedian7 = false;
		m_nPreFilterMedian9 = -1;
		m_bPreFilterMedian9 = false;
		m_nPreFilterMedian15 = -1;
		m_bPreFilterMedian15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterMedian3(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterMedian3 == 0);
}

void CBMAlgoDevView::OnFilterMedian5()
{
	if (m_bPreFilterMedian5)
	{
		m_nPreFilterMedian5 = -1;
		m_bPreFilterMedian5 = false;
	}
	else
	{
		m_nPreFilterMedian5 = 0;
		m_bPreFilterMedian5 = true;
		m_nPreFilterMedian3 = -1;
		m_bPreFilterMedian3 = false;
		m_nPreFilterMedian7 = -1;
		m_bPreFilterMedian7 = false;
		m_nPreFilterMedian9 = -1;
		m_bPreFilterMedian9 = false;
		m_nPreFilterMedian15 = -1;
		m_bPreFilterMedian15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterMedian5(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterMedian5 == 0);
}

void CBMAlgoDevView::OnFilterMedian7()
{
	if (m_bPreFilterMedian7)
	{
		m_nPreFilterMedian7 = -1;
		m_bPreFilterMedian7 = false;
	}
	else
	{
		m_nPreFilterMedian7 = 0;
		m_bPreFilterMedian7 = true;
		m_nPreFilterMedian3 = -1;
		m_bPreFilterMedian3 = false;
		m_nPreFilterMedian5 = -1;
		m_bPreFilterMedian5 = false;
		m_nPreFilterMedian9 = -1;
		m_bPreFilterMedian9 = false;
		m_nPreFilterMedian15 = -1;
		m_bPreFilterMedian15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterMedian7(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterMedian7 == 0);
}

void CBMAlgoDevView::OnFilterMedian9()
{
	if (m_bPreFilterMedian9)
	{
		m_nPreFilterMedian9 = -1;
		m_bPreFilterMedian9 = false;
	}
	else
	{
		m_nPreFilterMedian9 = 0;
		m_bPreFilterMedian9 = true;
		m_nPreFilterMedian3 = -1;
		m_bPreFilterMedian3 = false;
		m_nPreFilterMedian5 = -1;
		m_bPreFilterMedian5 = false;
		m_nPreFilterMedian7 = -1;
		m_bPreFilterMedian7 = false;
		m_nPreFilterMedian15 = -1;
		m_bPreFilterMedian15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterMedian9(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterMedian9 == 0);
}

void CBMAlgoDevView::OnFilterMedian15()
{
	if (m_bPreFilterMedian15)
	{
		m_nPreFilterMedian15 = -1;
		m_bPreFilterMedian15 = false;
	}
	else
	{
		m_nPreFilterMedian15 = 0;
		m_bPreFilterMedian15 = true;
		m_nPreFilterMedian3 = -1;
		m_bPreFilterMedian3 = false;
		m_nPreFilterMedian5 = -1;
		m_bPreFilterMedian5 = false;
		m_nPreFilterMedian7 = -1;
		m_bPreFilterMedian7 = false;
		m_nPreFilterMedian9 = -1;
		m_bPreFilterMedian9 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterMedian15(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterMedian15 == 0);
}

//Gauss
void CBMAlgoDevView::OnFilterGauss3()
{
	if (m_bPreFilterGauss3)
	{
		m_nPreFilterGauss3 = -1;
		m_bPreFilterGauss3 = false;
	}
	else
	{
		m_nPreFilterGauss3 = 0;
		m_bPreFilterGauss3 = true;
		m_nPreFilterGauss5 = -1;
		m_bPreFilterGauss5 = false;
		m_nPreFilterGauss7 = -1;
		m_bPreFilterGauss7 = false;
		m_nPreFilterGauss9 = -1;
		m_bPreFilterGauss9 = false;
		m_nPreFilterGauss15 = -1;
		m_bPreFilterGauss15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterGauss3(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterGauss3 == 0);
}

void CBMAlgoDevView::OnFilterGauss5()
{
	if (m_bPreFilterGauss5)
	{
		m_nPreFilterGauss5 = -1;
		m_bPreFilterGauss5 = false;
	}
	else
	{
		m_nPreFilterGauss5 = 0;
		m_bPreFilterGauss5 = true;
		m_nPreFilterGauss3 = -1;
		m_bPreFilterGauss3 = false;
		m_nPreFilterGauss7 = -1;
		m_bPreFilterGauss7 = false;
		m_nPreFilterGauss9 = -1;
		m_bPreFilterGauss9 = false;
		m_nPreFilterGauss15 = -1;
		m_bPreFilterGauss15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterGauss5(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterGauss5 == 0);
}

void CBMAlgoDevView::OnFilterGauss7()
{
	if (m_bPreFilterGauss7)
	{
		m_nPreFilterGauss7 = -1;
		m_bPreFilterGauss7 = false;
	}
	else
	{
		m_nPreFilterGauss7 = 0;
		m_bPreFilterGauss7 = true;
		m_nPreFilterGauss3 = -1;
		m_bPreFilterGauss3 = false;
		m_nPreFilterGauss5 = -1;
		m_bPreFilterGauss5 = false;
		m_nPreFilterGauss9 = -1;
		m_bPreFilterGauss9 = false;
		m_nPreFilterGauss15 = -1;
		m_bPreFilterGauss15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterGauss7(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterGauss7 == 0);
}

void CBMAlgoDevView::OnFilterGauss9()
{
	if (m_bPreFilterGauss9)
	{
		m_nPreFilterGauss9 = -1;
		m_bPreFilterGauss9 = false;
	}
	else
	{
		m_nPreFilterGauss9 = 0;
		m_bPreFilterGauss9 = true;
		m_nPreFilterGauss3 = -1;
		m_bPreFilterGauss3 = false;
		m_nPreFilterGauss5 = -1;
		m_bPreFilterGauss5 = false;
		m_nPreFilterGauss7 = -1;
		m_bPreFilterGauss7 = false;
		m_nPreFilterGauss15 = -1;
		m_bPreFilterGauss15 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterGauss9(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterGauss9 == 0);
}

void CBMAlgoDevView::OnFilterGauss15()
{
	if (m_bPreFilterGauss15)
	{
		m_nPreFilterGauss15 = -1;
		m_bPreFilterGauss15 = false;
	}
	else
	{
		m_nPreFilterGauss15 = 0;
		m_bPreFilterGauss15 = true;
		m_nPreFilterGauss3 = -1;
		m_bPreFilterGauss3 = false;
		m_nPreFilterGauss5 = -1;
		m_bPreFilterGauss5 = false;
		m_nPreFilterGauss7 = -1;
		m_bPreFilterGauss7 = false;
		m_nPreFilterGauss9 = -1;
		m_bPreFilterGauss9 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterGauss15(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterGauss15 == 0);
}

void CBMAlgoDevView::OnFilterSobel3()
{
	if (m_bPreFilterSobel3)
	{
		m_nPreFilterSobel3 = -1;
		m_bPreFilterSobel3 = false;
	}
	else
	{
		m_nPreFilterSobel3 = 0;
		m_bPreFilterSobel3 = true;
		m_nPreFilterSobel5 = -1;
		m_bPreFilterSobel5 = false;
		m_nPreFilterSobel7 = -1;
		m_bPreFilterSobel7 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterSobel3(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterSobel3 == 0);
}

void CBMAlgoDevView::OnFilterSobel5()
{
	if (m_bPreFilterSobel5)
	{
		m_nPreFilterSobel5 = -1;
		m_bPreFilterSobel5 = false;
	}
	else
	{
		m_nPreFilterSobel5 = 0;
		m_bPreFilterSobel5 = true;
		m_nPreFilterSobel3 = -1;
		m_bPreFilterSobel3 = false;
		m_nPreFilterSobel7 = -1;
		m_bPreFilterSobel7 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterSobel5(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterSobel5 == 0);
}

void CBMAlgoDevView::OnFilterSobel7()
{
	if (m_bPreFilterSobel7)
	{
		m_nPreFilterSobel7 = -1;
		m_bPreFilterSobel7 = false;
	}
	else
	{
		m_nPreFilterSobel7 = 0;
		m_bPreFilterSobel7 = true;
		m_nPreFilterSobel3 = -1;
		m_bPreFilterSobel3 = false;
		m_nPreFilterSobel5 = -1;
		m_bPreFilterSobel5 = false;
	}
}
void CBMAlgoDevView::OnUpdateFilterSobel7(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nPreFilterSobel7 == 0);
}

/******************************* Segment *******************************/
void CBMAlgoDevView::OnSegmentMannual()
{
	if (m_bSegmentMannual)
	{
		m_nSegmentMannual = -1;
		m_bSegmentMannual = false;
	}
	else
	{
		m_nSegmentMannual = 0;
		m_bSegmentMannual = true;
		m_nSegmentOtsu = -1;
		m_bSegmentOtsu = false;
		m_nSegmentDynamic = -1;
		m_bSegmentDynamic = false;
		m_nSegmentCntDots = -1;
		m_bSegmentCntDots = false;
	}
}
void CBMAlgoDevView::OnUpdateSegmentMannual(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nSegmentMannual == 0);
}

void CBMAlgoDevView::OnSegmentOtsu()
{
	if (m_bSegmentOtsu)
	{
		m_nSegmentOtsu = -1;
		m_bSegmentOtsu = false;
	}
	else
	{
		m_nSegmentOtsu = 0;
		m_bSegmentOtsu = true;
		m_nSegmentMannual = -1;
		m_bSegmentMannual = false;
		m_nSegmentDynamic = -1;
		m_bSegmentDynamic = false;
		m_nSegmentCntDots = -1;
		m_bSegmentCntDots = false;
	}
}
void CBMAlgoDevView::OnUpdateSegmentOtsu(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nSegmentOtsu == 0);
}

void CBMAlgoDevView::OnSegmentDynamic()
{
	if (m_bSegmentDynamic)
	{
		m_nSegmentDynamic = -1;
		m_bSegmentDynamic = false;
	}
	else
	{
		m_nSegmentDynamic = 0;
		m_bSegmentDynamic = true;
		m_nSegmentMannual = -1;
		m_bSegmentMannual = false;
		m_nSegmentOtsu = -1;
		m_bSegmentOtsu = false;
		m_nSegmentCntDots = -1;
		m_bSegmentCntDots = false;
	}
}
void CBMAlgoDevView::OnUpdateSegmentDynamic(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nSegmentDynamic == 0);
}

void CBMAlgoDevView::OnSegmentCntDots()
{
	if (m_bSegmentCntDots)
	{
		m_nSegmentCntDots = -1;
		m_bSegmentCntDots = false;
	}
	else
	{
		m_nSegmentCntDots = 0;
		m_bSegmentCntDots = true;
		m_nSegmentMannual = -1;
		m_bSegmentMannual = false;
		m_nSegmentOtsu = -1;
		m_bSegmentOtsu = false;
		m_nSegmentDynamic = -1;
		m_bSegmentDynamic = false;
	}
}
void CBMAlgoDevView::OnUpdateSegmentCntDots(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nSegmentCntDots == 0);
}

/******************************** Morphology *********************************/

void CBMAlgoDevView::OnMorphonogyErode()
{
	if (m_bMorphonogyErode)
	{
		m_nMorphonogyErode = -1;
		m_bMorphonogyErode = false;
	}
	else
	{
		m_nMorphonogyErode = 0;
		m_bMorphonogyErode = true;
	}
}
void CBMAlgoDevView::OnUpdateMorphonogyErode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nMorphonogyErode == 0);
}

void CBMAlgoDevView::OnMorphonogyDilation()
{
	if (m_bMorphonogyDilation)
	{
		m_nMorphonogyDilation = -1;
		m_bMorphonogyDilation = false;
	}
	else
	{
		m_nMorphonogyDilation = 0;
		m_bMorphonogyDilation = true;
	}
}
void CBMAlgoDevView::OnUpdateMorphonogyDilation(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nMorphonogyDilation == 0);
}

void CBMAlgoDevView::OnMorphonogyOpen()
{
	if (m_bMorphonogyOpen)
	{
		m_nMorphonogyOpen = -1;
		m_bMorphonogyOpen = false;
	}
	else
	{
		m_nMorphonogyOpen = 0;
		m_bMorphonogyOpen = true;
	}
}
void CBMAlgoDevView::OnUpdateMorphonogyOpen(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nMorphonogyOpen == 0);
}

void CBMAlgoDevView::OnMorphonogyClose()
{
	if (m_bMorphonogyClose)
	{
		m_nMorphonogyClose = -1;
		m_bMorphonogyClose = false;
	}
	else
	{
		m_nMorphonogyClose = 0;
		m_bMorphonogyClose = true;
	}
}
void CBMAlgoDevView::OnUpdateMorphonogyClose(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nMorphonogyClose == 0);
}


/******************************** IA978 *********************************/

void CBMAlgoDevView::OnIa978DutTrayLocationCoarse()
{
	if (m_bIa978DutTrayLocationCoarse)
	{
		m_nIa978DutTrayLocationCoarse = -1;
		m_bIa978DutTrayLocationCoarse = false;
	}
	else
	{
		m_nIa978DutTrayLocationCoarse = 0;
		m_bIa978DutTrayLocationCoarse = true;
	}
}
void CBMAlgoDevView::OnUpdateIa978DutTrayLocationCoarse(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nIa978DutTrayLocationCoarse == 0);
}

void CBMAlgoDevView::OnIa978DutTrayLocation()
{
	if (m_bIa978DutTrayLocation)
	{
		m_nIa978DutTrayLocation = -1;
		m_bIa978DutTrayLocation = false;
	}
	else
	{
		m_nIa978DutTrayLocation = 0;
		m_bIa978DutTrayLocation = true;
	}
}
void CBMAlgoDevView::OnUpdateIa978DutTrayLocation(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nIa978DutTrayLocation == 0);
}

void CBMAlgoDevView::OnIa978DutSocketLocationCoarse()
{
	if (m_bIa978DutSocketLocationCoarse)
	{
		m_nIa978DutSocketLocationCoarse = -1;
		m_bIa978DutSocketLocationCoarse = false;
	}
	else
	{
		m_nIa978DutSocketLocationCoarse = 0;
		m_bIa978DutSocketLocationCoarse = true;
	}
}
void CBMAlgoDevView::OnUpdateIa978DutSocketLocationCoarse(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nIa978DutSocketLocationCoarse == 0);
}

void CBMAlgoDevView::OnIa978DutSocketLocation()
{
	if (m_bIa978DutSocketLocation)
	{
		m_nIa978DutSocketLocation = -1;
		m_bIa978DutSocketLocation = false;
	}
	else
	{
		m_nIa978DutSocketLocation = 0;
		m_bIa978DutSocketLocation = true;
	}
}
void CBMAlgoDevView::OnUpdateIa978DutSocketLocation(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nIa978DutSocketLocation == 0);
}

void CBMAlgoDevView::OnIa978SocketLocationCoarse()
{
	if (m_bIa978SocketLocationCoarse)
	{
		m_nIa978SocketLocationCoarse = -1;
		m_bIa978SocketLocationCoarse = false;
	}
	else
	{
		m_nIa978SocketLocationCoarse = 0;
		m_bIa978SocketLocationCoarse = true;
	}
}
void CBMAlgoDevView::OnUpdateIa978SocketLocationCoarse(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nIa978SocketLocationCoarse == 0);
}

void CBMAlgoDevView::OnIa978SocketLocation()
{
	if (m_bIa978SocketLocation)
	{
		m_nIa978SocketLocation = -1;
		m_bIa978SocketLocation = false;
	}
	else
	{
		m_nIa978SocketLocation = 0;
		m_bIa978SocketLocation = true;
	}
}
void CBMAlgoDevView::OnUpdateIa978SocketLocation(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nIa978SocketLocation == 0);
}

void CBMAlgoDevView::OnIa978SocketLockStatus()
{
	if (m_bIa978SocketLockStatus)
	{
		m_nIa978SocketLockStatus = -1;
		m_bIa978SocketLockStatus = false;
	}
	else
	{
		m_nIa978SocketLockStatus = 0;
		m_bIa978SocketLockStatus = true;
	}
}
void CBMAlgoDevView::OnUpdateIa978SocketLockStatus(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nIa978SocketLockStatus == 0);
}

void CBMAlgoDevView::OnIa978SocketStatus()
{
	if (m_bIa978SocketStatus)
	{
		m_nIa978SocketStatus = -1;
		m_bIa978SocketStatus = false;
	}
	else
	{
		m_nIa978SocketStatus = 0;
		m_bIa978SocketStatus = true;
	}
}
void CBMAlgoDevView::OnUpdateIa978SocketStatus(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nIa978SocketStatus == 0);
}

void CBMAlgoDevView::OnIa978PocketLocation()
{
	if (m_bIa978PocketLocation)
	{
		m_nIa978PocketLocation = -1;
		m_bIa978PocketLocation = false;
	}
	else
	{
		m_nIa978PocketLocation = 0;
		m_bIa978PocketLocation = true;
	}
}
void CBMAlgoDevView::OnUpdateIa978PocketLocation(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nIa978PocketLocation == 0);
}

void CBMAlgoDevView::OnIa978PocketStatus()
{
	if (m_bIa978PocketStatus)
	{
		m_nIa978PocketStatus = -1;
		m_bIa978PocketStatus = false;
	}
	else
	{
		m_nIa978PocketStatus = 0;
		m_bIa978PocketStatus = true;
	}
}
void CBMAlgoDevView::OnUpdateIa978PocketStatus(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nIa978PocketStatus == 0);
}

void CBMAlgoDevView::OnIa978TrayLocation()
{
	if (m_bIa978TrayLocation)
	{
		m_nIa978TrayLocation = -1;
		m_bIa978TrayLocation = false;
	}
	else
	{
		m_nIa978TrayLocation = 0;
		m_bIa978TrayLocation = true;
	}
}
void CBMAlgoDevView::OnUpdateIa978TrayLocation(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nIa978TrayLocation == 0);
}


/******************************** Active Alignment *********************************/

void CBMAlgoDevView::OnAaAreas()
{
	if (m_bAaChartArea)
	{
		m_nAaChartArea = -1;
		m_bAaChartArea = false;
	}
	else
	{
		m_nAaChartArea = 0;
		m_bAaChartArea = true;
	}
}
void CBMAlgoDevView::OnUpdateAaAreas(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAaChartArea == 0);
}


void CBMAlgoDevView::OnAaRectangles()
{
	if (m_bAaRectangles)
	{
		m_nAaRectangles = -1;
		m_bAaRectangles = false;
	}
	else
	{
		m_nAaRectangles = 0;
		m_bAaRectangles = true;
	}
}
void CBMAlgoDevView::OnUpdateAaRectangles(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAaRectangles == 0);
}


void CBMAlgoDevView::OnAaSfrMtf()
{
	if (m_bAaSfrMtf)
	{
		m_nAaSfrMtf = -1;
		m_bAaSfrMtf = false;
	}
	else
	{
		m_nAaSfrMtf = 0;
		m_bAaSfrMtf = true;
		m_nAaMtfStdDev = -1;
		m_bAaMtfStdDev = false;
		m_nAaMtfMaxMin = -1;
		m_bAaMtfMaxMin = false;
		m_nAaMtfGradMax = -1;
		m_bAaMtfGradMax = false;
		m_nAaMtfGradAve = -1;
		m_bAaMtfGradAve = false;
	}
}
void CBMAlgoDevView::OnUpdateAaSfrMtf(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAaSfrMtf == 0);
}

void CBMAlgoDevView::OnAaMtfStdDev()
{
	if (m_bAaMtfStdDev)
	{
		m_nAaMtfStdDev = -1;
		m_bAaMtfStdDev = false;
	}
	else
	{
		m_nAaMtfStdDev = 0;
		m_bAaMtfStdDev = true;
		m_nAaSfrMtf = -1;
		m_bAaSfrMtf = false;
		m_nAaMtfMaxMin = -1;
		m_bAaMtfMaxMin = false;
		m_nAaMtfGradMax = -1;
		m_bAaMtfGradMax = false;
		m_nAaMtfGradAve = -1;
		m_bAaMtfGradAve = false;
	}
}
void CBMAlgoDevView::OnUpdateAaMtfStdDev(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAaMtfStdDev == 0);
}

void CBMAlgoDevView::OnAaMtfMaxMin()
{
	if (m_bAaMtfMaxMin)
	{
		m_nAaMtfMaxMin = -1;
		m_bAaMtfMaxMin = false;
	}
	else
	{
		m_nAaMtfMaxMin = 0;
		m_bAaMtfMaxMin = true;
		m_nAaSfrMtf = -1;
		m_bAaSfrMtf = false;
		m_nAaMtfStdDev = -1;
		m_bAaMtfStdDev = false;
		m_nAaMtfGradMax = -1;
		m_bAaMtfGradMax = false;
		m_nAaMtfGradAve = -1;
		m_bAaMtfGradAve = false;
	}
}
void CBMAlgoDevView::OnUpdateAaMtfMaxMin(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAaMtfMaxMin == 0);
}

void CBMAlgoDevView::OnAaMtfGradMax()
{
	if (m_bAaMtfGradMax)
	{
		m_nAaMtfGradMax = -1;
		m_bAaMtfGradMax = false;
	}
	else
	{
		m_nAaMtfGradMax = 0;
		m_bAaMtfGradMax = true;
		m_nAaSfrMtf = -1;
		m_bAaSfrMtf = false;
		m_nAaMtfStdDev = -1;
		m_bAaMtfStdDev = false;
		m_nAaMtfMaxMin = -1;
		m_bAaMtfMaxMin = false;
		m_nAaMtfGradAve = -1;
		m_bAaMtfGradAve = false;
	}
}
void CBMAlgoDevView::OnUpdateAaMtfGradMax(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAaMtfGradMax == 0);
}

void CBMAlgoDevView::OnAaMtfGradAve()
{
	if (m_bAaMtfGradAve)
	{
		m_nAaMtfGradAve = -1;
		m_bAaMtfGradAve = false;
	}
	else
	{
		m_nAaMtfGradAve = 0;
		m_bAaMtfGradAve = true;
		m_nAaSfrMtf = -1;
		m_bAaSfrMtf = false;
		m_nAaMtfStdDev = -1;
		m_bAaMtfStdDev = false;
		m_nAaMtfGradMax = -1;
		m_bAaMtfGradMax = false;
		m_nAaMtfMaxMin = -1;
		m_bAaMtfMaxMin = false;
	}
}
void CBMAlgoDevView::OnUpdateAaMtfGradAve(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAaMtfGradAve == 0);
}

void CBMAlgoDevView::OnAaLightDistribution()
{
	if (m_bAaLightDistribution)
	{
		m_nAaLightDistribution = -1;
		m_bAaLightDistribution = false;
		m_nAaLightClose = -1;
		m_bAaLightClose = false;
		m_nAaLightOpen = -1;
		m_bAaLightOpen = false;
	}
	else
	{
		m_nAaLightDistribution = 0;
		m_bAaLightDistribution = true;
	}
}
void CBMAlgoDevView::OnUpdateAaLightDistribution(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAaLightDistribution == 0);
}

void CBMAlgoDevView::OnAaLightOpen()
{
	if (m_bAaLightOpen)
	{
		m_nAaLightOpen = -1;
		m_bAaLightOpen = false;
	}
	else
	{
		m_nAaLightOpen = 0;
		m_bAaLightOpen = true;
	}
}
void CBMAlgoDevView::OnUpdateAaLightOpen(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAaLightOpen == 0);
}

void CBMAlgoDevView::OnAaLightClose()
{
	if (m_bAaLightClose)
	{
		m_nAaLightClose = -1;
		m_bAaLightClose = false;
	}
	else
	{
		m_nAaLightClose = 0;
		m_bAaLightClose = true;
	}
}
void CBMAlgoDevView::OnUpdateAaLightClose(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAaLightClose == 0);
}


void CBMAlgoDevView::OnAaAll()
{
	m_sMTF.nMTFzpos = 0;
	int imageNum = m_imageNum;
	if (imageNum > MTF_MAX_Z_POS)
	{
		imageNum = MTF_MAX_Z_POS;
	}

	for (int n = 0; n < imageNum; n++)
	{
		m_srcW = m_ImageVolume[n].width;
		m_srcH = m_ImageVolume[n].height;
		int nS = m_srcW * m_srcH;
		m_pBmpData.width = m_ImageVolume[n].width;
		m_pBmpData.height = m_ImageVolume[n].height;
		if (m_pBmpData.pData)
			delete[]m_pBmpData.pData;
		m_pBmpData.pData = new BYTE[nS];
		memcpy(m_pBmpData.pData, m_ImageVolume[n].pData, nS);
		//CSize size(m_pBmpData.width, m_pBmpData.height);
		//SetScrollSizes(MM_TEXT, size);//滚动窗口的最大区域/////////////////////////////////////

		m_src.ptr = m_pBmpData.pData;
		m_src.size.width = m_pBmpData.width;
		m_src.size.height = m_pBmpData.height;
		m_src.linestep = m_pBmpData.width;

		if (m_dst.ptr)
			delete[]m_dst.ptr;
		m_dst.ptr = new IMG_UBYTE[m_src.size.width * m_src.size.height];
		m_dst.size.width = m_pBmpData.width;
		m_dst.size.height = m_pBmpData.height;

		/*******************   AA process   *******************************/
		if (m_bAaChartArea)
		{
			m_bmAaAlgorithm.AaChartArea(m_src, m_dst, m_sChartArea);
			if (m_bAaRectangles)
			{
				m_bmAaAlgorithm.AaChartBlobs(m_src, m_dst, m_sChartBlobs);
				if (m_bAaSfrMtf)
				{
					m_bmAaAlgorithm.AaMtfSfr(m_src, m_pMTF);
				}
				if (m_bAaMtfStdDev)
				{
					m_bmAaAlgorithm.AaMtfStdDev(m_src, m_pMTF);
				}
				if (m_bAaMtfMaxMin)
				{
					m_bmAaAlgorithm.AaMtfMaxMin(m_src, m_pMTF);
				}
				if (m_bAaMtfGradMax)
				{
					m_bmAaAlgorithm.AaMtfGradMax(m_src, m_pMTF);
				}
				if (m_bAaMtfGradAve)
				{
					m_bmAaAlgorithm.AaMtfGradAve(m_src, m_pMTF);
				}
			}
		}
		memcpy(m_sMTF.ppMTF[n], m_pMTF, CHART_BLOB_NUM * sizeof(double));
		/*************************** end **********************************/
	}
	m_sMTF.nMTFzpos = imageNum;

	Invalidate(TRUE);
}

void CBMAlgoDevView::OnLightRangeIncrease()
{
	if(m_bmAaAlgorithm.m_nLightRangeD < 255)
		m_bmAaAlgorithm.m_nLightRangeD++;

	On_Doing();

	CString str;
	str.Format(_T("AA Algorithm Light Range Increased to: %d"), m_bmAaAlgorithm.m_nLightRangeD);
	CStatusBar& statusBar = ((CMainFrame *)(AfxGetApp()->m_pMainWnd))->m_wndStatusBar;
	statusBar.SetPaneText(0, (LPCTSTR)str);
}

void CBMAlgoDevView::OnLightRangeDecrease()
{
	if (m_bmAaAlgorithm.m_nLightRangeD > 0)
		m_bmAaAlgorithm.m_nLightRangeD--;

	On_Doing();

	CString str;
	str.Format(_T("AA Algorithm Light Range Decreased to: %d"), m_bmAaAlgorithm.m_nLightRangeD);
	CStatusBar& statusBar = ((CMainFrame *)(AfxGetApp()->m_pMainWnd))->m_wndStatusBar;
	statusBar.SetPaneText(0, (LPCTSTR)str);
}
