//windows进程相关函数
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>

DWORD WINAPI GetLastError(VOID);

//取cpu核数
int mm_get_cpu_cores(){
	return atoi(getenv("NUMBER_OF_PROCESSORS"));
}
//取自进程ID
DWORD WINAPI GetCurrentProcessId(VOID);
int mm_process_get_pid_self(){
	return GetCurrentProcessId();
}
//打开进程，完全访问权限，失败返回0
HANDLE WINAPI OpenProcess(DWORD,WINBOOL,DWORD);
WINBOOL WINAPI CloseHandle(HANDLE hObject);
int mm_process_open_process(int pid){
	return (int)OpenProcess(2035711, 0, pid);
}
//强力打开进程，失败返回0，编译时需要 -lntdll
typedef struct tag_OBJECT_ATTRIBUTES{
	int Length;
	int RootDirectory;
	int ObjectName;
	int Attributes;
	int SecurityDescriptor;
	int SecurityQualityOfService;
}OBJECT_ATTRIBUTES;
typedef struct tag_CLIENT_ID{
	int UniqueProcess;
	int UniqueThread;
}CLIENT_ID;
void __stdcall ZwOpenProcess(int*,int,OBJECT_ATTRIBUTES,CLIENT_ID);
int mm_process_open_process_x(int pid){
	OBJECT_ATTRIBUTES oa;
	CLIENT_ID ci;
	int ret=0;
	oa.Length=24;
	ci.UniqueThread=0;
	ci.UniqueProcess=pid;
	ZwOpenProcess(&ret, 2035711, oa, ci);
	//int err=GetLastError();
	//printf("err:%d\n",err);
	return ret;
}
//取进程占用内存量KB，pid=-1取自进程，编译时需要 -lpsapi
typedef struct tag_PROCESS_MEMORY_COUNTERS{
	int cb;
	int PageFaultCount;
	int PeakWorkingSetSize;
	int WorkingSetSize;
	int QuotaPeakPagedPoolUsage;
	int QuotaPagedPoolUsage;
	int QuotaPeakNonPagedPoolUsage;
	int QuotaNonPagedPoolUsage;
	int PagefileUsage;
	int PeakPagefileUsage;
}PROCESS_MEMORY_COUNTERS;
BOOL WINAPI GetProcessMemoryInfo(HANDLE,PROCESS_MEMORY_COUNTERS*,DWORD);
int mm_process_get_process_memory_KB(int pid){
	int rets= -3;
	int hProcess = -1;
	if(pid>0){
		hProcess=(int)OpenProcess(2035711, 0, pid);
		if(hProcess<=0)return -2;
	}
	//printf("OpenProcess<%d>\n",hProcess);
	//printf("PROCESS_MEMORY_COUNTERS<%d>\n",sizeof(PROCESS_MEMORY_COUNTERS));
	PROCESS_MEMORY_COUNTERS tPmC;
	if(GetProcessMemoryInfo((HANDLE)hProcess,(PROCESS_MEMORY_COUNTERS*)&tPmC,sizeof(PROCESS_MEMORY_COUNTERS)) != 0){
		//printf("rets<%d>\n",rets);
		rets = ((tPmC.WorkingSetSize)>>10);//KB
	}
	CloseHandle((HWND)hProcess);
	return rets;
}
//取进程亲和cpu编号1~n，失败返回-1，返回0亲和全部核。pid=-1取自进程
DWORD WINAPI GetCurrentProcessId(VOID);
WINBOOL WINAPI GetProcessAffinityMask(HANDLE,PDWORD_PTR,PDWORD_PTR);
int mm_process_get_cpu_kiss(int pid){
	int a,b,bk,cores;
	HANDLE ids;
	if(pid==-1)pid=GetCurrentProcessId();
	//printf("<AA>\n");
	ids=OpenProcess(2035711, 0, pid);
	//printf("<%d>\n",ids);
	if(0==ids)return -1;
	bk=GetProcessAffinityMask(ids, &a, &b);
	CloseHandle(ids);
	//printf("<%d>%d\t%d\n",bk,a,b);
	if(bk==1){
		cores=mm_get_cpu_cores();
		if(a>cores)return 0;
		return a;
	}
	return bk;
}
//置cpu亲和，失败返回-1，成功返回1。pid=-1取自进程
//cpu_id为0全部亲和、1~n亲和第n核
WINBOOL WINAPI SetProcessAffinityMask(HANDLE,DWORD_PTR);
int mm_process_set_cpu_kiss(int pid,int cpu_id){
	HWND handls;
	int i=0,cpus_n=0,cpus=0,ret=0;
	if(pid==-1)pid=GetCurrentProcessId();
	handls=OpenProcess(2035711, 0, pid);
	//printf("<%d>\n",handls);
	//可改为强力打开
	if(cpu_id==0){
		cpus=mm_get_cpu_cores();
		for(i=1;i<=cpus;i++){
			//printf(">%d\n",i);
			cpus_n+=i;
		}
		//printf("<%d>%d\n",cpus_n,cpus);
	}else{
		cpus_n=cpu_id;
	}
	ret=SetProcessAffinityMask(handls, cpus_n);
	CloseHandle (handls);
	return ret;
}
//提升当前进程权限，编译参数 -lntdll -lAdvapi32
typedef struct tag_LUID{
	int lowpart;
	int highpart;
}LUIDs;
typedef struct tag_TOKEN_PRIVILEGES{
	int PrivilegeCount;
	int lowpart;
	int highpart;
	int Attributes;
}TOKEN_PRIVILEGES_;
int WINAPI RtlAdjustPrivilege(int,int,int,int);
WINBOOL WINAPI OpenProcessToken(HANDLE,DWORD,PHANDLE);
WINBOOL WINAPI LookupPrivilegeValueA(LPCSTR,LPCSTR,PLUID);
WINADVAPI WINBOOL WINAPI AdjustTokenPrivileges(HANDLE,WINBOOL,PTOKEN_PRIVILEGES,DWORD,PTOKEN_PRIVILEGES,PDWORD);
void mm_process_power_up_self(){//mm_power_up
	/*int i=30;
	for(;i--;){
		printf("%d\n",30-i);
		RtlAdjustPrivilege(30-i, 1, 0, 0);
	}*/
	//RtlAdjustPrivilege(20, 1, 0, 0);
	//printf("<A>\n");
	//打开当前进程令牌对象
	HWND TokenHandle;
	LUIDs luid;
	TOKEN_PRIVILEGES_ NewState;
	//OpenProcessToken (GetCurrentProcessId(), 983551, &TokenHandle);
	//OpenProcessToken((HANDLE)GetCurrentProcessId(),40,(PHANDLE)&TokenHandle);
	OpenProcessToken((HANDLE)GetCurrentProcessId(),983551,(PHANDLE)&TokenHandle);
	//printf("<B>\n");
	//取当前进程权限令牌
	//LookupPrivilegeValue (NULL,"SeDebugPrivilege",&luid);//SE_DEBUG_NAME
	LookupPrivilegeValueA(NULL,(LPCTSTR)"SeDebugPrivilege",(PLUID)&luid);//SE_DEBUG_NAME
	//printf("<C>\n");
	NewState.PrivilegeCount=1;
	NewState.highpart=luid.highpart;
	NewState.lowpart=luid.lowpart;
	NewState.Attributes=2;
	//获取令牌特权
	AdjustTokenPrivileges((HANDLE)TokenHandle, 0, (PTOKEN_PRIVILEGES)&NewState, 0, 0, 0);
	//AdjustTokenPrivileges((HANDLE)TokenHandle, 0, (PTOKEN_PRIVILEGES)&NewState, 28, 0, 0);
	//printf("<D>\n");
	CloseHandle(TokenHandle);
	//printf("<E>\n");
}
//压缩进程内存占用，成功返回1，失败返回-1
WINBOOL WINAPI SetProcessWorkingSetSize(HANDLE,SIZE_T,SIZE_T);
int mm_process_mem_free(int pid){
	HANDLE hProcess;
	int ret=-1;
	if(pid>0){
		hProcess=OpenProcess(2035711,0,pid);
	}
	ret=SetProcessWorkingSetSize(hProcess, -1, -1);
	CloseHandle(hProcess);
	//printf("free:%d\n",ret);
	return ret;
}
//取自进程cmd，失败返回负数，成功返回cmd长度
LPSTR WINAPI GetCommandLineA(VOID);
int mm_process_get_cmd_self(char*out){
	char*p;
	p=GetCommandLineA();
	if(p==NULL)return -1;
	strcpy(out,p);
	return strlen(p);
}
/**
 * 通过进程PID取进程名
 * @param  pid 进程PID,-1为自身进程
 * @param  out 类型为char*，长度最小为256
 * @return     返回进程名字符串长度
 */
DWORD WINAPI GetModuleBaseNameA(HANDLE,HMODULE,LPSTR,DWORD);
int mm_process_get_name_form_pid(int pid,char*out){
	HANDLE p;
	int n;
	if(pid==-1)pid=GetCurrentProcessId();
	p=OpenProcess(1040, 0, pid);
	if((int)p<1)return -1;
	n=GetModuleBaseNameA(p, 0, out, 255);
	CloseHandle(p);
	return n;
}
/**
 * 关闭进程
 * @param  pid 进程PID
 * @return     成功返回1，失败返回-1
 */
WINBOOL WINAPI TerminateProcess(HANDLE hProcess,UINT uExitCode);
int mm_process_shutdown(int pid){
	int i=0;
	//HANDLE p=mm_process_openprocess_x(pid);
	HANDLE p=OpenProcess(2035711, 0, pid);
	//printf("p:%d\n",p);
	if((int)p<1)return -1;
	return TerminateProcess(p,0);
	//printf("shutdown:%d\n",i);
	//return (i!=0?-2:1);
}
/**
 * 恢复挂起的进程，挂起n次恢复时需要恢复n次，编译参数 -lntdll
 * @return 返回0成功，失败返回负数
 */
int __stdcall ZwResumeProcess(int hProcess);
int mm_process_resume(int PID){
	int hProcess=OpenProcess(2035711, 0, PID);
	if(hProcess<=0)return -1;
	return ((ZwResumeProcess(hProcess)==0)?0:-2);
}
/**
 * 挂起进程，挂起n次恢复时需要恢复n次，编译参数 -lntdll
 * @return 返回0成功，失败返回负数
 */
int __stdcall ZwSuspendProcess(int hProcess);
int mm_process_stop(int PID){
	int hProcess=OpenProcess(2035711, 0, PID);
	if(hProcess<=0)return -1;
	return ((ZwSuspendProcess(hProcess)==0)?0:-2);
}
/**
 * Unicode转Ansi
 * @param  in  宽字符串
 * @param  out 输出，长度必须为2048
 * @return     成功返回装换后字符串长度
 */
#include <locale.h>
#include <tchar.h>
int UnicodeToAnsi_c(char*in,char*out){
	_tsetlocale(LC_ALL,_T(""));
	wcstombs(out, (wchar_t*)in, 2048);
	//printf("\nsChar|%s|\n\n",out);
	return strlen(out);
}
/**
 * 取进程 cmd、完整路径、执行目录。编译参数 -lntdll
 * @param  pid      进程PID，自身进程为-1
 * @param  get_what 获取什么数据 1=cmd，2=exe文件完整路径，3=执行目录
 * @return          成功返回装换后字符串长度
char out[1024]={0};
char*outP=&out;
int n=mm_process_get_cmd2(pid,1,&outP);
printf("cmd2|%s|%d\n",out,n);
 */
typedef struct tag_PROCESS_BASIC_INFORMATION{
	int32_t ExitStatus;
	int32_t PebBaseAddress;
	int32_t AffinityMask;
	int32_t BasePriority;
	int32_t UniqueProcessId;
	int32_t InheritedFromUniqueProcessId;
}PROCESS_BASIC_INFORMATION_;
void __stdcall ZwQueryInformationProcess(HANDLE,int,PVOID*,int,int);
WINBOOL WINAPI ReadProcessMemory(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
int mm_process_get_cmd(int pid,int get_what_1cmd_2exe_3runpath,char*out){
	HANDLE p;
	char*buf;
	char*retP;
	int a,b,n,PebBaseAddress,id;
	PROCESS_BASIC_INFORMATION_ info;
	switch(get_what_1cmd_2exe_3runpath){
		case 1:
			id=0x44;
			break;
		case 2:
			id=0x3c;
			break;
		case 3:
			id=0x28;
			break;
		default :
			return -1;
	}
	if(pid==-1)pid=GetCurrentProcessId();
	p=OpenProcess(1040, 0, pid);
	if((int)p<1)return -2;
	ZwQueryInformationProcess(p, 0, (PVOID)&info, sizeof(info), 0);//24
	if(info.PebBaseAddress<1)return -3;
	PebBaseAddress=info.PebBaseAddress;
	//[[peb+10h]+28h]进程运行目录,[[peb+10h]+3ch]进程路径,[[peb+10h]+44h]命令行
	ReadProcessMemory(p,(LPCVOID)(PebBaseAddress+16),(LPVOID)&a,sizeof(a),&n);
	//NtReadVirtualMemory (p, info.PebBaseAddress+16, aP, sizeof(a), &n);
	if(n<4)return -4;
	ReadProcessMemory(p,(LPCVOID)(a+id),(LPVOID)&b,sizeof(b),&n);
	if(n<4)return -5;
	buf=malloc(2048);
	memset(buf,0,2048);
	//NtReadVirtualMemory (p, b, buf, 2048, &n);
	ReadProcessMemory (p, (LPCVOID)b, (LPVOID)buf, 2048, &n);
	n=UnicodeToAnsi_c(buf,out);
	free(buf);
	CloseHandle(p);
	return n;
}
/**
 * 遍历全部进程列表，编译时需要 -lkernel32
 * @param  do_function 处理每一条进程信息的函数指针，int do_f(char*name,int pid,int father_pid){...}，do_f返回1将停止遍历
 * @return             返回遍历过的进程数
int do_f(char*name,int pid,int father_pid){
	printf("do_f:\t%d\t%d\t|%s|\n",pid,father_pid,name);
	if(strcmp("notepad.exe",name)==0)return 1;
	return 0;
}
int main(int argc, char const *argv[]){
	mm_process_get_all_list(&do_f);
	return 0;
}
 */
typedef struct tagPROCESSENTRY32 {
	DWORD dwSize;
	DWORD cntUsage;//引用数
	DWORD th32ProcessID;//PID
	DWORD th32DefaultHeapID;//堆ID
	DWORD th32ModuleID;//进程模块标识符
	DWORD cntThreads;//线程数
	DWORD th32ParentProcessID;//父进程PID
	LONG pcPriClassBase;//线程优先权
	DWORD dwFlags;
	CHAR  szExeFile[260];//进程名
} PROCESSENTRY32;
HANDLE __stdcall CreateToolhelp32Snapshot(int,int);
HANDLE __stdcall Process32First(HANDLE,PROCESSENTRY32*);
HANDLE __stdcall Process32Next(HANDLE,PROCESSENTRY32*);
int mm_process_get_all_list(void*do_function){
	PROCESSENTRY32 proc;
	HANDLE hSnapshot;
	HANDLE hP;
	int timess=0;
	memset(&proc,0,sizeof(PROCESSENTRY32));
	proc.dwSize=sizeof(PROCESSENTRY32);
	hSnapshot=CreateToolhelp32Snapshot(15,0);
	if(hSnapshot<=0)return 0;
	timess=0;
	//回调处理函数
	int(*f)(char*,int,int);
	f=do_function;
	//开始遍历
	hP=Process32First(hSnapshot,&proc);
	while(hP>0){
		++timess;
		if(1==f(proc.szExeFile,proc.th32ProcessID,proc.th32ParentProcessID))break;
		hP=Process32Next(hSnapshot, &proc);
	}
	CloseHandle(hSnapshot);
	//printf("timess:%d\n",timess);
	return timess;
}
/**
 * 创建进程，返回新进程PID，编译时需要 -luser32
 * @param  name          全路径
 * @param  cmd           全路径、CMD
 * @param  wait_end      是否等待子进程运行完毕
 * @param  show_type     子进程窗口显示模式 1、#隐藏窗口； 2、#普通激活； 3、#最小化激活； 4、#最大化激活； 5、#普通不激活； 6、#最小化不激活
 * @param  run_path      运行目录
 * @param  hold_on_creat 创建后是否挂起，1=挂起，0=不挂起
 * @return               返回新进程>1的PID
int pid=mm_process_creat("C:\\Program Files\\Internet Explorer\\iexplore.exe","http://www.baidu.com/ --a --bbb",0,2,"G:\\Sublime Text 3",0);
printf("pid:%d\n",pid);
 */
DWORD __stdcall WaitForSingleObject(HANDLE,DWORD);
DWORD WINAPI WaitForInputIdle(HANDLE hProcess,DWORD dwMilliseconds);
int mm_process_creat(char*name,char*cmd,unsigned wait_end,unsigned show_type_1hide_2normal_3min_4max_5noactive_6minnoactive,char*run_path,unsigned hold_on_creat){
	STARTUPINFOA pStartupInfo;
	memset(&pStartupInfo,0,sizeof(STARTUPINFOA));
	PROCESS_INFORMATION pProcessInfo;
	switch(show_type_1hide_2normal_3min_4max_5noactive_6minnoactive){
		case 1:
			pStartupInfo.wShowWindow=0;
			break;
		case 3:
			pStartupInfo.wShowWindow=2;
			break;
		case 4:
			pStartupInfo.wShowWindow=3;
			break;
		case 5:
			pStartupInfo.wShowWindow=4;
			break;
		case 6:
			pStartupInfo.wShowWindow=6;
			break;
		default :
			pStartupInfo.wShowWindow=1;
			break;
	}
	pStartupInfo.dwFlags=1;
	//启动后是否挂起
	if(hold_on_creat!=0)hold_on_creat=4;
	//创建
	int hHandle=CreateProcessA(name, cmd, 0, 0, 0, hold_on_creat, 0, run_path, &pStartupInfo, &pProcessInfo);
	//等待执行完毕
	if(wait_end==1){
		WaitForSingleObject(pProcessInfo.hProcess, -1);
	}else{
		WaitForInputIdle(pProcessInfo.hProcess, 1000);
	}
	CloseHandle(pProcessInfo.dwThreadId);
	CloseHandle(pProcessInfo.dwProcessId);
	if(hHandle==0)return -1;
	return pProcessInfo.dwProcessId;
}
