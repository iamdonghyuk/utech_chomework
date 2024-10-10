
// UDPServerDlg.cpp : ���� ����
//
#include "stdafx.h"
#include "UDPServer.h"
#include "UDPServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection tx_cs; // �۽� ũ��Ƽ�� ���� ��ü ����
CCriticalSection rx_cs; // ���� ũ��Ƽ�� ���� ��ü ����

CString PeerAddr; // �Ǿ� �ּҸ� �����ϴ� ���ڿ� ��ü

UINT srcPort = 8000; // �ҽ� ��Ʈ ��ȣ
UINT dstPort = -1; // ��� ��Ʈ ��ȣ, �ʱⰪ�� -1

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


// CUDPServerDlg ��ȭ ����
CUDPServerDlg::CUDPServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UDPSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUDPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tx_edit_short);
	DDX_Control(pDX, IDC_EDIT2, m_tx_edit);
	DDX_Control(pDX, IDC_EDIT3, m_rx_edit);
}

BEGIN_MESSAGE_MAP(CUDPServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CUDPServerDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_CLOSE, &CUDPServerDlg::OnBnClickedClose)
END_MESSAGE_MAP()

// ��׶��忡�� ����Ǵ� ������ �Լ�.
// ThreadArg ����ü�� ����� CStringList���� ���ڿ��� �����ͼ� CUDPServerDlg�� ���� ���ڿ� ǥ����
UINT RXThread(LPVOID arg)
{
	// arg �����͸� ThreadArg �����ͷ� ĳ�����Ͽ� �����
	ThreadArg* pArg = (ThreadArg*)arg;

	// pArg���� CStringList �����͸� ������
	CStringList* plist = pArg->pList;

	// pArg���� CUDPServerDlg �����͸� ������
	CUDPServerDlg* pDlg = (CUDPServerDlg*)pArg->pDlg;

	// Thread_run �÷��װ� true�� ���� ������ ����
	while (pArg->Thread_run)
	{
		// CStringList�� ù ��° ��ġ�� ������
		POSITION pos = plist->GetHeadPosition();

		// ���� ��ġ�� ������ ������ ����
		POSITION current_pos;

		// ����Ʈ�� ���� ������ ������ ������ ����
		while (pos != NULL)
		{
			// ���� ��ġ�� �����մϴ�.
			current_pos = pos;

			// �ٸ� ��������� ���� ������ ���� ���� �Ӱ� ������ ����
			rx_cs.Lock();

			// ���� ���ڿ��� �����´�
			CString str = plist->GetNext(pos);

			// �Ӱ� �������� ��������
			rx_cs.Unlock();

			// ���ڿ��� message ������ �߰�
			CString message;
			pDlg->m_rx_edit.GetWindowTextW(message);
			message += str;

			// ���� ���ڿ� ���ο� ���ڿ��� ����
			pDlg->m_rx_edit.SetWindowTextW(message);

			// ���� ���ڸ� ��ũ���Ͽ� ������ ������ ǥ��
			pDlg->m_rx_edit.LineScroll(pDlg->m_rx_edit.GetLineCount());

			// ���� ��ġ�� �׸��� ����Ʈ���� ����
			plist->RemoveAt(current_pos);
		}

		// 10 �и��� ���� �����带 �Ͻ� ����
		Sleep(10);
	}

	// ������ �Լ��� ��ȯ���� 0���� �����Ͽ� ����
	return 0;
}

// ��׶��忡�� ����Ǵ� ������ �Լ�
UINT TXThread(LPVOID arg)
{
	// arg �����͸� ThreadArg �����ͷ� ĳ�����Ͽ� ����Ѵ�
	ThreadArg* pArg = (ThreadArg*)arg;

	// pArg���� CStringList �����͸� ������
	CStringList* plist = pArg->pList;

	// pArg���� CUDPServerDlg �����͸� ������
	CUDPServerDlg* pDlg = (CUDPServerDlg*)pArg->pDlg;

	// Thread_run �÷��װ� true�� ���� ������ ����
	while (pArg->Thread_run)
	{
		// CStringList�� ù ��° ��ġ�� ������
		POSITION pos = plist->GetHeadPosition();

		// ���� ��ġ�� ������ ������ ����
		POSITION current_pos;

		// ����Ʈ�� ���� ������ ������ ������ ����
		while (pos != NULL)
		{
			// ���� ��ġ�� ����
			current_pos = pos;

			// �ٸ� ��������� ���� ������ ���� ���� �Ӱ� ������ ����
			tx_cs.Lock();

			// ���� ���ڿ��� ������
			CString str = plist->GetNext(pos);

			// �Ӱ� �������� ��������
			tx_cs.Unlock();

			// ���� ���ڸ� ��ũ���Ͽ� ������ ������ ǥ��
			pDlg->m_tx_edit.LineScroll(pDlg->m_tx_edit.GetLineCount());

			// ������ ��Ʈ�� ������ ��� ���ڿ��� ����
			if (dstPort != -1)
			{
				pDlg->m_pDataSocket->SendToEx((LPCTSTR)str, 1024, dstPort, PeerAddr);
			}
			// ������ ��Ʈ�� �������� ���� ��� Ŭ���̾�Ʈ�� ������ �˸��� �޽����� ǥ��
			else
			{
				pDlg->m_tx_edit.SetWindowTextW(_T("###���ӵ� Client�� �����ϴ�.###\n"));
			}

			// ���� ��ġ�� �׸��� ����Ʈ���� ����
			plist->RemoveAt(current_pos);
		}

		// 10 �и��� ���� �����带 �Ͻ� ����
		Sleep(10);
	}

	// ������ �Լ��� ��ȯ���� 0���� �����Ͽ� ����
	return 0;
}

// MFC ��ȭ ������ �ʱ� ������ �����ϴ� ������ �Ѵ�
BOOL CUDPServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "About" �޴� �׸��� �߰�
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		// �޴� �׸� ���� ���ڿ��� �ε���
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);

		// �ε�� ���ڿ��� ������� ���� ��쿡�� ó��
		if (!strAboutMenu.IsEmpty())
		{
			// �ý��� �޴��� ���м��� �߰���
			pSysMenu->AppendMenu(MF_SEPARATOR);

			// �ý��� �޴��� "About" �׸��� �߰�
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ��ȭ ���ڿ� ������ ����
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_pDataSocket = NULL;

	// Winsock �ʱ�ȭ
	WSADATA wsa;
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
	{
		TCHAR buffer[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		AfxMessageBox(buffer, MB_ICONERROR);
	}

	// CStringList�� ���� �����Ͽ� ThreadArg ����ü�� ����
	CStringList* newlist1 = new CStringList;
	arg1.pList = newlist1;
	arg1.Thread_run = 1;
	arg1.pDlg = this;

	CStringList* newlist2 = new CStringList;
	arg2.pList = newlist2;
	arg2.Thread_run = 1;
	arg2.pDlg = this;

	// CDataSocket ��ü ���� �� ��Ʈ ����
	m_pDataSocket = new CDataSocket(this);
	m_pDataSocket->Create(srcPort, SOCK_DGRAM); // ��Ʈ 8000�� UDP ���� ����

	// ������ ���� �� ����
	pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1); // �۽� ������
	pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2); // ���� ������

	// TRUE ��ȯ�Ͽ� ��Ŀ���� ��Ʈ�ѿ� �����ϰ� �ʱ�ȭ�� ��ħ
	return TRUE;
}


void CUDPServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// About ��ȭ���ڰ� ���õǾ��� ���
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		// About ��ȭ���ڸ� �����ϰ� ��޷� ����
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		// �⺻ ������ ����
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CUDPServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CUDPServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUDPServerDlg::OnBnClickedSend()
{
	// �ؽ�Ʈ ���ڿ��� �޽����� ������
	CString tx_message;
	m_tx_edit_short.GetWindowTextW(tx_message);

	// �޽����� ���� ���ڸ� �߰�
	tx_message += _T("\r\n");

	// �Ӱ� ������ �����Ͽ� �ٸ� ��������� ���� ���� ����
	tx_cs.Lock();

	// ���� ������ �ִ� CStringList�� �޽����� �߰�
	arg1.pList->AddTail(tx_message);

	// �Ӱ� �������� ��������
	tx_cs.Unlock();

	// ª�� �ؽ�Ʈ ���ڸ� ����
	m_tx_edit_short.SetWindowTextW(_T(""));

	// ª�� �ؽ�Ʈ ���ڿ� ��Ŀ���� ����
	m_tx_edit_short.SetFocus();

	// �� �ؽ�Ʈ ������ ���� ���̸� ������
	int len = m_tx_edit.GetWindowTextLengthW();

	// �� �ؽ�Ʈ ������ Ŀ���� �� ������ �̵�
	m_tx_edit.SetSel(len, len);

	// �� �ؽ�Ʈ ���ڿ� �޽����� �߰�
	m_tx_edit.ReplaceSel(tx_message);
}



void CUDPServerDlg::OnBnClickedClose()
{
	// ª�� �ؽ�Ʈ ���ڿ��� �޽����� �����´�
	CString tx_message;
	m_tx_edit_short.GetWindowText(tx_message);
	tx_message += _T("\r\n");

	// �Ӱ� ������ �����Ͽ� �ٸ� ��������� ���� ���� ����
	tx_cs.Lock();

	// ���� ������ �ִ� CStringList�� �޽����� �߰�
	arg1.pList->AddTail(tx_message);

	// �Ӱ� �������� ��������
	tx_cs.Unlock();

	// ª�� �ؽ�Ʈ ���ڸ� ����
	m_tx_edit_short.SetWindowText(_T(""));

	// �� �ؽ�Ʈ ���ڿ� �޽����� ����
	m_tx_edit.SetWindowText(tx_message);
}

void CUDPServerDlg::ProcessReceive(CDataSocket* pSocket, int nErrorCode)
{
	// ���ۿ� ������ �ʱ�ȭ
	TCHAR pBuf[1024 + 1];
	int nbytes;
	CString strData;

	// �������κ��� �����͸� ����
	nbytes = pSocket->ReceiveFromEx(pBuf, 1024, PeerAddr, dstPort);
	pBuf[nbytes] = NULL;
	strData = (LPCTSTR)pBuf;

	// ũ��Ƽ�� ���ǿ� �����Ͽ� �ٸ� ��������� ���� ������ ���´�
	rx_cs.Lock();

	// ���� ������ �ִ� CStringList�� �޽����� �߰�
	arg2.pList->AddTail((LPCTSTR)strData);

	// ũ��Ƽ�� ���ǿ��� ��������
	rx_cs.Unlock();
}

void CUDPServerDlg::ProcessClose(CDataSocket* pSocket, int nErrorCode)
{
	// ������ �ݰ� �޸𸮸� ����
	pSocket->Close();
	delete m_pDataSocket;
	m_pDataSocket = NULL;

	// ���� �ؽ�Ʈ ������ ���̸� ������
	int len = m_rx_edit.GetWindowTextLengthW();

	// ���� ���� �޽����� ����
	CString message = _T("### ���� ���� ###\r\n\n");

	// ���� �ؽ�Ʈ ������ Ŀ���� �� ������ �̵��ϰ� �޽����� ����
	m_rx_edit.SetSel(len, len);
	m_rx_edit.ReplaceSel(message);
}
