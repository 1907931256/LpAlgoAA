#pragma once
enum _BACK_CODE
{
	//_CODE_THROW = -1,               //�׳��쳣����
	//_CODE_OK    = 0,               //����ɹ� /  Status OK
//	_CODE_ANGLE_FAIL,              //����ĽǶȱ�ʶ�Ȳ���0��Ҳ����180��
//	_CODE_PATH_LOST ,              //�����·��������
//	_CODE_FILE_LOST,               //�ļ���ʧ
//	_CODE_LOCATE_FAIL,             //��λʧ��
//	_CODE_UP_LINE_FAIL,            //UpLine Fit Fail
//	_CODE_DOWN_LINE_FAIL,          //DownLine Fit Fail
//	_CODE_LEFT_LINE_FAIL,          //LeftLine Fit Fail
//	_CODE_RIGHT_LINE_FAIL,          //RightLine Fit Fail
//	_CODE_LEFT_RIGHT_OFFSET_ERR ,   // Left_RightLine �Ƕȱ���
//	_CODE_UP_DOWN_OFFSET_ERR,      //Up_DownLine �Ƕȱ���
//	_CODE_UP_LEFT_OFFSET_ERR,       //Up_LeftLine �Ƕȱ���
//	_CODE_FIT_NUM_ERR,              //���������ƥ��
//	_CODE_GET_XLD_EMPTY,           //����Ϊ��

	//Status( socket/Tray )
//	_CODE_EMPTY,//����
//	_CODE_PROERROR, //�����쳣
//	_CODE_DUT_NORMAL, //������DUT
	
	//Socket lock
//	_CODE_LOCK,//����
//	_CODE_UNLOCK, //δ��
//	_CODE_OVERRANGE, //�����жϵ������ѳ���ͼ��Χ����ͼ���㷶Χȱʧ

	_CODE_THROW = -1,    //�׳��쳣����
	_CODE_OK = 0,  //����ɹ� /  Status OK
	_CODE_NG = 1,
	_CODE_FIT_NUM_ERR = 2,     //��ϸ�������
	_CODE_GET_XLD_EMPTY = 3, //����ȡ����Ϊ��
	_CODE_MDL_PARA_PATH_EMPTY = 4, //ģ��·��������
	_CODE_MDL_FILE_LOST = 5,       //ģ���ļ���ʧ
	_CODE_LOCATE_FAIL = 6,         //����λʧ��
	_CODE_CLIP_NUM_ERR = 7,        //������޳�����ڿ��߿����ĵ�
	_CODE_LINE_DOT_LESS = 8,       //���߿����ĵ����
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

	_CODE_NOTFOUNDDUTINTRAY = 20,//Tray��û��DUT
	_CODE_FOUNDDUTINTRAY = 21,//Tray����DUT
	_CODE_TRAYLOCATIONERROR = 22,//Tray��λʧ��

	_CODE_ANGLE_DIFF_TOO_LARGE = 23,
	_CODE_LEFT_DOWN_OFFSET_ERR = 24,  //�����ױ߼нǹ���


	_CODE_ANGLE_FAIL,              //����ĽǶȱ�ʶ�Ȳ���0��Ҳ����180��
	_CODE_PATH_LOST,              //�����·��������
	_CODE_FILE_LOST,               //�ļ���ʧ
	//_CODE_LOCATE_FAIL,             //��λʧ��
	//_CODE_UP_LINE_FAIL,            //UpLine Fit Fail
	//_CODE_DOWN_LINE_FAIL,          //DownLine Fit Fail
	//_CODE_LEFT_LINE_FAIL,          //LeftLine Fit Fail
	//_CODE_RIGHT_LINE_FAIL,          //RightLine Fit Fail
	//_CODE_LEFT_RIGHT_OFFSET_ERR,   // Left_RightLine �Ƕȱ���
	//_CODE_UP_DOWN_OFFSET_ERR,      //Up_DownLine �Ƕȱ���
	//_CODE_UP_LEFT_OFFSET_ERR,       //Up_LeftLine �Ƕȱ���
	//_CODE_FIT_NUM_ERR,              //���������ƥ��
	//_CODE_GET_XLD_EMPTY,           //����Ϊ��

	//Status( socket/Tray )
	_CODE_EMPTY,//����
	_CODE_PROERROR, //�����쳣

	_CODE_SOCKETPOSPROTECT, //Socket location error

	_CODE_SOCKETLOCK,				//Socket lock
	_CODE_SOCKETUNLOCK,				//Socket unlock
	_CODE_FIT_CIRCLE_NUM_ERR,
	_CODE_FIND_CIRCLE_ERR,


	_CODE_DUT_NORMAL, //������DUT

					  //Socket lock
	_CODE_LOCK,//����
	_CODE_UNLOCK, //δ��
	_CODE_OVERRANGE, //�����жϵ������ѳ���ͼ��Χ����ͼ���㷶Χȱʧ
};