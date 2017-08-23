#pragma once

#include "resource.h"

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int OnEventMakeSocket(HWND windowHandle);
int OnEventSocketNotify(HWND windowHandle, WPARAM wParameter, LPARAM lParameter);
void PopupErrorMessage(wchar_t* message);