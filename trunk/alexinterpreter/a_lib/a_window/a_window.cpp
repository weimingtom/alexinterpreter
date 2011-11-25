// a_window.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "alex_ani.h"
#include <tchar.h>
#include <stdlib.h>

typedef  struct _w_handle{
	HDC hdc;
	HWND w_m;
	int key;
}w_handle;
#define MAX_WINDOW_LEN 24
static w_handle handle_list[MAX_WINDOW_LEN] = {0};

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

// sleep����
int alex_sleep(void* vm_p)
{
	int w_t = (int)ani_pop_number(vm_p);
	Sleep((DWORD)w_t);
	
	return 0;
}

// ��ʾ��Ϣ����
int alex_message_box(void* vm_p)
{
	a_string t = ani_pop_string(vm_p);
	a_string n = ani_pop_string(vm_p);
	
	MessageBox(	NULL,
		t.s_ptr,
		n.s_ptr,
		MB_OK
		);
	return 0;
}


// ���ϵͳ���� ��ǰ�ĺ�����
int alex_t_time(void* vm_p)
{
	ani_push_number(vm_p, (ALEX_NUMBER)(GetTickCount()));
	return 1;
}

int search_handle(HWND hWnd)
{
	int i;

	for(i=0; (i<MAX_WINDOW_LEN) && hWnd; i++)
	{
		if(handle_list[i].w_m == hWnd)
			return i;
	}
	return -1;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	int handle = -1;

	switch(message) 
	{
	case  WM_KEYDOWN:
		handle = search_handle(hWnd);
		if(handle != -1)
			handle_list[handle].key = (int)wParam;
		break;
	case WM_CREATE:
		break;
	case WM_COMMAND:
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ���ƾ���
int alex_rectangle(void* vm_p)
{
	int h = (int)ani_pop_number(vm_p);
	int w = (int)ani_pop_number(vm_p);
	int y = (int)ani_pop_number(vm_p);
	int x = (int)ani_pop_number(vm_p);
	int handle = (int)ani_pop_number(vm_p);

//	ms_tt(NULL);
	Rectangle(handle_list[handle].hdc, x, y, w, h);
	
	ValidateRect(handle_list[handle].w_m, NULL);
	UpdateWindow(handle_list[handle].w_m);
	
	return 0;
}

int alex_get_key(void* vm_p)
{
	int ret = 0;
	int handle = (int)ani_pop_number(vm_p);
	if(handle_list[handle].key)
	{
		ret = handle_list[handle].key;
		handle_list[handle].key = 0;
	}

	ani_push_number(vm_p, (ALEX_NUMBER)ret);
	return 1;
}


DWORD WINAPI create_window_p(LPVOID lp)
{
	byte* w_lp = (byte*)lp;
	MSG msg;
	a_string name={0};
	int x, y, w, h, handle;
	TCHAR* szTitle = NULL;
	TCHAR* szWindowClass = NULL;
	WNDCLASSEX wcex={0};
	HINSTANCE hInstance = GetModuleHandle(NULL);
	BOOL bl;
	
	name = *((a_string*)w_lp);
	w_lp += sizeof(a_string);
	x = *((int*)w_lp);
	w_lp+=sizeof(int);
	y = *((int*)w_lp);
	w_lp+=sizeof(int);
	w = *((int*)w_lp);
	w_lp+=sizeof(int);
	h = *((int*)w_lp);
	w_lp+=sizeof(int);
	handle = *((int*)w_lp);

	szTitle       = (TCHAR*)_TEXT(name.s_ptr);     // �������ı�
	szWindowClass = (TCHAR*)_TEXT(name.s_ptr);     // ����������
    wcex.cbSize = sizeof(WNDCLASSEX);
	
	wcex.style           = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc     = (WNDPROC)WndProc;
	wcex.cbClsExtra      = 0;
	wcex.cbWndExtra      = 0;
	wcex.hInstance       = hInstance;
	wcex.hIcon           = NULL;
	wcex.hCursor         = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground   = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName    = NULL;
	wcex.lpszClassName   = szWindowClass;
    wcex.hIconSm         = NULL;
    bl = RegisterClassEx(&wcex);												//ע�ᴰ����
	
	handle_list[handle].w_m = CreateWindow(szWindowClass, szTitle,   WS_OVERLAPPEDWINDOW &(~WS_SIZEBOX),			//��������
		x,y, w, h, NULL, NULL, hInstance, NULL);
	
	ShowWindow(handle_list[handle].w_m, SW_SHOW);
	UpdateWindow(handle_list[handle].w_m);

	handle_list[handle].hdc = GetWindowDC(handle_list[handle].w_m);

	free((void*)lp);
	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

int alex_clear(void* vm_p)
{
	int handle = (int)ani_pop_number(vm_p);
	if(handle>=0)
	{
		InvalidateRect(handle_list[handle].w_m, NULL, TRUE);
		UpdateWindow(handle_list[handle].w_m);
	}
	return 0;
}

// �������ں���
int alex_create_window(void* vm_p)
{
	int i=0;
	int handle = -1;
	byte* m_p = NULL;
	byte* m_p_p = NULL;
	int len=0;
	DWORD tp;
	int h = (int)ani_pop_number(vm_p);
	int w = (int)ani_pop_number(vm_p);
	int y = (int)ani_pop_number(vm_p);
	int x = (int)ani_pop_number(vm_p);
	a_string name = ani_pop_string(vm_p);
	
	
	for(i=0; i<MAX_WINDOW_LEN; i++)
	{
		if(handle_list[i].w_m == NULL)
		{
			handle = i;
			break;
		}
	}

	len = sizeof(a_string)+sizeof(int)*5;
	m_p = (byte*)malloc((size_t)len);
	m_p_p = m_p;
	*((a_string*)m_p_p) = name;
	m_p_p += sizeof(a_string);
	*((int*)m_p_p) = x;
	m_p_p +=sizeof(int);
	*((int*)m_p_p) = y;
	m_p_p+=sizeof(int);
	*((int*)m_p_p) = w;
	m_p_p += sizeof(int);
	*((int*)m_p_p) = h;
	m_p_p += sizeof(int);
	*((int*)m_p_p) = handle;

	CreateThread(NULL, 0, create_window_p, m_p, 0, &tp);
	while(1)
	{
		if(handle>=0 && (handle_list[handle].w_m==NULL || handle_list[handle].hdc ==NULL))
			break;
	}
	
	ani_push_number(vm_p, (ALEX_NUMBER)handle);
	return 1;
}


// dll ע��alex�����ӿ�
extern "C" _declspec(dllexport)
void alex_dll_reg()
{
	ani_reg_func("sleep", alex_sleep);
	ani_reg_func("clear", alex_clear);
	ani_reg_func("create_window", alex_create_window);
	ani_reg_func("message_box", alex_message_box);
	ani_reg_func("rectangle", alex_rectangle);
	ani_reg_func("t_time", alex_t_time);
	ani_reg_func("get_key", alex_get_key);
}