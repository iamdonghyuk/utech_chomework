// DataSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UDPServer.h"
#include "DataSocket.h"


// CDataSocket

CDataSocket::CDataSocket(CUDPServerDlg *pDlg)
{
	m_pDlg = pDlg;
}

CDataSocket::~CDataSocket()
{
}

void CDataSocket::OnReceive(int nErrorCode)
{
	CSocket::OnReceive(nErrorCode);
	m_pDlg->ProcessReceive(this, nErrorCode);
}

void CDataSocket::OnClose(int nErrorCode)
{
	CSocket::OnClose(nErrorCode);
	m_pDlg->ProcessClose(this, nErrorCode);
}


// CDataSocket 멤버 함수
