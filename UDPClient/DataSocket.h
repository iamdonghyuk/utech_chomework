#pragma once
#include "afxsock.h"
#include "UDPClientDlg.h"
// CDataSocket

class CUDPClientDlg;

class CDataSocket : public CSocket
{
public:
	CDataSocket(CUDPClientDlg *pDlg);
	virtual ~CDataSocket();
	CUDPClientDlg *m_pDlg;
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};


