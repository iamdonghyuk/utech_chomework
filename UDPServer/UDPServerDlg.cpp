
// UDPServerDlg.cpp : 구현 파일
//
#include "stdafx.h"
#include "UDPServer.h"
#include "UDPServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection tx_cs; // 송신 크리티컬 섹션 객체 선언
CCriticalSection rx_cs; // 수신 크리티컬 섹션 객체 선언

CString PeerAddr; // 피어 주소를 저장하는 문자열 객체

UINT srcPort = 8000; // 소스 포트 번호
UINT dstPort = -1; // 대상 포트 번호, 초기값은 -1

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


// CUDPServerDlg 대화 상자
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

// 백그라운드에서 실행되는 스레드 함수.
// ThreadArg 구조체에 저장된 CStringList에서 문자열을 가져와서 CUDPServerDlg의 편집 상자에 표시함
UINT RXThread(LPVOID arg)
{
	// arg 포인터를 ThreadArg 포인터로 캐스팅하여 사용함
	ThreadArg* pArg = (ThreadArg*)arg;

	// pArg에서 CStringList 포인터를 가져옴
	CStringList* plist = pArg->pList;

	// pArg에서 CUDPServerDlg 포인터를 가져옴
	CUDPServerDlg* pDlg = (CUDPServerDlg*)pArg->pDlg;

	// Thread_run 플래그가 true인 동안 루프를 실행
	while (pArg->Thread_run)
	{
		// CStringList의 첫 번째 위치를 가져옴
		POSITION pos = plist->GetHeadPosition();

		// 현재 위치를 저장할 변수를 선언
		POSITION current_pos;

		// 리스트의 끝에 도달할 때까지 루프를 실행
		while (pos != NULL)
		{
			// 현재 위치를 저장합니다.
			current_pos = pos;

			// 다른 스레드와의 동시 접근을 막기 위해 임계 영역에 진입
			rx_cs.Lock();

			// 다음 문자열을 가져온다
			CString str = plist->GetNext(pos);

			// 임계 영역에서 빠져나옴
			rx_cs.Unlock();

			// 문자열을 message 변수에 추가
			CString message;
			pDlg->m_rx_edit.GetWindowTextW(message);
			message += str;

			// 편집 상자에 새로운 문자열을 설정
			pDlg->m_rx_edit.SetWindowTextW(message);

			// 편집 상자를 스크롤하여 마지막 라인을 표시
			pDlg->m_rx_edit.LineScroll(pDlg->m_rx_edit.GetLineCount());

			// 현재 위치의 항목을 리스트에서 제거
			plist->RemoveAt(current_pos);
		}

		// 10 밀리초 동안 스레드를 일시 정지
		Sleep(10);
	}

	// 스레드 함수의 반환값을 0으로 설정하여 종료
	return 0;
}

// 백그라운드에서 실행되는 스레드 함수
UINT TXThread(LPVOID arg)
{
	// arg 포인터를 ThreadArg 포인터로 캐스팅하여 사용한다
	ThreadArg* pArg = (ThreadArg*)arg;

	// pArg에서 CStringList 포인터를 가져옴
	CStringList* plist = pArg->pList;

	// pArg에서 CUDPServerDlg 포인터를 가져옴
	CUDPServerDlg* pDlg = (CUDPServerDlg*)pArg->pDlg;

	// Thread_run 플래그가 true인 동안 루프를 실행
	while (pArg->Thread_run)
	{
		// CStringList의 첫 번째 위치를 가져옴
		POSITION pos = plist->GetHeadPosition();

		// 현재 위치를 저장할 변수를 선언
		POSITION current_pos;

		// 리스트의 끝에 도달할 때까지 루프를 실행
		while (pos != NULL)
		{
			// 현재 위치를 저장
			current_pos = pos;

			// 다른 스레드와의 동시 접근을 막기 위해 임계 영역에 진입
			tx_cs.Lock();

			// 다음 문자열을 가져옴
			CString str = plist->GetNext(pos);

			// 임계 영역에서 빠져나옴
			tx_cs.Unlock();

			// 편집 상자를 스크롤하여 마지막 라인을 표시
			pDlg->m_tx_edit.LineScroll(pDlg->m_tx_edit.GetLineCount());

			// 목적지 포트가 설정된 경우 문자열을 전송
			if (dstPort != -1)
			{
				pDlg->m_pDataSocket->SendToEx((LPCTSTR)str, 1024, dstPort, PeerAddr);
			}
			// 목적지 포트가 설정되지 않은 경우 클라이언트가 없음을 알리는 메시지를 표시
			else
			{
				pDlg->m_tx_edit.SetWindowTextW(_T("###접속된 Client가 없습니다.###\n"));
			}

			// 현재 위치의 항목을 리스트에서 제거
			plist->RemoveAt(current_pos);
		}

		// 10 밀리초 동안 스레드를 일시 정지
		Sleep(10);
	}

	// 스레드 함수의 반환값을 0으로 설정하여 종료
	return 0;
}

// MFC 대화 상자의 초기 설정을 수행하는 역할을 한다
BOOL CUDPServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "About" 메뉴 항목을 추가
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		// 메뉴 항목에 대한 문자열을 로드함
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);

		// 로드된 문자열이 비어있지 않은 경우에만 처리
		if (!strAboutMenu.IsEmpty())
		{
			// 시스템 메뉴에 구분선을 추가합
			pSysMenu->AppendMenu(MF_SEPARATOR);

			// 시스템 메뉴에 "About" 항목을 추가
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 대화 상자에 아이콘 설정
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_pDataSocket = NULL;

	// Winsock 초기화
	WSADATA wsa;
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0)
	{
		TCHAR buffer[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		AfxMessageBox(buffer, MB_ICONERROR);
	}

	// CStringList를 새로 생성하여 ThreadArg 구조체에 저장
	CStringList* newlist1 = new CStringList;
	arg1.pList = newlist1;
	arg1.Thread_run = 1;
	arg1.pDlg = this;

	CStringList* newlist2 = new CStringList;
	arg2.pList = newlist2;
	arg2.Thread_run = 1;
	arg2.pDlg = this;

	// CDataSocket 객체 생성 및 포트 설정
	m_pDataSocket = new CDataSocket(this);
	m_pDataSocket->Create(srcPort, SOCK_DGRAM); // 포트 8000번 UDP 소켓 생성

	// 쓰레드 생성 및 실행
	pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1); // 송신 쓰레드
	pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2); // 수신 쓰레드

	// TRUE 반환하여 포커스를 컨트롤에 설정하고 초기화를 마침
	return TRUE;
}


void CUDPServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// About 대화상자가 선택되었을 경우
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		// About 대화상자를 생성하고 모달로 실행
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		// 기본 동작을 수행
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CUDPServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CUDPServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUDPServerDlg::OnBnClickedSend()
{
	// 텍스트 상자에서 메시지를 가져옴
	CString tx_message;
	m_tx_edit_short.GetWindowTextW(tx_message);

	// 메시지에 개행 문자를 추가
	tx_message += _T("\r\n");

	// 임계 영역에 진입하여 다른 스레드와의 동시 접근 통제
	tx_cs.Lock();

	// 전역 변수에 있는 CStringList에 메시지를 추가
	arg1.pList->AddTail(tx_message);

	// 임계 영역에서 빠져나옴
	tx_cs.Unlock();

	// 짧은 텍스트 상자를 지움
	m_tx_edit_short.SetWindowTextW(_T(""));

	// 짧은 텍스트 상자에 포커스를 설정
	m_tx_edit_short.SetFocus();

	// 긴 텍스트 상자의 현재 길이를 가져옴
	int len = m_tx_edit.GetWindowTextLengthW();

	// 긴 텍스트 상자의 커서를 맨 끝으로 이동
	m_tx_edit.SetSel(len, len);

	// 긴 텍스트 상자에 메시지를 추가
	m_tx_edit.ReplaceSel(tx_message);
}



void CUDPServerDlg::OnBnClickedClose()
{
	// 짧은 텍스트 상자에서 메시지를 가져온다
	CString tx_message;
	m_tx_edit_short.GetWindowText(tx_message);
	tx_message += _T("\r\n");

	// 임계 영역에 진입하여 다른 스레드와의 동시 접근 통제
	tx_cs.Lock();

	// 전역 변수에 있는 CStringList에 메시지를 추가
	arg1.pList->AddTail(tx_message);

	// 임계 영역에서 빠져나옴
	tx_cs.Unlock();

	// 짧은 텍스트 상자를 지움
	m_tx_edit_short.SetWindowText(_T(""));

	// 긴 텍스트 상자에 메시지를 설정
	m_tx_edit.SetWindowText(tx_message);
}

void CUDPServerDlg::ProcessReceive(CDataSocket* pSocket, int nErrorCode)
{
	// 버퍼와 변수를 초기화
	TCHAR pBuf[1024 + 1];
	int nbytes;
	CString strData;

	// 소켓으로부터 데이터를 수신
	nbytes = pSocket->ReceiveFromEx(pBuf, 1024, PeerAddr, dstPort);
	pBuf[nbytes] = NULL;
	strData = (LPCTSTR)pBuf;

	// 크리티컬 섹션에 진입하여 다른 스레드와의 동시 접근을 막는다
	rx_cs.Lock();

	// 전역 변수에 있는 CStringList에 메시지를 추가
	arg2.pList->AddTail((LPCTSTR)strData);

	// 크리티컬 섹션에서 빠져나옴
	rx_cs.Unlock();
}

void CUDPServerDlg::ProcessClose(CDataSocket* pSocket, int nErrorCode)
{
	// 소켓을 닫고 메모리를 해제
	pSocket->Close();
	delete m_pDataSocket;
	m_pDataSocket = NULL;

	// 수신 텍스트 상자의 길이를 가져옴
	int len = m_rx_edit.GetWindowTextLengthW();

	// 접속 종료 메시지를 생성
	CString message = _T("### 접속 종료 ###\r\n\n");

	// 수신 텍스트 상자의 커서를 맨 끝으로 이동하고 메시지를 설정
	m_rx_edit.SetSel(len, len);
	m_rx_edit.ReplaceSel(message);
}
