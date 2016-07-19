#ifndef MM_PROCESS_WINDOWS_H
#define MM_PROCESS_WINDOWS_H

//windows进程相关函数
//额外编译参数 -lntdll -lpsapi -lAdvapi32 -lkernel32 -luser32

#ifdef _cplusplus
extern "C"{
#endif

//取cpu核数
int mm_get_cpu_cores();
//取自进程ID
int mm_process_get_pid_self();
//打开进程，完全访问权限，失败返回0
int mm_process_open_process(int pid);
//强力打开进程，失败返回0，编译时需要 -lntdll
int mm_process_open_process_x(int pid);
//取进程占用内存量KB，pid=-1取自进程，编译时需要 -lpsapi
int mm_process_get_process_memory_KB(int pid);
//取进程亲和cpu编号1~n，失败返回-1，返回0亲和全部核。pid=-1取自进程
int mm_process_get_cpu_kiss(int pid);
//置cpu亲和，失败返回-1，成功返回1。pid=-1取自进程
//cpu_id为0全部亲和、1~n亲和第n核
int mm_process_set_cpu_kiss(int pid,int cpu_id);
//提升当前进程权限，编译参数 -lntdll -lAdvapi32
void mm_process_power_up_self();
//压缩进程内存占用，成功返回1，失败返回-1
int mm_process_mem_free(int pid);
//取自进程cmd，失败返回负数，成功返回cmd长度
int mm_process_get_cmd_self(char*out);
/**
 * 通过进程PID取进程名
 * @param  pid 进程PID,-1为自身进程
 * @param  out 类型为char*，长度最小为256
 * @return     返回进程名字符串长度
 */
int mm_process_get_name_form_pid(int pid,char*out);
/**
 * 关闭进程
 * @param  pid 进程PID
 * @return     成功返回1，失败返回-1
 */
int mm_process_shutdown(int pid);
/**
 * 挂起进程，挂起n次恢复时需要恢复n次，编译参数 -lntdll
 * @return 返回0成功，失败返回负数
 */
int mm_process_stop(int PID);
/**
 * 恢复挂起的进程，挂起n次恢复时需要恢复n次，编译参数 -lntdll
 * @return 返回0成功，失败返回负数
 */
int mm_process_resume(int PID);
/**
 * Unicode转Ansi
 * @param  in  宽字符串
 * @param  out 输出，长度必须为2048
 * @return     成功返回装换后字符串长度
 */
int UnicodeToAnsi_c(char*in,char*out);
/**
 * 取进程 cmd、完整路径、执行目录。编译参数 -lntdll
 * @param  pid      进程PID，自身进程为-1
 * @param  get_what_1cmd_2exe_3runpath 获取什么数据 1=cmd，2=exe文件完整路径，3=执行目录
 * @return          成功返回装换后字符串长度
char out[1024]={0};
char*outP=&out;
int n=mm_process_get_cmd(pid,1,&outP);
printf("cmd2|%s|%d\n",out,n);
 */
int mm_process_get_cmd(int pid,int get_what_1cmd_2exe_3runpath,char*out);
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
int mm_process_get_all_list(void*do_function);
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
int mm_process_creat(char*name,char*cmd,unsigned wait_end,unsigned show_type_1hide_2normal_3min_4max_5noactive_6minnoactive,char*run_path,unsigned hold_on_creat);




#ifdef _cplusplus
}
#endif

#endif