#pragma once
#include"ViType.h"
#include <vector>
#include <ipp.h>
#include <mkl.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "BmErrorCode.h"
#include<windows.h>

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
//
//
//};
//enum EdgeDirection//��Ե����
//{
//	BLACKWHITE,//�ڵ���
//	WHITEBLACK, //�׵���
//	BOTH  //ȫ��
//};
//
//
//
//
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

typedef struct
{
	EdgeDirection edDirectint[4] = { BLACKWHITE,WHITEBLACK,BLACKWHITE,WHITEBLACK };//ֱ�߱�Ե����
	IMG_UBBUF prcImage = { NULL,{ 0,0 },0 };
	IMG_INT roiNormalvari[4] = { 25,50,50,50 };
	IMG_REAL threshold[4] = { 100,100,200,200 };
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
	IMG_INT edgeMaskColSize = 5;
	IMG_INT edgeMaskRowSize = 2;

	IMG_INT m_CorasePyraLevel = 2; //����������
	IMG_REAL m_CoraseHisSpeMean = 60;//ֱ��ͼ�涨����ֵ
	IMG_REAL m_CoraseHisSpeVarian = 40;
    IMG_UINT m_CoraseFilterKern = 5;//��ֵ�˲�  
	IMG_INT m_CoraseFilterDivisor = 15;
	IMG_INT medianFilterMasksize = 3;
	//��
	IMG_INT m_CoraseSL = 22;
	IMG_INT m_CoraseKL = 67;  //Ѱ�ҵķ�Χ
	IMG_INT m_CoraseReL = 15; //���ο�Ŀ��  
	
	//��
	IMG_INT m_CoraseSR = 25;
	IMG_INT m_CoraseKR = 35;
	IMG_INT m_CoraseReR = 5;
	//��
	IMG_INT m_CoraseSU = 85;
	IMG_INT m_CoraseKU = 25;
	IMG_INT m_CoraseReU = 4;
	//��
	IMG_INT m_CoraseSD = 10;
	IMG_INT m_CoraseKD = 40;
	IMG_INT m_CoraseReD = 11;

	IMG_INT m_CoraseGrayValue = 100;//�ж�1������������ο�ĻҶ�ֵ�Ĳ�ֵС��grayValue������-1
	IMG_INT m_CoraseDisLR = 655;//�ж�2�����������Ҵ����������ҵ�ľ���Ӹ��жϣ�Ĭ�Ͼ��ο��ֵ���Ϊ׼ȷ�ĵ㣬������һ����

} TrayPara;



class DLLEXPORT CBmDUTTrayLocation
{
public:
	CBmDUTTrayLocation() {
		InputPara.edDirectint[0] = { BLACKWHITE};//ֱ�߱�Ե����
		InputPara.edDirectint[1] = { WHITEBLACK };//ֱ�߱�Ե����
		InputPara.edDirectint[2] = { BLACKWHITE };//ֱ�߱�Ե����
		InputPara.edDirectint[3] = { WHITEBLACK };//ֱ�߱�Ե����

		InputPara.prcImage = { NULL,{ 0,0 },0 };
		InputPara.roiNormalvari[0] = { 25};
		InputPara.roiNormalvari[1] = { 50};
		InputPara.roiNormalvari[2] = { 50 };
		InputPara.roiNormalvari[3] = { 50 };
		InputPara.threshold[0] = { 100 };
		InputPara.threshold[1] = { 100 };
		InputPara.threshold[2] = { 200 };
		InputPara.threshold[3] = { 200 };

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

		InputPara.m_CorasePyraLevel = 2; //����������
		InputPara.m_CoraseHisSpeMean = 60;//ֱ��ͼ�涨����ֵ
		InputPara.m_CoraseHisSpeVarian = 40;
		InputPara.m_CoraseFilterKern = 5;//��ֵ�˲�  
		InputPara.m_CoraseFilterDivisor = 15;
		InputPara.medianFilterMasksize = 3;
		//��
		InputPara.m_CoraseSL = 22;
		InputPara.m_CoraseKL = 67;  //Ѱ�ҵķ�Χ
		InputPara.m_CoraseReL = 15; //���ο�Ŀ��  

								  //��
		InputPara.m_CoraseSR = 25;
		InputPara.m_CoraseKR = 35;
		InputPara.m_CoraseReR = 5;
		//��
		InputPara.m_CoraseSU = 85;
		InputPara.m_CoraseKU = 25;
		InputPara.m_CoraseReU = 4;
		//��
		InputPara.m_CoraseSD = 10;
		InputPara.m_CoraseKD = 40;
		InputPara.m_CoraseReD = 11;

		InputPara.m_CoraseGrayValue = 100;//�ж�1������������ο�ĻҶ�ֵ�Ĳ�ֵС��grayValue������-1
		InputPara.m_CoraseDisLR = 655;//�ж�2�����������Ҵ����������ҵ�ľ���Ӹ��жϣ�Ĭ�Ͼ��ο��ֵ���Ϊ׼ȷ�ĵ㣬������һ����

	}
	~CBmDUTTrayLocation()
	{

	}

private:
	TrayPara InputPara;
	IMG_COORD LightCenter;


	IMG_INT VisOmmETEdgeLine_Scale(IMG_UBBUF src, GenerateRectRoi roi, RectScanDirection scanDirect, EdgeDirection edDirectint, IMG_INT threshold, LINE_INFO & FinalResult);
	IMG_INT ROI_ROTATION_COORDINATE_COMPUTE_IN_SOURCEIMAG_SL(IMG_REAL rAngle, IMG_RCOORD SrcCenterCo, IMG_RCOORD DstCenterCo, IMG_RCOORD RoiCo, IMG_RCOORD &RealCo);
	IMG_WORD DSP_RTS_R_8uC1(IMG_UBBUF prbSrc, IMG_RCOORD prcoSrcCenter, IMG_UBBUF &prbDst, IMG_RCOORD prcoDstCenter, IMG_REAL rAngle, IMG_SIZE pszOp);
	IMG_WORD RTS_R_8uC1(IMG_UBBUF prbSrc, IMG_RCOORD prcoSrcCenter, IMG_UBBUF &prbDst, IMG_RCOORD prcoDstCenter, IMG_REAL rAngle, IMG_SIZE pszOp, IMG_REAL scale);
	IMG_INT ROI_HORIZONTAL_ROTATION(IMG_UBBUF pSrc, IMG_RCOORD PointStart, IMG_RCOORD PointEnd, IMG_REAL NormalVari, RecRoiStruct &RoiStruct, IMG_UBBUF &pubDst);
	IMG_INT VisRoiGnerate_StraightLine(IMG_UBBUF pSrc, GenerateRectRoi Input, IMG_UBBUF &pubDst, RecRoiStruct &RoiStruct);
	IMG_INT Score(std::vector<LINE_INFO> &ScoreResult, std::vector<edgeInformation> EdgeDetecResult, IMG_REAL ScoreAcceptDistance);
	//3.3ֱ��
	IMG_INT EDGEPOINT_FILTER(IMG_RCOORD PointStart, IMG_RCOORD PointEnd,  RectScanDirection scanDirect, EdgeDirection edDirectint, edgeInformation *EdgePoint, IMG_INT EdgePointNumber, vector<edgeInformation> &PointFilterResult);
	IMG_VVOID HOUGH_LINE(IMG_REAL rAngle, IMG_SIZE pSrc, vector<LINE_INFO> &HoughResult_Slope, vector<edgeInformation> EdgeDetecResult, IMG_REAL HoughAcceptScore, IMG_REAL AngleTol);
	IMG_VVOID OVERLAPLINE_REJECT(vector<LINE_INFO> &FittingResult);
	IMG_INT LINE_FITTING(vector<LINE_INFO>& FittingResult, vector<LINE_INFO> OriginLine, vector<edgeInformation> EdgeDetecResult, IMG_SIZE SrcSize, IMG_REAL Sigma);

	//2����Ե���
	IMG_VVOID SobelFilter_8u16s_C1_3x3(IMG_UBYTE * pSrc, IppiSize roiSize, Ipp16s *&pDst, Ipp32f *&pAngle);
	IMG_REAL getIntraClassVariance(Ipp16s* src, int srcRows, int srcCols, int &varTh);
	IMG_INT VisEdge_detection(IMG_UBYTE *srcRoi, IMG_SIZE roiSize, int threshold, IMG_WORD *&dstRoi, IMG_UBYTE *dstRoiE, edgeInformation *&edgeArray, IMG_INT &eNum);
	IMG_INT VisDoubleRoiLineFit(IMG_UBBUF src, IMG_UWORD width, LineIn lineInside1,LINE_INFO & LineOutside);
	int CBmDUTTrayLocationLineDetection(IMG_UBBUF src, IMG_UWORD Width, LineIn lineInside1, LineIn lineInside2, LineIn lineInside3, LineIn lineInside4, IMG_RCOORD PointCo[], IMG_RCOORD & CenterPoint, IMG_RCOORD & FinalPoint, IMG_REAL &RotationAngle);
	int DownLookCorasePosition_down(IMG_UBBUF sour, IMG_COORD & leftPoint, IMG_COORD & rightPoint, IMG_COORD & upPoint, IMG_COORD & downPoint);
	int DownLookCorasePosition_up(IMG_UBBUF sour, IMG_COORD & leftPoint, IMG_COORD & rightPoint, IMG_COORD & upPoint, IMG_COORD & downPoint);
	void findOneConnCom_floodFill(Ipp8u *srcSeg, int srcRows, int srcCols, int newVal, IppiConnectedComp &pRegion);
	void connComFloodFilling(Ipp8u *srcSeg, int srcRows, int srcCols, int threNum, IppiConnectedComp *&pRegion, int &ccNum);
	int getRegionInfo(unsigned char *pMarker,IppiSize roiSize,int regionNum,int *regionArea,RegionPeakPoint *regionPeakPoint,RegionWidthHeight *regionWH,IppiPoint *regionGraCenPoint);
	int VisOpen(unsigned char * src, int srcWidth, int srcHeight, unsigned char * dst);
	int VisSegmentCntDots(unsigned char * pSrc, const unsigned int nW, const unsigned int nH, unsigned char * pDst, const double fWhiteRatio);
	int VisConnCom(unsigned char * src1, int srcHeight, int srcWidth, unsigned char * src, int & ccNum, IppiPoint & cen, int & w, int & h, int & left, int & right, int & up, int & down, int & val);
	

	IppStatus pyramid(IMG_UBBUF src, unsigned char * pDst, int & pyramid_width, int & pyramid_height, int level);
	int VisHistogSpeci(IMG_UBBUF src, float meanValue, float stdDevValue, IMG_UBBUF dst);
	int VisGaussianFilter(unsigned char * src, int srcHeight, int srcWidth, float sigma, unsigned int kernelSize, unsigned char * dst);
	int VisMeanFilter(unsigned char * src, int srcHeight, int srcWidth, unsigned int kernelSize, int divisor, unsigned char * dst);
	int VisMedianFilter(unsigned char * src, int srcHeight, int srcWidth, unsigned char * dst, int masksize);

	_BACK_CODE SetSrcImage(IMG_UBBUF srcImage, IMG_INT angleFlg);

	IMG_INT EDGEPOINT_FILTER_MASK(IMG_RCOORD PointStart, IMG_RCOORD PointEnd, IMG_RCOORD MaskEnd, IMG_RCOORD MaskStart, RectScanDirection scanDirect, EdgeDirection edDirectint, edgeInformation *EdgePoint, IMG_INT EdgePointNumber, vector<edgeInformation> &PointFilterResult);
	IppStatus VisPyramids(unsigned char* pSrc, IppiSize roiSize, IppiPyramid*&pPyrStruct,unsigned char ** &pPyrImage, int level);
	IMG_INT GetResult(IMG_RCOORD PointCo[], IMG_RCOORD & FinalCo, IMG_RCOORD & CenterPoint, IMG_REAL &RotationAngle);
	int GradientCompute(unsigned char * src, int srcWidth, int srcHeight, int rowNum, int colNum, float threshold, int * dst, vector<edgeInformation>& edgePoint);
	IMG_INT EDGEPOINT_FILTER_MASK(IMG_RCOORD PointStart, IMG_RCOORD PointEnd, IMG_RCOORD MaskEnd, IMG_RCOORD MaskStart, vector<edgeInformation> InitialCo, vector<edgeInformation>& PointFilterResult);


public:
	


	bool LoadParameter(char path[]);



	/**********************************************/
	// DUTLocation, �����ĸ����㡢���ġ�����λ�����ꡢ��ת�Ƕȣ�
	// Input:
	// IMG_UBBUF pImg; ����ͼƬ
	// Output:
	// DUTOutPutParam &param
	// Return:
	//     _CODE_OK; ��λ�ɹ�
	//     _CODE_LOCATE_FAIL - ��λʧ��
	// Author: Ling Tan/4/25/2017
	/**********************************************/


	_BACK_CODE DUTLocation(IMG_UBBUF pImg, int angleFlg, DUTOutPutParam & param);
	_BACK_CODE CoarseLocation(IMG_UBBUF pImg, IMG_INT angleFlg, IMG_COORD & LeftCo, IMG_COORD & RightCo, IMG_COORD & UpCo, IMG_COORD & DownCo, IMG_COORD & LightCenCo);
	_BACK_CODE ExactLocation(IMG_UBBUF prcImage, IMG_INT angleFlg, IMG_COORD LeftCo, IMG_COORD RightCo, IMG_COORD UpCo, IMG_COORD DownCo, DUTOutPutParam & param);



	


	
};