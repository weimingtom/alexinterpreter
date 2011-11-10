#include "alex_window.h"
#include "alex_lib.h"
#include "alex_interpret.h"


// 显示消息函数
ret_node* alex_message_box(ret_node* arg_list)
{
	MessageBox(	NULL,
		pop_arg_str(arg_list),
		pop_arg_str(arg_list),
		MB_OK
		);
	return NULL;
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
	
	static ALEX_FUNC  func[2] = {0};
	
	switch(message) 
	{
	case  WM_KEYDOWN:
		if(func[1])
		{
			inter_env env = new_inter_env();
			ret_node*  arg_list = new_ret_node(sym_type_num);
			arg_list->ret_value.r_v.num = (ALEX_NUMBER)wParam;
			inter_p_call(env, (tree_node*)func[1], arg_list);
			free_inter_env(env);
		}
		break;
	case  mes_key:
		func[1] = (ALEX_FUNC)wParam;
		break;
	case mes_pen:
		func[0] = (ALEX_FUNC)wParam;
		break;
	case WM_CREATE:
		break;
	case WM_COMMAND:
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
	//	Rectangle(hdc, 400, 400, 50, 50);
		if(func[0])
		{
			inter_env env = new_inter_env();
			ret_node* arg_list = new_ret_node(sym_type_pointer);
			arg_list->ret_value.r_v.ptr = (void*)(hWnd);
			arg_list->next = new_ret_node(sym_type_pointer);
			arg_list->next->ret_value.r_v.ptr = (void*)(hdc);
			inter_p_call(env, (tree_node*)func[0], arg_list);
			free_inter_env(env);
		}

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
ret_node* alex_rectangle(ret_node* arg_list)
{
	HDC hdc = (HDC)pop_arg_ptr(arg_list);
	int x = (int)pop_arg_num(arg_list);
	int y = (int)pop_arg_num(arg_list);
	int w = (int)pop_arg_num(arg_list);
	int h = (int)pop_arg_num(arg_list);
	
	Rectangle(hdc, x, y, w, h);
		
	return NULL;
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
	SendMessage(hd, mes_pen, (WPARAM)func, NULL);
	InvalidateRect(hd, NULL, TRUE);
	free(lp);
	return 0;
} 



// 注册绘制事件
ret_node* alex_reg_pen(ret_node* arg_list)
{
	DWORD tp;
	byte* lp = NULL;
	byte* t_lp = NULL;
	HWND hd = (HWND)pop_arg_ptr(arg_list);
	ALEX_FUNC func = pop_arg_func(arg_list);
	int tt = (int)pop_arg_num(arg_list);
	int len = sizeof(HWND)+sizeof(ALEX_FUNC)+sizeof(int);
	lp = (byte*)malloc(len);
	memset(lp, 0, len);
	
	t_lp = lp;
	*((HWND*)lp) = hd;
	lp +=sizeof(hd);
	*((ALEX_FUNC*)lp) = func;
	lp +=sizeof(func);
	*((int*)lp) = tt;

	CreateThread(NULL, 0, mes_tp, t_lp, 0, &tp);
	return NULL;
}



ret_node* alex_reg_key(ret_node* arg_list)
{
	HWND hd = pop_arg_ptr(arg_list);
	ALEX_FUNC func = pop_arg_func(arg_list);
	
	SendMessage(hd, mes_key, (WPARAM)func, NULL);
	
	return NULL;
}





// 创建窗口函数
ret_node* alex_create_window(ret_node* arg_list)
{
	MSG msg;
	char* name = pop_arg_str(arg_list);
	int x = (int)pop_arg_num(arg_list);
	int y = (int)pop_arg_num(arg_list);
	int w = (int)pop_arg_num(arg_list);	
	int h = (int)pop_arg_num(arg_list);
	ALEX_FUNC c_f = pop_arg_func(arg_list);

	TCHAR* szTitle       = _TEXT(name);     // 标题栏文本
	TCHAR* szWindowClass = _TEXT(name);     // 主窗口类名
	WNDCLASSEX wcex={0};
	HWND w_m;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	BOOL bl;
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
	
	w_m = CreateWindow(szWindowClass, szTitle,   WS_OVERLAPPEDWINDOW &(~WS_SIZEBOX),			//创建窗口
		x,y, w, h, NULL, NULL, hInstance, NULL);
	
	ShowWindow(w_m, SW_SHOW);
	UpdateWindow(w_m);
	
	{
		inter_env env = new_inter_env();
		ret_node* arg_list = new_ret_node(sym_type_pointer);
		arg_list->ret_value.r_v.ptr = (void*)w_m;
		inter_p_call(env, c_f, arg_list);
		free_inter_env(env);
	}
	
	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return NULL;
}

