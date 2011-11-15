#include "alex_window.h"
#include "alex_lib.h"
#include "alex_interpret.h"
#include "alex_sym.h"

typedef  struct _w_handle{
	HDC hdc;
	HWND w_m;
	r_value key_func;
}w_handle;

#define MAX_WINDOW_LEN 24
w_handle handle_list[MAX_WINDOW_LEN] = {0};
HDC hdc;
HWND hd;
DWORD WINAPI ms_tt(LPVOID lp);


// 显示消息函数
int alex_message_box(vm_env* vm_p)
{
	a_string n = pop_string(vm_p);
	a_string t = pop_string(vm_p);
	
	MessageBox(	NULL,
		t.s_ptr,
		n.s_ptr,
		MB_OK
		);
	return 0;
}


// 获得系统启动 当前的毫秒数
int alex_t_time(vm_env* vm_p)
{
	push_data(&vm_p->data_ptr, new_number((ALEX_NUMBER)(GetTickCount())));

	return 1;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	
	switch(message) 
	{
	case  WM_KEYDOWN:
/*		if(w_func[1].r_t != sym_type_error)
		{
			push_number(&alex_vm_env, (ALEX_NUMBER)wParam);
			vm_p_call(&alex_vm_env, &w_func[1]);
		}
*/		break;
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

// 绘制矩形
int alex_rectangle(vm_env* vm_p)
{
	int h = (int)pop_number(vm_p);
	int w = (int)pop_number(vm_p);
	int y = (int)pop_number(vm_p);
	int x = (int)pop_number(vm_p);
	int handle = (int)pop_number(vm_p);

//	ms_tt(NULL);
	Rectangle(handle_list[handle].hdc, x, y, w, h);
	
	ValidateRect(handle_list[handle].w_m, NULL);
	UpdateWindow(handle_list[handle].w_m);
	
	return 0;
}

// 窗口消息线程
DWORD WINAPI mes_tp(LPVOID lp)
{
	byte* t_lp = (byte*)lp;
	ALEX_FUNC func = NULL;
	int tt = 0;
	HWND hd = *((HWND*)t_lp);
	t_lp +=sizeof(hd);
	func = *((ALEX_FUNC*)t_lp);
	t_lp += sizeof(ALEX_FUNC);
	tt = *((int*)t_lp);

	Sleep(tt);
	SendMessage(hd, mes_pen, 0, 0);
	InvalidateRect(hd, NULL, TRUE);
	free(lp);
	return 0;
} 



// 注册绘制事件
int alex_reg_pen(vm_env* vm_p)
{
	DWORD tp;
	byte* lp = NULL;
	byte* t_lp = NULL;
	
	int tt = (int)pop_number(vm_p);
	r_value func = pop_func(vm_p);
	HWND hd = (HWND)pop_ptr(vm_p);
	
	int len = sizeof(HWND)+sizeof(r_value)+sizeof(int);
	lp = (byte*)malloc(len);
	memset(lp, 0, len);

	t_lp = lp;
	*((HWND*)lp) = hd;
	lp +=sizeof(hd);
	*((r_value*)lp) = func;
	lp +=sizeof(func);
	*((int*)lp) = tt;

	CreateThread(NULL, 0, mes_tp, t_lp, 0, &tp);
	return 0;
}


int alex_reg_key(vm_env* vm_p)
{
	HWND hd;
//	w_func[1] = pop_func(vm_p);
	hd = (HWND)pop_ptr(vm_p);

	return 0;
}

DWORD WINAPI ms_tt(LPVOID lp)
{
	int i=0;
	while(1)
	{
		Rectangle(handle_list[0].hdc, 40+i*10, 40, 200+i*10, 100);
		ValidateRect(handle_list[0].w_m, NULL);
		UpdateWindow(handle_list[0].w_m);

		i++;
		print(" i= %d\n", i);
		Sleep(1000);
	}
	return 0;
}

DWORD WINAPI create_window_p(LPVOID lp)
{

	DWORD tp;
	byte* w_lp = lp;
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

	szTitle       = _TEXT(name.s_ptr);     // 标题栏文本
	szWindowClass = _TEXT(name.s_ptr);     // 主窗口类名
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
    bl = RegisterClassEx(&wcex);												//注册窗口类
	
	handle_list[handle].w_m = CreateWindow(szWindowClass, szTitle,   WS_OVERLAPPEDWINDOW &(~WS_SIZEBOX),			//创建窗口
		x,y, w, h, NULL, NULL, hInstance, NULL);
	
	ShowWindow(handle_list[handle].w_m, SW_SHOW);
	UpdateWindow(handle_list[handle].w_m);

	handle_list[handle].hdc = GetWindowDC(handle_list[handle].w_m);

//	CreateThread(NULL, 0, ms_tt, NULL, 0, &tp);

	free(lp);
	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

int alex_clear(vm_env* vm_p)
{
	int handle = (int)pop_number(vm_p);
	if(handle>=0)
	{
		InvalidateRect(handle_list[handle].w_m, NULL, TRUE);
		UpdateWindow(handle_list[handle].w_m);
	}
	return 0;
}

// 创建窗口函数
int alex_create_window(vm_env* vm_p)
{
	int i=0;
	int handle = -1;
	byte* m_p = NULL;
	byte* m_p_p = NULL;
	int len=0;
	DWORD tp;
	int h = (int)pop_number(vm_p);
	int w = (int)pop_number(vm_p);
	int y = (int)pop_number(vm_p);
	int x = (int)pop_number(vm_p);
	a_string name = pop_string(vm_p);
	
	
	for(i=0; i<MAX_WINDOW_LEN; i++)
	{
		if(handle_list[i].w_m == NULL)
		{
			handle = i;
			break;
		}
	}

	len = sizeof(a_string)+sizeof(int)*5;
	m_p = (byte*)malloc(len);
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
//	create_window_p((LPVOID)m_p);
	while(handle>=0 && (handle_list[handle].w_m==NULL || handle_list[handle].hdc ==NULL))
	{}
	
//	CreateThread(NULL, 0, ms_tt, NULL, 0, &tp);
	push_number(vm_p, (ALEX_NUMBER)handle);
	return 1;
}


