#include "stdafx.h"
#include "EchoServerAsyncEvent.h"

//------------------------------------------------------------------------------
#define MAX_LOADSTRING 100

#define BUFFER_LENGTH_MAX (1024)
#define PORT_NUMBER (3600)

#define WM_SOCKET_EVENT (WM_USER + 1)

//------------------------------------------------------------------------------
// ���� ����:
//------------------------------------------------------------------------------
HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
TCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.

SOCKET listenSocket;
char packetBuffer[BUFFER_LENGTH_MAX];

//------------------------------------------------------------------------------
// ��Ʈ�� ����Ʈ
//------------------------------------------------------------------------------
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPWSTR lpCmdLine,
                      int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.
    MSG msg;
    HACCEL hAccelTable;

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_ECHOSERVERASYNCEVENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ECHOSERVERASYNCEVENT));

    // �⺻ �޽��� �����Դϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
//  ����:
//
//    Windows 95���� �߰��� 'RegisterClassEx' �Լ����� ����
//    �ش� �ڵ尡 Win32 �ý��۰� ȣȯ�ǵ���
//    �Ϸ��� ��쿡�� �� �Լ��� ����մϴ�. �� �Լ��� ȣ���ؾ�
//    �ش� ���� ���α׷��� �����
//    '�ùٸ� ������' ���� �������� ������ �� �ֽ��ϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
//------------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����: �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND	- ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT	- �� â�� �׸��ϴ�.
//  WM_DESTROY	- ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
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

            // �޴� ������ ���� �м��մϴ�.
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

            // TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.

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
// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
