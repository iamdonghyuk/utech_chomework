#pragma once
#include "UDPServerDlg.h"
// CDataSocket ��� ����Դϴ�.

class CUDPServerDlg;

class CDataSocket : public CSocket
{
public:
	CDataSocket(CUDPServerDlg *pDlg);
	virtual ~CDataSocket();
	CUDPServerDlg *m_pDlg;
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};


