#pragma once
#include "UDPServerDlg.h"
// CDataSocket 명령 대상입니다.

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


