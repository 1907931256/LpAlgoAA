

#ifndef __SMARTCSS_VITYPE_H__
#define __SMARTCSS_VITYPE_H__



#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifndef CONST
#define CONST               const
#endif

#define FALSE   0
#define TRUE    1
#define NULL    0


#ifndef VI_BASETYPES_DEF
#define VI_BASETYPES_DEF
typedef void				IMG_VVOID;
typedef signed char			IMG_BYTE;
typedef unsigned char		IMG_UBYTE;
typedef signed short		IMG_WORD;
typedef unsigned short		IMG_UWORD;
typedef signed int			IMG_INT;
typedef unsigned int		IMG_UINT;
typedef signed long			IMG_LWORD;
typedef unsigned long		IMG_ULWORD;
typedef signed __int64		IMG_XLWORD;
typedef unsigned __int64	IMG_UXLWORD;
typedef float				IMG_REAL;
typedef double				IMG_LREAL;
typedef IMG_VVOID			*IMG_PVVOID;
typedef IMG_BYTE			*IMG_PBYTE;
typedef IMG_UBYTE			*IMG_PUBYTE;
typedef IMG_WORD			*IMG_PWORD;
typedef IMG_UWORD			*IMG_PUWORD;
typedef IMG_INT				*IMG_PINT;
typedef IMG_UINT			*IMG_PUINT;
typedef IMG_LWORD			*IMG_PLWORD;
typedef IMG_ULWORD			*IMG_PULWORD;
typedef IMG_XLWORD			*IMG_PXLWORD;
typedef IMG_UXLWORD			*IMG_PUXLWORD;
typedef IMG_REAL			*IMG_PREAL;
typedef IMG_LREAL			*IMG_PLREAL;
typedef char				IMG_CHAR;
typedef wchar_t				IMG_WCHAR;
typedef int                 IMG_BOOL;
typedef IMG_CHAR			*IMG_LPSTR;
typedef IMG_WCHAR			*IMG_LPWSTR;
typedef IMG_BOOL            *IMG_PBOOL;

typedef IMG_VVOID			*IMG_PVIBASE;
#endif  /* !VI_BASETYPES_DEF IMG*/




typedef char CHAR;
typedef wchar_t WCHAR;

typedef WCHAR *PWCHAR, *LPWCH, *PWCH;
typedef CONST WCHAR *LPCWCH, *PCWCH;

typedef CHAR *PCHAR, *LPCH, *PCH;
typedef CONST CHAR *LPCCH, *PCCH;


typedef WCHAR *LPWSTR, *PWSTR;
typedef CONST WCHAR *LPCWSTR, *PCWSTR;

typedef CHAR *LPSTR, *PSTR;
typedef CONST CHAR *LPCSTR, *PCSTR;



#ifdef  UNICODE

#ifndef _TCHAR_DEFINED
typedef WCHAR TCHAR, *PTCHAR;
typedef WCHAR TBYTE, *PTBYTE;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */

typedef LPWCH LPTCH, PTCH;
typedef LPCWCH LPCTCH, PCTCH;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR PCTSTR, LPCTSTR;

#else   /* UNICODE */               

#ifndef _TCHAR_DEFINED
typedef char TCHAR, *PTCHAR;
typedef unsigned char TBYTE, *PTBYTE;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */

typedef LPCH LPTCH, PTCH;
typedef LPCCH LPCTCH, PCTCH;
typedef LPSTR PTSTR, LPTSTR;
typedef LPCSTR PCTSTR, LPCTSTR;

#endif /* UNICODE */ 



typedef struct
{
	IMG_WORD     x;
	IMG_WORD     y;
} IMG_COORD;

typedef struct
{
	IMG_REAL     x;
	IMG_REAL     y;
} IMG_RCOORD;

typedef struct
{
	IMG_REAL x, y, z;
} IMG_RCOORD3D;

typedef struct
{
	IMG_UWORD     width;
	IMG_UWORD     height;
} IMG_SIZE;

typedef struct {
	IMG_REAL        width;
	IMG_REAL        height;
}IMG_RSIZE;

typedef struct
{
	IMG_BYTE  *ptr;      /* pointer to the image buffer start address */
	IMG_SIZE  size;
	IMG_UWORD linestep;  /* offset from one row of image buffer to
						 another on the same column in term of pixel */
} IMG_BBUF;

typedef struct
{
	IMG_UBYTE  *ptr;
	IMG_SIZE   size;
	IMG_UWORD  linestep; /* offset from one row of image buffer to
						 another on the same column in term of pixel */
} IMG_UBBUF;

typedef struct
{
	IMG_WORD  *ptr;
	IMG_SIZE  size;
	IMG_UWORD linestep;  /* offset from one row of image buffer to
						 another on the same column in term of pixel */
} IMG_WBUF;

typedef struct
{
	IMG_UWORD  *ptr;
	IMG_SIZE   size;
	IMG_UWORD linestep; /* offset from one row of image buffer to
						another on the same column in term of pixel */
} IMG_UWBUF;

typedef struct
{
	IMG_LWORD  *ptr;
	IMG_SIZE   size;
	IMG_UWORD  linestep; /* offset from one row of image buffer to
						 another on the same column in term of pixel */
} IMG_LWBUF;

typedef struct
{
	IMG_ULWORD *ptr;
	IMG_SIZE   size;
	IMG_UWORD  linestep; /* offset from one row of image buffer to
						 another on the same column in term of pixel */
} IMG_ULBUF;

typedef struct
{
	IMG_REAL  *ptr;
	IMG_SIZE  size;
	IMG_UWORD linestep;  /* offset from one row of image buffer to
						 another on the same column in term of pixel */
} IMG_RBUF;

typedef struct
{
	IMG_LREAL *ptr;
	IMG_SIZE  size;
	IMG_UWORD linestep;  /* offset from one row of image buffer to
						 another on the same column in term of pixel */
} IMG_LRBUF;

typedef struct
{
	IMG_VVOID *ptr;
	IMG_SIZE  size;
	IMG_UWORD linestep;  /* offset from one row of image buffer to
						 another on the same column in term of pixel */
} IMG_VVBUF;

typedef struct
{
	IMG_UXLWORD	*ptr;
	IMG_SIZE	size;
	IMG_UWORD	linestep;
} IMG_UXLBUF;

typedef struct {
	IMG_COORD	coWindowOff;
	IMG_SIZE	szWindowSize;
} IMG_WINDOW;

typedef struct
{
	IMG_RCOORD startPoint;
	IMG_RCOORD endPoint;
}FloatLine;//直线，方向待定

typedef struct
{
	IMG_RCOORD CirCen;//圆心
	IMG_REAL Radius;//半径
}StructCircle;//圆

enum ArcDirection//圆弧方向
{
	CLOCKWISE,//顺时针
	ANTICLOCLWISE//逆时针
};
typedef struct
{
	IMG_RCOORD CirCen;//圆心
	IMG_REAL Radius;//半径
	IMG_RCOORD startPoint;//起点
	IMG_RCOORD endPoint;//终点
	IMG_REAL startAngle;
	IMG_REAL sweepAngle;
	ArcDirection direct;//方向
}StructArc;//圆弧

//typedef struct Circular_Fit
//{
//	IMG_REAL a = 0;
//	IMG_REAL b = 0;
//	IMG_REAL r = 0;
//}CIRCULAR_FITT;//圆拟合

typedef struct
{
	IMG_COORD xyInteger; //像素点
	IMG_RCOORD xyDecimal;//亚像素点
	int gradient;
	float angle;
}edgeInformation;//边缘点

//typedef struct
//{
//	int xMin;
//	int xMax;
//	int yMin;
//	int yMax;
//}RegionPeakPoint;	//连通域四个极值点
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

typedef struct
{
	IMG_COORD topLeft;
	IMG_COORD downRight;
}VisRoiRect;

//typedef struct {
//	int min_x;
//	int min_y;
//	int max_x;
//	int max_y;
//}RoiEdge;
//typedef struct {
//	RoiEdge leftPoint;
//	RoiEdge rightPoint;
//}standardPoint;
typedef struct {
	int min_x;
	int min_y;
	int max_x;
	int max_y;
	float CenterX;
	float CenterY;
}RoiFeature;
typedef struct {
	RoiFeature leftPoint;
	RoiFeature rightPoint;
}standardPoint;

//typedef struct
//{
//	int width;
//	int height;
//}RegionWidthHeight;	//连通域宽高
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

typedef struct
{
	IMG_RCOORD CirCen;//圆心
	IMG_REAL Radius;//半径
	IMG_INT miny;
	IMG_INT minx;
	IMG_INT maxy;
	IMG_INT maxx;
}TempStructCircle;//圆

struct DUTOutPutParam
{
	double x1;	//四个角点(左上、右上、右下、左下)
	double y1;

	double x2;
	double y2;

	double x3;
	double y3;

	double x4;
	double y4;

	double cx;	//原来定义的DUT输出点
	double cy;
	double angle;	//输出角度逆时针为正
	double dist;	//cx,xy到底边距离的一半

};

enum EdgeDirection//边缘方向
{
	BLACKWHITE,//黑到白
	WHITEBLACK, //白到黑
	BOTH  //全部
};
enum RectScanDirection//矩形ROI扫描方向
{
	LEFT, //正面左侧，从位于顶部的开始点看，向左扫描边缘检测区域。
	RIGHT //正面右侧，从位于顶部的开始点看，向右扫描边缘检测区域。
};
typedef struct//谭玲
{
	IMG_RCOORD PointStart; //start point coordinate
	IMG_RCOORD PointEnd;   //end point coordinate
	IMG_REAL NormalVari;   //half width of ROI suject to the direction of normal line //{NOROTATION,VERTICAL,HORIZONTAL}NOROTATION means no rotation in targeted image; VERTICAL means the straight line offered by users is parallelled to y-axis in targeted image; HORIZONTAL means the straight line offered by users is parallelled to x-axis in targeted image
}GenerateRectRoi;  //This struct is the input to generate ROI in terms of straight line
typedef struct
{
	IMG_RCOORD LeftVertex;
	IMG_RCOORD SrcCenterCo;//the center coordinate of ROI in source image
	IMG_RCOORD DstCenterCo;//the center coordinate of ROI 
	IMG_SIZE RectangleSize;
	IMG_REAL RotationAngle;
}RecRoiStruct;//This struct describes the information in source image of targeted ROI
typedef struct HoughLineInfo
{
IMG_REAL roRadius;
IMG_REAL theta;
IMG_INT votePoNum;
			  
}HOUGH_TRANS_INFO;
typedef struct LineSlopeEquation
{
	IMG_REAL Slope;
	IMG_REAL b;
	IMG_REAL Score;
	IMG_RCOORD PointStart;
	IMG_RCOORD PointEnd;
	IMG_REAL FittingError;
}LINE_INFO;
typedef struct {
	GenerateRectRoi roiLine;//线段起始点坐标，范围，以及是否需要旋转
	IMG_INT threshold;//阈值
	RectScanDirection scanDirect;//直线扫描方向
	EdgeDirection edDirectint;//直线边缘方向
	IMG_RCOORD MaskStart;
	IMG_RCOORD MaskEnd;
}LineIn;//线的输入
typedef struct
{
	int c; //像素和的差
	int p; //对应的列
}FP;//记录两个矩形块相减的时候的信息

// namespace vi
// {
	/*
	* VI_VARENUM usage key,
	*/
	enum /*VI_VARENUM*/VIVARTYPE
	{
		TYPE_EMPTY		= 0,
		TYPE_BYTE		= 1,
		TYPE_UBYTE		= 2,
		TYPE_WORD		= 3,
		TYPE_UWORD		= 4,
		TYPE_INT		= 5,
		TYPE_UINT		= 6,
		TYPE_LWORD		= 7,
		TYPE_ULWORD		= 8,
		TYPE_XLWORD		= 9,
		TYPE_UXLWORD	= 10,
		TYPE_REAL		= 11,
		TYPE_LREAL		= 12,

		TYPE_PPVOID		= 100,
		TYPE_PBYTE		= 101,
		TYPE_PUBYTE		= 102,
		TYPE_PWORD		= 103,
		TYPE_PUWORD		= 104,
		TYPE_PINT		= 105,
		TYPE_PUINT		= 106,
		TYPE_PLWORD		= 107,
		TYPE_PULWORD	= 108,
		TYPE_PXLWORD	= 109,
		TYPE_PUXLWORD	= 110,
		TYPE_PREAL		= 111,
		TYPE_PLREAL		= 112,
		TYPE_LPSTR		= 113,
		TYPE_LPWSTR		= 114,

		TYPE_PVIVARIANT = 160,
		TYPE_LPVIBASE	= 161
	};

	//typedef unsigned short VIVARTYPE;

	typedef struct tagVIVARIANT {
		VIVARTYPE vt;
		union
		{
											//TYPE_EMPTY = 0,
				IMG_BYTE		cVal;		//TYPE_BYTE = 1,
				IMG_UBYTE		ucVal;		//TYPE_UBYTE = 2,
				IMG_WORD		wVal;		//TYPE_WORD = 3,
				IMG_UWORD		uwVal;		//TYPE_UWORD = 4,
				IMG_INT			iVal;		//TYPE_INT = 5,
				IMG_UINT		uiVal;		//TYPE_UINT = 6,
				IMG_LWORD		lwVal;		//TYPE_LWORD = 7,
				IMG_ULWORD		ulwVal;		//TYPE_ULWORD = 8,
				IMG_XLWORD		xlwVal;		//TYPE_XLWORD = 9,
				IMG_UXLWORD		uxlwVal;	//TYPE_UXLWORD = 10,
				IMG_REAL		rVal;		//TYPE_REAL = 11,
				IMG_LREAL		lrVal;		//TYPE_LREAL = 12,

				IMG_PVVOID		pVoid;		//TYPE_PPVOID = 100,
				IMG_PBYTE		pcVal;		//TYPE_PBYTE = 101,
				IMG_PUBYTE		pucVal;		//TYPE_PUBYTE = 102,
				IMG_PWORD		pwVal;		//TYPE_PWORD = 103,
				IMG_PUWORD		puwVal;		//TYPE_PUWORD = 104,
				IMG_PINT		piVal;		//TYPE_PINT = 105,
				IMG_PUINT		puiVal;		//TYPE_PUINT = 106,
				IMG_PLWORD		plwVal;		//TYPE_PLWORD = 107,
				IMG_PULWORD		pulwVal;	//TYPE_PULWORD = 108,
				IMG_PXLWORD		pxlwVal;	//TYPE_PXLWORD = 109,
				IMG_PUXLWORD	puxlwVal;	//TYPE_PUXLWORD = 110,
				IMG_PREAL		prVal;		//TYPE_PREAL = 111,
				IMG_PLREAL		plrVal;		//TYPE_PLREAL = 112,
				IMG_LPSTR		pszVal;		//TYPE_LPSTR = 113,
				IMG_LPWSTR		pwszVal;	//TYPE_LPWSTR = 114,
					
				tagVIVARIANT*	pvtVal;		//TYPE_PVIVARIANT = 160
				IMG_PVIBASE     pvibVal;	//TYPE_LPVIBASE = 161,
		} Val;

	} VIVARIANT, *PVIVARIANT;

//}

#endif
/* End of file. */