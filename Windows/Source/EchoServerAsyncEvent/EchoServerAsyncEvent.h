#pragma once

#include "resource.h"

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int OnEventMakeSocket(HWND windowHandle);
int OnEventSocketNotify(HWND windowHandle, WPARAM wParameter, LPARAM lParameter);
void PopupErrorMessage(wchar_t* message);