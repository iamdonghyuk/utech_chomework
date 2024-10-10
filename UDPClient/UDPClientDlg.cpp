// UDPClientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "UDPClient.h"
#include "UDPClientDlg.h"
#include "afxdialogex.h"
#include "DataSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection tx_cs; // �۽ſ� ���Ǵ� ũ��Ƽ�� ���� ��ü ����
CCriticalSection rx_cs; // ���ſ� ���Ǵ� ũ��Ƽ�� ���� ��ü ����

CString PeerAddr = _T("127.0.0.1"); // ������ �ּҸ� ��Ÿ���� ���ڿ�, �⺻���� "127.0.0.1"�� ����ȣ��Ʈ�� �ǹ���

UINT srcPort = 7000; // �۽ſ� ���� �ҽ� ��Ʈ ��ȣ, �⺻���� 7000, UDPClient1�� �����Ͻ�
//UINT srcPort = 7001; // �۽ſ� ���� �ҽ� ��Ʈ ��ȣ, �⺻���� 7001, UDPClient2�� �����Ͻ�
UINT dstPort = 8000; // ���� ��� ��Ʈ ��ȣ, �⺻���� 8000

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUDPClientDlg ��ȭ ����

UINT RXThread(LPVOID arg)
{
	ThreadArg* pArg = (ThreadArg*)arg; // ������ ���� ����ü�� ĳ����
	CStringList* plist = pArg->pList; // ���� �޽��� ����Ʈ ������
	CUDPClientDlg* pDlg = (CUDPClientDlg*)pArg->pDlg; // Ŭ���̾�Ʈ ��ȭ���� ������

	while (pArg->Thread_run) // ������ ���� �÷��� Ȯ��
	{
		POSITION pos = plist->GetHeadPosition(); // ����Ʈ�� ù ��° ��ġ�� ������
		POSITION current_pos; // ���� ��ġ�� ������ ����

		while (pos != NULL)
		{
			current_pos = pos; // ���� ��ġ ����

			rx_cs.Lock(); // ���� ũ��Ƽ�� ���� ���
			CString str = plist->GetNext(pos); // ���� ���� �޽����� ������
			rx_cs.Unlock(); // ���� ũ��Ƽ�� ���� ��� ����

			CString message;
			pDlg->m_rx_edit.GetWindowTextW(message); // ���� ���� â�� �ؽ�Ʈ�� ������
			message += str; // ������ �޽��� �߰�
			pDlg->m_rx_edit.SetWindowTextW(message); // ���� â�� �ؽ�Ʈ ����
			pDlg->m_rx_edit.LineScroll(pDlg->m_rx_edit.GetLineCount()); // ���� â ��ũ���� ���� �Ʒ��� �̵�

			plist->RemoveAt(current_pos); // ó���� �޽����� ����Ʈ���� ����
		}
		Sleep(10); // ��� ���
	}
	return 0;
}

UINT TXThread(LPVOID arg)
{
	ThreadArg* pArg = (ThreadArg*)arg; // ������ ���� ����ü�� ĳ����
	CStringList* plist = pArg->pList; // �۽� �޽��� ����Ʈ ������
	CUDPClientDlg* pDlg = (CUDPClientDlg*)pArg->pDlg; // Ŭ���̾�Ʈ ��ȭ���� ������

	while (pArg->Thread_run) // ������ ���� �÷��� Ȯ��
	{
		POSITION pos = plist->GetHeadPosition(); // ����Ʈ�� ù ��° ��ġ�� ������
		POSITION current_pos; // ���� ��ġ�� ������ ����

		while (pos != NULL)
		{
			current_pos = pos; // ���� ��ġ ����

			tx_cs.Lock(); // �۽� ũ��Ƽ�� ���� ���
			CString str = plist->GetNext(pos); // ���� �۽� �޽����� ������
			tx_cs.Unlock(); // �۽� ũ��Ƽ�� ���� ��� ����

			pDlg->m_tx_edit.LineScroll(pDlg->m_tx_edit.GetLineCount()); // �۽� â ��ũ���� ���� �Ʒ��� �̵�
			pDlg->m_pDataSocket->SendToEx((LPCTSTR)str, 1024, dstPort, PeerAddr); // �޽����� ���
			plist->RemoveAt(current_pos); 
		}

		Sleep(10); 
	}
	return 0;
}

CUDPClientDlg::CUDPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UDPCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUDPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipaddr);
	DDX_Control(pDX, IDC_EDIT1, m_tx_edit_short);
	DDX_Control(pDX, IDC_EDIT2, m_tx_edit);
	DDX_Control(pDX, IDC_EDIT3, m_rx_edit);
}

BEGIN_MESSAGE_MAP(CUDPClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CUDPClientDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_CLOSE, &CUDPClientDlg::OnBnClickedClose)
END_MESSAGE_MAP()


// CUDPClientDlg �޽��� ó����

BOOL CUDPClientDlg::OnInitDialog()
{
	// �⺻ ���̾�α� �ʱ�ȭ
	CDialogEx::OnInitDialog();

	// About ��ȭ���� �޴� �׸� �߰�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ������ ����
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// �ʱ� IP �ּ� ����
	m_ipaddr.SetWindowTextW(_T("127.0.0.1"));

	// WSA �ʱ�ȭ
	WSADATA wsa;
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
	{
		TCHAR buffer[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		AfxMessageBox(buffer, MB_ICONERROR);
	}

	// TXThread�� RXThread�� ����� CStringList ����
	CStringList* newlist = new CStringList;
	arg1.pList = newlist;
	arg1.Thread_run = 1;
	arg1.pDlg = this;

	CStringList* newlist2 = new CStringList;
	arg2.pList = newlist2;
	arg2.Thread_run = 1;
	arg2.pDlg = this;

	// ������ ���� ���� �� ��Ʈ ����
	m_pDataSocket = new CDataSocket(this);
	m_pDataSocket->Create(srcPort, SOCK_DGRAM);

	// TXThread�� RXThread ����
	pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1);
	pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2);

	return TRUE;
}

void CUDPClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CUDPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		// �ּ�ȭ�� ���¿����� �������� �׸���
		CPaintDC dc(this);

		// ����� ����� ���� WM_ICONERASEBKGND �޽����� ����
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �������� ũ��� ��ġ�� ���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸�
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// �ּ�ȭ�� ���°� �ƴ� ��� �⺻ OnPaint() �Լ��� ȣ��
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CUDPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUDPClientDlg::OnBnClickedSend()
{
	if (m_pDataSocket == NULL)
	{
		// ������ ���Ͽ� ���ӵǾ� ���� ���� ���, ���� �޽��� ���
		MessageBox(_T("������ ���� ����!"), _T("�˸�"), MB_ICONERROR);
	}
	else
	{
		CString tx_message;
		m_tx_edit_short.GetWindowTextW(tx_message);
		tx_message += _T("\r\n");  // �޽����� ���� ���� �߰�

		tx_cs.Lock();  // ���� ������ �����ϱ� ���� ���ؽ� ���
		arg1.pList->AddTail(tx_message);  // ����Ʈ�� �޽��� �߰�
		tx_cs.Unlock();  // ���ؽ� ��� ����

		m_tx_edit_short.SetWindowTextW(_T(""));  // ª�� �޽��� �Է� ���� �ʱ�ȭ
		m_tx_edit_short.SetFocus();  // ª�� �޽��� �Է� ���ڿ� ��Ŀ�� ����

		int len = m_tx_edit.GetWindowTextLengthW();
		m_tx_edit.SetSel(len, len);
		m_tx_edit.ReplaceSel(tx_message);  // ��ü �޽��� �Է� ���ڿ� �޽��� �߰�
	}
}

void CUDPClientDlg::OnBnClickedClose()
{
	CString tx_message;
	m_tx_edit_short.GetWindowText(tx_message);  // ª�� �޽��� �Է� ���ڿ��� �޽����� ������
	tx_message += _T("\r\n");  // �޽����� ���� ���ڸ� �߰�

	tx_cs.Lock();  // ���� ������ �����ϱ� ���� ���ؽ� ���
	arg1.pList->AddTail(tx_message);  // ����Ʈ�� �޽��� �߰�
	tx_cs.Unlock();  // ���ؽ� ��� ����

	m_tx_edit_short.SetWindowText(_T(""));  // ª�� �޽��� �Է� ���� �ʱ�ȭ
	m_tx_edit.SetWindowText(tx_message);  // ��ü �޽��� �Է� ���ڿ� �޽��� ����
}


void CUDPClientDlg::ProcessReceive(CDataSocket* pSocket, int nErrorCode)
{
	TCHAR pBuf[1024 + 1];  // ���ŵ� �����͸� ������ ����
	CString strData;  // ��ȯ�� ���ڿ� �����͸� ������ ����
	int nbytes;  // ���ŵ� ����Ʈ ��

	nbytes = pSocket->ReceiveFromEx(pBuf, 1024, PeerAddr, dstPort);  // ���Ͽ��� �����͸� �����մϴ�.
	pBuf[nbytes] = NULL;  // ���� �������� �� ���ڸ� �߰��մϴ�.
	strData = (LPCTSTR)pBuf;  // ������ �����͸� CString���� ��ȯ�մϴ�.

	rx_cs.Lock();  // ���� ������ �����ϱ� ���� ���ؽ� ���
	arg2.pList->AddTail((LPCTSTR)strData);  // ����Ʈ�� �����͸� �߰��մϴ�.
	rx_cs.Unlock();  // ���ؽ� ��� ����
}

void CUDPClientDlg::ProcessClose(CDataSocket* pSocket, int nErrorCode)
{
	pSocket->Close();  // ������ �ݽ��ϴ�.
	delete m_pDataSocket;  // ������ ���� ��ü�� �����մϴ�.
	m_pDataSocket = NULL;  // ������ ���� �����͸� NULL�� �����մϴ�.

	int len = m_rx_edit.GetWindowTextLengthW();  // ���� �ؽ�Ʈ ������ ���̸� �����ɴϴ�.
	CString message = _T("��������\n");  // �ؽ�Ʈ�� ǥ���� �޽����Դϴ�.
	m_rx_edit.SetSel(len, len);  // �ؽ�Ʈ ������ ������ ��ġ�� Ŀ���� �̵��մϴ�.
	m_rx_edit.ReplaceSel(message);  // �ؽ�Ʈ ���ڿ� �޽����� �߰��մϴ�.
}
