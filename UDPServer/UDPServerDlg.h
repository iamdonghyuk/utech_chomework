
// UDPServerDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcoll.h"
#include "DataSocket.h"

struct ThreadArg
{
	CStringList *pList;
	CDialogEx *pDlg;
	int Thread_run;
};

class CDataSocket;

// CUDPServerDlg 대화 상자
class CUDPServerDlg : public CDialogEx
{
// 생성입니다.
public:
	CUDPServerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UDPSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CEdit m_tx_edit_short;
	CEdit m_tx_edit;
	CEdit m_rx_edit;
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedClose();
	void ProcessReceive(CDataSocket *pSocket, int nErrorCode);
	void ProcessClose(CDataSocket *pSocket, int nErrorCode);
	CWinThread *pThread1, *pThread2;
	ThreadArg arg1, arg2;
	CDataSocket *m_pDataSocket;
};
