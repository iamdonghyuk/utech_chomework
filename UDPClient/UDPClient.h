
// UDPClient.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CUDPClientApp:
// �� Ŭ������ ������ ���ؼ��� UDPClient.cpp�� �����Ͻʽÿ�.
//

class CUDPClientApp : public CWinApp
{
public:
	CUDPClientApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CUDPClientApp theApp;