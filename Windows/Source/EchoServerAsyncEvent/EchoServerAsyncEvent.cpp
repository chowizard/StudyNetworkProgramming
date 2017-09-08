#include "stdafx.h"
#include "EchoServerAsyncEvent.h"

//------------------------------------------------------------------------------
#define MAX_LOADSTRING 100

#define BUFFER_LENGTH_MAX (1024)
#define PORT_NUMBER (3600)

#define WM_SOCKET_EVENT (WM_USER + 1)

//------------------------------------------------------------------------------
// 전역 변수:
//------------------------------------------------------------------------------
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

SOCKET listenSocket;
char packetBuffer[BUFFER_LENGTH_MAX];

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR lpCmdLine,
                      int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    MSG msg;
    HACCEL hAccelTable;

    // 전역 문자열을 초기화합니다.
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_ECHOSERVERASYNCEVENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ECHOSERVERASYNCEVENT));

    // 기본 메시지 루프입니다.
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//------------------------------------------------------------------------------
//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
//  설명:
//
//    Windows 95에서 추가된 'RegisterClassEx' 함수보다 먼저
//    해당 코드가 Win32 시스템과 호환되도록
//    하려는 경우에만 이 함수를 사용합니다. 이 함수를 호출해야
//    해당 응용 프로그램에 연결된
//    '올바른 형식의' 작은 아이콘을 가져올 수 있습니다.
//
//------------------------------------------------------------------------------
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ECHOSERVERASYNCEVENT));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_ECHOSERVERASYNCEVENT);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//------------------------------------------------------------------------------
//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
//------------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    hWnd = CreateWindow(szWindowClass, 
                        szTitle, 
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, 
                        0, 
                        200, 
                        300, 
                        NULL, 
                        NULL, 
                        hInstance, 
                        NULL);

    if(!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//------------------------------------------------------------------------------
//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, 
                         UINT message, 
                         WPARAM wParam, 
                         LPARAM lParam)
{
    int wmId;
    int wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    listenSocket = 0;
    memset(static_cast<void*>(packetBuffer), 0x00, sizeof(char) * BUFFER_LENGTH_MAX);

    switch(message)
    {
    case WM_CREATE:
        {
            OnEventMakeSocket(hWnd);
        }
        break;

    case WM_SOCKET_EVENT:
        {
            OnEventSocketNotify(hWnd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        {
            wmId = LOWORD(wParam);
            wmEvent = HIWORD(wParam);

            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBoxW(hInst, MAKEINTRESOURCEW(IDD_ABOUTBOX), hWnd, About);
                break;

            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;

            default:
                return DefWindowProc(hWnd, 
                                     message, 
                                     wParam, 
                                     lParam);
            }
        }
        break;

    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);

            // TODO: 여기에 그리기 코드를 추가합니다.

            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

//------------------------------------------------------------------------------
// 정보 대화 상자의 메시지 처리기입니다.
//------------------------------------------------------------------------------
INT_PTR CALLBACK About(HWND hDlg, 
                       UINT message, 
                       WPARAM wParam, 
                       LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        {
            if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
        }
        break;
    }

    return (INT_PTR)FALSE;
}

//------------------------------------------------------------------------------
int OnEventMakeSocket(HWND windowHandle)
{
    WSADATA windowSocketAgentData;
    WSAStartup(WINSOCK_VERSION, &windowSocketAgentData);

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenSocket == INVALID_SOCKET)
    {
        PopupErrorMessage(L"Socket create failed!");
        return SEVERITY_ERROR;
    }

    sockaddr_in serverAddress;
    memset(reinterpret_cast<void*>(&serverAddress), 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT_NUMBER);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    int bindResult = bind(listenSocket, 
                          reinterpret_cast<const sockaddr*>(&serverAddress), 
                          sizeof(serverAddress));
    if(bindResult == INVALID_SOCKET)
    {
        PopupErrorMessage(L"Socket bind failed!");
        return SEVERITY_ERROR;
    }

    int listenResult = listen(listenSocket, 5);
    if(bindResult == INVALID_SOCKET)
    {
        PopupErrorMessage(L"Socket listen failed!");
        return SEVERITY_ERROR;
    }

    int asyncResult = WSAAsyncSelect(listenSocket, 
                                     windowHandle, 
                                     WM_SOCKET_EVENT, 
                                     FD_ACCEPT | FD_CLOSE);

    return SEVERITY_SUCCESS;
}

//------------------------------------------------------------------------------
int OnEventSocketNotify(HWND windowHandle, WPARAM wParameter, LPARAM lParameter)
{
    WORD selectEvent = WSAGETSELECTEVENT(lParameter);
    switch(selectEvent)
    {
    case FD_ACCEPT:
        {
            listenSocket = accept((SOCKET)wParameter, NULL, NULL);

            WSAAsyncSelect(listenSocket, 
                           windowHandle, 
                           WM_SOCKET_EVENT, 
                           FD_READ | FD_CLOSE);
        }
        break;

    case FD_READ:
        {
            int readNumber = recv((SOCKET)wParameter, 
                                     packetBuffer, 
                                     BUFFER_LENGTH_MAX, 
                                     0);
            if(readNumber <= 0)
            {
                PopupErrorMessage(L"Recieve message error!\n");
            }

            int writeNumber = send((SOCKET)wParameter, 
                                   packetBuffer, 
                                   strlen(packetBuffer), 
                                  0);
            if(writeNumber <= 0)
            {
                PopupErrorMessage(L"Send message error!\n");
            }
        }
        break;

    case FD_CLOSE:
        {
            closesocket(listenSocket);
        }
        break;
    }

    return SEVERITY_SUCCESS;
}

//------------------------------------------------------------------------------
void PopupErrorMessage(wchar_t* message)
{
    if(message == nullptr)
    {
        return;
    }

    int messageResult = MessageBoxW(NULL, 
                                    message, 
                                    L"Error", 
                                    MB_ICONEXCLAMATION | MB_OK);
    if(messageResult != S_OK)
    {
        abort();
    }
}
