
// BMAlgoDev.h : BMAlgoDev Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CBMAlgoDevApp:
// �йش����ʵ�֣������ BMAlgoDev.cpp
//

class CBMAlgoDevApp : public CWinApp
{
public:
	CBMAlgoDevApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	//afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CBMAlgoDevApp theApp;
