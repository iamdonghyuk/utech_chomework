
// UDPServerDlg.h : ��� ����
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

// CUDPServerDlg ��ȭ ����
class CUDPServerDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CUDPServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UDPSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
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
