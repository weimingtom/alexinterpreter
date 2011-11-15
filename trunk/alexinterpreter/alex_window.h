#ifndef		_ALEX_WINDOW_H_
#define		_ALEX_WINDOW_H_
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <tchar.h>
#include "alex_vm.h"

enum mes{
	mes_pen=0x11234,
	mes_key
};

int alex_clear(vm_env* vm_p);
int alex_create_window(vm_env* vm_p);
int alex_message_box(vm_env* vm_p);
int alex_rectangle(vm_env* vm_p);
int alex_reg_pen(vm_env* vm_p);
int alex_reg_key(vm_env* vm_p);
int alex_t_time(vm_env* vm_p);

#endif