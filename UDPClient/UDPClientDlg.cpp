// UDPClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "UDPClient.h"
#include "UDPClientDlg.h"
#include "afxdialogex.h"
#include "DataSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection tx_cs; // 송신에 사용되는 크리티컬 섹션 객체 선언
CCriticalSection rx_cs; // 수신에 사용되는 크리티컬 섹션 객체 선언

CString PeerAddr = _T("127.0.0.1"); // 서버의 주소를 나타내는 문자열, 기본값은 "127.0.0.1"로 로컬호스트를 의미함

UINT srcPort = 7000; // 송신에 사용될 소스 포트 번호, 기본값은 7000, UDPClient1번 컴파일시
//UINT srcPort = 7001; // 송신에 사용될 소스 포트 번호, 기본값은 7001, UDPClient2번 컴파일시
UINT dstPort = 8000; // 수신 대상 포트 번호, 기본값은 8000

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CUDPClientDlg 대화 상자

UINT RXThread(LPVOID arg)
{
	ThreadArg* pArg = (ThreadArg*)arg; // 쓰레드 인자 구조체로 캐스팅
	CStringList* plist = pArg->pList; // 수신 메시지 리스트 포인터
	CUDPClientDlg* pDlg = (CUDPClientDlg*)pArg->pDlg; // 클라이언트 대화상자 포인터

	while (pArg->Thread_run) // 쓰레드 실행 플래그 확인
	{
		POSITION pos = plist->GetHeadPosition(); // 리스트의 첫 번째 위치를 가져옴
		POSITION current_pos; // 현재 위치를 저장할 변수

		while (pos != NULL)
		{
			current_pos = pos; // 현재 위치 저장

			rx_cs.Lock(); // 수신 크리티컬 섹션 잠금
			CString str = plist->GetNext(pos); // 다음 수신 메시지를 가져옴
			rx_cs.Unlock(); // 수신 크리티컬 섹션 잠금 해제

			CString message;
			pDlg->m_rx_edit.GetWindowTextW(message); // 현재 수신 창의 텍스트를 가져옴
			message += str; // 수신한 메시지 추가
			pDlg->m_rx_edit.SetWindowTextW(message); // 수신 창에 텍스트 설정
			pDlg->m_rx_edit.LineScroll(pDlg->m_rx_edit.GetLineCount()); // 수신 창 스크롤을 가장 아래로 이동

			plist->RemoveAt(current_pos); // 처리한 메시지를 리스트에서 제거
		}
		Sleep(10); // 잠시 대기
	}
	return 0;
}

UINT TXThread(LPVOID arg)
{
	ThreadArg* pArg = (ThreadArg*)arg; // 쓰레드 인자 구조체로 캐스팅
	CStringList* plist = pArg->pList; // 송신 메시지 리스트 포인터
	CUDPClientDlg* pDlg = (CUDPClientDlg*)pArg->pDlg; // 클라이언트 대화상자 포인터

	while (pArg->Thread_run) // 쓰레드 실행 플래그 확인
	{
		POSITION pos = plist->GetHeadPosition(); // 리스트의 첫 번째 위치를 가져옴
		POSITION current_pos; // 현재 위치를 저장할 변수

		while (pos != NULL)
		{
			current_pos = pos; // 현재 위치 저장

			tx_cs.Lock(); // 송신 크리티컬 섹션 잠금
			CString str = plist->GetNext(pos); // 다음 송신 메시지를 가져옴
			tx_cs.Unlock(); // 송신 크리티컬 섹션 잠금 해제

			pDlg->m_tx_edit.LineScroll(pDlg->m_tx_edit.GetLineCount()); // 송신 창 스크롤을 가장 아래로 이동
			pDlg->m_pDataSocket->SendToEx((LPCTSTR)str, 1024, dstPort, PeerAddr); // 메시지를 대상
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


// CUDPClientDlg 메시지 처리기

BOOL CUDPClientDlg::OnInitDialog()
{
	// 기본 다이얼로그 초기화
	CDialogEx::OnInitDialog();

	// About 대화상자 메뉴 항목 추가
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

	// 아이콘 설정
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// 초기 IP 주소 설정
	m_ipaddr.SetWindowTextW(_T("127.0.0.1"));

	// WSA 초기화
	WSADATA wsa;
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
	{
		TCHAR buffer[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		AfxMessageBox(buffer, MB_ICONERROR);
	}

	// TXThread와 RXThread에 사용할 CStringList 생성
	CStringList* newlist = new CStringList;
	arg1.pList = newlist;
	arg1.Thread_run = 1;
	arg1.pDlg = this;

	CStringList* newlist2 = new CStringList;
	arg2.pList = newlist2;
	arg2.Thread_run = 1;
	arg2.pDlg = this;

	// 데이터 소켓 생성 및 포트 설정
	m_pDataSocket = new CDataSocket(this);
	m_pDataSocket->Create(srcPort, SOCK_DGRAM);

	// TXThread와 RXThread 실행
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CUDPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		// 최소화된 상태에서는 아이콘을 그린다
		CPaintDC dc(this);

		// 배경을 지우기 위해 WM_ICONERASEBKGND 메시지를 전송
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 아이콘의 크기와 위치를 계산
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그림
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// 최소화된 상태가 아닌 경우 기본 OnPaint() 함수를 호출
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CUDPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUDPClientDlg::OnBnClickedSend()
{
	if (m_pDataSocket == NULL)
	{
		// 데이터 소켓에 접속되어 있지 않을 경우, 오류 메시지 출력
		MessageBox(_T("서버에 접속 안함!"), _T("알림"), MB_ICONERROR);
	}
	else
	{
		CString tx_message;
		m_tx_edit_short.GetWindowTextW(tx_message);
		tx_message += _T("\r\n");  // 메시지에 개행 문자 추가

		tx_cs.Lock();  // 공유 변수에 접근하기 전에 뮤텍스 잠금
		arg1.pList->AddTail(tx_message);  // 리스트에 메시지 추가
		tx_cs.Unlock();  // 뮤텍스 잠금 해제

		m_tx_edit_short.SetWindowTextW(_T(""));  // 짧은 메시지 입력 상자 초기화
		m_tx_edit_short.SetFocus();  // 짧은 메시지 입력 상자에 포커스 설정

		int len = m_tx_edit.GetWindowTextLengthW();
		m_tx_edit.SetSel(len, len);
		m_tx_edit.ReplaceSel(tx_message);  // 전체 메시지 입력 상자에 메시지 추가
	}
}

void CUDPClientDlg::OnBnClickedClose()
{
	CString tx_message;
	m_tx_edit_short.GetWindowText(tx_message);  // 짧은 메시지 입력 상자에서 메시지를 가져옴
	tx_message += _T("\r\n");  // 메시지에 개행 문자를 추가

	tx_cs.Lock();  // 공유 변수에 접근하기 전에 뮤텍스 잠금
	arg1.pList->AddTail(tx_message);  // 리스트에 메시지 추가
	tx_cs.Unlock();  // 뮤텍스 잠금 해제

	m_tx_edit_short.SetWindowText(_T(""));  // 짧은 메시지 입력 상자 초기화
	m_tx_edit.SetWindowText(tx_message);  // 전체 메시지 입력 상자에 메시지 설정
}


void CUDPClientDlg::ProcessReceive(CDataSocket* pSocket, int nErrorCode)
{
	TCHAR pBuf[1024 + 1];  // 수신된 데이터를 저장할 버퍼
	CString strData;  // 변환된 문자열 데이터를 저장할 변수
	int nbytes;  // 수신된 바이트 수

	nbytes = pSocket->ReceiveFromEx(pBuf, 1024, PeerAddr, dstPort);  // 소켓에서 데이터를 수신합니다.
	pBuf[nbytes] = NULL;  // 버퍼 마지막에 널 문자를 추가합니다.
	strData = (LPCTSTR)pBuf;  // 버퍼의 데이터를 CString으로 변환합니다.

	rx_cs.Lock();  // 공유 변수에 접근하기 전에 뮤텍스 잠금
	arg2.pList->AddTail((LPCTSTR)strData);  // 리스트에 데이터를 추가합니다.
	rx_cs.Unlock();  // 뮤텍스 잠금 해제
}

void CUDPClientDlg::ProcessClose(CDataSocket* pSocket, int nErrorCode)
{
	pSocket->Close();  // 소켓을 닫습니다.
	delete m_pDataSocket;  // 데이터 소켓 객체를 삭제합니다.
	m_pDataSocket = NULL;  // 데이터 소켓 포인터를 NULL로 설정합니다.

	int len = m_rx_edit.GetWindowTextLengthW();  // 현재 텍스트 상자의 길이를 가져옵니다.
	CString message = _T("접속종료\n");  // 텍스트로 표시할 메시지입니다.
	m_rx_edit.SetSel(len, len);  // 텍스트 상자의 마지막 위치로 커서를 이동합니다.
	m_rx_edit.ReplaceSel(message);  // 텍스트 상자에 메시지를 추가합니다.
}
