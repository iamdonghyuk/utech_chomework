// DataSocket.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "UDPClient.h"
#include "UDPClientDlg.h"
#include "DataSocket.h"


// CDataSocket

CDataSocket::CDataSocket(CUDPClientDlg * pDlg)
{
	m_pDlg = pDlg;
}

CDataSocket::~CDataSocket()
{
}

void CDataSocket::OnClose(int nErrorCode)
{
	CSocket::OnClose(nErrorCode);
	m_pDlg->ProcessClose(this, nErrorCode);
}

void CDataSocket::OnReceive(int nErrorCode)
{
	CSocket::OnReceive(nErrorCode);
	m_pDlg->ProcessReceive(this, nErrorCode);
}