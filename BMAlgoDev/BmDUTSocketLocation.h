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
#include<windows.h>
#include "BmSocketLocatingCoarse.h"
#define PI 3.14159265386
#define DLL_EXPORTS

#ifdef  DLL_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)   
#endif
using namespace std;

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
//enum EdgeDirection//��Ե����
//{
//	BLACKWHITE,//�ڵ���
//	WHITEBLACK, //�׵���
//	BOTH  //ȫ��
//};
//enum RectScanDirection//����ROIɨ�跽��
//{
//	LEFT, //������࣬��λ�ڶ����Ŀ�ʼ�㿴������ɨ���Ե�������
//	RIGHT //�����Ҳ࣬��λ�ڶ����Ŀ�ʼ�㿴������ɨ���Ե�������
//};
//typedef struct//̷��
//{
//	IMG_RCOORD PointStart; //start point coordinate
//	IMG_RCOORD PointEnd;   //end point coordinate
//	IMG_REAL NormalVari;   //half width of ROI suject to the direction of normal line //{NOROTATION,VERTICAL,HORIZONTAL}NOROTATION means no rotation in targeted image; VERTICAL means the straight line offered by users is parallelled to y-axis in targeted image; HORIZONTAL means the straight line offered by users is parallelled to x-axis in targeted image
//}GenerateRectRoi;  //This struct is the input to generate ROI in terms of straight line
//typedef struct
//{
//	IMG_RCOORD LeftVertex;
//	IMG_RCOORD SrcCenterCo;//the center coordinate of ROI in source image
//	IMG_RCOORD DstCenterCo;//the center coordinate of ROI 
//	IMG_SIZE RectangleSize;
//	IMG_REAL RotationAngle;
//}RecRoiStruct;//This struct describes the information in source image of targeted ROI
//typedef struct
//{
//	IMG_COORD xyInteger; //���ص�
//	IMG_RCOORD xyDecimal;//�����ص�
//	int gradient;
//	float angle;
//}edgeInformation;//��Ե��
//typedef struct HoughLineInfo
//{
//	IMG_REAL roRadius;
//	IMG_REAL theta;
//	IMG_INT votePoNum;
//
//}HOUGH_TRANS_INFO;
//typedef struct LineSlopeEquation
//{
//	IMG_REAL Slope;
//	IMG_REAL b;
//	IMG_REAL Score;
//	IMG_RCOORD PointStart;
//	IMG_RCOORD PointEnd;
//	IMG_REAL FittingError;
//}LINE_INFO;
//typedef struct {
//	GenerateRectRoi roiLine;//�߶���ʼ�����꣬��Χ���Լ��Ƿ���Ҫ��ת
//	IMG_INT threshold;//��ֵ
//	RectScanDirection scanDirect;//ֱ��ɨ�跽��
//	EdgeDirection edDirectint;//ֱ�߱�Ե����
//	IMG_RCOORD MaskStart;
//	IMG_RCOORD MaskEnd;
//}LineIn;//�ߵ�����
//typedef struct
//{
//	IMG_RCOORD startPoint;
//	IMG_RCOORD endPoint;
//}FloatLine;//ֱ�ߣ�����Ϊ��㵽�յ�
//typedef struct
//{
//	int c; //���غ͵Ĳ�
//	int p; //��Ӧ����
//}FP;//��¼�������ο������ʱ�����Ϣ
//typedef struct
//{
//	int xMin;
//	int xMax;
//	int yMin;
//	int yMax;
//
//}RegionPeakPoint;	//��ͨ���ĸ���ֵ��
//
//typedef struct
//{
//	int width;
//	int height;
//}RegionWidthHeight;	//��ͨ����

struct BmDutSocketLocationPara
{

	IMG_UBBUF prcImage = { NULL,{ 0,0 },0 };
	IMG_INT roiNormalvari[4] = { 40,30,30,30 };
	IMG_REAL threshold[4] = { 100,100,150,100 };
	EdgeDirection edDirectint[4] = { BLACKWHITE,WHITEBLACK,BLACKWHITE,WHITEBLACK };//ֱ�߱�Ե����
	IMG_INT medianFilterMasksize = 5;
	IMG_REAL PointFilterAngleTolerance = 30.f;
	IMG_REAL HoughAcceptScore = 50.f;
	IMG_REAL HoughScoreGap = 10.f;
	IMG_REAL FittingSigmaFirst = 9.f;
	IMG_REAL FittingSigmaSecond = 6.f;
	IMG_REAL ScoreAccptDis = 2.f;
	IMG_REAL angleControl = 5.f;
	IMG_REAL lowSafeRatio = 0.85;
	IMG_REAL UpSafeRatio = 1.15;
	IMG_REAL HoughAngelTolerance = 10.f;
	
	int edgeMaskColSize = 5;
	int edgeMaskRowSize = 2;
	float m_CoraseHisSpeMean = 100; //ֱ��ͼ�涨��
	float m_CoraseHisSpeVarian = 75;

	
	unsigned int m_CoraseFilterKern = 3; //��ֵ�˲�
	int m_CoraseFilterDivisor = 25;

	int m_CorasePyraLevel = 2;//����������

	int DutCorase_RoiX = 370; //���ݻ�ȷ��ROI
	int DutCorase_RoiY = 500;
	int DutCorase_RoiWidth = 770;
	int DutCorase_RoiHeight = 520;

	float m_CorasePointPercent = 0.08;//

	int m_CoraseMinArea = 1000;//Ŀ����ͨ����������С�ڸ�ֵ

	int m_CoraseWidth1 = 180;//�жϿ���Ƿ���ڸÿ��
	int m_CoraseWidth2 = 150;//�øÿ��ȷ����ֵ��

	int m_CoraseCC1AreaMin = 562; //��ͨ��1
	int m_CoraseCC1AreaMax = 3200; //ͨ�����������ͨ��

	int m_CoraseCC1X1 = 60; //λ�ù���
	
	int m_CoraseCC1X2 = 350;
	int m_CoraseCC1Y1 = 125;
	int m_CoraseCC1Y2 = 265;

	int m_CoraseCC1WidthMin = 25;//��߹���
	int m_CoraseCC1WidthMax = 80;
	int m_CoraseCC1HeightMin = 25;
	int m_CoraseCC1HeightMax = 60;

	int m_CoraseCC1Width1 = 45; //�жϿ���Ƿ���ڸÿ��
	int m_CoraseCC1Width2 = 75;//�øÿ��ȷ����ֵ��

	

	int m_CoraseROIX = 300; //��ͨ�����ĵ������Ϸ��ƶ���x��y��
	int m_CoraseROIY = 150;
	int m_CoraseROIWidth = 350;    //roi�Ŀ�
	int m_CoraseROIHeight = 600;  // roi�ĸ�

	int m_CoraseCC2Width1 = 180;//�жϿ���Ƿ���ڸÿ��
	int m_CoraseCC2Width2 = 150;//�øÿ��ȷ����ֵ��

	int m_CoraseFitNormWidth = 20; // Ѱ����ͨ��ı�Ե��� ���
	int m_CoraseFitNormHeight = 60;  //���

	int m_CoraseLeftOff = 376;  //Ŀ����ͨ��������DUT�ĸ������ƫ����
	int	m_CoraseUpOff = 497;
	int	m_CoraseRightOff = 283;    
	int	m_CoraseDownOff = 210;

};




class DLLEXPORT CBmDUTSocketLocation
{
public:
	CBmDUTSocketLocation() {
		InputPara.edDirectint[0] = { BLACKWHITE };//ֱ�߱�Ե����
		InputPara.edDirectint[1] = { WHITEBLACK };//ֱ�߱�Ե����
		InputPara.edDirectint[2] = { BLACKWHITE };//ֱ�߱�Ե����
		InputPara.edDirectint[3] = { WHITEBLACK };//ֱ�߱�Ե����
		InputPara.prcImage = { NULL,{ 0,0 },0 };
		InputPara.roiNormalvari[0] = { 25 };
		InputPara.roiNormalvari[1] = { 50 };
		InputPara.roiNormalvari[2] = { 50 };
		InputPara.roiNormalvari[3] = { 50 };
		InputPara.threshold[0] = { 100 };
		InputPara.threshold[1] = { 100 };
		InputPara.threshold[2] = { 150 };
		InputPara.threshold[3] = { 100 };
		InputPara.medianFilterMasksize = 5;
		InputPara.PointFilterAngleTolerance = 30.f;
		InputPara.HoughAcceptScore = 50.f;
		InputPara.HoughScoreGap = 10.f;
		InputPara.FittingSigmaFirst = 9.f;
		InputPara.FittingSigmaSecond = 6.f;
		InputPara.ScoreAccptDis = 2.f;
		InputPara.angleControl = 5.f;
		InputPara.lowSafeRatio = 0.85;
		InputPara.UpSafeRatio = 1.15;
		InputPara.HoughAngelTolerance = 10.f;
		InputPara.edgeMaskColSize = 5;
		InputPara.edgeMaskRowSize = 2;
		InputPara.m_CoraseHisSpeMean = 100; //ֱ��ͼ�涨��
		InputPara.m_CoraseHisSpeVarian = 75;
		InputPara.m_CoraseFilterKern = 3; //��ֵ�˲�
		InputPara.m_CoraseFilterDivisor = 25;
		InputPara.m_CorasePyraLevel = 2;//����������
		InputPara.DutCorase_RoiX = 370; //���ݻ�ȷ��ROI
		InputPara.DutCorase_RoiY = 500;
		InputPara.DutCorase_RoiWidth = 770;
		InputPara.DutCorase_RoiHeight = 520;
		InputPara.m_CorasePointPercent = 0.08;//
		InputPara.m_CoraseMinArea = 1000;//Ŀ����ͨ����������С�ڸ�ֵ
		InputPara.m_CoraseWidth1 = 180;//�жϿ���Ƿ���ڸÿ��
		InputPara.m_CoraseWidth2 = 150;//�øÿ��ȷ����ֵ��
		InputPara.m_CoraseCC1AreaMin = 562; //��ͨ��1
		InputPara.m_CoraseCC1AreaMax = 3200; //ͨ�����������ͨ��
		InputPara.m_CoraseCC1X1 = 60; //λ�ù���
		InputPara.m_CoraseCC1X2 = 350;
		InputPara.m_CoraseCC1Y1 = 125;
		InputPara.m_CoraseCC1Y2 = 265;
		InputPara.m_CoraseCC1WidthMin = 25;//��߹���
		InputPara.m_CoraseCC1WidthMax = 80;
		InputPara.m_CoraseCC1HeightMin = 25;
		InputPara.m_CoraseCC1HeightMax = 60;
		InputPara.m_CoraseCC1Width1 = 45; //�жϿ���Ƿ���ڸÿ��
		InputPara.m_CoraseCC1Width2 = 75;//�øÿ��ȷ����ֵ��
		InputPara.m_CoraseROIX = 300; //��ͨ�����ĵ������Ϸ��ƶ���x��y��
		InputPara.m_CoraseROIY = 150;
		InputPara.m_CoraseROIWidth = 350;    //roi�Ŀ�
		InputPara.m_CoraseROIHeight = 600;  // roi�ĸ�
		InputPara.m_CoraseCC2Width1 = 180;//�жϿ���Ƿ���ڸÿ��
		InputPara.m_CoraseCC2Width2 = 150;//�øÿ��ȷ����ֵ��
		InputPara.m_CoraseFitNormWidth = 20; // Ѱ����ͨ��ı�Ե��� ���
		InputPara.m_CoraseFitNormHeight = 60;  //���
		InputPara.m_CoraseLeftOff = 376;  //Ŀ����ͨ��������DUT�ĸ������ƫ����
		InputPara.m_CoraseUpOff = 497;
		InputPara.m_CoraseRightOff = 283;
		InputPara.m_CoraseDownOff = 210;
	};
	~CBmDUTSocketLocation()
	{
		
	};

public:
	bool LoadParameter(char path[]);
	_BACK_CODE DUTLocation(IMG_UBBUF pImg,
		const int angleFlg,	//0,180��
		DUTOutPutParam &param);
	

private:
	int GradientCompute(unsigned char * src, int srcWidth, int srcHeight, int rowNum, int colNum, float threshold, int * dst, vector<edgeInformation>& edgePoint);

	//coarse position
	BmDutSocketLocationPara InputPara;
	IppStatus pyramid(IMG_UBBUF src,
		unsigned char* pDst,
		int &pyramid_width,
		int &pyramid_height,
		int level);
	int  VisHistogSpeci(IMG_UBBUF src, float meanValue, float stdDevValue, IMG_UBBUF dst);
	int VisGaussianFilter(unsigned char *src, int srcHeight, int srcWidth, float sigma, unsigned int kernelSize,
		unsigned char *dst);
	int VisFilterMean(const unsigned char * src, const int srcHeight, const int srcWidth, unsigned char * dst, const unsigned char kernelSize, unsigned int divisor);
	int VisFilterMedian(const unsigned char * src, const int srcHeight, const int srcWidth, unsigned char * dst, const unsigned int winWidth);
	void  findOneConnCom_floodFill(Ipp8u *srcSeg, int srcRows, int srcCols, int newVal, IppiConnectedComp &pRegion);
	int VisSegmentCntDots(unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char * pDst, const double fWhiteRatio);
	int VisHistogram(IMG_UBBUF src, float percent, int & threshold);
	int VisOpen(unsigned char * src, int srcWidth, int srcHeight, unsigned char * dst);
	void  connComFloodFilling(Ipp8u *srcSeg, int srcRows, int srcCols, int threNum, IppiConnectedComp *&pRegion, int &ccNum);
	int getRegionInfo(unsigned char *pMarker, IppiSize roiSize, int regionNum, int *regionArea, RegionPeakPoint *regionPeakPoint, RegionWidthHeight *regionWH, IppiPoint *regionGraCenPoint);
	int VisConnCom(unsigned char * src1, int srcHeight, int srcWidth, unsigned char * src, int & ccNum, IppiPoint & cen, int & w, int & h, int & left, int & right, int & up, int & down, int & val);
	int  Line_LeastSquare(double x[], double y[], int num, double &k, double &b);
	int FitLine_Ransac(double x[], double y[], int num, double inlierPercent, int iter, double &k, double &b, double &bestError);
	int  GNERAL_LINE_FITTING(vector<IMG_RCOORD>PointCor, IMG_REAL &Slope, IMG_REAL &B, IMG_REAL Sigma, IMG_INT FLAG);
	int GNERAL_LINE_FITTING_NO_WEIGHT(vector<IMG_RCOORD> PointCor, IMG_REAL & Slope, IMG_REAL & B, IMG_REAL & ErrorRecord);
	int  DownLookCorasePosition_down(IMG_UBBUF sour,//unsigned char *sour, int sourRows, int sourCols,
		IMG_RCOORD &upleft, IMG_RCOORD &upright, IMG_RCOORD &downleft, IMG_RCOORD &downright, double &ang);
	//int DownLookCorasePosition_up(IMG_UBBUF sour, IMG_COORD &leftPoint, IMG_COORD &rightPoint, IMG_COORD &upPoint, IMG_COORD &downPoint);
	
	//line fitting

	IMG_INT VisOmmETEdgeLine_Scale(IMG_UBBUF src, GenerateRectRoi roi, RectScanDirection scanDirect, EdgeDirection edDirectint, IMG_INT threshold, LINE_INFO & FinalResult);
	IMG_INT ROI_ROTATION_COORDINATE_COMPUTE_IN_SOURCEIMAG_SL(IMG_REAL rAngle, IMG_RCOORD SrcCenterCo, IMG_RCOORD DstCenterCo, IMG_RCOORD RoiCo, IMG_RCOORD &RealCo);
	IMG_WORD DSP_RTS_R_8uC1(IMG_UBBUF prbSrc, IMG_RCOORD prcoSrcCenter, IMG_UBBUF &prbDst, IMG_RCOORD prcoDstCenter, IMG_REAL rAngle, IMG_SIZE pszOp);
	IMG_WORD RTS_R_8uC1(IMG_UBBUF prbSrc, IMG_RCOORD prcoSrcCenter, IMG_UBBUF &prbDst, IMG_RCOORD prcoDstCenter, IMG_REAL rAngle, IMG_SIZE pszOp, IMG_REAL scale);
	IMG_INT ROI_HORIZONTAL_ROTATION(IMG_UBBUF pSrc, IMG_RCOORD PointStart, IMG_RCOORD PointEnd, IMG_REAL NormalVari, RecRoiStruct &RoiStruct, IMG_UBBUF &pubDst);
	IMG_INT VisRoiGnerate_StraightLine(IMG_UBBUF pSrc, GenerateRectRoi Input, IMG_UBBUF &pubDst, RecRoiStruct &RoiStruct);
	IMG_INT Score(std::vector<LINE_INFO> &ScoreResult, std::vector<edgeInformation> EdgeDetecResult, IMG_REAL ScoreAcceptDistance);
	IMG_INT EDGEPOINT_FILTER(IMG_RCOORD PointStart, IMG_RCOORD PointEnd, RectScanDirection scanDirect, EdgeDirection edDirectint, edgeInformation *EdgePoint, IMG_INT EdgePointNumber, vector<edgeInformation> &PointFilterResult);
	IMG_VVOID HOUGH_LINE(IMG_REAL rAngle, IMG_SIZE pSrc, vector<LINE_INFO> &HoughResult_Slope, vector<edgeInformation> EdgeDetecResult, IMG_REAL HoughAcceptScore, IMG_REAL AngleTol);
	IMG_VVOID OVERLAPLINE_REJECT(vector<LINE_INFO> &FittingResult);
	IMG_INT LINE_FITTING(vector<LINE_INFO>& FittingResult, vector<LINE_INFO> OriginLine, vector<edgeInformation> EdgeDetecResult, IMG_SIZE SrcSize, IMG_REAL Sigma);
	IMG_VVOID SobelFilter_8u16s_C1_3x3(IMG_UBYTE * pSrc, IppiSize roiSize, Ipp16s *&pDst, Ipp32f *&pAngle);
	IMG_REAL getIntraClassVariance(Ipp16s* src, int srcRows, int srcCols, int &varTh);
	IMG_INT VisEdge_detection(IMG_UBYTE *srcRoi, IMG_SIZE roiSize, int threshold, IMG_WORD *&dstRoi, IMG_UBYTE *dstRoiE, edgeInformation *&edgeArray, IMG_INT &eNum);
	IMG_INT VisDoubleRoiLineFit(IMG_UBBUF src, IMG_UWORD width, LineIn lineInside1, LINE_INFO & LineOutside);
	int CBmDUTTrayLocationLineDetection(IMG_UBBUF src, IMG_UWORD Width, LineIn lineInside1, LineIn lineInside2, LineIn lineInside3, LineIn lineInside4, IMG_RCOORD PointCo[], IMG_RCOORD & CenterPoint, IMG_RCOORD & FinalPoint, IMG_REAL &RotationAngle);
	IMG_INT EDGEPOINT_FILTER_MASK(IMG_RCOORD PointStart, IMG_RCOORD PointEnd, IMG_RCOORD MaskEnd, IMG_RCOORD MaskStart, RectScanDirection scanDirect, EdgeDirection edDirectint, edgeInformation *EdgePoint, IMG_INT EdgePointNumber, vector<edgeInformation> &PointFilterResult);
	IMG_INT EDGEPOINT_FILTER_MASK(IMG_RCOORD PointStart, IMG_RCOORD PointEnd, IMG_RCOORD MaskEnd, IMG_RCOORD MaskStart, vector<edgeInformation> InitialCo, vector<edgeInformation>& PointFilterResult);
	IMG_VVOID SetSrcImage(IMG_UBBUF srcImage);
};

