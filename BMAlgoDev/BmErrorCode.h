#pragma once
enum _BACK_CODE
{
	//_CODE_THROW = -1,               //抛出异常错误
	//_CODE_OK    = 0,               //处理成功 /  Status OK
//	_CODE_ANGLE_FAIL,              //输入的角度标识既不是0度也不是180度
//	_CODE_PATH_LOST ,              //传入的路径不存在
//	_CODE_FILE_LOST,               //文件丢失
//	_CODE_LOCATE_FAIL,             //定位失败
//	_CODE_UP_LINE_FAIL,            //UpLine Fit Fail
//	_CODE_DOWN_LINE_FAIL,          //DownLine Fit Fail
//	_CODE_LEFT_LINE_FAIL,          //LeftLine Fit Fail
//	_CODE_RIGHT_LINE_FAIL,          //RightLine Fit Fail
//	_CODE_LEFT_RIGHT_OFFSET_ERR ,   // Left_RightLine 角度保护
//	_CODE_UP_DOWN_OFFSET_ERR,      //Up_DownLine 角度保护
//	_CODE_UP_LEFT_OFFSET_ERR,       //Up_LeftLine 角度保护
//	_CODE_FIT_NUM_ERR,              //拟合数量不匹配
//	_CODE_GET_XLD_EMPTY,           //轮廓为空

	//Status( socket/Tray )
//	_CODE_EMPTY,//空料
//	_CODE_PROERROR, //物料异常
//	_CODE_DUT_NORMAL, //有正常DUT
	
	//Socket lock
//	_CODE_LOCK,//锁定
//	_CODE_UNLOCK, //未锁
//	_CODE_OVERRANGE, //所需判断的区域已超出图像范围，该图拍摄范围缺失

	_CODE_THROW = -1,    //抛出异常错误
	_CODE_OK = 0,  //处理成功 /  Status OK
	_CODE_NG = 1,
	_CODE_FIT_NUM_ERR = 2,     //拟合个数不对
	_CODE_GET_XLD_EMPTY = 3, //粗提取轮廓为空
	_CODE_MDL_PARA_PATH_EMPTY = 4, //模板路径不存在
	_CODE_MDL_FILE_LOST = 5,       //模板文件丢失
	_CODE_LOCATE_FAIL = 6,         //初定位失败
	_CODE_CLIP_NUM_ERR = 7,        //拟合线剔除点大于卡尺卡到的点
	_CODE_LINE_DOT_LESS = 8,       //卡尺卡到的点过少
	_CODE_LEFT_LINE_FAIL = 9,
	_CODE_RIGHT_LINE_FAIL = 10,
	_CODE_UP_LINE_FAIL = 11,
	_CODE_DOWN_LINE_FAIL = 12,
	_CODE_GET_SOCKET_POS_FAIL = 13,
	_CODE_MATCH_SOCKET_FAIL = 14,
	_CODE_WRONG_ANGLE_FLAG = 15,
	_CODE_DETECT_BARCODE_FAIL = 16,
	_CODE_LEFT_RIGHT_OFFSET_ERR = 17,
	_CODE_UP_DOWN_OFFSET_ERR = 18,
	_CODE_UP_LEFT_OFFSET_ERR = 19,

	_CODE_NOTFOUNDDUTINTRAY = 20,//Tray中没有DUT
	_CODE_FOUNDDUTINTRAY = 21,//Tray中有DUT
	_CODE_TRAYLOCATIONERROR = 22,//Tray定位失败

	_CODE_ANGLE_DIFF_TOO_LARGE = 23,
	_CODE_LEFT_DOWN_OFFSET_ERR = 24,  //左边与底边夹角过大


	_CODE_ANGLE_FAIL,              //输入的角度标识既不是0度也不是180度
	_CODE_PATH_LOST,              //传入的路径不存在
	_CODE_FILE_LOST,               //文件丢失
	//_CODE_LOCATE_FAIL,             //定位失败
	//_CODE_UP_LINE_FAIL,            //UpLine Fit Fail
	//_CODE_DOWN_LINE_FAIL,          //DownLine Fit Fail
	//_CODE_LEFT_LINE_FAIL,          //LeftLine Fit Fail
	//_CODE_RIGHT_LINE_FAIL,          //RightLine Fit Fail
	//_CODE_LEFT_RIGHT_OFFSET_ERR,   // Left_RightLine 角度保护
	//_CODE_UP_DOWN_OFFSET_ERR,      //Up_DownLine 角度保护
	//_CODE_UP_LEFT_OFFSET_ERR,       //Up_LeftLine 角度保护
	//_CODE_FIT_NUM_ERR,              //拟合数量不匹配
	//_CODE_GET_XLD_EMPTY,           //轮廓为空

	//Status( socket/Tray )
	_CODE_EMPTY,//空料
	_CODE_PROERROR, //物料异常

	_CODE_SOCKETPOSPROTECT, //Socket location error

	_CODE_SOCKETLOCK,				//Socket lock
	_CODE_SOCKETUNLOCK,				//Socket unlock
	_CODE_FIT_CIRCLE_NUM_ERR,
	_CODE_FIND_CIRCLE_ERR,


	_CODE_DUT_NORMAL, //有正常DUT

					  //Socket lock
	_CODE_LOCK,//锁定
	_CODE_UNLOCK, //未锁
	_CODE_OVERRANGE, //所需判断的区域已超出图像范围，该图拍摄范围缺失
};