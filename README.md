# QQEE_CLIB

***

### 跨平台C语言基础库，适配任意ANSI_C编译器

###### 字符串、文件、目录、时间、数组/队列、线程池、BASE64、CRC32、MD5、SHA1、URL编解码、码表实现UTF8/GBK编码转换

###### WINDOWS平台进程操作、窗口操作、注册表操作

[http://www.qq.ee](http://www.qq.ee/)

***

####1/3. 目录/文件说明.   

* 【./qqee_clib/】目录中包含 *.h 头文件.

* 【*_windows.h】文件为 windows 平台专用.

####2/3. 使用方法.    

* 1/3. 下载压缩包并解压. <a href="https://github.com/qqee/qqee_clib/archive/master.zip">[下载]</a>  

* 3/3. 项目源码中引用需要组件，如【#include "qqee_clib/base64.h"】.

####3/3. APIs list.


* base64.h

```lua
#include "./qqee_clib/base64.h"
//---------------
int base64encode(char*in,unsigned in_len,char*out)
int base64decode(char*in,unsigned in_len,char*out)
```


* crc32.h.   

```lua
#include "./qqee_clib/crc32.h"
//---------------
uint32_t crc32int(char*in,unsigned Len)
```


* md5.h.   

```lua
#include "./qqee_clib/md5.h"
//---------------
void md5(char*message,unsigned len,char*out)
```


* sha1.h.   

```lua
#include "./qqee_clib/crc32.h"
//---------------
void sha1(char*message,unsigned len,char*out)
```


* url.h.   

```lua
#include "./qqee_clib/url.h"
//---------------
void urlencode(char*in,char*out)
void urldecode(char*in,char*out)
```


* utf8_gbk_mem.h  【码表实现utf8与gbk编码转换】   

```lua
#include "./qqee_clib/utf8_gbk_mem.h"
//---------------
/*
	char s[] = "翻譯繁體發反反復複	。？ス|без зш|'ès|!！0123789zbcXYZ?{}";
	char strGbk[1024];
	char strUtf8[1024];
	memset(strGbk,0,sizeof(strGbk));
	memset(strUtf8,0,sizeof(strUtf8));
	//utf8转gbk
	utf82gbk_(s,strGbk);
	//gbk转utf8
	gbk2utf8_(strGbk,strUtf8);
*/
```


* utf8_gbk_windows.h  【windows平台utf8与gbk编码转换】   

```lua
#include "./qqee_clib/utf8_gbk_windows.h"
//---------------
/*
	char in[]="中文AbcdEFG?繁體";
	//char in[]="涓枃AbcdEFG?绻侀珨";
	char  out[2048]={0};
	char out2[2048]={0};
	//utf8转gbk
	int n=utf8_to_gbk(in,out,2048);
	//gbk转utf8
	n=gbk_to_utf8(out,out2,2048);
*/
```


* mm_array.v2.h  【双向循环链表实现动态数组/队列】   

```lua
#include "./qqee_clib/mm_array.v2.h"
//---------------
/*
int lock;
typedef struct tag_gear_list_obj{
	struct mm_array_head list;
	uint8_t active_type;
	uint8_t socket_obj_arr_id;
}gear_list_obj;
//初始化头部
struct mm_array_head head;
MM_ARRAY_INIT(&head);
//添加
	gear_list_obj*tmp=(gear_list_obj*)malloc(sizeof(gear_list_obj));
	tmp->active_type=(15-i);
	tmp->socket_obj_arr_id=((15-i));
	LOCK_(lock)
	MM_ARRAY_PUSH(&(tmp->list),&head);
	UNLOCK_(lock)
//取出
	LOCK_(lock)
	gear_list_obj*tmps=(gear_list_obj*)(head.next);
	MM_ARRAY_DEL(head.next);
	UNLOCK_(lock)
	printf("%d\t%d\n",70-i,tmps->socket_obj_arr_id );//50
	free(tmps);
	if(MM_ARRAY_IS_EMPTY(&head))break;
*/
```


* mm_threadpool_chain.h  【基于 mm_array.v2.h 构建的线程池】   

```lua
#include "./qqee_clib/mm_threadpool_chain.h"
//---------------
/*
//初始化线程池
mm_threadpool*threadpool=mm_threadpool_init(50,10000);

//需要线程池中执行的函数
int test(int n){
printf("%d\n",n);
return 0;
}

//添加任务
int n=mm_threadpool_add_job((mm_threadpool*)threadpool,f,(uint64_t)times);

//等待线程池执行完毕
while(mm_threadpool_get_jobs_n(threadpool)!=0 || mm_threadpool_get_running_n(threadpool)!=0)SLEEP_MS__(100);
*/
```


* lib.h.   

```lua
#include "./qqee_clib/lib.h"
//------------------------------------------------------------
//宏

SLEEP_MS(x)
DLL_EXPORT    //WIN平台DLL导出标记
SWAP_8(x)     //0x31 --> 0x13
SWAP_16(x)    //0x1122 --> 0x2211
SWAP_32(x)    //0x11223344 --> 0x44332211
SWAP_32_1(x)  //0x11223344 --> 0x33441122
ROUND(x)      //四舍五入
//------------------------------------------------------------
//字符处理

int char_is_num(char*s)
                        //判断字符串是否为整数，返回1全部为整数，否则返回0
uint64_t mm_16to10(char*_16)
                        //16进制转10进制
uint8_t mm_10to16(uint64_t value,char*out)
                        //10进制转16进制，返回字符串长度
uint8_t itoa_32(int32_t value, char*out,int not_write_end_0)
                        //int32整数转字符串，返回字符串长度
uint8_t itoa_u32(uint32_t value, char*out,int not_write_end_0)
                        //uint32整数转字符串，返回字符串长度
uint8_t itoa_64(int64_t value, char*out,int not_write_end_0)
                        //int64整数转字符串，返回字符串长度
uint8_t itoa_u64(uint64_t value, char*out,int not_write_end_0)
                        //uint64整数转字符串，返回字符串长度
int32_t mm_rand(int32_t min,int32_t max)
                        //取随机数
void mm_bin2hex(char*in,int64_t len,char*out)
                        //字节集查看，查看字节长度len，输出如: 5ab12e0c3f
void mm_hex2bin(char*in,char*out)
                        //十六进制字符串转字节集，输入如: 5ab12e0c3f 或 5AB12E0C3F
int mm_get_cpu_cores()
                        //取cpu核数
uint32_t mm_ip_ipv4_string_to_uint32(char*ip,uint8_t little_end)
                        //IPv4字符转长整数，失败返回0
                        //little_end=1返回小端序(与ip同方向)，0返回大端字节序
unsigned mm_ip_uint32_to_ipv4_string(uint32_t ipv4,char*out)
                        //uint32整数转ip字符串
                        //70619515 ----> 123.145.53.4  //正向字节序

int mm_string_strstr(char*big,char*key,int where_start)
                        //从字符串 big 中正向寻找字符串 key 的位置
                        //起始位置 where_start 首字符=0，未找到返回-1
char*mm_string_strstr_P(char*big,char*key,int where_start,int out_without_key)
                        //从字符串 big 中正向寻找字符串 key
                        //起始位置 where_start 首字符=0
                        //返回结果是否排除关键字with_key=1排除/=0包含
                        //返回找到字符串到字符的指针，未找到返回0
int mm_string_strrstr(char*big,char*key,int back_type_0where_1p)
                        //从字符串 big 中逆向寻找字符串 key，未找到返回-1
                        //back_type_0where_1p=0返回偏移,否则返回指针
int mm_string_trim(char*str,char*out,int with_0)
                        //删首尾空格，返回剩余字符串长度。with_0=1时结尾补'\0'
void mm_string_trimall(char*str,char*out,int with_0)
                        //删全部多种空白符。with_0=1时结尾补'\0'
void mm_string_space2one(char*s_in,char*s_out,int with_0)
                        //将>2个连续的空白符替换为1个空格。with_0=1时结尾补'\0'
unsigned mm_string_replace(char*big,char*key,char*new_,char*out,int times,int with_0)
                        //字符串替换，被替换关键字 key 长度和 替换为关键字 new_
                        //替换次数 times=0 时替换全部
                        //with_0=1时结尾补'\0'
                        //返回成功替换的次数
int mm_string_cut_by_keyword_to_end(const char*s_in,char*key,char*s_out,int with_key)
                        //从 s_in 中截取 key 至结尾的字符
                        //失败返回负数，成功返回s_out字节数
                        //with_key=1时将保留key，否则不包含key
int mm_string_cut_by_keyword_form_start(const char*s_in,char*key,char*s_out,int with_key,int with_0)
                        //从字符串头部开始截取字符串，遇关键字后停止
                        //失败返回负数，成功返回s_out字节数
                        //with_0=1时结尾补'\0'
                        //with_key=1时将保留key，否则不包含key
int mm_string_cut_by_where_start_and_end(const char*s_in,int start,int end,char*s_out,int with_0)
                        //根据开始结束位置截取字符,首位置为0
                        //返回截取出的字符长度。with_0=1时结尾补'\0'
int mm_string_cut_by_keyword_start_and_end(const char*s_in,char*key_start,char*key_end,int types,char*s_out,int with_key_start,int with_key_end,int with_0)
                        //根据 开始/结束 关键字截取字符
                        //types，就近/远匹配(-1=最远匹配，0=最近匹配)
                        //with_key_start 值为1时输出中包含开始关键字
                        //with_key_end   值为1时输出中包含结尾关键字
                        //with_0         值为1时输出结尾将补'\0'
                        //失败返回负数，成功返回截取字符长度
void mm_string_upper(char*in,char*out)
                        //字符串转大写
void mm_string_lower(char*in,char*out)
                        //字符串转小写
uint32_t mm_string_explode(char*in,char*key,int(*function)(char*,int,int,uint64_t,uint64_t,uint64_t,uint64_t),uint64_t var1,uint64_t var2,uint64_t var3,uint64_t var4)
                        //根据 key 关键字分割文本
                        //funciton 回调处理函数指针，int do_f(char*s,int len,int times){...}
                        //回调函数 do_f 返回1将停止遍历
                        //uint64_t var1~var4  传递给 funciton 的参数
                        //返回遍历的次数
/*
int do_f(char*s,int len,int times,uint64_t var1,uint64_t var2,uint64_t var3,uint64_t var4){
	char str[1024]={0};
	memcpy(str,s,len);
	printf("%d:[%s]\n",times,str);
	return 0;
}
int main(){
	char s[]="a|+bb|+ccc|+dddd|+|+|+z";
	int n=mm_string_explode(s,"|+",&do_f,0,0,0,0);
	return 0;
}
*/
//------------------------------------------------------------
//文件处理

int mm_file_exists_file(char*file_name,int test_power)
                        //文件是否存在，1=存在，0=不存在/无权限
                        //文件全路径、权限检查(0=只检查是否存在，1=执行权限，2=写权限，4=读权限，6=读写权限)
int mm_file_exists_dir(char*dir)
                        //目录是否存在，1=存在，0=不存在/无权限
FILE* mm_file_open(char*path,char*type)
                        //打开文件，失败返回0
/*
打开模式：
r 只读
r+ 读写
rb+ 二进制读写
w 只写并清空旧内容，不存在则新建
w+ 可读写并清空旧内容，不存在则新建
a 追加只写，不存在则新建
a+ 追加可读写，不存在则新建
wb 只写二进制
wb+ 读写二进制
ab+ 二进制追加
*/
FILE*mm_file_open_tmpfile()
                        //取临时文件名，返回文件句柄
                        //临时文件在程序退出时自动删除。
long mm_file_size(FILE *stream)
                        //取文件尺寸字节数
long mm_file_read(FILE *stream,char*buf,long read_len)
                        //读入数据，返回实际读入字节数，不成功或读到文件末尾返回 0
long mm_file_write(FILE *stream,char*msg)
                        //写入数据，返回实际写入的数据块数目
int mm_file_close(FILE *stream)
                        //关闭文件，成功则返回 0
int mm_file_rename(char*name_old,char*name_new)
                        //文件更名重命名，成功则返回0，失败返回-1
int mm_file_delete_unlink(char*filename)
                        //删除文件
int mm_file_delete_remove(char*filename)
                        //删除文件
long mm_file_getfilesize(char*filename)
                        //仅取文件尺寸，返回-1失败
long mm_file_readall(char*filename,char**out,long out_size,char*type_rb_r)
                        //读入文件全部内容，失败返回-1，成功返回文件字节数
                        //out_size为char*tmp第一次申请的尺寸
/*
char*tmp=malloc(1024);
char*out=&tmp;
mm_file_readall("Z:\\editplus\\x86\\CSS2.CTL",out,1024,"rb");
printf("%s\n",tmp);
free(tmp);
*/
long mm_file_writeall(char*filename,char*writedata,uint32_t len,int types)
                        //写到文件，失败返回-1，成功返回写入字节数
                        //文件名、写入数据、写入模式(0=追加写入,1=替换写入)
int mm_file_chmod(char*path,int power)
                        //修改文件权限，改变成功返回0，否则返回-1
                        //常用权限: 全最高777、全读写666、全只读444、所有者最高700、所有者读写600、所有者读400
                        //windows下设置为 0 则只读
int mm_file_list_all(char*dir,int get_type_0all_1file_2dir,int include_subpath,int(*function)(char*path_file_name,int power,int time_create,int time_access,int time_write,int size_Byte))
                        //遍历目录下全部文件/子文件夹并回调
                        //function 回调函数指针，返回1将停止遍历，目录的大小恒为-1
                        //根据第2参返回遍历过的文件/目录数，空目录返回0
//------------------------------------------------------------
//目录处理

int mm_mkdir(char*name)
                        //创建目录，不存在的父目录会自动创建
                        //失败返回-1，成功返回1，目录名尾部可选有斜杠
char*mm_get_path_work(char*out,int buf_size)
                        //取当前工作目录绝对路径，最后无反斜杠符：Z:\github\pathname
                        //工作目录不等于程序所在目录，是调用该程序时的工作目录
void mm_get_path_exe_all(char*out_path)
                        //取当前执行文件所在目录的绝对路径包含执行文件名
                        //Z:\github\pathname\test.exe 或 /root/test/filename
                        //out_path缓冲区最少256字节
int mm_get_path_all2dir(char*in_path,char*out_dir)
                        //完整路径取目录，最后无反斜杠符：Z:\github\pathname
                        //返回 out_dir 长度，失败返回0
int mm_get_path_exe_dir(char*out,int buf_size)
                        //取执行文件所在目录，最后无反斜杠符：Z:\github\pathname
int mm_get_path_filename(char*in_path,char*out_filename)
                        //完整路径取文件名: tcc.exe
                        //返回 out_dir 长度，失败返回0

//------------------------------------------------------------
//时间处理

int mm_time_get_ms()
                        //CPU时钟非UTC，用于测试效率，如 28922437
uint32_t mm_time_get_utc(int timezone)
                        //获取utc标准时间戳如 1424809727
void mm_time_get_formatstring(time_t in_utc_time,uint32_t in_time_zone,char*back_format_string,char*format_ruler){
                        //取时间的格式化字符串，会使用本机时区设置
                        //mm_time_get_formatstring(0,8,out,"%Y%m%d %H:%M:%S")

```










***
