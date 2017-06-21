#include "stdafx.h"
#include"BmDUTTrayRecognization.h"

//#include<Windows.h>

bool CBmDUTTrayRecognization::LoadParameter(char path[])
{
	//_try{
	char *ConfigFileName = path;
	bool status = true;
	LPSTR temp = (LPSTR)new char[MAX_PATH];

	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "Weightedproportion", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.Weightedproportion = atof((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "linefitintimes", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.linefitintimes = atoi((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "linefiterrorAdjacenterror", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.linefiterrorAdjacenterror = atof((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "intpylevel", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.intpylevel = atoi((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "firstdutareaminportion", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.firstdutareaminportion = atof((char*)temp);


	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "firstdutareamaxportion", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.firstdutareamaxportion = atof((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "firstdutareamax", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.firstdutareamax = atof((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "firstdutareamin", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.firstdutareamin = atof((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "seconddutsummin", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.seconddutsummin = atof((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "seconddutsummax", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.seconddutsummax = atof((char*)temp);


	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "imageedgeintmin", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.imageedgeintmin = atoi((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "intlinerecoderowdstRoicopy", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.intlinerecoderowdstRoicopy = atoi((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "intsilineangleto", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.intsilineangleto = atoi((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "intlinedis", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.intlinedis = atoi((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "inttwltrdangleto", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.inttwltrdangleto = atoi((char*)temp);


	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "thirdareaprotionmin", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.thirdareaprotionmin = atof((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "thirdareaprotionmax", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.thirdareaprotionmax = atof((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "thirdmaxcirclera", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.thirdmaxcirclera = atoi((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "thirdmincirclera", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.thirdmincirclera = atoi((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "thirdcircleedgemaxpro", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.thirdcircleedgemaxpro = atoi((char*)temp);

	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "thirdcircleedgeminpro", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.thirdcircleedgeminpro = atoi((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "thirdcircleedgemax", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.thirdcircleedgemax = atoi((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "lastareaprotionmax", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.lastareaprotionmax = atof((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "lastareaprotionmin", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.lastareaprotionmin = atof((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "lastwidthto", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.lastwidthto = atoi((char*)temp);

	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "lastheightto", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.lastheightto = atoi((char*)temp);
	GetPrivateProfileStringA("CBMDUTTRAYRECOGNIZATION", "lastfindth", "", temp, MAX_PATH, ConfigFileName);
	if (strcmp(temp, "") == 0)
	{
		delete temp;
		return false;
	}
	cbmdut.lastfindth = atoi((char*)temp);


	delete temp;
	return status;
	//}
	//catch(...)
	/*	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	return _CODE_THROW;
	}*/
}

IMG_VVOID CBmDUTTrayRecognization::VisBC_Pyramid(IMG_UBBUF src, Ipp8u *&srcRoi, IppiSize &Roi, IMG_INT level, IMG_LREAL sigma)
{
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

	//for (int i = 0; i < Roi.height; i++)
	//{
	//	for (int j = 0; j < Roi.width; j++)
	//	{
	//		srcRoi[i*Roi.width + j] = pPyrImage[2][i*Roi.width + j];
	//	}
	//}

	for (int i = 1; i <= level; i++)
		free(pPyrImage[i]);
	free(pPyrStrBuffer);
	free(pPyrBuffer);
	free(pPyrLBuffer);
	free(pPyrLStateBuf);
}

IMG_VVOID CBmDUTTrayRecognization::VisBC_LabelMarker(Ipp8u *srcRoi, IppiSize Roi, IMG_INT &markersNum)
{
	//IppiSize Roi = { src.size.width,src.size.height };
	int markerStep = Roi.width * sizeof(Ipp8u);
	int minLabel = 1;
	int maxLabel = 254;
	//int markersNum = 0;
	Ipp8u *pBuffer = NULL;
	int bufferSize = 0;

	ippiLabelMarkersGetBufferSize_8u_C1R(Roi, &bufferSize);

	pBuffer = ippsMalloc_8u(bufferSize);

	ippiLabelMarkers_8u_C1IR(srcRoi, markerStep, Roi, minLabel, maxLabel, ippiNormL1, &markersNum, pBuffer);

	//printf("markersNum=%d\n", markersNum);

	ippsFree(pBuffer);
}

IMG_VVOID CBmDUTTrayRecognization::VisBC_OpenClose(Ipp8u *srcRoi, IppiSize Roi)
{
	//IppiSize Roi = { src.size.width,src.size.height };
	IppiMorphAdvState *pSpec = NULL;
	Ipp8u* pBuffer = NULL;
	Ipp8u pMask[9] = { 1,1,1,1,0,1,1,1,1 };
	IppiSize maskSize = { 3,3 };

	Ipp8u *dstRoi;
	dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

	int srcStep = Roi.width * sizeof(Ipp8u);
	int dstStep = Roi.width * sizeof(Ipp8u);
	int dstSize = Roi.width;

	int specSize = 0, bufferSize = 0;
	IppiBorderType borderType = ippBorderRepl;
	Ipp8u borderValue = 0;

	ippiMorphAdvGetSize_8u_C1R(Roi, maskSize, &specSize, &bufferSize);

	pSpec = (IppiMorphAdvState*)ippsMalloc_8u(specSize);
	pBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);

	ippiMorphAdvInit_8u_C1R(Roi, pMask, maskSize, pSpec, pBuffer);

	ippiMorphCloseBorder_8u_C1R(srcRoi, srcStep, dstRoi, dstStep, Roi, borderType, borderValue, pSpec, pBuffer);

	ippiMorphOpenBorder_8u_C1R(dstRoi, srcStep, srcRoi, dstStep, Roi, borderType, borderValue, pSpec, pBuffer);

#ifdef DEBUG_BlobCircle
	FILE *hp1;
	hp1 = fopen("Open.txt", "w");
	for (int i = 0; i < Roi.height; i++)
	{
		for (int j = 0; j < Roi.width; j++)
		{
			fprintf(hp1, "%d ", dstRoi[i*Roi.width + j]);
		}
		fprintf(hp1, "\n");
	}
	fclose(hp1);
	FILE *hp2;
	hp2 = fopen("Close.txt", "w");
	for (int i = 0; i < Roi.height; i++)
	{
		for (int j = 0; j < Roi.width; j++)
		{
			fprintf(hp2, "%d ", srcRoi[i*Roi.width + j]);
		}
		fprintf(hp2, "\n");
	}
	fclose(hp2);
#endif // !1

	ippsFree(pBuffer);
	ippsFree(pSpec);
	free(dstRoi);

}

_BACK_CODE CBmDUTTrayRecognization::DUTLocation(const IMG_UBBUF src, DUTOutPutParam &param, int & Angle)
{
	_BACK_CODE status = _CODE_DUT_NORMAL;
	Angle = 361;
	param.angle = 361;
	param.cx = -1;
	param.cy = -1;
	param.dist = -1;
	param.x1 = -1;
	param.x2 = -1;
	param.x3 = -1;
	param.x4 = -1;
	param.y1 = -1;
	param.y2 = -1;
	param.y3 = -1;
	param.y4 = -1;



	int l = DUTMATCH(src, param, Angle);
	if (l == -100)
	{
		return _CODE_PROERROR;
	}
	else if (l == 90)
	{
		return _CODE_LOCATE_FAIL;
	}


	return status;
}

int CBmDUTTrayRecognization::DUTMATCH(IMG_UBBUF src, DUTOutPutParam &param, int & Angle)
{
	//缺少没有图片的判定
	int level = cbmdut.intpylevel;
	double sigma = 3.0;
	IppiSize Roi;
	Roi.width = src.size.width;
	Roi.height = src.size.height;
	Ipp8u* srcRoi;
	srcRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

	VisBC_Pyramid(src, srcRoi, Roi, level, sigma);

	/*FILE *hp111;
	hp111 = fopen("pyramid.txt", "w");
	for (int i = 0;i < Roi.height;i++)
	{
	for (int j = 0;j < Roi.width;j++)
	{
	fprintf(hp111, "%d ", srcRoi[i*Roi.width + j]);
	}
	fprintf(hp111, "\n");
	}
	fclose(hp111);*/

	Ipp8u*dstRoi;
	dstRoi = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));
	double sumpixel = 0;
	Histogram(srcRoi, Roi, dstRoi, sumpixel);

	/*hp111 = fopen("pyramid1.txt", "w");
	for (int i = 0;i < Roi.height;i++)
	{
	for (int j = 0;j < Roi.width;j++)
	{
	fprintf(hp111, "%d ", dstRoi[i*Roi.width + j]);
	}
	fprintf(hp111, "\n");
	}
	fclose(hp111);
	*/
	IMG_INT markersNum = 0;
	VisBC_LabelMarker(dstRoi, Roi, markersNum);
	/*hp111 = fopen("p3.txt", "w");
	for (int i = 0;i < Roi.height;i++)
	{
	for (int j = 0;j < Roi.width;j++)
	{
	fprintf(hp111, "%d ", dstRoi[i*Roi.width + j]);
	}
	fprintf(hp111, "\n");
	}
	fclose(hp111);*/

	VisBC_OpenClose(dstRoi, Roi);

	markersNum = 0;
	VisBC_LabelMarker(dstRoi, Roi, markersNum);

	//hp111 = fopen("p43.txt", "w");
	//for (int i = 0;i < Roi.height;i++)
	//{
	//	for (int j = 0;j < Roi.width;j++)
	//	{
	//		fprintf(hp111, "%d ", dstRoi[i*Roi.width + j]);
	//	}
	//	fprintf(hp111, "\n");
	//}
	//fclose(hp111);

	if (markersNum < 10)
	{
		free(srcRoi);
		free(dstRoi);
		return -100;//不是正常DUT
	}

	int *NumMar;//area
	NumMar = (int*)malloc(markersNum * sizeof(int));

	for (int i = 0; i < markersNum; i++)
	{
		NumMar[i] = 0;
	}

	int grade = 0;

	for (int i = 0; i < Roi.height; i++)
	{
		for (int j = 0; j < Roi.width; j++)
		{
			grade = dstRoi[i*Roi.width + j];
			if (grade > 0)
			{
				NumMar[grade - 1]++;
			}
		}
	}

	Ipp8u*dstRoicopy;
	dstRoicopy = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));

	for (int i = 0; i < Roi.height; i++)
	{
		for (int j = 0; j < Roi.width; j++)
		{
			if (NumMar[dstRoi[i*Roi.width + j] - 1] > (float)Roi.width*(float)Roi.height*cbmdut.firstdutareamaxportion || NumMar[dstRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*cbmdut.firstdutareaminportion)
				//if (NumMar[srcRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*0.005 || NumMar[srcRoi[i*Roi.width + j] - 1] > (float)Roi.width*(float)Roi.height*0.025)
			{
				dstRoicopy[i*Roi.width + j] = 0;
				//dstRoi[i*Roi.width + j] = 0;
			}
			else
			{
				dstRoicopy[i*Roi.width + j] = dstRoi[i*Roi.width + j];
			}
		}
	}


	/*hp111 = fopen("p43.txt", "w");
	for (int i = 0;i < Roi.height;i++)
	{
	for (int j = 0;j < Roi.width;j++)
	{
	fprintf(hp111, "%d ", dstRoicopy[i*Roi.width + j]);
	}
	fprintf(hp111, "\n");
	}
	fclose(hp111);*/

	int remarkersnummic = 0;
	VisBC_LabelMarker(dstRoicopy, Roi, remarkersnummic);

	int *remarkersnummicarea = (int*)malloc(sizeof(int)*remarkersnummic);
	for (int i = 0;i < remarkersnummic;i++)
	{
		remarkersnummicarea[i] = 0;
	}
	for (int i = 0;i < Roi.height;i++)
	{
		for (int j = 0;j < Roi.width;j++)
		{
			if (0 != (int)dstRoicopy[i*Roi.width + j])
			{
				remarkersnummicarea[(int)dstRoicopy[i*Roi.width + j] - 1]++;
			}
		}
	}

	int tempremarkersnummicarea = 0;
	for (int i = 1;i < remarkersnummic;i++)
	{
		if (remarkersnummicarea[i] > remarkersnummicarea[tempremarkersnummicarea])
			tempremarkersnummicarea = i;
	}

	if (remarkersnummic < 1 || remarkersnummicarea[tempremarkersnummicarea] < cbmdut.firstdutareamin || remarkersnummicarea[tempremarkersnummicarea]>cbmdut.firstdutareamax)
	{
		free(dstRoicopy);
		free(srcRoi);
		free(dstRoi);
		free(NumMar);
		free(remarkersnummicarea);
		return -100;//不是正常DUT
	}
	/*hp111 = fopen("p63.txt", "w");
	for (int i = 0;i < Roi.height;i++)
	{
	for (int j = 0;j < Roi.width;j++)
	{
	fprintf(hp111, "%d ", dstRoicopy[i*Roi.width + j]);
	}
	fprintf(hp111, "\n");
	}
	fclose(hp111);*/
	int *recodelinedstRoicopy = (int*)malloc(Roi.height * sizeof(int));
	int *recoderowdstRoicopy = (int*)malloc(Roi.width * sizeof(int));
	for (int j = 0;j < Roi.width;j++)
	{
		recoderowdstRoicopy[j] = 0;
	}
	float sumrl = 0;
	for (int i = 0;i < Roi.height;i++)
	{
		recodelinedstRoicopy[i] = 0;
		for (int j = 0;j < Roi.width;j++)
		{
			if (0 != (int)dstRoicopy[i*Roi.width + j])
			{
				recodelinedstRoicopy[i]++;
				recoderowdstRoicopy[j]++;
				sumrl++;
			}
		}
	}
	if (sumrl < cbmdut.seconddutsummin || sumrl >cbmdut.seconddutsummax)
	{
		free(recodelinedstRoicopy);
		free(recoderowdstRoicopy);
		free(dstRoicopy);
		free(srcRoi);
		free(dstRoi);
		free(NumMar);
		free(remarkersnummicarea);
		return -100;//不是正常DUT
	}
	int tempheightmin = 0;
	int tempheightmax = 0;
	int tempwidthmin = 0;
	int tempwidthmax = 0;
	for (int j = 0;j < Roi.width - 1;j++)
	{
		if (fabs(recoderowdstRoicopy[j + 1] - recoderowdstRoicopy[j]) != 0 && (recoderowdstRoicopy[j] == 0))
		{
			tempwidthmin = j;
		}
		if ((recoderowdstRoicopy[j + 1] == 0) && fabs(recoderowdstRoicopy[j + 1] - recoderowdstRoicopy[j]) != 0)
		{
			tempwidthmax = j;
			break;
		}

	}

	for (int i = 0;i < Roi.height - 1;i++)
	{
		if (fabs(recodelinedstRoicopy[i + 1] - recodelinedstRoicopy[i]) != 0 && (recodelinedstRoicopy[i] == 0))
		{
			tempheightmin = i;
		}
		if ((recodelinedstRoicopy[i + 1] == 0) && fabs(recodelinedstRoicopy[i + 1] - recodelinedstRoicopy[i]) != 0)
		{
			tempheightmax = i;
			break;
		}
	}
	//if (fabs(tempwidthmax - tempwidthmin - 164) > 25 || fabs(tempheightmax - tempheightmin - 178) > 25)
	//{
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	free(remarkersnummicarea);
	//	return -100;//不是正常DUT
	//}
	//if (/*tempheightmin < 45 || fabs(tempheightmax - Roi.height) < 45
	//	|| tempwidthmin < 45 || fabs(tempwidthmax - Roi.width) < 45*/
	//	tempheightmin < cbmdut.imageedgeintmin || fabs(tempheightmax - Roi.height) < cbmdut.imageedgeintmin
	//	|| tempwidthmin < cbmdut.imageedgeintmin || fabs(tempwidthmax - Roi.width) < cbmdut.imageedgeintmin
	//	)
	//{
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	free(remarkersnummicarea);
	//	return -100;//不是正常DUT
	//}

	vector<IMG_RCOORD> pointtop;
	vector<IMG_RCOORD> pointdown;
	vector<IMG_RCOORD> pointleft;
	vector<IMG_RCOORD> pointright;

	float linelefttempth = 0;
	float linerighttempth = 0;
	for (int i = tempwidthmin + 8, j = tempwidthmax - 8, z = 0;z < 5;z++, i++, j--)
	{
		linelefttempth += recoderowdstRoicopy[i];
		linerighttempth += recoderowdstRoicopy[j];
	}

	if (linelefttempth < linerighttempth)
	{
		linelefttempth = 130;
		linerighttempth = 120;
	}
	else
	{
		linelefttempth = 130;
		linerighttempth = 120;
	}

	int lleftrightstep = 1;

	for (int i = tempheightmin - 5; i < tempheightmax + 5; i = i + lleftrightstep)
	{
		for (int j = tempwidthmin - 5; j < tempwidthmax + 5; j++)
		{
			if (recodelinedstRoicopy[i] >= linelefttempth)
			{
				if (dstRoicopy[i*Roi.width + j] == 0 && dstRoicopy[i*Roi.width + j + 1] != 0)
				{
					//cout << (double)dstRoicopy[i*Roi.width + j] << "   " << (double)dstRoicopy[i*Roi.width + j + 1] << endl;
					pointleft.push_back({ float(i),float(j + 0.5) });
					break;
				}
			}
		}
	}
	for (int i = tempheightmin - 5; i < tempheightmax + 5; i = i + lleftrightstep)
	{
		for (int j = tempwidthmax + 5; j > tempwidthmin - 5; j--)
		{
			if (recodelinedstRoicopy[i] >= linerighttempth)
			{
				if (dstRoicopy[i*Roi.width + j] == 0 && dstRoicopy[i*Roi.width + j - 1] != 0)
				{
					pointright.push_back({ float(i), (float)(j + 0.5) });
					break;
				}
			}
		}

	}


	for (int i = tempwidthmin - 5; i < tempwidthmax + 5; i++)
	{
		for (int j = tempheightmin - 5; j < tempheightmax + 5; j++)
		{
			if (recoderowdstRoicopy[i] >= cbmdut.intlinerecoderowdstRoicopy)
			{
				if (dstRoicopy[j*Roi.width + i] == 0 && dstRoicopy[(j + 1)*Roi.width + i] != 0)
				{
					pointtop.push_back({ (float)i,float(j + 0.5) });
					break;
				}
			}
		}
	}
	for (int i = tempwidthmin - 5; i < tempwidthmax + 5; i++)
	{
		for (int j = tempheightmax + 5; j > tempheightmin - 5; j--)
		{
			if (recoderowdstRoicopy[i] >= cbmdut.intlinerecoderowdstRoicopy)
			{
				if (dstRoicopy[j*Roi.width + i] != 0 && dstRoicopy[(j + 1)*Roi.width + i] == 0)
				{
					pointdown.push_back({ (float)i,float(j + 0.5) });
					break;
				}
			}
		}
	}


	int m = pointleft.size();
	NEWLINE lineleft;
	IMG_RCOORD* point = (IMG_RCOORD*)malloc(m * sizeof(IMG_RCOORD));
	//ofstream lf("1.txt");
	for (int i = 0;i < m;i++)
	{
		point[i] = pointleft[i];
		//lf << pointleft[i].x << "  " << pointleft[i].y << endl;
	}
	//lf.close();

	IMG_BOOL kj = VisOmmLine_Fitting(point, lineleft, m);
	lineleft.A = lineleft.B;
	lineleft.B = lineleft.k;
	//lineleft.k = -1 / lineleft.k;
	lineleft.b = -lineleft.b / lineleft.k;
	free(point);

	m = pointright.size();
	NEWLINE lineright;
	point = (IMG_RCOORD*)malloc(m * sizeof(IMG_RCOORD));
	//ofstream lf2("2.txt");
	for (int i = 0;i < m;i++)
	{
		point[i] = pointright[i];
		//lf2 << pointright[i].x << "  " << pointright[i].y << endl;
	}
	//lf2.close();

	kj = VisOmmLine_Fitting(point, lineright, m);
	lineright.A = lineright.B;
	lineright.B = lineright.k;
	//lineleft.k = -1 / lineleft.k;
	lineright.b = -lineright.b / lineright.k;
	free(point);

	m = pointtop.size();
	NEWLINE linetop;
	point = (IMG_RCOORD*)malloc(m * sizeof(IMG_RCOORD));
	//ofstream lf23("3.txt");
	for (int i = 0;i < m;i++)
	{
		point[i] = pointtop[i];
		//lf23 << pointtop[i].x << "  " << pointtop[i].y << endl;
	}
	//lf23.close();

	kj = VisOmmLine_Fitting(point, linetop, m);
	free(point);

	m = pointdown.size();
	NEWLINE linedown;
	point = (IMG_RCOORD*)malloc(m * sizeof(IMG_RCOORD));
	//ofstream lf234("4.txt");
	for (int i = 0;i < m;i++)
	{
		point[i] = pointdown[i];
		//lf234 << pointdown[i].x << "  " << pointdown[i].y << endl;
	}
	//lf234.close();

	kj = VisOmmLine_Fitting(point, linedown, m);
	free(point);

	vector<IMG_RCOORD>().swap(pointtop);
	vector<IMG_RCOORD>().swap(pointdown);
	vector<IMG_RCOORD>().swap(pointleft);
	vector<IMG_RCOORD>().swap(pointright);

	IMG_RCOORD midpoint;
	midpoint.x = float((tempwidthmin + tempwidthmax)*1.0 / 2);
	midpoint.y = float((tempheightmax + tempheightmin)*1.0 / 2);

	double angleleft = atan2(-1.0 / lineleft.k, 1) * 180 / PI;
	double angleright = atan2(-1.0 / lineright.k, 1) * 180 / PI;
	double angletop = atan2(linetop.k, 1) * 180 / PI;
	double angledown = atan2(linedown.k, 1) * 180 / PI;

	double disleft = fabs(lineleft.A*midpoint.x + lineleft.B*midpoint.y + lineleft.C) / (sqrt(lineleft.A*lineleft.A + lineleft.B*lineleft.B));
	double disright = fabs(lineright.A*midpoint.x + lineright.B*midpoint.y + lineright.C) / (sqrt(lineright.A*lineright.A + lineright.B*lineright.B));
	double distop = fabs(linetop.A*midpoint.x + linetop.B*midpoint.y + linetop.C) / (sqrt(linetop.A*linetop.A + linetop.B*linetop.B));
	double disdown = fabs(linedown.A*midpoint.x + linedown.B*midpoint.y + linedown.C) / (sqrt(linedown.A*linedown.A + linedown.B*linedown.B));

	double lefttopangle = atan(fabs((-1.0 / lineleft.k - linetop.k) / (1 - 1.0 / lineleft.k*linetop.k))) * 180 / PI;
	double leftdownangle = atan(fabs((-1.0 / lineleft.k - linedown.k) / (1 - 1.0 / lineleft.k*linedown.k))) * 180 / PI;
	double righttopangle = atan(fabs((-1.0 / lineright.k - linetop.k) / (1 - 1.0 / lineright.k*linetop.k))) * 180 / PI;
	double rightdownangle = atan(fabs((-1.0 / lineright.k - linedown.k) / (1 - 1.0 / lineright.k*linedown.k))) * 180 / PI;

	//if (fabs(fabs(angleleft) - fabs(angleright)) > cbmdut.intsilineangleto ||
	//	fabs(fabs(angletop) - fabs(angledown)) > cbmdut.intsilineangleto ||
	//	fabs(fabs(angleleft) - 90) > cbmdut.intsilineangleto ||
	//	fabs(fabs(angleright) - 90) > cbmdut.intsilineangleto ||
	//	fabs(angletop) > cbmdut.intsilineangleto ||
	//	fabs(angledown) > cbmdut.intsilineangleto ||
	//	fabs(disleft + disright - 161) > cbmdut.intlinedis ||
	//	fabs(distop + disdown - 180) >  cbmdut.intlinedis ||
	//	fabs(fabs(lefttopangle) - 90) > cbmdut.inttwltrdangleto ||
	//	fabs(fabs(leftdownangle) - 90) > cbmdut.inttwltrdangleto ||
	//	fabs(fabs(righttopangle) - 90) > cbmdut.inttwltrdangleto ||
	//	fabs(fabs(rightdownangle) - 90) > cbmdut.inttwltrdangleto
	//	)
	//{
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	free(remarkersnummicarea);
	//	return -100;
	//}

	param.x1 = (lineleft.b - linetop.b) / (linetop.k - 1 / lineleft.k);
	param.y1 = param.x1*linetop.k + linetop.b;
	param.x2 = (lineright.b - linetop.b) / (linetop.k - 1 / lineright.k);
	param.y2 = param.x2*linetop.k + linetop.b;
	param.x3 = (lineleft.b - linedown.b) / (linedown.k - 1 / lineleft.k);
	param.y3 = param.x3*linedown.k + linedown.b;
	param.x4 = (lineright.b - linedown.b) / (linedown.k - 1 / lineright.k);
	param.y4 = param.x4*linedown.k + linedown.b;
	param.cx = (param.x1 + param.x2 + param.x3 + param.x4) / 4;
	param.cy = (param.y1 + param.y2 + param.y3 + param.y4) / 4;
	param.dist = fabs((linedown.A*param.cx + linedown.B*param.cy + linedown.C)) / sqrt(linedown.A*linedown.A + linedown.B*linedown.B) / 2;

	//FloatLine line1;
	//line1.startPoint.x = 0;
	//line1.startPoint.y = lineleft.b;
	//line1.endPoint.x = Roi.width;
	//line1.endPoint.y = Roi.width*(1) / lineleft.k + lineleft.b;
	//FloatLine line2;
	//line2.startPoint.x = 0;
	//line2.startPoint.y = linetop.b;
	//line2.endPoint.x = Roi.width;
	//line2.endPoint.y = Roi.width*linetop.k + linetop.b;
	//IMG_RCOORD intersectionpoint;

	///*double x1 = (linetop.B*lineleft.C - lineleft.B*linetop.C) / (linetop.A*lineleft.B - lineleft.A*linetop.B);
	//double x2 = x1*linetop.k + linetop.b;
	//double y1 = (lineleft.b - linetop.b) / (linetop.k - 1 / lineleft.k);
	//double y2 = y1*linetop.k + linetop.b;*/
	//IMG_INT  dut = VisOmmIntersection_oftwoline(line1, line2, intersectionpoint);
	//if (dut != 0)
	//{
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	free(remarkersnummicarea);
	//	return dut;
	//}


	//cout << "haha" << endl;

	//Ipp8u*srcroiroi = (Ipp8u*)malloc(src.size.height * 20 * sizeof(Ipp8u));
	//NEWLINE lineleft;
	//IMG_SIZE roiSizeleft;
	//roiSizeleft.height = Roi.height;
	//roiSizeleft.width = 20;
	//lineleft.A = 0;
	//lineleft.B = 0;
	//lineleft.C = 0;
	//lineleft.k = 0;
	//lineleft.b = 0;
	//ofstream  lf("left.txt");
	//for (int i = 0;i < Roi.height;i++)
	//{
	//	for (int j = tempwidthmin - 3;j < tempwidthmin + 18;j++)
	//	{
	//		srcroiroi[i * 20 + j - tempwidthmin + 3] = dstRoicopy[i*Roi.width + j];
	//		lf << (double)dstRoicopy[i*Roi.width + j] << "  ";
	//	}
	//	lf << endl;
	//}

	//int returnvalue = SrcLine(srcroiroi, roiSizeleft, lineleft);
	//if (returnvalue != 0)
	//{
	//	free(srcroiroi);
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	return -1;
	//}
	//double left = atan2(lineleft.k, 1) * 180 / PI;
	//if (fabs(left - 90) > 5)
	//{
	//	free(srcroiroi);
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	return -1000;//倾斜过大
	//}

	//free(srcroiroi);
	//srcroiroi = (Ipp8u*)malloc(src.size.height * 20 * sizeof(Ipp8u));
	//NEWLINE lineright;
	//IMG_SIZE roiSizeright;
	//roiSizeright.height = Roi.height;
	//roiSizeright.width = 20;
	//lineright.A = 0;
	//lineright.B = 0;
	//lineright.C = 0;
	//lineright.k = 0;
	//lineright.b = 0;
	//ofstream  ri("right.txt");
	//for (int i = 0;i < Roi.height;i++)
	//{
	//	for (int j = tempwidthmax - 18;j < tempwidthmax + 3;j++)
	//	{
	//		srcroiroi[i * 20 + j - tempwidthmax + 18] = dstRoicopy[i*Roi.width + j];
	//		ri << (double)dstRoicopy[i*Roi.width + j] << "  ";
	//	}
	//	ri << endl;
	//}

	//returnvalue = SrcLine(srcroiroi, roiSizeright, lineright);
	//if (returnvalue != 0)
	//{
	//	free(srcroiroi);
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	return -1;
	//}
	//double right = atan2(lineright.k, 1) * 180 / PI;
	//if (fabs(right - 90) > 5)
	//{
	//	free(srcroiroi);
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	return -1000;//倾斜过大
	//}

	//free(srcroiroi);
	//srcroiroi = (Ipp8u*)malloc(src.size.width * 20 * sizeof(Ipp8u));
	//NEWLINE linetop;
	//IMG_SIZE roiSizetop;
	//roiSizetop.height = 20;
	//roiSizetop.width = Roi.width;
	//linetop.A = 0;
	//linetop.B = 0;
	//linetop.C = 0;
	//linetop.k = 0;
	//linetop.b = 0;
	//ofstream  tp("top.txt");
	//for (int i = tempheightmin - 3;i < tempheightmin + 18;i++)
	//{
	//	for (int j = 0;j < Roi.width;j++)
	//	{
	//		srcroiroi[(i - tempheightmin + 3) * Roi.width + j] = dstRoicopy[i*Roi.width + j];
	//		tp << (double)dstRoicopy[i*Roi.width + j] << "  ";
	//	}
	//	tp << endl;
	//}

	//returnvalue = SrcLine(srcroiroi, roiSizetop, linetop);
	//if (returnvalue != 0)
	//{
	//	free(srcroiroi);
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	return -1;//倾斜过大
	//}
	//double top = atan2(linetop.k, 1) * 180 / PI;
	//if (fabs(top) > 5)
	//{
	//	free(srcroiroi);
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	return -1000;//倾斜过大
	//}

	//free(srcroiroi);
	//srcroiroi = (Ipp8u*)malloc(src.size.width * 20 * sizeof(Ipp8u));
	//NEWLINE linedown;
	//IMG_SIZE roiSizedown;
	//roiSizedown.height = 20;
	//roiSizedown.width = Roi.width;
	//linedown.A = 0;
	//linedown.B = 0;
	//linedown.C = 0;
	//linedown.k = 0;
	//linedown.b = 0;
	//ofstream dow("down.txt");
	//for (int i = tempheightmax - 18;i < tempheightmax + 3;i++)
	//{
	//	for (int j = 0;j < Roi.width;j++)
	//	{
	//		srcroiroi[(i - tempheightmax + 18) * Roi.width + j] = dstRoicopy[i*Roi.width + j];
	//		dow << (double)dstRoicopy[i*Roi.width + j] << "  ";
	//	}
	//	dow << endl;
	//}

	//returnvalue = SrcLine(srcroiroi, roiSizedown, linedown);
	//if (returnvalue != 0)
	//{
	//	free(srcroiroi);
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	return -1;//倾斜过大
	//}
	//double down = atan2(linedown.k, 1) * 180 / PI;
	//if (fabs(down) > 5)
	//{
	//	free(srcroiroi);
	//	free(recodelinedstRoicopy);
	//	free(recoderowdstRoicopy);
	//	free(dstRoicopy);
	//	free(srcRoi);
	//	free(dstRoi);
	//	free(NumMar);
	//	return -1000;//倾斜过大
	//}


	/*Mat M = imread("e:\\Users\\bm00020\\Desktop\\p63.bmp", 0);
	IplImage *haha;
	haha = cvCreateImage(cvSize(M.cols, M.rows), IPL_DEPTH_8U, 3);
	for (int i = 0;i < haha->height;i++)
	{
	for (int j = 0;j < haha->width;j++)
	{
	for (int k = 0;k < haha->nChannels;k++)
	{
	*(uchar*)(haha->imageData + i*haha->widthStep + j*haha->nChannels + k) =
	*(uchar*)(M.data + i*haha->width + j);
	}
	if (fabs(linedown.k*j + linedown.b - i) < 1)
	{
	*(uchar*)(haha->imageData + (int)(i)*haha->widthStep + (int)(j)* haha->nChannels + 0) = 0;
	*(uchar*)(haha->imageData + (int)(i)* haha->widthStep + (int)(j)* haha->nChannels + 1) = 255;
	*(uchar*)(haha->imageData + (int)(i)* haha->widthStep + (int)(j)* haha->nChannels + 2) = 0;
	}
	if (fabs(linetop.k*j + linetop.b - i) < 1)
	{
	*(uchar*)(haha->imageData + (int)(i)*haha->widthStep + (int)(j)* haha->nChannels + 0) = 0;
	*(uchar*)(haha->imageData + (int)(i)* haha->widthStep + (int)(j)* haha->nChannels + 1) = 255;
	*(uchar*)(haha->imageData + (int)(i)* haha->widthStep + (int)(j)* haha->nChannels + 2) = 0;
	}
	if (fabs(lineleft.k*j + lineleft.b - i) < 2)
	{
	*(uchar*)(haha->imageData + (int)(i)*haha->widthStep + (int)(j)* haha->nChannels + 0) = 0;
	*(uchar*)(haha->imageData + (int)(i)* haha->widthStep + (int)(j)* haha->nChannels + 1) = 255;
	*(uchar*)(haha->imageData + (int)(i)* haha->widthStep + (int)(j)* haha->nChannels + 2) = 0;
	}
	if (fabs(lineright.k*j + lineright.b - i) < 2)
	{
	*(uchar*)(haha->imageData + (int)(i)*haha->widthStep + (int)(j)* haha->nChannels + 0) = 0;
	*(uchar*)(haha->imageData + (int)(i)* haha->widthStep + (int)(j)* haha->nChannels + 1) = 255;
	*(uchar*)(haha->imageData + (int)(i)* haha->widthStep + (int)(j)* haha->nChannels + 2) = 0;
	}
	}
	}

	cvSaveImage("e:\\Users\\bm00020\\Desktop\\haxihaxiha.bmp", haha);*/


	//if (sumpixel < 50) 
	{

		IppiSize DutRoi;
		DutRoi.width = tempwidthmax - tempwidthmin - 10;
		DutRoi.height = tempheightmax - tempheightmin - 10;
		Ipp8u*dutRoi;
		dutRoi = (Ipp8u*)malloc(DutRoi.width*DutRoi.height * sizeof(Ipp8u));

		for (int i = 0;i < DutRoi.height;i++)
		{
			for (int j = 0;j < DutRoi.width;j++)
			{
				if (dstRoicopy[(i + tempheightmin + 5)*Roi.width + j + tempwidthmin + 5] == 0)
					dutRoi[i*DutRoi.width + j] = 255;
				else
				{
					dutRoi[i*DutRoi.width + j] = 0;
				}
			}
		}


		int Dutm = 0;
		VisBC_LabelMarker(dutRoi, DutRoi, Dutm);

		/*ofstream lm0("roiroi0.txt");
		for (int i = 0;i < DutRoi.height;i++)
		{
		for (int j = 0;j < DutRoi.width;j++)
		{

		lm0 << (double)dutRoi[i*DutRoi.width + j] << "  ";

		}
		lm0 << endl;
		}
		lm0.close();*/

		int *roiroiar = (int*)malloc(sizeof(int)*Dutm);
		memset(roiroiar, 0, sizeof(int)*Dutm);
		for (int i = 0;i < DutRoi.height;i++)
		{
			for (int j = 0;j < DutRoi.width;j++)
			{
				if (0 != (int)dutRoi[i*DutRoi.width + j])
				{
					roiroiar[(int)dutRoi[i*DutRoi.width + j] - 1]++;
				}
			}
		}
		int *roiroinmaxxy = (int*)malloc(4 * Dutm * sizeof(int));
		for (int i = 0;i < Dutm;i++)
		{
			roiroinmaxxy[i * 4] = DutRoi.width;
			roiroinmaxxy[i * 4 + 1] = 0;
			roiroinmaxxy[i * 4 + 2] = DutRoi.height;
			roiroinmaxxy[i * 4 + 3] = 0;
		}

		for (int ii = 1;ii < Dutm + 1;ii++)
		{
			for (int i = 0;i < DutRoi.height;i++)
			{
				for (int j = 0;j < DutRoi.width;j++)
				{
					if (ii == dutRoi[i*DutRoi.width + j])
					{
						if (i < roiroinmaxxy[(ii - 1) * 4 + 2])
						{
							roiroinmaxxy[(ii - 1) * 4 + 2] = i;
						}
						if (i > roiroinmaxxy[(ii - 1) * 4 + 3])
						{
							roiroinmaxxy[(ii - 1) * 4 + 3] = i;
						}
						if (j < roiroinmaxxy[(ii - 1) * 4])
						{
							roiroinmaxxy[(ii - 1) * 4] = j;
						}
						if (j > roiroinmaxxy[(ii - 1) * 4 + 1])
						{
							roiroinmaxxy[(ii - 1) * 4 + 1] = j;
						}
					}
				}

			}
		}

		for (int i = 0; i < DutRoi.height; i++)
		{
			for (int j = 0; j < DutRoi.width; j++)
			{
				if (roiroiar[dutRoi[i*DutRoi.width + j] - 1] > (float)DutRoi.width*(float)DutRoi.height*cbmdut.thirdareaprotionmax || roiroiar[dutRoi[i*DutRoi.width + j] - 1] < (float)DutRoi.width*(float)DutRoi.height*cbmdut.thirdareaprotionmin
					|| (roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 1] - roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4])>cbmdut.thirdmaxcirclera
					|| (roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 3] - roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 2]) < cbmdut.thirdmincirclera

					|| (fabs((roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 3] - DutRoi.height)) < cbmdut.thirdcircleedgeminpro && ((roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 1] - roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4]) < 30 || (roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 3] - roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 2]) < cbmdut.thirdmincirclera))
					|| (fabs((roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 2])) < cbmdut.thirdcircleedgemaxpro && ((roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 1] - roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4]) < 30 || (roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 3] - roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 2]) < cbmdut.thirdmincirclera))

					|| (((roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 1] - roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4]) > cbmdut.thirdmaxcirclera) && ((roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 3] - roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 2]) < cbmdut.thirdcircleedgemax))

					|| (((roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 1] - roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4]) > cbmdut.thirdmaxcirclera) && ((roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 3] - roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 2]) < cbmdut.thirdcircleedgemax) && (fabs((roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 1] - DutRoi.width)) <= cbmdut.thirdcircleedgeminpro
						|| fabs((roiroinmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4])) < 2))
					//
					)
					//if (NumMar[srcRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*0.005 || NumMar[srcRoi[i*Roi.width + j] - 1] > (float)Roi.width*(float)Roi.height*0.025)
				{
					dutRoi[i*DutRoi.width + j] = 0;
				}
			}
		}



		Dutm = 0;
		VisBC_LabelMarker(dutRoi, DutRoi, Dutm);


		/*ofstream lm("roiroi.txt");
		for (int i = 0;i < DutRoi.height;i++)
		{
		for (int j = 0;j < DutRoi.width;j++)
		{

		lm << (double)dutRoi[i*DutRoi.width + j] << "  ";

		}
		lm << endl;
		}
		lm.close();*/

		//int *DutNumMar;//area
		//DutNumMar = (int*)malloc(Dutm * sizeof(int));
		//for (int i = 0; i < Dutm; i++)
		//{
		//	DutNumMar[i] = 0;
		//}
		////int grade = 0;
		//for (int i = 0; i < DutRoi.height; i++)
		//{
		//	for (int j = 0; j < DutRoi.width; j++)
		//	{
		//		grade = dutRoi[i*DutRoi.width + j];
		//		if (grade > 0)
		//		{
		//			DutNumMar[grade - 1]++;
		//		}
		//	}
		//}
		//int dutroimakeri = 0;
		//for (int i = 0; i < Dutm; i++)
		//{
		//	if (DutNumMar[i] > DutNumMar[dutroimakeri])
		//	{
		//		dutroimakeri = i;
		//	}
		//}
		//int dutcirroiminx = DutRoi.width;
		//int dutcirroimaxx = 0;
		//int dutcirroiminy = DutRoi.height;
		//int dutcirroimaxy = 0;
		//for (int i = 0;i < DutRoi.height;i++)
		//{
		//	for (int j = 0;j < DutRoi.width;j++)
		//	{
		//		if (dutroimakeri == ((int)dutRoi[i*DutRoi.width + j] - 1))
		//		{
		//			if (i < dutcirroiminy)
		//			{
		//				dutcirroiminy = i;
		//			}
		//			if (i > dutcirroimaxy)
		//			{
		//				dutcirroimaxy = i;
		//			}
		//			if (j < dutcirroiminx)
		//			{
		//				dutcirroiminx = j;
		//			}
		//			if (j > dutcirroimaxx)
		//			{
		//				dutcirroimaxx = j;
		//			}
		//		}
		//	}
		//}
		//
		///*Ipp8u*dstRoicopy;
		//dstRoicopy = (Ipp8u*)malloc(Roi.width*Roi.height * sizeof(Ipp8u));*/
		//IMG_RCOORD dutcirmaxroixy;
		//dutcirmaxroixy.x = (float)((dutcirroiminx + dutcirroimaxx)*1.0 / 2);
		//dutcirmaxroixy.y = (float)((dutcirroiminy + dutcirroimaxy)*1.0 / 2);
		//for (int i = 0; i < DutRoi.height; i++)
		//{
		//	for (int j = 0; j < DutRoi.width; j++)
		//	{
		//		if (DutNumMar[dutRoi[i*DutRoi.width + j] - 1] > (float)DutRoi.width*(float)DutRoi.height*0.30 || DutNumMar[dutRoi[i*DutRoi.width + j] - 1] < (float)DutRoi.width*(float)DutRoi.height*0.011
		//			//|| (i < dutcirroiminy&&j < dutcirroiminx)
		//			//|| (i < dutcirroiminy&&j > dutcirroimaxx)
		//			//|| (i > dutcirroimaxy&&j < dutcirroiminx)
		//			//|| (i > dutcirroimaxy&&j > dutcirroimaxx)
		//			)
		//			//if (NumMar[srcRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*0.005 || NumMar[srcRoi[i*Roi.width + j] - 1] > (float)Roi.width*(float)Roi.height*0.025)
		//		{
		//			dutRoi[i*DutRoi.width + j] = 0;
		//		}
		//	}
		//}
		/*Dutm = 0;
		VisBC_LabelMarker(dutRoi, DutRoi, Dutm);
		ofstream lj("dut1.txt");
		for (int i = 0;i < DutRoi.height;i++)
		{
		for (int j = 0;j < DutRoi.width;j++)
		{
		lj << (double)dutRoi[i*DutRoi.width + j] << "  ";
		}
		lj << endl;
		}*/

		int *pominmaxxy = (int*)malloc(4 * Dutm * sizeof(int));
		for (int i = 0;i < Dutm;i++)
		{
			pominmaxxy[i * 4] = DutRoi.width;
			pominmaxxy[i * 4 + 1] = 0;
			pominmaxxy[i * 4 + 2] = DutRoi.height;
			pominmaxxy[i * 4 + 3] = 0;
		}

		for (int ii = 1;ii < Dutm + 1;ii++)
		{
			for (int i = 0;i < DutRoi.height;i++)
			{
				for (int j = 0;j < DutRoi.width;j++)
				{
					if (ii == dutRoi[i*DutRoi.width + j])
					{
						if (i < pominmaxxy[(ii - 1) * 4 + 2])
						{
							pominmaxxy[(ii - 1) * 4 + 2] = i;
						}
						if (i > pominmaxxy[(ii - 1) * 4 + 3])
						{
							pominmaxxy[(ii - 1) * 4 + 3] = i;
						}
						if (j < pominmaxxy[(ii - 1) * 4])
						{
							pominmaxxy[(ii - 1) * 4] = j;
						}
						if (j > pominmaxxy[(ii - 1) * 4 + 1])
						{
							pominmaxxy[(ii - 1) * 4 + 1] = j;
						}
					}
				}

			}
		}

		IMG_RCOORD *dispominmaxxy = (IMG_RCOORD*)malloc(Dutm * sizeof(IMG_RCOORD));
		for (int i = 0;i < Dutm;i++)
		{
			dispominmaxxy[i].x = float((pominmaxxy[i * 4 + 1] + pominmaxxy[i * 4]) * 1.0 / 2);
			dispominmaxxy[i].y = float((pominmaxxy[i * 4 + 3] + pominmaxxy[i * 4] + 2) * 1.0 / 2);
		}

		int *DutroiNumMar;//area
		DutroiNumMar = (int*)malloc(Dutm * sizeof(int));

		for (int i = 0; i < Dutm; i++)
		{
			DutroiNumMar[i] = 0;
		}


		//int grade = 0;

		for (int i = 0; i < DutRoi.height; i++)
		{
			for (int j = 0; j < DutRoi.width; j++)
			{
				grade = dutRoi[i*DutRoi.width + j];
				if (grade > 0)
				{
					DutroiNumMar[grade - 1]++;
				}
			}
		}


		for (int i = 0; i < DutRoi.height; i++)
		{
			for (int j = 0; j < DutRoi.width; j++)
			{
				double temparea = fabs(pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 1] - pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4])*fabs(pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 3] - pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 2]);

				/*	double distance =((dispominmaxxy[(dutRoi[i*DutRoi.width + j] - 1)].x - dutcirmaxroixy.x)*(dispominmaxxy[(dutRoi[i*DutRoi.width + j] - 1)].x - dutcirmaxroixy.x)
				+
				(dispominmaxxy[(dutRoi[i*DutRoi.width + j] - 1)].y - dutcirmaxroixy.y)*(dispominmaxxy[(dutRoi[i*DutRoi.width + j] - 1)].y - dutcirmaxroixy.y));*/
				if (DutroiNumMar[dutRoi[i*DutRoi.width + j] - 1] > cbmdut.lastareaprotionmax*temparea
					||
					DutroiNumMar[dutRoi[i*DutRoi.width + j] - 1] < cbmdut.lastareaprotionmin*temparea
					||
					fabs(pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 1] - pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4]) < cbmdut.lastwidthto
					||
					fabs(pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 3] - pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 2]) < cbmdut.lastheightto
					||
					(fabs(pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 3] - pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 2]) > 30 && fabs(pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 1] - pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4]) < 40 && DutroiNumMar[(dutRoi[i*DutRoi.width + j] - 1)] < 1000)
					||
					(fabs(pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 3] - pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 2]) < 30 && fabs(pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4 + 1] - pominmaxxy[(dutRoi[i*DutRoi.width + j] - 1) * 4]) > 30 && DutroiNumMar[(dutRoi[i*DutRoi.width + j] - 1)] < 1000)
					//||
					//distance > 3600 ||
					//distance < 1600
					)
				{
					dutRoi[i*DutRoi.width + j] = 0;
				}
			}
		}

		/*ofstream ljk("dut.txt");
		for (int i = 0;i < DutRoi.height;i++)
		{
		for (int j = 0;j < DutRoi.width;j++)
		{
		ljk << (double)dutRoi[i*DutRoi.width + j] << "  ";
		}
		ljk << endl;
		}*/


		Dutm = 0;
		VisBC_LabelMarker(dutRoi, DutRoi, Dutm);
		if (Dutm <= 0)
		{
			free(dutRoi);
			free(DutroiNumMar);
			free(pominmaxxy);
			free(dispominmaxxy);
			free(recodelinedstRoicopy);
			free(recoderowdstRoicopy);
			free(dstRoicopy);
			free(srcRoi);
			free(dstRoi);
			free(NumMar);
			free(remarkersnummicarea);
			free(roiroiar);
			free(roiroinmaxxy);
			return -100;
		}

		int * arar = (int*)malloc(Dutm * sizeof(int));
		for (int i = 0;i < Dutm;i++)
		{
			arar[i] = 0;
		}
		for (int i = 0;i < DutRoi.height;i++)
		{
			for (int j = 0;j < DutRoi.width;j++)
			{
				if (0 != (int)dutRoi[i*DutRoi.width + j])
				{
					arar[(int)dutRoi[i*DutRoi.width + j] - 1]++;
				}
			}
		}

		int temmaker = 0;
		for (int i = 0;i < Dutm;i++)
		{
			if (arar[i] > arar[temmaker])
			{
				temmaker = i;
			}
		}

		int *dutroipominmaxxy = (int*)malloc(4 * Dutm * sizeof(int));
		for (int i = 0;i < Dutm;i++)
		{
			dutroipominmaxxy[i * 4] = DutRoi.width;
			dutroipominmaxxy[i * 4 + 1] = 0;
			dutroipominmaxxy[i * 4 + 2] = DutRoi.height;
			dutroipominmaxxy[i * 4 + 3] = 0;
		}

		for (int ii = 1;ii < Dutm + 1;ii++)
		{
			for (int i = 0;i < DutRoi.height;i++)
			{
				for (int j = 0;j < DutRoi.width;j++)
				{
					if (ii == (int)dutRoi[i*DutRoi.width + j])
					{
						if (i < dutroipominmaxxy[(ii - 1) * 4 + 2])
						{
							dutroipominmaxxy[(ii - 1) * 4 + 2] = i;
						}
						if (i > dutroipominmaxxy[(ii - 1) * 4 + 3])
						{
							dutroipominmaxxy[(ii - 1) * 4 + 3] = i;
						}
						if (j < dutroipominmaxxy[(ii - 1) * 4])
						{
							dutroipominmaxxy[(ii - 1) * 4] = j;
						}
						if (j > dutroipominmaxxy[(ii - 1) * 4 + 1])
						{
							dutroipominmaxxy[(ii - 1) * 4 + 1] = j;
						}
					}
				}

			}
		}
		int dutcirroiminy = dutroipominmaxxy[temmaker * 4 + 2];
		int dutcirroimaxy = dutroipominmaxxy[temmaker * 4 + 3];

		IMG_RCOORD*dutroidispominmaxxy = (IMG_RCOORD*)malloc(Dutm * sizeof(IMG_RCOORD));
		for (int i = 0;i < Dutm;i++)
		{
			dutroidispominmaxxy[i].x = float((dutroipominmaxxy[i * 4 + 1] + dutroipominmaxxy[i * 4]) * 1.0 / 2);
			dutroidispominmaxxy[i].y = float((dutroipominmaxxy[i * 4 + 3] + dutroipominmaxxy[i * 4 + 2]) * 1.0 / 2);
		}

		if (fabs(dutcirroimaxy - dutcirroiminy) > 100)
		{
			int tempx = (int)(dutroipominmaxxy[temmaker * 4] + dutroipominmaxxy[temmaker * 4 + 1]) / 2;
			int temp = 0;
			for (int i = 1;i < DutRoi.height - 1;i++)
			{

				if (0 != (int)dutRoi[i*DutRoi.width + tempx] && 0 == (int)dutRoi[(i - 1)*DutRoi.width + tempx] && temp == 0)
				{
					dutcirroiminy = i;
					temp++;
				}
				if (0 == (int)dutRoi[i*DutRoi.width + tempx] && 0 != (int)dutRoi[(i - 1)*DutRoi.width + tempx] && temp <= 1)
				{
					dutcirroimaxy = i;
					temp++;
				}
				if (temp == 2)
				{
					break;
				}
			}

		}


		/*	ofstream ljki("dut00.txt");
		for (int i = 0;i < DutRoi.height;i++)
		{
		for (int j = 0;j < DutRoi.width;j++)
		{
		ljki << (double)dutRoi[i*DutRoi.width + j] << "  ";
		}
		ljki << endl;
		}
		ljki.close();*/

		int tempdum = 0;
		for (int i = 0;i < Dutm;i++)
		{
			if (i == temmaker)
			{
				continue;
			}
			if (dutcirroiminy > dutroidispominmaxxy[i].y || dutcirroimaxy < dutroidispominmaxxy[i].y)
			{
				tempdum++;
			}
		}
		if (tempdum != 0)
		{
			if (Dutm > 3)
			{
				if ((tempdum - (Dutm - 3)) > 0)
				{
					free(dutroidispominmaxxy);
					free(dutroipominmaxxy);
					free(arar);
					free(dutRoi);
					//free(DutNumMar);
					free(DutroiNumMar);
					free(pominmaxxy);
					free(dispominmaxxy);
					free(recodelinedstRoicopy);
					free(recoderowdstRoicopy);
					free(dstRoicopy);
					free(srcRoi);
					free(dstRoi);
					free(NumMar);
					free(remarkersnummicarea);
					free(roiroiar);
					free(roiroinmaxxy);
					return 90;
				}
			}
			else if (Dutm >= 2)
			{
				if ((tempdum - (Dutm - 2)) > 0)
				{
					free(dutroidispominmaxxy);
					free(dutroipominmaxxy);
					free(arar);
					free(dutRoi);
					//free(DutNumMar);
					free(DutroiNumMar);
					free(pominmaxxy);
					free(dispominmaxxy);
					free(recodelinedstRoicopy);
					free(recoderowdstRoicopy);
					free(dstRoicopy);
					free(srcRoi);
					free(dstRoi);
					free(NumMar);
					free(remarkersnummicarea);
					free(roiroiar);
					free(roiroinmaxxy);
					return 90;
				}

			}
		}
		else if (tempdum > 0 || Dutm > 3 || Dutm < 2)
		{
			free(dutroidispominmaxxy);
			free(dutroipominmaxxy);
			free(arar);
			free(dutRoi);
			//free(DutNumMar);
			free(DutroiNumMar);
			free(pominmaxxy);
			free(dispominmaxxy);
			free(recodelinedstRoicopy);
			free(recoderowdstRoicopy);
			free(dstRoicopy);
			free(srcRoi);
			free(dstRoi);
			free(NumMar);
			free(remarkersnummicarea);
			free(roiroiar);
			free(roiroinmaxxy);
			return 90;
		}

		int treecirclepos = 0;
		int treecircleposmax = 0;
		int treecircleposmin = 0;
		//vector<IMG_RCOORD> xy;
		vector<int> xypos;
		for (int i = tempheightmin;i < tempheightmax + 1;i++)
		{
			if ((recodelinedstRoicopy[i] > cbmdut.lastfindth && recodelinedstRoicopy[i + 1] <= cbmdut.lastfindth) ||
				(recodelinedstRoicopy[i] <= cbmdut.lastfindth && recodelinedstRoicopy[i + 1] > cbmdut.lastfindth))
			{
				xypos.push_back(i);
			}
		}

		for (unsigned int i = 1;i < xypos.size() - 1;i = i + 2)
		{
			if (treecirclepos < fabs(xypos[i + 1] - xypos[i]))
			{
				treecirclepos = (int)fabs(xypos[i + 1] - xypos[i]);
				treecircleposmin = i;
				treecircleposmax = i + 1;
			}
		}
		//if ((treecirclepos - 25) > 10)
		//{
		//	vector<int>().swap(xypos);
		//	free(recodelinedstRoicopy);
		//	free(recoderowdstRoicopy);
		//	free(dstRoicopy);
		//	free(srcRoi);
		//	free(dstRoi);
		//	free(NumMar);
		//	return -100;//不是正常DUT
		//}
		treecirclepos = int((xypos[treecircleposmin] + xypos[treecircleposmax])*1.0 / 2 + 0.5);

		int angle = (treecirclepos - tempheightmin) < (tempheightmax - treecirclepos) ? 0 : 180;
		param.angle = angle;
		Angle = angle;
		//free(srcroiroi);
		vector<int>().swap(xypos);
		free(arar);
		free(dutroidispominmaxxy);
		free(dutroipominmaxxy);
		free(dutRoi);
		//free(DutNumMar);
		free(DutroiNumMar);
		free(pominmaxxy);
		free(dispominmaxxy);
		free(recodelinedstRoicopy);
		free(recoderowdstRoicopy);
		free(dstRoicopy);
		free(srcRoi);
		free(dstRoi);
		free(NumMar);
		free(remarkersnummicarea);
		free(roiroiar);
		free(roiroinmaxxy);
		return angle;
		//ippiAbsDiffC_8u_C1R(dstRoicopy, Roi.width, dstRoicopy, Roi.width, Roi, 255);
	}
	//else
	{
		int treecirclepos = 0;
		int treecircleposmax = 0;
		int treecircleposmin = 0;
		//vector<IMG_RCOORD> xy;
		vector<int> xypos;
		for (int i = tempheightmin;i < tempheightmax + 1;i++)
		{
			if ((recodelinedstRoicopy[i] > cbmdut.lastfindth && recodelinedstRoicopy[i + 1] <= cbmdut.lastfindth) ||
				(recodelinedstRoicopy[i] <= cbmdut.lastfindth && recodelinedstRoicopy[i + 1] > cbmdut.lastfindth))
			{
				xypos.push_back(i);
			}
		}

		for (unsigned int i = 1;i < xypos.size() - 1;i = i + 2)
		{
			if (treecirclepos < fabs(xypos[i + 1] - xypos[i]))
			{
				treecirclepos = (int)fabs(xypos[i + 1] - xypos[i]);
				treecircleposmin = i;
				treecircleposmax = i + 1;
			}
		}
		//if ((treecirclepos - 25) > 10)
		//{
		//	vector<int>().swap(xypos);
		//	free(recodelinedstRoicopy);
		//	free(recoderowdstRoicopy);
		//	free(dstRoicopy);
		//	free(srcRoi);
		//	free(dstRoi);
		//	free(NumMar);
		//	return -100;//不是正常DUT
		//}
		treecirclepos = int((xypos[treecircleposmin] + xypos[treecircleposmax])*1.0 / 2 + 0.5);

		int angle = (treecirclepos - tempheightmin) < (tempheightmax - treecirclepos) ? 0 : 180;
		param.angle = angle;
		Angle = angle;
		//	free(srcroiroi);
		vector<int>().swap(xypos);

		free(recodelinedstRoicopy);
		free(recoderowdstRoicopy);
		free(dstRoicopy);
		free(srcRoi);
		free(dstRoi);
		free(NumMar);
		free(remarkersnummicarea);
		return angle;
	}
	//

	//int reversal = 0;
	//VisBC_LabelMarker(dstRoicopy, Roi, reversal);
	//VisBC_OpenClose(dstRoicopy, Roi);
	//reversal = 0;
	//VisBC_LabelMarker(dstRoicopy, Roi, reversal);
	//hp111 = fopen("p73.txt", "w");
	//for (int i = 0;i < Roi.height;i++)
	//{
	//	for (int j = 0;j < Roi.width;j++)
	//	{
	//		fprintf(hp111, "%d ", dstRoicopy[i*Roi.width + j]);
	//	}
	//	fprintf(hp111, "\n");
	//}
	//fclose(hp111);
	//int *reversaltreecircle = (int*)malloc(reversal * sizeof(int));
	//for (int i = 0;i < reversal;i++)
	//{
	//	reversaltreecircle[i] = 0;
	//}
	//for (int i = 0;i < Roi.height;i++)
	//{
	//	for (int j = 0;j < Roi.width;j++)
	//	{
	//		if (0 != (int)dstRoicopy[i*Roi.width + j])
	//		{
	//			reversaltreecircle[(int)dstRoicopy[i*Roi.width + j] - 1]++;
	//		}
	//	}
	//}
	//for (int i = 0; i < Roi.height; i++)
	//{
	//	for (int j = 0; j < Roi.width; j++)
	//	{
	//		if (NumMar[dstRoi[i*Roi.width + j] - 1] >(float)Roi.width*(float)Roi.height*0.050 || NumMar[dstRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*0.0016)
	//			//if (NumMar[srcRoi[i*Roi.width + j] - 1] < (float)Roi.width*(float)Roi.height*0.005 || NumMar[srcRoi[i*Roi.width + j] - 1] > (float)Roi.width*(float)Roi.height*0.025)
	//		{
	//			dstRoicopy[i*Roi.width + j] = 0;
	//			//dstRoi[i*Roi.width + j] = 0;
	//		}
	//		
	//	}
	//}
	//reversal = 0;
	//VisBC_LabelMarker(dstRoicopy, Roi, reversal);
	//cout << "haha" << endl;
	//IMG_INT status = 0;
	//IMG_INT threshold = 200;
	//IMG_UBYTE *dstRoiE;// = NULL;
	//edgeInformation *edgeArray_roi;
	//IMG_INT numroi = 0;
	//IMG_WORD *dstRoi0;
	//dstRoiE = (IMG_UBYTE*)malloc(Roi.height*Roi.width * sizeof(IMG_UBYTE));
	//dstRoi0 = (Ipp16s *)malloc(sizeof(Ipp16s)*Roi.height*Roi.width);
	//IMG_SIZE roiSize;
	//roiSize.height = (IMG_UWORD)Roi.height;
	//roiSize.width = (IMG_UWORD)Roi.width;
	//status = VisEdge_detection(dstRoi, roiSize, threshold, dstRoi0, dstRoiE, edgeArray_roi, numroi);
	//hp111 = fopen("pyramid2.txt", "w");
	//for (int i = 0;i < Roi.height;i++)
	//{
	//	for (int j = 0;j < Roi.width;j++)
	//	{
	//		fprintf(hp111, "%d ", dstRoiE[i*Roi.width + j]);
	//	}
	//	fprintf(hp111, "\n");
	//}
	//fclose(hp111);
	//ofstream kp("1.txt");
	//for (int i=0;i < numroi;i++)
	//{
	//	kp << edgeArray_roi[i].xyDecimal.x << "  " << edgeArray_roi[i].xyDecimal.y << endl;
	//}
	//kp << endl;
	//free(dstRoiE);
	//free(dstRoi0);

	return 0;
}

int CBmDUTTrayRecognization::SrcLine(Ipp8u*srcRoi, IMG_SIZE roiSize, NEWLINE &line)
{
	int status = 0;
	IMG_INT threshold = 0;
	IMG_UBYTE *dstRoiE;// = NULL;
	edgeInformation *edgeArray_roi;
	IMG_INT numroi = 0;
	IMG_WORD *dstRoi0;
	dstRoiE = (IMG_UBYTE*)malloc(roiSize.height*roiSize.width * sizeof(IMG_UBYTE));
	dstRoi0 = (Ipp16s *)malloc(sizeof(Ipp16s)*roiSize.height*roiSize.width);

	status = VisEdge_detection(srcRoi, roiSize, threshold, dstRoi0, dstRoiE, edgeArray_roi, numroi);
	if (status != 0)
	{
		return -1;
	}

	IMG_RCOORD* point = (IMG_RCOORD*)malloc(numroi * sizeof(IMG_RCOORD));
	for (int i = 0;i < numroi;i++)
	{
		point[i] = edgeArray_roi[i].xyDecimal;
	}

	IMG_BOOL kj = VisOmmLine_Fitting(point, line, numroi);
	/*if (kj == false)
	{
	return -1;
	}*/

	if (edgeArray_roi != nullptr)
		free(edgeArray_roi);
	free(dstRoiE);
	free(point);
	free(dstRoi0);
	return status;
}

IMG_BOOL CBmDUTTrayRecognization::VisOmmLine_Fitting(IMG_RCOORD* point, NEWLINE &line, IMG_INT m)
{
	bool status = true;
	if (m < 2)
	{
		return false;
	}
	float tempx = 0;
	float avgx = 0;
	float tempx2 = 0;
	for (int i = 0;i < m;i++)
	{
		tempx += point[i].x;
	}
	avgx = tempx / m;
	for (int i = 0;i < m;i++)
	{
		if (fabs(point[i].x - avgx) < 0.5)
		{
			tempx2++;
		}
	}
	if ((tempx2*1.0 / m) >= 0.7)
	{
		line.k = 9999;//斜率不存在
		line.b = avgx;
		line.A = 1;
		line.B = 0;
		line.C = -avgx;
		return status;
	}

	float lastsum = 0, nowsum = 0;
	float *A;
	A = (float *)malloc(2 * sizeof(float));
	A[0] = A[1] = 0;
	float *B, *C, *recordB, *recordC;
	B = (float *)malloc(2 * m * sizeof(float));
	C = (float *)malloc(m * sizeof(float));
	recordB = (float *)malloc(2 * m * sizeof(float));
	recordC = (float *)malloc(m * sizeof(float));
	float *error;
	float *error_weight;
	error_weight = (float *)malloc(m * sizeof(float));
	error = (float *)malloc(m * sizeof(float));
	for (int i = 0; i < m; i++)
	{
		error_weight[i] = 1.0;
		error[i] = 0;
	}
	int temp_time = 0;
	do {
		lastsum = nowsum;
		nowsum = 0;

		for (int i = 0, j = 0;i < 2 * m;i = i + 2, j++)
		{
			recordB[i] = B[i] = point[j].x * error_weight[j];
			recordB[i + 1] = B[i + 1] = 1 * error_weight[j];
			recordC[j] = C[j] = point[j].y * error_weight[j];
		}

		int info = LAPACKE_sgels(LAPACK_ROW_MAJOR, 'N', m, 2, 1, B, 2, C, 1);
		if (info != 0)
		{
			status = false;
			goto end;
		}
		A[0] = C[0];
		A[1] = C[1];
		cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, 1, 2, 1, recordB, 2, A, 1, -1, recordC, 1);

		for (int i = 0; i < (m); i++)
		{
			error[i] = recordC[i] * recordC[i];
			nowsum = error[i] + nowsum;
		}
		for (int i = 0; i < m; i++)
		{
			error_weight[i] = float(1.0 / (1 + exp((error[i]) / (fitting_sigma*fitting_sigma))));
		}
		temp_time++;
	} while (abs(nowsum - lastsum) > cbmdut.linefiterrorAdjacenterror && temp_time < cbmdut.linefitintimes);

	float b = C[1];
	float k = C[0];
	float fit_error = nowsum;

	line.k = k;
	line.b = b;
	line.A = k;
	line.B = -1;
	line.C = b;

end:
	if (error_weight != nullptr) free(error_weight);
	if (A != nullptr) free(A);
	if (B != nullptr) free(B);
	if (C != nullptr) free(C);
	if (recordB != nullptr) free(recordB);
	if (recordC != nullptr) free(recordC);
	if (error != nullptr) free(error);
	return status;
}

IMG_VVOID CBmDUTTrayRecognization::VisBC_Fix_Sobel(IMG_UBYTE * pSrc, IppiSize roiSize, Ipp16s *&pDst, Ipp32f *&pAngle)
{
	IppiMaskSize mask = ippMskSize3x3;
	IppiBorderType bordertype = ippBorderRepl; //Border is replicated from the edge pixels
	Ipp16s *pHoriz, *pVert;

	int srcStep = roiSize.width * sizeof(Ipp8u);

	int dstStep = roiSize.width * sizeof(Ipp16s);
	int angleStep = roiSize.width * sizeof(Ipp32f);
	int bufferSize;
	int bufLen = roiSize.width * roiSize.height;
	//IppStatus statusVert, statusHoriz, status;
	Ipp8u *pBuffer;
	IppNormType normType = ippNormL2;//input gradient magnitude

	pVert = (Ipp16s *)malloc(sizeof(Ipp16s)*bufLen);
	pHoriz = (Ipp16s *)malloc(sizeof(Ipp16s)*bufLen);
	//pAngle = (Ipp32f *)malloc(sizeof(Ipp32f)*bufLen);
	//pDst = (Ipp16s *)malloc(sizeof(Ipp16s)*bufLen);

	ippiGradientVectorGetBufferSize(roiSize, mask, ipp16s, 1, &bufferSize);
	pBuffer = (Ipp8u *)malloc(bufferSize);
	ippiGradientVectorSobel_8u16s_C1R(pSrc, srcStep, pVert, dstStep, pHoriz, dstStep, pDst, dstStep, pAngle, angleStep, roiSize, mask, normType, bordertype, NULL, pBuffer);

	if (pVert != nullptr)
		free(pVert);
	if (pHoriz != nullptr)
		free(pHoriz);
	if (pBuffer != nullptr)
		free(pBuffer);
}
//给定一个值threshold，计算最大类间方差
IMG_REAL CBmDUTTrayRecognization::getIntraClassVariance(Ipp16s* src, IMG_INT srcRows, IMG_INT srcCols, IMG_INT &varTh)//int &varian)
{
	//intra-class variance
	float varian = 0;

	int sumPixel = srcRows*srcCols;
	int sumGrayValue = 0;
	int average = 0;

	int sumApixel = 0;
	double PA = 0;
	int sumAgray = 0;
	int averageA = 0;

	int sumBpixel = 0;
	double PB = 0;
	int sumBgray = 0;
	int averageB = 0;

	for (int i = 0; i < sumPixel; i++)
	{
		sumGrayValue = sumGrayValue + src[i];
		if (src[i] < varTh)
		{
			sumApixel++;
			sumAgray = sumAgray + src[i];
		}
	}

	average = sumGrayValue / sumPixel;
	PA = (double)sumApixel / sumPixel;
	if (sumApixel > 0)
	{
		averageA = sumAgray / sumApixel;
	}
	else
	{
		averageA = 0;
	}

	sumBpixel = sumPixel - sumApixel;
	PB = 1.0 - PA;
	sumBgray = sumGrayValue - sumAgray;
	if (sumBpixel > 0)
	{
		averageB = sumBgray / sumBpixel;
	}
	else
	{
		averageB = 0;
	}

	//ICV = PA?(MA?M)2 + PB?(MB?M)2
	varian = (float)(PA * (averageA - average) * (averageA - average)) + (float)(PB * (averageB - average) * (averageB - average));

	return varian;
}
//抛物线边缘检测
IMG_INT CBmDUTTrayRecognization::VisEdge_detection(IMG_UBYTE *srcRoi, IMG_SIZE roiSize, IMG_INT threshold, IMG_WORD *&dstRoi, IMG_UBYTE *dstRoiE, edgeInformation *&edgeArray, IMG_INT &eNum)
{
	//如果阈值小于0，函数直接返回-1
	if (threshold < 0)
	{
		return -1;
	}

	int roiRows = roiSize.height;
	int roiCols = roiSize.width;

	//角度信息
	Ipp32f *angAll;
	angAll = (Ipp32f*)malloc(roiRows*roiCols * sizeof(Ipp32f));

	std::vector<edgeInformation> edgeInfor;
	edgeInformation edInf;

	int k = 0;//记录边缘点的个数
	float k1;//抛物线拟合的三个已知点
	float k2;
	float k3;
	float deci;//抛物线拟合顶点的小数部分，即对应的亚像素
	float sumx = 0;//边缘点的x坐标之和
	float sumy = 0;
	int numberChannels = 1; //the source image is single channel

	IppiSize dstRoiSize = { roiCols,roiRows };

	//dstRoi = (Ipp16s *)malloc(sizeof(Ipp16s)*roiRows*roiCols);
	VisBC_Fix_Sobel(srcRoi, dstRoiSize, dstRoi, angAll);

	if (threshold == 0)
	{
		//Otsu法，遍历所有的灰度值，从1到255，使intra-class invariance最大的那个值，即为要求的阈值
		int varian = 0;
		int temp = 0;
		for (int p = 1; p < 1250; p++)
		{
			temp = (int)getIntraClassVariance(dstRoi, roiRows, roiCols, p);
			if (varian < temp)
			{
				varian = temp;
				threshold = p;
			}
		}
	}
	//printf("%d\n",threshold);

	//到亚像素
	for (int i = 1; i < roiRows - 1; i++)
	{
		for (int j = 1; j < roiCols - 1; j++)
		{
			if (dstRoi[j + i*roiCols] > threshold)
			{
				angAll[j + i * roiCols] = (float)180 - (float)angAll[j + i * roiCols] / (float)(PI * 180);
				if ((angAll[j + i*roiCols] > 22.5) && (angAll[j + i*roiCols] < 67.5))
				{
					if ((dstRoi[j + i*roiCols] > dstRoi[j - 1 + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j + 1) + (i + 1)*roiCols]))
					{
						k1 = (float)dstRoi[j - 1 + (i - 1)*roiCols];
						k2 = (float)dstRoi[j + i*roiCols];
						k3 = (float)dstRoi[(j + 1) + (i + 1)*roiCols];
						deci = (float)(k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));


						edInf.xyInteger.x = j;
						edInf.xyInteger.y = i;
						edInf.xyDecimal.x = j + deci;
						edInf.xyDecimal.y = i + deci;
						edInf.gradient = dstRoi[j + i*roiCols];
						edInf.angle = angAll[j + i*roiCols];
						edgeInfor.push_back(edInf);
						k++;
					}
				}
				else
				{
					if ((angAll[j + i*roiCols] > 202.5) && (angAll[j + i*roiCols] < 247.5))
					{
						if ((dstRoi[j + i*roiCols] > dstRoi[j - 1 + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j + 1) + (i + 1)*roiCols]))
						{
							k3 = (float)dstRoi[j - 1 + (i - 1)*roiCols];
							k2 = (float)dstRoi[j + i*roiCols];
							k1 = (float)dstRoi[(j + 1) + (i + 1)*roiCols];
							deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

							edInf.xyInteger.x = j;
							edInf.xyInteger.y = i;
							edInf.xyDecimal.x = j - deci;
							edInf.xyDecimal.y = i - deci;
							edInf.gradient = dstRoi[j + i*roiCols];
							edInf.angle = angAll[j + i*roiCols];
							edgeInfor.push_back(edInf);
							k++;
						}
					}
					else
					{
						if ((angAll[j + i*roiCols] > 112.5) && (angAll[j + i*roiCols] < 157.5))
						{

							if ((dstRoi[j + i*roiCols] > dstRoi[(j + 1) + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j - 1) + (i + 1)*roiCols]))
							{
								k1 = (float)dstRoi[(j + 1) + (i - 1)*roiCols];
								k2 = (float)dstRoi[j + i*roiCols];
								k3 = (float)dstRoi[(j - 1) + (i + 1)*roiCols];
								deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

								edInf.xyInteger.x = j;
								edInf.xyInteger.y = i;
								edInf.xyDecimal.x = j - deci;
								edInf.xyDecimal.y = i + deci;
								edInf.gradient = dstRoi[j + i*roiCols];
								edInf.angle = angAll[j + i*roiCols];
								edgeInfor.push_back(edInf);
								k++;
							}
						}
						else
						{
							if ((angAll[j + i*roiCols] > 292.5) && (angAll[j + i*roiCols] < 337.5))
							{
								if ((dstRoi[j + i*roiCols] > dstRoi[(j + 1) + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j - 1) + (i + 1)*roiCols]))
								{
									k3 = (float)dstRoi[(j + 1) + (i - 1)*roiCols];
									k2 = (float)dstRoi[j + i*roiCols];
									k1 = (float)dstRoi[(j - 1) + (i + 1)*roiCols];
									deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

									edInf.xyInteger.x = j;
									edInf.xyInteger.y = i;
									edInf.xyDecimal.x = j + deci;
									edInf.xyDecimal.y = i - deci;
									edInf.gradient = dstRoi[j + i*roiCols];
									edInf.angle = angAll[j + i*roiCols];
									edgeInfor.push_back(edInf);
									k++;
								}
							}
							else
							{
								if (((angAll[j + i*roiCols] >= -1) && (angAll[j + i*roiCols] <= 22.5)) || ((angAll[j + i*roiCols] >= 337.5) && (angAll[j + i*roiCols] <= 361)))
								{
									if ((dstRoi[j + i*roiCols] > dstRoi[(j - 1) + i*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j + 1) + i*roiCols]))
									{
										k1 = (float)dstRoi[(j - 1) + i*roiCols];
										k2 = (float)dstRoi[j + i*roiCols];
										k3 = (float)dstRoi[(j + 1) + i*roiCols];
										deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

										edInf.xyInteger.x = j;
										edInf.xyInteger.y = i;
										edInf.xyDecimal.x = j + deci;
										edInf.xyDecimal.y = (float)i;
										edInf.gradient = dstRoi[j + i*roiCols];
										edInf.angle = angAll[j + i*roiCols];
										edgeInfor.push_back(edInf);
										k++;
									}
								}
								else
								{
									if ((angAll[j + i*roiCols] <= 202.5) && (angAll[j + i*roiCols] >= 157.5))
									{
										if ((dstRoi[j + i*roiCols] > dstRoi[(j - 1) + i*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[(j + 1) + i*roiCols]))
										{
											k3 = (float)dstRoi[(j - 1) + i*roiCols];
											k2 = (float)dstRoi[j + i*roiCols];
											k1 = (float)dstRoi[(j + 1) + i*roiCols];
											deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

											edInf.xyInteger.x = j;
											edInf.xyInteger.y = i;
											edInf.xyDecimal.x = j - deci;
											edInf.xyDecimal.y = (float)i;
											edInf.gradient = dstRoi[j + i*roiCols];
											edInf.angle = angAll[j + i*roiCols];
											edgeInfor.push_back(edInf);
											k++;
										}
									}
									else
									{
										if ((angAll[j + i*roiCols] >= 67.5) && (angAll[j + i*roiCols] <= 112.5))
										{

											if ((dstRoi[j + i*roiCols] > dstRoi[j + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[j + (i + 1)*roiCols]))
											{
												k1 = (float)dstRoi[j + (i - 1)*roiCols];
												k2 = (float)dstRoi[j + i*roiCols];
												k3 = (float)dstRoi[j + (i + 1)*roiCols];
												deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

												edInf.xyInteger.x = j;
												edInf.xyInteger.y = i;
												edInf.xyDecimal.x = (float)j;
												edInf.xyDecimal.y = i + deci;
												edInf.gradient = dstRoi[j + i*roiCols];
												edInf.angle = angAll[j + i*roiCols];
												edgeInfor.push_back(edInf);
												k++;
											}
										}
										else
										{
											if ((angAll[j + i*roiCols] >= 247.5) && (angAll[j + i*roiCols] <= 292.5))
											{
												if ((dstRoi[j + i*roiCols] > dstRoi[j + (i - 1)*roiCols]) && (dstRoi[j + i*roiCols] >= dstRoi[j + (i + 1)*roiCols]))
												{
													k3 = dstRoi[j + (i - 1)*roiCols];
													k2 = dstRoi[j + i*roiCols];
													k1 = dstRoi[j + (i + 1)*roiCols];
													deci = (k3 - k1) / (float)((float)2.0*(2.0*k2 - k1 - k3));

													edInf.xyInteger.x = j;
													edInf.xyInteger.y = i;
													edInf.xyDecimal.x = (float)j;
													edInf.xyDecimal.y = i - deci;
													edInf.gradient = dstRoi[j + i*roiCols];
													edInf.angle = angAll[j + i*roiCols];
													edgeInfor.push_back(edInf);
													k++;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}


	eNum = k;
	//二值图
	for (int t = 0; t < roiCols*roiRows; t++)//二值图像，所有像素先都赋值为0，边缘点赋值255
	{
		dstRoiE[t] = 0;
	}
	for (int q = 0; q < k; q++)
	{
		dstRoiE[edgeInfor[q].xyInteger.x + edgeInfor[q].xyInteger.y * roiCols] = 255;
	}
	edgeArray = (edgeInformation*)malloc(k * sizeof(edgeInformation));
	for (int i = 0; i < k; i++)
	{
		edgeArray[i] = edgeInfor[i];
	}

	vector<edgeInformation>().swap(edgeInfor);

	free(angAll);
	return 0;
}

IMG_INT CBmDUTTrayRecognization::VisOmmIntersection_oftwoline(FloatLine line1, FloatLine line2, IMG_RCOORD &intersectionpoint)
{
	int status = 0;

	//if (line1.startPoint.x < 0 || line1.startPoint.y < 0 ||
	//	line1.endPoint.x < 0 || line1.endPoint.y < 0 ||
	//	line2.startPoint.x < 0 || line2.startPoint.y < 0 ||
	//	line2.endPoint.x < 0 || line2.endPoint.y < 0)
	//{
	//	status = -1;
	//	return status;
	//}//越界
	//else 
	if ((((line1.endPoint.x == line2.endPoint.x) && (line1.endPoint.y == line2.endPoint.y)) &&
		((line1.endPoint.x == line2.startPoint.x) && (line1.endPoint.y == line2.startPoint.y))) ||
		(((line1.startPoint.x == line2.endPoint.x) && (line1.startPoint.y == line2.endPoint.y)) &&
		((line1.startPoint.x == line2.startPoint.x) && (line1.startPoint.y == line2.startPoint.y))))
	{
		//重合
		status = -2;
		intersectionpoint.x = -1;
		intersectionpoint.y = -1;
		return status;
	}//重合

	if (line1.startPoint.x == line1.endPoint.x)
	{
		intersectionpoint.x = line1.startPoint.x;
		intersectionpoint.y = float(line2.endPoint.y + 1.0*(line2.startPoint.y - line2.endPoint.y) / (line2.startPoint.x - line2.endPoint.x)*(line1.startPoint.x - line2.endPoint.x));
		return status;
	}
	if (line2.startPoint.x == line2.endPoint.x)
	{
		intersectionpoint.x = line2.startPoint.x;
		intersectionpoint.y = float(line1.endPoint.y + 1.0*(line1.startPoint.y - line1.endPoint.y) / (line1.startPoint.x - line1.endPoint.x)*(line2.startPoint.x - line1.endPoint.x));
		return status;
	}

	if (line1.startPoint.x == line1.endPoint.x&&line2.startPoint.x == line2.endPoint.x)
	{
		status = -3;
		return status;
	}

	double temp_k1 = 1.0*(line1.startPoint.y - line1.endPoint.y) / (line1.startPoint.x - line1.endPoint.x);
	double temp_k2 = 1.0*(line2.startPoint.y - line2.endPoint.y) / (line2.startPoint.x - line2.endPoint.x);

	if (temp_k1 == temp_k2)//平行
	{
		status = -3;
		intersectionpoint.x = -2;
		intersectionpoint.y = -2;
		return status;
	}

	intersectionpoint.x = float((line1.endPoint.y - line2.endPoint.y + temp_k2*line2.endPoint.x - temp_k1*line1.endPoint.x)*1.0 / (temp_k2 - temp_k1));
	intersectionpoint.y = float(line1.endPoint.y + temp_k1*(intersectionpoint.x - line1.endPoint.x));

	if (intersectionpoint.x < 0 || intersectionpoint.y < 0)
	{
		status = -1;
		return status;
	}
	return status;
}

int CBmDUTTrayRecognization::Histogram(Ipp8u*srcRoi, IppiSize Roi, Ipp8u*dstRoi, double &sumpixel)
{

	float *histo = (float*)malloc(256 * sizeof(float));
	float *histopro = (float*)malloc(256 * sizeof(float));
	for (int i = 0;i < 256;i++)
	{
		histo[i] = 0;
		histopro[i] = 0;
	}
	sumpixel = 0;
	for (int i = 0;i < Roi.height;i++)
	{
		for (int j = 0;j < Roi.width;j++)
		{
			sumpixel += srcRoi[i*Roi.width + j];
			histo[srcRoi[i*Roi.width + j]]++;
		}
	}
	float sumpixel1 = (float)Roi.width*Roi.height;
	sumpixel = sumpixel*1.0 / sumpixel1;


	histopro[0] = histo[0];
	for (int i = 1;i < 256;i++)
	{
		histopro[i] = histopro[i - 1] + histo[i];
	}

	//0.7针对不同亮度图片加权处理
	for (int i = 0;i < Roi.height;i++)
	{
		for (int j = 0;j < Roi.width;j++)
		{
			if (histopro[srcRoi[i*Roi.width + j]] / sumpixel1 < cbmdut.Weightedproportion)
			{
				dstRoi[i*Roi.width + j] = 0;
			}
			else
			{
				dstRoi[i*Roi.width + j] = (Ipp8u)(255 * histopro[srcRoi[i*Roi.width + j]] / sumpixel1);
			}
		}
	}


	free(histo);
	free(histopro);
	return 0;
}