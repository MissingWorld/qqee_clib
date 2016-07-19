//windows窗体相关函数
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <winuser.h>

/*
SEEK_SET： 文件开头
SEEK_CUR： 当前位置
SEEK_END： 文件结尾
*/

//  -luser32
LRESULT WINAPI SendMessageA(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
int WINAPI GetWindowTextA(HWND hWnd,LPSTR lpString,int nMaxCount);
WINBOOL WINAPI SetWindowTextA(HWND hWnd,LPCSTR lpString);
UINT_PTR WINAPI SetTimer(HWND hWnd,UINT_PTR nIDEvent,UINT uElapse,TIMERPROC lpTimerFunc);
WINBOOL WINAPI KillTimer(HWND hWnd,UINT_PTR uIDEvent);
DWORD WINAPI GetWindowThreadProcessId(HWND hWnd,LPDWORD lpdwProcessId);
HWND WINAPI FindWindowExA(HWND hWndParent,HWND hWndChildAfter,LPCSTR lpszClass,LPCSTR lpszWindow);
WINBOOL WINAPI IsWindow(HWND hWnd);
HWND WINAPI GetActiveWindow(VOID);

//通过句柄取标题字符串，返回取回字符串的长度
//如果*out预先malloc的长度不够，可能会被
int mm_windows_get_text_message(HWND hWnd,char**out,unsigned out_len){
	int len=SendMessageA(hWnd,14,0,0);
	if(len>0){
		if(len>out_len)*out=(char*)realloc(out,len+1);
		SendMessageA(hWnd,13,len+1,(LPARAM)*out);
	}
	return strlen(out);
}
//通过句柄取标题字符串，返回取回字符串的长度
int mm_windows_get_text_api(HWND hWnd,char*out,unsigned out_len){
	return GetWindowTextA(hWnd,out,out_len);
}
//通过句柄置标题字符串，返回1成功，返回0失败，需要注意编码格式
int mm_windows_set_text_message(HWND hWnd,char*in){
	return SetWindowTextA(hWnd,in);
}
//窗口重画
int mm_windows_renew(HWND hWnd){
	return SendMessageA(hWnd,15,0,0);
}
//设置时钟
int mm_windows_settimer(HWND hWnd,int flag,unsigned time_ms,void*function){
	return SetTimer(hWnd,flag,time_ms,function);
}
//销毁时钟
int mm_windows_killtimer(HWND hWnd,int flag){
	return KillTimer(hWnd,flag);
}
//通过句柄取类名，返回取回字符串的长度
int mm_windows_get_class(HWND hWnd,char*out,unsigned out_len){
	return GetClassName(hWnd,out,out_len);
}
//通过句柄取进程PID，失败返回-1
int mm_windows_get_pid_from_windows_hwnd(HWND hWnd){
	int pid=-1;
	GetWindowThreadProcessId(hWnd,&pid);//返回值为该窗口所在线程ID
	return pid;
}
/**
 * 查找指定窗口的子级窗口
 * @param  hWnd       父窗口句柄  0为桌面.找顶级窗口
 * @param  start_hwnd 从该句柄后开始查找, 0找第一个符合条件的
 * @param  class_name 欲查找的子窗口类名
 * @return            返回子窗口句柄
 */
int mm_windows_find_windows(HWND hWnd,HWND start_hwnd,char*class_name){
	return (int)FindWindowExA(hWnd,start_hwnd,(LPCSTR)class_name,(LPCSTR)0);
}
//发送点击点击坐标消息，click_times为点击次数1或2
void mm_windows_click(HWND hWnd,unsigned click_times,unsigned msg_type_0513_1245_2doubleclick,int x,int y){
	int i=y*65536+x;
	for(;click_times--;){
		if(msg_type_0513_1245_2doubleclick==0){
			SendMessageA(hWnd,513,0,i);
			SendMessageA(hWnd,514,0,i);
		}else if(msg_type_0513_1245_2doubleclick==1){
			SendMessageA(hWnd,245,0,i);
		}else if(msg_type_0513_1245_2doubleclick==2){
			SendMessageA(hWnd,515,0,i);
		}
	}
}
//通过类名和标题寻找窗口句柄，未找到返回负数
//mm_windows_find_windows_from_title_and_class(HWND main_hwnd,"Button","确定")
int mm_windows_find_windows_from_title_and_class(HWND main_hwnd,char*class_name,char*title){
	HWND backs,tmp=0;
	char out[256]={0};
	unsigned len=strlen(title);
	unsigned len2=0;
	while(1){
		backs=FindWindowExA(main_hwnd,tmp,(LPCSTR)class_name,(LPCSTR)0);
		if(backs==0)return -1;
		len2=GetWindowTextA(backs,out,255);
		tmp=backs;
		if(len2==len && strcmp(title,out)==0)return (int)backs;
	}
	return -2;
}
//判断窗口句柄是否有效，0=无效句柄，1=窗口句柄有效
int mm_windows_iswindow(HWND hWnd){
	return IsWindow(hWnd);
}
/*//闪动窗
void mm_windows_flash_window(HWND hWnd){
	FlashWindow(hWnd,1);
}*/
//获得活动窗口的句柄
HWND mm_windows_get_active_window(){
	return GetActiveWindow();
}
//窗口是否已经最大化
int mm_windows_is_max(HWND hWnd){
	return IsZoomed(hWnd);
}
//窗口是否已经最小化
int mm_windows_is_min(HWND hWnd){
	return IsIconic(hWnd);
}
//使窗口获取焦点
int mm_windows_set_focus(HWND hWnd){
	return SetFocus(hWnd);
}
//窗口是否可见
int mm_windows_is_can_show(HWND hWnd){
	return IsWindowVisible(hWnd);
}
//取窗口宽/高/左/顶
void mm_windows_get_size(HWND hWnd,int*width,int*height,int*left,int*top){
	RECT xy;
	GetWindowRect(hWnd,&xy);
	if(width!=NULL)*width=(xy.right-xy.left);
	if(height!=NULL)*height=(xy.bottom-xy.top);
	if(left!=NULL)*left=xy.left;
	if(top!=NULL)*left=xy.top;
}
//显示桌面，模拟 win+d 键
void mm_windows_win_and_d(){
	keybd_event (91, 0, 0, 0);
	keybd_event (68, 0, 0, 0);
	keybd_event (91, 0, 2, 0);
	keybd_event (68, 0, 2, 0);
}
//设置窗口置顶，type_1top_0notop为0取消置顶
int mm_windows_set_top(HWND hWnd,int type_1top_0notop){
	//return BringWindowToTop(hWnd);
	return SetWindowPos(hWnd,((type_1top_0notop==1)?-1:-2), 0, 0, 0, 0, 67);
}
//设置窗口禁止，type_1lock_0unlock为0取消禁止
int mm_windows_set_enable(HWND hWnd,int type_0lock_1unlock){
	//return BringWindowToTop(hWnd);
	return EnableWindow(hWnd,type_0lock_1unlock);
}
//设置窗口尺寸
int mm_windows_move_window(HWND hWnd,int left,int top,int width,int height,int renew){
	return MoveWindow (hWnd, left, top, width, height, renew);
}
//设置窗口可视状态
//0 隐藏取消激活 1 最小化取消激活 2 还原激活 3 显示激活 4 最大化激活 5 最小化激活 6 最小化 7 显示 8 最近状态显示 9 还原
int mm_windows_set_show_hide(HWND hWnd,int type_0hide_1minNoActive_2resum_3show_4max_5minActive_6min_7show_8near_9resum){
	return ShowWindow(hWnd,type_0hide_1minNoActive_2resum_3show_4max_5minActive_6min_7show_8near_9resum);
}











int main(int argc, char const *argv[]){
	char out[2048]={0};
	//printf("get_text:|%s|%d\n",out,n);
	
	int n=mm_windows_get_pid_from_windows_hwnd((HWND)7799820);
	printf("get_text_api:|%s|%d\n",out,n);

	mm_windows_click(4655020,5,0,0,0);
	//mm_windows_flash_window(4655020);
	//Sleep(1000);
	n=mm_windows_get_active_window();
	printf("active_window:%d\n",n);

	n=mm_windows_set_top(1640070,0);
	printf("set_top:%d\n",n);

	//n=mm_windows_move_window(1508998,100,200,800,400,1);
	//printf("move_window:%d\n",n);
	//mm_windows_win_and_d();

	/*n=mm_windows_get_width(1640070);
	printf("get_width:%d\n",n);*/

	//mm_windows_set_enable(460460,1);
	//mm_windows_set_show_hide(460460,1);

	return 0;
}
