#include "stdafx.h"
#include "BmSocketLockStatus.h"
#include<windows.h>

/**********************************************/
// VisBC_Binaryzation, 功能说明:对图像进行二值化。
// Input:
// 	Ipp8u *srcRoi, 输入图片
//	IppiSize Roi,感兴趣区域大小
//	int threshold,梯度强度的阈值（理论上是大于0小于1250的整数）。如果用户不知道阈值设为多少合适，可以输入0（算法自动获取合适的阈值）。
//     ...
// Output:
// 	IMG_UBBUF src,输出图像
//     ...
// Return:
//     0 - 正常
//     -1 - 暂无待定
//     -2 - 暂无待定
// Author: Huang Yige/03/18/2017
/**********************************************/
_BACK_CODE CBmSocketLockStatus::VisSS_Binaryzation(Ipp8u *srcRoi, IppiSize Roi)
{
	__try {
		Ipp8u pMin = 0;

		int threshold = 0;
		int srcDstStep = Roi.width * sizeof(Ipp8u);

		ippiMin_8u_C1R(srcRoi, srcDstStep, Roi, &pMin);

		//printf("%d\n", pMin);


		if (pMin > 13)
		{
			threshold = 235;
		}
		if (pMin > 10 && pMin < 14)
		{
			threshold = 230;
		}
		if (pMin > 6 && pMin < 11)
		{
			threshold = 200;
		}
		if (pMin < 7)
		{
			threshold = 170;
		}

		ippiThreshold_LTValGTVal_8u_C1IR(srcRoi, srcDstStep, Roi, threshold, 0, threshold, 255);

#ifdef DEBUG_BmSocketStatus
		FILE *hp2;
		hp2 = fopen("SS_Binaryzation.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp2, "%d ", srcRoi[i*Roi.width + j]);
			}
			fprintf(hp2, "\n");
		}
		fclose(hp2);
#endif
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)

	{
		return _CODE_THROW;
	}
}


/**********************************************/
// VisBC_OpenClose, 功能说明:对图像进行开运算，消除小的Blob。
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
// Author: Huang Yige/03/18/2017
/**********************************************/
_BACK_CODE CBmSocketLockStatus::VisSS_OpenClose(Ipp8u *srcRoi, IppiSize Roi)
{
	__try {
		IppiMorphAdvState *pSpec1 = NULL;
		Ipp8u* pBuffer1 = NULL;
		Ipp8u pMask1[25] = { 1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1 };
		IppiSize maskSize1 = { 5,5 };

		IppiMorphAdvState *pSpec2 = NULL;
		Ipp8u* pBuffer2 = NULL;
		Ipp8u pMask2[25] = { 1,1,1,1,0,1,1,1,1 };
		IppiSize maskSize2 = { 3,3 };

		Ipp8u *dstRoi;
		dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

		int srcStep = Roi.width * sizeof(Ipp8u);
		int dstStep = Roi.width * sizeof(Ipp8u);
		int dstSize = Roi.width;

		int specSize = 0, bufferSize = 0;
		IppiBorderType borderType = ippBorderRepl;
		Ipp8u borderValue = 0;

		/*ippiMorphAdvGetSize_8u_C1R(Roi, maskSize1, &specSize, &bufferSize);

		pSpec1 = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
		pBuffer1 = (Ipp8u*)ippsMalloc_8u(bufferSize);

		ippiMorphAdvInit_8u_C1R(Roi, pMask1, maskSize1, pSpec1, pBuffer1);

		ippiMorphOpenBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec1, pBuffer1);*/

		specSize = 0;
		bufferSize = 0;

		ippiMorphAdvGetSize_8u_C1R(Roi, maskSize2, &specSize, &bufferSize);

		pSpec2 = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
		pBuffer2 = (Ipp8u*)ippsMalloc_8u(bufferSize);

		ippiMorphAdvInit_8u_C1R(Roi, pMask2, maskSize2, pSpec2, pBuffer2);

		//ippiMorphOpenBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec2, pBuffer2);

		ippiMorphOpenBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec2, pBuffer2);

		ippiMorphCloseBorder_8u_C1R(dstRoi, srcStep, srcRoi, dstStep, Roi, borderType, borderValue, pSpec2, pBuffer2);

		/*specSize = 0;
		bufferSize = 0;

		ippiMorphAdvGetSize_8u_C1R(Roi, maskSize1, &specSize, &bufferSize);

		pSpec1 = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
		pBuffer1 = (Ipp8u*)ippsMalloc_8u(bufferSize);

		ippiMorphAdvInit_8u_C1R(Roi, pMask1, maskSize1, pSpec1, pBuffer1);

		ippiMorphCloseBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec1, pBuffer1);

		memcpy(srcRoi, dstRoi, Roi.width*Roi.height * sizeof(Ipp8u));*/

#ifdef DEBUG_BmSocketStatus
		FILE *hp4;
		hp4 = fopen("SS_Open.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp4, "%d ", dstRoi[i*Roi.width + j]);
			}
			fprintf(hp4, "\n");
		}
		fclose(hp4);
		FILE *hp5;
		hp5 = fopen("Close.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp5, "%d ", srcRoi[i*Roi.width + j]);
			}
			fprintf(hp5, "\n");
		}
		fclose(hp5);
#endif // !1

		ippsFree(pBuffer1);
		ippsFree(pSpec1);
		ippsFree(pBuffer2);
		ippsFree(pSpec2);
		free(dstRoi);
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)

	{

		return _CODE_THROW;

	}
}



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
// Author: Huang Yige/03/18/2017
/**********************************************/
bool CBmSocketLockStatus::LoadParameter(char path[]) {
	LPSTR temp = (LPSTR)new char[MAX_PATH];

	GetPrivateProfileStringA("BmSocketLockStatus", "CircleWidth ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.CircleWidth = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLockStatus", "CircleHeight ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.CircleHeight = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLockStatus", "LockRangeWidth ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.LockRangeWidth = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLockStatus", "LockRangeHeight ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.LockRangeHeight = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLockStatus", "circleLockHeight ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.circleLockHeight = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLockStatus", "circleLockWidth ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.circleLockWidth = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLockStatus", "blackHeight ", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.blackHeight = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLockStatus", "halfBlackHeight", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.halfBlackHeight = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLockStatus", "whiteHeight", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.whiteHeight = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLockStatus", "lockEffHeight", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.lockEffHeight = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLockStatus", "botHeight", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.botHeight = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLockStatus", "roiBotVerBgnHeight", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.roiBotVerBgnHeight = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLockStatus", "avgErrThr", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.avgErrThr = atof((char*)temp);
	GetPrivateProfileStringA("BmSocketLockStatus", "ratioThr", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.ratioThr = atof((char*)temp);

	GetPrivateProfileStringA("BmSocketLockStatus", "diffThr", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.diffThr = atoi((char*)temp);

	GetPrivateProfileStringA("BmSocketLockStatus", "bigRatioThr", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.bigRatioThr = atof((char*)temp);
	GetPrivateProfileStringA("BmSocketLockStatus", "smallRatioThr", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.smallRatioThr = atof((char*)temp);

	GetPrivateProfileStringA("BmSocketLockStatus", "roiEffThr1", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.roiEffThr1 = atoi((char*)temp);
	GetPrivateProfileStringA("BmSocketLockStatus", "roiEffThr2", "FAULT", temp, MAX_PATH, path);
	if (0 == strcmp(temp, "FAULT")) {
		delete temp;
		return 0;
	}
	lockParas.roiEffThr2 = atoi((char*)temp);

	delete temp;
	return TRUE;
}
_BACK_CODE CBmSocketLockStatus::lockBinaryzation(Ipp8u *srcRoi, IppiSize Roi)
{
	__try {
		int srcDstStep = Roi.width * sizeof(Ipp8u);
		ippiThreshold_LTValGTVal_8u_C1IR(srcRoi, srcDstStep, Roi, threshold, 0, threshold, 255);
		/*for (int i = 0; i < Roi.height; i++) {
			for (int j = 0; j < Roi.width; j++) {
				if (srcRoi[i*Roi.width + j] <= threshold) {
					srcRoi[i*Roi.width + j] = 0;
				}
				else {
					srcRoi[i*Roi.width + j] = 255;
				}
			}
		}*/
#ifdef DEBUG_BmSocketStatus
		FILE *hp2;
		hp2 = fopen("lock_Binaryzation.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				if (i == 63 && j == 6) {
					i = i;
				}
				fprintf(hp2, "%d ", srcRoi[i*Roi.width + j]);
			}
			fprintf(hp2, "\n");
		}
		fclose(hp2);
#endif
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)

	{
		return _CODE_THROW;
	}
}

_BACK_CODE CBmSocketLockStatus::lockEffective(Ipp8u * srcRoi, IppiSize Roi, int &offset, bool &effective, standardPoint twoPoint)
{
	__try {
		int *grayAve = new int[Roi.width];
		bool effective = true;
		for (int i = 0; i < Roi.width; i++) {
			grayAve[i] = 0;
		}
		for (int i = 0; i < Roi.width; i++) {
			for (int j = 0; j < curLockEffHeight; j++) {
				grayAve[i] += srcRoi[j*Roi.width + i];
			}
		}
		for (int i = 0; i < Roi.width; i++) {
			grayAve[i] = grayAve[i] / curLockEffHeight;
		}

		int maxDiff = 0;
		int maxDiffIndex = 0;
		for (int i = 0; i < Roi.width - 1; i++) {
			int diff = abs(grayAve[i] - grayAve[i + 1]);
			if (diff > maxDiff) {
				maxDiff = diff;
				maxDiffIndex = i;
			}
		}
	//	int thr = 3;
		if (maxDiff >= lockParas.roiEffThr1) {
			effective = false;
			if (grayAve[maxDiffIndex] > grayAve[maxDiffIndex + 1]) {
				for (int i = maxDiffIndex; i < Roi.width - 1; i++) {
					int diff = grayAve[i] - grayAve[i + 1];
					if (diff > lockParas.roiEffThr2) {
						offset = i + 1;
					}
				}
			}
			else {
				for (int i = maxDiffIndex + 1; i > 1; i--) {
					int diff = grayAve[i] - grayAve[i - 1];
					if (diff > lockParas.roiEffThr2) {
						offset = 0 - (Roi.width - i - 1);
					}
				}
			}
		}

		delete grayAve;
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)

	{
		return _CODE_THROW;

	}
}

_BACK_CODE CBmSocketLockStatus::VisSS_lockStatus(Ipp8u *srcRoi, Ipp8u *roiImg, IppiSize Roi, standardPoint twoPoint, int exceedNum)
{
	__try {
		int blackSize = 0;
		int whiteSize = 0;
		bool blackPass = false;
		bool whitePass = false;
		for (int i = 0; i < curBlackHeight; i++) {
			for (int j = 0; j < Roi.width; j++) {
				if (i == 63 && j == 6) {
					i = i;
				}
				if (srcRoi[i*Roi.width + j] != 255) {
					blackSize++;
				}
			}
		}
		int blackRegionSize = Roi.width * curBlackHeight;
		float blackRatio = (float)blackSize / (float)blackRegionSize;
		if (blackRatio > lockParas.ratioThr) {
			blackPass = true;
		}

		int whiteHeight = 0;
		if (curBlackHeight + curHalgBlackHeight + curWhiteHeight > Roi.height) {
			whiteHeight = Roi.height - curBlackHeight - curHalgBlackHeight;
		}
		else {
			whiteHeight = curWhiteHeight;
		}
		for (int i = curBlackHeight + curHalgBlackHeight; i < curBlackHeight + curHalgBlackHeight + whiteHeight; i++) {
			for (int j = 0; j < Roi.width; j++) {
				if (srcRoi[i*Roi.width + j] != 0) {
					whiteSize++;
				}
			}
		}
		int WhiteRegionSize = Roi.width * whiteHeight;
		float whiteRatio = (float)whiteSize / (float)WhiteRegionSize;
		if (whiteRatio > lockParas.ratioThr) {
			whitePass = true;
		}
		int *whiteAvg = new int[Roi.width];
		for (int i = 0; i < Roi.width; i++) {
			whiteAvg[i] = 0;
		}
	for (int i = 0; i < Roi.width; i++) {
		for (int j = curBlackHeight + curHalgBlackHeight; j < curBlackHeight + curHalgBlackHeight + whiteHeight; j++) {
			whiteAvg[i] += roiImg[j*Roi.width + i];
		}
	}
	for (int i = 0; i < Roi.width; i++) {
		whiteAvg[i] = whiteAvg[i] / whiteHeight;
	}

	bool whiteErrPass = true;
	for (int i = 0; i < Roi.width - 1; i++) {
		int diff = whiteAvg[i] - whiteAvg[i + 1];
		if (diff > lockParas.diffThr) {
			whiteErrPass = false;
			break;
		}
	}

		
		for (int i = 0; i < Roi.width; i++) {
			whiteAvg[i] = 0;
		}
		int curBotVerBgn = curRoiBotHeight;
		if (whiteErrPass == true) {
			if (curRoiBotHeight + curBotHeight < Roi.height) {
				whiteErrPass = false;
				for (int j = curRoiBotHeight; j < curRoiBotHeight + curBotHeight; j++) {
					for (int i = 0; i < Roi.width; i++) {
						whiteAvg[i] = roiImg[j*Roi.width + i];
					}
					if (whiteAvg[Roi.width - 1] < whiteAvg[0] * 2 || whiteAvg[Roi.width - 1] == whiteAvg[0]) {
						whiteErrPass = true;
						break;
					}
				}
			}
		}

		if (blackPass && whitePass && whiteErrPass) {
			delete whiteAvg;
			return _CODE_SOCKETLOCK;
		}
		if (whiteRatio > lockParas.bigRatioThr && blackRatio > lockParas.smallRatioThr || whiteRatio > lockParas.smallRatioThr && blackRatio > lockParas.bigRatioThr) {
			if (whiteErrPass == false) {
				delete whiteAvg;
				return _CODE_SOCKETUNLOCK;
			}
			else {
				delete whiteAvg;
				return _CODE_SOCKETLOCK;
			}
		}
		delete whiteAvg;
		return _CODE_SOCKETUNLOCK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)

	{
		return _CODE_THROW;
	}
}

_BACK_CODE CBmSocketLockStatus::calcVerInfo(standardPoint twoPoint, IppiSize Roi)
{
	__try {

		//int CirclesDis = 488;   //左圆的最下点与右圆的最上点之间的高度
		//int CircleHeight = 685;   //两个圆心之间的高度
	//	int blackHeight = 258;
		int curCirclesHeight = twoPoint.rightPoint.CenterY - twoPoint.leftPoint.CenterY;
		if (curCirclesHeight <= 0) {
			return _CODE_THROW;
		}
		curBlackHeight = curCirclesHeight * lockParas.blackHeight / lockParas.CircleHeight;
//		int halfBlackHeight = 80;
//		halfBlackHeight = 76;
		curHalgBlackHeight = lockParas.halfBlackHeight * curCirclesHeight / lockParas.CircleHeight;
//		int whiteHeight = 240;
		curWhiteHeight = lockParas.whiteHeight * curCirclesHeight / lockParas.CircleHeight;
//		int lockEffHeight = 136;
		curLockEffHeight = lockParas.lockEffHeight*curCirclesHeight / lockParas.CircleHeight;
//		int botHeight = 135;
		curBotHeight = lockParas.botHeight*curCirclesHeight / lockParas.CircleHeight;
	//	int roiBotVerBgnHeight = 610;
		curRoiBotHeight = lockParas.roiBotVerBgnHeight*curCirclesHeight / lockParas.CircleHeight;

		if (curBlackHeight + curHalgBlackHeight > Roi.height) {
			return _CODE_THROW;
		}
		else {
			return _CODE_OK;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)

	{

		return _CODE_THROW;

	}
}

_BACK_CODE CBmSocketLockStatus::calcThr(Ipp8u *srcRoi, IppiSize Roi, bool &blackWhiteEqual)
{
	__try {
		int blackAvgValue = 0, whiteAvgValue = 0;

		for (int i = 0; i < curBlackHeight; i++) {
			for (int j = 0; j < Roi.width; j++) {
				blackAvgValue += srcRoi[i*Roi.width + j];
			}
		}
		blackAvgValue = blackAvgValue / (curBlackHeight * Roi.width);

		int whiteHeight = 0;
		if (curBlackHeight + curHalgBlackHeight + curWhiteHeight > Roi.height) {
			whiteHeight = Roi.height - curBlackHeight - curHalgBlackHeight;
		}
		else {
			whiteHeight = curWhiteHeight;
		}
		for (int i = curBlackHeight + curHalgBlackHeight; i < curBlackHeight + curHalgBlackHeight + whiteHeight; i++) {
			for (int j = 0; j < Roi.width; j++) {
				whiteAvgValue += srcRoi[i*Roi.width + j];
			}
		}
		whiteAvgValue = whiteAvgValue / (whiteHeight * Roi.width);
		threshold = blackAvgValue + (whiteAvgValue - blackAvgValue) * 0.2;
		if (blackAvgValue - whiteAvgValue > 30 && blackAvgValue > 90) {		
			_BACK_CODE status  = detectDutAbnormal(srcRoi, Roi.height, Roi.width, 3);
			if(status != _CODE_OK)
			return status;
		}
		if (whiteAvgValue - blackAvgValue < 3) {
			return _CODE_SOCKETUNLOCK;
		}
	
		if (threshold < 0) {
			threshold = 0;
		}
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)

	{
		return _CODE_THROW;
	}
}
_BACK_CODE CBmSocketLockStatus::VisSS_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma)
{
	__try {
		int srcStep = src.size.width * sizeof(Ipp8u);
		Ipp32f      rate = 2.f;                  // Neighbour levels ratio
		Ipp16s kernel[3] = { 0,3,0 };		// Separable symmetric kernel of odd length

		IppiSize roiSize = { src.size.width ,src.size.height };

		int pyrBufferSize = 0;
		int pyrStructSize = 0;
		IppiPyramid *pPyrStruct = NULL;
		Ipp8u       *pPyrBuffer = NULL;
		Ipp8u       *pPyrStrBuffer = NULL;

		int      pyrLStateSize = 0;
		int      pyrLBufferSize = 0;
		Ipp8u   *pPyrLStateBuf = NULL;
		Ipp8u   *pPyrLBuffer = NULL;
		Ipp8u **pPyrImage = NULL;

		// Computes the temporary work buffer size
		ippiPyramidGetSize(&pyrStructSize, &pyrBufferSize, level, roiSize, rate);


		pPyrBuffer = (Ipp8u*)malloc(pyrBufferSize * sizeof(Ipp8u));
		pPyrStrBuffer = (Ipp8u*)malloc(pyrStructSize * sizeof(Ipp8u));

		// Initializes Gaussian structure for pyramids
		ippiPyramidInit(&pPyrStruct, level, roiSize, rate, pPyrStrBuffer, pPyrBuffer);

		// Correct maximum scale level 
		level = pPyrStruct->level;

		// Allocate structures to calculate pyramid layers
		ippiPyramidLayerDownGetSize_8u_C1R(roiSize, rate, 3, &pyrLStateSize, &pyrLBufferSize);

		pPyrLStateBuf = (Ipp8u*)malloc(pyrLStateSize * sizeof(Ipp8u));
		pPyrLBuffer = (Ipp8u*)malloc(pyrLBufferSize * sizeof(Ipp8u));

		// Initialize the structure for creating a lower pyramid layer
		ippiPyramidLayerDownInit_8u_C1R((IppiPyramidDownState_8u_C1R**)&pPyrStruct->pState, roiSize, rate, kernel, 3, IPPI_INTER_LINEAR, pPyrLStateBuf, pPyrLBuffer);

		// Allocate pyramid layers
		pPyrImage = pPyrStruct->pImage;
		pPyrImage[0] = src.ptr;
		pPyrStruct->pStep[0] = srcStep;
		for (int i = 1; i <= level; i++)
		{
			pPyrImage[i] = (Ipp8u*)malloc((pPyrStruct->pRoi[i].width) * (pPyrStruct->pRoi[i].height) * sizeof(Ipp8u));
			pPyrStruct->pStep[i] = (pPyrStruct->pRoi[i].width) * sizeof(Ipp8u);
		}

		// Perform downsampling of the image with 5x5 Gaussian kernel
		for (int i = 1; i <= level; i++)
		{
			ippiPyramidLayerDown_8u_C1R(pPyrImage[i - 1], pPyrStruct->pStep[i - 1], pPyrStruct->pRoi[i - 1], pPyrImage[i], pPyrStruct->pStep[i], pPyrStruct->pRoi[i], (IppiPyramidDownState_8u_C1R*)pPyrStruct->pState);
		}

		Roi.width = pPyrStruct->pRoi[2].width;
		Roi.height = pPyrStruct->pRoi[2].height;

		//srcRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));
		memcpy(srcRoi, pPyrImage[2], Roi.width*Roi.height * sizeof(Ipp8u));

#ifdef DEBUG_BmSocketStatus
		FILE *hp1;
		hp1 = fopen("SS_pyramid.txt", "w");
		for (int i = 0; i < Roi.height; i++)
		{
			for (int j = 0; j < Roi.width; j++)
			{
				fprintf(hp1, "%d ", srcRoi[i*Roi.width + j]);
			}
			fprintf(hp1, "\n");
		}
		fclose(hp1);
#endif // !1

		for (int i = 1; i <= level; i++)
			free(pPyrImage[i]);
		free(pPyrStrBuffer);
		free(pPyrBuffer);
		free(pPyrLBuffer);
		free(pPyrLStateBuf);
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)

	{
		return _CODE_THROW;
	}
}

_BACK_CODE CBmSocketLockStatus::GetSocketOpenStatus(const IMG_UBBUF srcRoi, const int angleFlg)

{
	__try {
		_BACK_CODE test;
		char configurationfile[100];
		sprintf_s(configurationfile, ".\\BmSocketLocatingCoarse.ini");
		bool boolreturn = locating.LoadParameter(configurationfile);
		test = locating.SocketLocatingCoarse(srcRoi, angleFlg);

		if (test != _CODE_OK) {
			return test;
		}
		standardPoint twoPoint;
		IMG_UBBUF srcPicInfo;

		//Ipp8u* dst;
		srcPicInfo.ptr = (Ipp8u*)malloc(srcRoi.size.width*srcRoi.size.height * sizeof(Ipp8u));
		//dst = srcPicInfo.ptr;
		locating.VisSS_GetPointZero(twoPoint);
		locating.VisSS_GetPictureZero(srcPicInfo);
		IppiSize Roi;
		Roi.width = srcPicInfo.size.width;
		Roi.height = srcPicInfo.size.height;
	
		
#ifdef DEBUG_BmSocketStatus
		   FILE *hp1;
			hp1 = fopen("SS_pyramid.txt", "w");
			for (int i = 0; i < Roi.height; i++)
			{
				for (int j = 0; j < Roi.width; j++)
				{
					fprintf(hp1, "%d ", srcPicInfo.ptr[i*Roi.width + j]);
				}
				fprintf(hp1, "\n");
			}
			fclose(hp1);
#endif



		IppiSize LockDetectRoi;
		//	int CircleDiameter = 192;  //根据一幅图像丈量
//		int CircleWidth = 579;   //两个圆心之间的宽度
//		int CircleHeight = 685;   //两个圆心之间的高度
//		int LockRangeWidth = 40; //根据同一幅图像的丈量,lock区域的宽度
	//	LockRangeWidth = 55;
	//	int LockRangeHeight = 565; //lock区域的高度
		int curCircleWidth = twoPoint.rightPoint.CenterX - twoPoint.leftPoint.CenterX;//当前图两圆心之间的X方向距离
		int curCirclesHeight = twoPoint.rightPoint.CenterY - twoPoint.leftPoint.CenterY;//当前图两圆心之间的Y方向距离
		LockDetectRoi.width = curCircleWidth * lockParas.LockRangeWidth / lockParas.CircleWidth;
		LockDetectRoi.height = curCirclesHeight * lockParas.LockRangeHeight / lockParas.CircleHeight;
//		int circleLockHeight = 239; //左圆的圆心与Lock检测区域之间的高度

		int lockVerBgn = curCirclesHeight * lockParas.circleLockHeight / lockParas.CircleHeight + twoPoint.leftPoint.CenterY;
		LockDetectRoi.height = Roi.height - lockVerBgn;
		//int circleLockWidth = 278;
//		int circleLockWidth = 370; //左圆的圆心与Lock检测区域的起始位置之间的距离
		int lockHorBgn = twoPoint.leftPoint.CenterX - lockParas.circleLockWidth * curCircleWidth / lockParas.CircleWidth;

		Ipp8u* RoiImg;
		RoiImg = (Ipp8u*)malloc(LockDetectRoi.width*LockDetectRoi.height * sizeof(Ipp8u));

		int exceedNum = LockDetectRoi.height + lockVerBgn - Roi.height;
		if (exceedNum > 0) {
			LockDetectRoi.height = LockDetectRoi.height - exceedNum;
		}
		for (int i = 0; i < LockDetectRoi.height; i++) {
			for (int j = 0; j < LockDetectRoi.width; j++) {
				if ((i + lockVerBgn)*Roi.width + j + lockHorBgn >= (Roi.width * Roi.height)) {  //越界判断
					return _CODE_THROW;
				}
				RoiImg[i*LockDetectRoi.width + j] = srcPicInfo.ptr[(i + lockVerBgn)*Roi.width + j + lockHorBgn];
			}
		}
		_BACK_CODE curLockStatus;
		curLockStatus = calcVerInfo(twoPoint, LockDetectRoi);
		if (curLockStatus != _CODE_OK) {
			free(RoiImg);
			free(srcPicInfo.ptr);
			return curLockStatus;
		}
		int offset = 0;
		
		bool roiEffective = false;
		curLockStatus = lockEffective(RoiImg, LockDetectRoi, offset, roiEffective, twoPoint);
		if (curLockStatus != _CODE_OK) {
			free(RoiImg);
			free(srcPicInfo.ptr);
			return curLockStatus;
		}
		if (roiEffective == false) {    //重新选取ROI
			for (int i = 0; i < LockDetectRoi.height; i++) {
				for (int j = 0; j < LockDetectRoi.width; j++) {
					if ((i + lockVerBgn)*Roi.width + j + lockHorBgn >= (Roi.width * Roi.height)) {
						i = i;
					}
					RoiImg[i*LockDetectRoi.width + j] = srcPicInfo.ptr[(i + lockVerBgn)*Roi.width + j + lockHorBgn + offset];
					//	RoiImg[i*LockDetectRoi.width + j] = 0;
				}
			}
		}

#ifdef DEBUG_BmSocketStatus
		FILE *hp2;
		hp2 = fopen("SS_roi.txt", "w");
		for (int i = 0; i < LockDetectRoi.height; i++)
		{
			for (int j = 0; j < LockDetectRoi.width; j++)
			{
				fprintf(hp2, "%d ", RoiImg[i*LockDetectRoi.width + j]);
			}
			fprintf(hp2, "\n");
		}
		fclose(hp2);
#endif
		bool blackWhiteEqual = false;
		curLockStatus = calcThr(RoiImg, LockDetectRoi, blackWhiteEqual);
		if (curLockStatus != _CODE_OK) {
			free(RoiImg);
			free(srcPicInfo.ptr);
			return curLockStatus;
		}
		Ipp8u* RoiSrc;
		RoiSrc = (Ipp8u*)malloc(LockDetectRoi.width*LockDetectRoi.height * sizeof(Ipp8u));
		memcpy(RoiSrc, RoiImg, LockDetectRoi.width*LockDetectRoi.height * sizeof(Ipp8u));
		curLockStatus = lockBinaryzation(RoiImg, LockDetectRoi);
		if (curLockStatus != _CODE_OK) {
			free(RoiImg);
			free(srcPicInfo.ptr);
			free(RoiSrc);
			return curLockStatus;
		}
		curLockStatus = VisSS_OpenClose(RoiImg, LockDetectRoi);
		if (curLockStatus != _CODE_OK) {
			free(RoiImg);
			free(srcPicInfo.ptr);
			free(RoiSrc);
			return curLockStatus;
		}

		curLockStatus = VisSS_lockStatus(RoiImg, RoiSrc, LockDetectRoi, twoPoint, exceedNum);
		free(srcPicInfo.ptr);
		free(RoiSrc);
		free(RoiImg);
		return curLockStatus;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)

	{
		return _CODE_THROW;
	}
}

_BACK_CODE CBmSocketLockStatus::VisFilterSobel_8u8u(const unsigned char *src, const int srcHeight, const int srcWidth, unsigned char *dst, const unsigned char winWidth)
{
	__try

	{
		IppiMaskSize mask;

		if (3 == winWidth)

			mask = ippMskSize3x3;

		else if (5 == winWidth)

			mask = ippMskSize5x5;

		else if (7 == winWidth)

			mask = (IppiMaskSize)77;

		else

			return _CODE_NG;



		IMG_UBYTE * pSrc = (IMG_UBYTE *)src;

		IppiSize roiSize;

		roiSize.width = srcWidth;

		roiSize.height = srcHeight;

		int bufLen = roiSize.width * roiSize.height;

		Ipp16s *pDst = new Ipp16s[bufLen];



		IppiBorderType bordertype = ippBorderRepl; //Border is replicated from the edge pixels

		Ipp16s *pHoriz, *pVert;



		int srcStep = roiSize.width * sizeof(Ipp8u);



		int dstStep = roiSize.width * sizeof(Ipp16s);

		int angleStep = roiSize.width * sizeof(Ipp32f);

		int bufferSize;



		Ipp8u *pBuffer;

		IppNormType normType = ippNormL2;//input gradient magnitude



		pVert = new Ipp16s[bufLen];

		pHoriz = new Ipp16s[bufLen];

		Ipp32f *pAngle = new Ipp32f[bufLen];

		//       pDst = new Ipp16s[bufLen];



		ippiGradientVectorGetBufferSize(roiSize, mask, ipp16s, 1, &bufferSize);

		pBuffer = new Ipp8u[bufferSize];

		ippiGradientVectorSobel_8u16s_C1R(pSrc, srcStep, pVert, dstStep, pHoriz, dstStep, pDst, dstStep, pAngle, angleStep, roiSize, mask, normType, bordertype, NULL, pBuffer);

		for (int i = 0; i < bufLen; i++)

		{

			//pDst[i] /= winWidth;

			if (pDst[i] > 255 || pDst[i] < -255)

				dst[i] = 255;

			else

				dst[i] = abs(pDst[i]);

		}
		delete[] pBuffer;

		delete[] pVert;

		delete[] pHoriz;

		delete[] pAngle;

		delete[] pDst;

	}

	__except (EXCEPTION_EXECUTE_HANDLER)

	{

		return _CODE_THROW;

	}
}

_BACK_CODE CBmSocketLockStatus::detectDutAbnormal(const unsigned char *src, const int srcHeight, const int srcWidth, const unsigned char winWidth)
{
	__try {
		unsigned char *dst = (Ipp8u*)malloc(srcHeight*srcWidth * sizeof(Ipp8u));
		_BACK_CODE STATUS = VisFilterSobel_8u8u(src, srcHeight, srcWidth, dst, winWidth);
#ifdef DEBUG_BmSocketStatus
		FILE *hp2;
		hp2 = fopen("SS_grad.txt", "w");
		for (int i = 0; i < srcHeight; i++)
		{
			for (int j = 0; j < srcWidth; j++)
			{
				fprintf(hp2, "%d ", dst[i*srcWidth + j]);
			}
			fprintf(hp2, "\n");
		}
		fclose(hp2);
#endif
		int blackAvgValue = 0, whiteAvgValue = 0;

		for (int i = 0; i < curBlackHeight; i++) {
			for (int j = 0; j < srcWidth; j++) {
				blackAvgValue += dst[i*srcWidth + j];
			}
		}
		blackAvgValue = blackAvgValue / (curBlackHeight * srcWidth);

		/*int whiteHeight = 0;
		if (curBlackHeight + curHalgBlackHeight + curWhiteHeight > srcHeight) {
			whiteHeight = srcHeight - curBlackHeight - curHalgBlackHeight;
		}
		else {
			whiteHeight = curWhiteHeight;
		}
		for (int i = curBlackHeight + curHalgBlackHeight; i < curBlackHeight + curHalgBlackHeight + whiteHeight; i++) {
			for (int j = 0; j < srcWidth; j++) {
				whiteAvgValue += dst[i*srcWidth + j];
			}
		}
		whiteAvgValue = whiteAvgValue / (whiteHeight * srcWidth);*/
		if (blackAvgValue > 100) {
			delete dst;
			return _CODE_PROERROR;
		}
		delete dst;
		return _CODE_OK;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return _CODE_THROW;

	}
}
