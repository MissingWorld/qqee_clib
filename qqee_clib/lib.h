#ifndef __Q_QE_E_LIB_H
#define __Q_QE_E_LIB_H
#ifdef _cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <dirent.h>

#ifdef _WIN32
	#include <windows.h>
	#include <io.h>
	#define DLL_EXPORT __declspec(dllexport)
	#define MKDIR(x) mkdir((x))
	#define SLEEP_MS(x) Sleep(x)
#else
	#include <unistd.h> //readlink
	#include <sys/time.h>
	#include <sys/io.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#define DLL_EXPORT   
	#define MKDIR(x) mkdir((x),S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH)
	#define SLEEP_MS(x) usleep(x*1000)
#endif

#ifndef _finddata32_t
typedef struct _finddata32_t_{
    uint32_t  attrib;
    uint32_t  time_create;    /* -1 for FAT file systems */
    uint32_t  time_access;    /* -1 for FAT file systems */
    uint32_t  time_write;
    uint32_t  size;
    char      name[260];
} _finddata32_t_;
#endif

//空白字符判断
#define IS_SPACE(ch) (ch==' ' || ch=='\n' || ch=='\t' || ch=='\r' || ch=='\b' || ch=='\f')

//反转高低位
#define SWAP_8(x) (((x)>>4)|(((x)&0x0f)<<4))                                    //0x31 --> 0x13
#define SWAP_16(x) (((x)>>8)|(((x)&0x00ff)<<8))                                 //0x1122 --> 0x2211
#define SWAP_32_1(x) (((x)>>16)|(((x)&0x0000ffff)<<16))                         //0x11223344 --> 0x33441122
//#define SWAP_32_2(x) SWAP_32_1((((x)&0xff00ffff)>>8)|(((x)&0xffff00ff)<<8))     //0x11223344 --> 0x44332211
#define SWAP_32_2(x) (((x)>>24)|(((x)&0x00ff0000)>>8)|(((x)&0x0000ff00)<<8)|(((x)&0x000000ff)<<24))     //0x11223344 --> 0x44332211
#define SWAP_32 SWAP_32_2

#define ROUND(x) ((int)((x)+0.5))    //四舍五入
#define RUN(x) system(x)           //执行、运行。会阻塞当前线程
//-------------------------------------------------------------------------------BASE
//执行shell/cmd指令，传回结果字符，返回结果字节数，返回负数则失败
int cmd_(char*cmd,char*back,int back_len){
    FILE*f=popen(cmd,"r");
    if(f==0)return -1;
    int n=fread(back,1,back_len,f);//读入数据 //fgets(back,back_len,f)
    pclose(f);
    return n;
}
//判断字符串是否为整数，返回1全部为整数，否则返回0
int char_is_num(char*s){
	int len=0;
	if(s==0||(len=strlen(s))<=0)return -1;
	while(len--)if(isdigit(*(s++))==0)return 0;
	
	return 1;
}
inline uint64_t _pow(int x,int y){
	uint64_t r=1;
	while(y--)r*=x;
	return r;
}
//16进制转10进制
uint64_t mm_16to10(char*_16){//qps>1400w
	//printf(">%s\n",_10to16(1240568749));
	//10进制值<<从右0至左的位置编号*4
	//printf(">>>>>%d\n",(11<<12));
	//char*_16="33";
	int slen=strlen(_16);
	int i=slen;
	//printf("i:%d\n",i);
	uint64_t hex=0;
	//printf("size: %d\n",sizeof(hex));
	//uint64_t ul=strtoul(_16,0,16);
	//printf ("strtoul: %lu\n", ul);
	while(i--){
		register uint8_t Z=_16[i];
		register uint8_t N=Z;
		//printf("%d -----> %c\n",i,Z);
		//printf("%ld    %ld    %ld\n",i,(slen-i-1),(slen-i-1)<<2);
		if(Z>='A'&&Z<='F'){
			//pow (7.0, 3.0)==7 ^ 3
			//hex+=((Z-55)<<((slen-i-1)<<2));
			//hex+=(Z-55)*_pow(16,(slen-i-1));
			N=55;
		}else if(Z>='a'&&Z<='f'){
			//hex+=((Z-87)<<((slen-i-1)<<2));
			//hex+=(Z-87)*_pow(16,(slen-i-1));
			N=87;
		}else if(Z>='0'&&Z<='9'){
			//hex+=((Z-48)<<((slen-i-1)<<2));
			//hex+=(Z-48)*_pow(16,(slen-i-1));
			N=48;
		}
		hex+=(Z-N)*_pow(16,(slen-i-1));
		//printf(">>> %I64d     %u\n",hex,_pow(16,(slen-i-1)));
		//printf(">>> %llu\n",hex);
	}
	//printf("-----------\n");
	return hex;
}
//10进制转16进制，返回字符串长度
uint8_t mm_10to16(uint64_t value,char*out){
	static const char table[]="0123456789abcdef";
	char tmp[12]={0};
	char*P=(char*)&tmp;
	char*P_save=P;
	P+=10;
	do{
		*(P--)=table[value&15];//%16
		if(P<P_save)break;
	}while((value=(value>>4))!=0);//value/=16
	++P;
	strcpy(out,P);//memcpy(out,P,(P_save+9-P+1));
	return strlen(tmp);
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------核心函数
//返回字符串长度
//-12345				速度 13361464
//0x7FFFFFFF			速度 6897438
//int32整数转字符串
uint8_t itoa_32(int32_t value, char*out,int not_write_end_0){
    char tmp[21];
    char*P_save=19+(char*)&tmp;
    char*P=P_save;
    uint8_t is_=0;
	if(value==0){
		*out='0';
		if(not_write_end_0!=1)*(out+1)=0;
		return 1;
	}else if(value<0){
		value=-value;
		is_=1;
	}
    while(value){
        *(P--)='0'+(value%10);
        value/=10;
    }
    uint8_t n=(P_save-P);
    if(is_){
		++n;
		*(P--)='-';
    }
    //printf("[%s]\n",P);
    memcpy(out,P+1,n);
    if(not_write_end_0!=1)*(out+n)=0;
    return(n);
}
//返回字符串长度
//12345					速度 14339391
//0x7FFFFFFF			速度 7075827
//0xfffffffe			速度 7006495
uint8_t itoa_u32(uint32_t value, char*out,int not_write_end_0){
    char tmp[21];
    char*P_save=19+(char*)&tmp;
    char*P=P_save;
    uint8_t is_=0;
	if(value==0){
		*out='0';
		if(not_write_end_0!=1)*(out+1)=0;
		return 1;
	}else if(value<0){
		value=-value;
		is_=1;
	}
    while(value){
        *(P--)='0'+(value%10);
        value/=10;
    }
    uint8_t n=(P_save-P);
    if(is_){
		++n;
		*(P--)='-';
    }
    //printf("[%s]\n",P);
    memcpy(out,P+1,n);
    if(not_write_end_0!=1)*(out+n)=0;
    return(n);
}
//返回字符串长度
//-12345				速度 3101203
//0xfffffffe			速度 1586532
//0x7FFFFFFFFFFFFFFF	速度 743504
uint8_t itoa_64(int64_t value, char*out,int not_write_end_0){
    char tmp[21];
    char*P_save=19+(char*)&tmp;
    char*P=P_save;
    uint8_t is_=0;
	if(value==0){
		*out='0';
		if(not_write_end_0!=1)*(out+1)=0;
		return 1;
	}else if(value<0){
		value=-value;
		is_=1;
	}
    while(value){
        *(P--)='0'+(value%10);
        value/=10;
    }
    uint8_t n=(P_save-P);
    if(is_){
		++n;
		*(P--)='-';
    }
    //printf("[%s]\n",P);
    memcpy(out,P+1,n);
    if(not_write_end_0!=1)*(out+n)=0;
    return(n);
}
//返回字符串长度
//12345					速度 3294034
//0xfffffffe			速度 1652074
//0x7FFFFFFFFFFFFFFF	速度 789224
//0xfffffffffffffffe	速度 762143
uint8_t itoa_u64(uint64_t value, char*out,int not_write_end_0){
    char tmp[21];
    char*P_save=19+(char*)&tmp;
    char*P=P_save;
	if(value==0){
		*out='0';
		if(not_write_end_0!=1)*(out+1)=0;
		return 1;
	}
    while(value){
        *(P--)='0'+(value%10);
        value/=10;
    }
    uint8_t n=(P_save-P);
    //printf("[%s]\n",P);
    memcpy(out,P+1,n);
    if(not_write_end_0!=1)*(out+n)=0;
    return(n);
}
/*
//uint32_t整数转10进制文本 0~2147483647，not_write_end_0=1时不自动补全结尾'\0'
//返回字符串长度
uint8_t mm_itoa(uint64_t value, char*out,int not_write_end_0){
    char tmp[21];
    char*P_save=19+(char*)&tmp;
    char*P=P_save;
	if(value==0){
		*out='0';
		if(not_write_end_0!=1)*(out+1)=0;
		return 1;
	}
    while(value){
        *(P--)='0'+(value%10);
        value/=10;
    }
    register uint8_t n=(P_save-P);
    memcpy(out,P+1,n);
    if(not_write_end_0!=1)*(out+n)=0;
    return(n);
}
*/
//----------------------------------------------------------------------------------------------------------------------------------------------------------核心函数
//取随机数
int32_t mm_rand(int32_t min,int32_t max){
	static int32_t flag;
	if(flag==0){
		//srand((int32_t)time(0));
		srand(mm_time_get_ms());
		flag=1;
	}
	return min+(int32_t)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}
//字节集查看，查看字节长度len，输出如: 5ab12e0c3f
//char*out长度必须大于2被char*in长度
void mm_bin2hex(char*in,int64_t len,char*out){
	static const char table[]="0123456789abcdef";
	int64_t value,i=0;
	for(;i<len;in++){
		if(len>0 && ++i>len)break;
		//printf("%c\t%d\n",(*in),(*in));
		value=( (*in) & 255 );
		*(++out)=table[value&15];//value%16
		value=(value>>4);//value/=16;
		*(--out)=table[value&15];
		out+=2;
	}
	*out=0;
}
//十六进制字符串转字节集，输入如: 5ab12e0c3f 或 5AB12E0C3F
void mm_hex2bin(char*in,char*out){
	//unsigned len_in=strlen(in);
	uint32_t value,value2;
	//printf("%s\n",in);
	for(;*in;in++){
		value = *(in++);
		//printf("%c %d ",value,value);
		if(value>='a' && value<='f'){
			value-=87;
		}else if(value>='A' && value<='F'){
			value-=55;
		}else{
			value-=48;
		}
		value2 = *(in);
		//printf("%c %d ",value2,value2);
		if(value2>='a' && value2<='f'){
			value2-=87;
		}else if(value2>='A' && value2<='F'){
			value2-=55;
		}else{
			value2-=48;
		}
		*(out++) = (value*16)+value2;
		//printf("%x\t%x\t%s\n",value,value2,out);
	}
	*out=0;
}
//取cpu核数
int mm_get_cpu_cores(){
#ifdef _WIN32
	return atoi(getenv("NUMBER_OF_PROCESSORS"));
	/*VOID WINAPI GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);
	SYSTEM_INFO Info;
	GetSystemInfo(&Info);
	return Info.dwNumberOfProcessors;*/
#else
	return get_nprocs();
#endif
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------核心函数
//IPv4字符转长整数， little_end==1否返回小端字节序(与ip同方向)，为0返回大端字节序，失败返回0
//mm_ip_ipv4_string_to_uint32("192.168.0.1",0);//16820416	    >>>  0100A8C0
//mm_ip_ipv4_string_to_uint32("192.168.0.1",1);//3232235521	    >>>  C0A80001
uint32_t mm_ip_ipv4_string_to_uint32(char*ip,uint8_t little_end){
	uint32_t tmp;
	unsigned char*p=(unsigned char*)&tmp;
	char*ip_save=ip;
	ip+=strlen(ip);//到尾
	int i=3;
	for(;ip>ip_save;ip--){
		if(*ip=='.'){
			*ip=0;
			*(p+i)=atoi(ip+1);
			*ip='.';
			--i;
		}
	}
	if(i!=0)return 0;
	*p=atoi(ip);
	return (little_end==1)?(SWAP_32(tmp)):tmp;
}
//70619515 ----> 123.145.53.4  //正向字节序
//返回out字符串的长度，预留  char*out[16]={0};mm_ip_uint32_to_ipv4_string("192.168.0.1",out);
unsigned mm_ip_uint32_to_ipv4_string(uint32_t ipv4,char*out){
    unsigned char*p=(unsigned char*)&ipv4;
    char*out_save=out;
    int n=itoa_u64(*p,out,1);
    out+=itoa_u64(*p,out,1);
    *(out++)='.';
    out+=itoa_u64(*(p+1),out,1);
    *(out++)='.';
    out+=itoa_u64(*(p+2),out,1);
    *(out++)='.';
    out+=itoa_u64(*(p+3),out,0);
	*out=0;
    return(out-out_save);
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------核心函数
//linux平台可获取微秒级时钟，是CPU时钟而非UTC时钟，用于测试效率，28922437
//测试机空转效率 >= 2000万
//int __stdcall GetTickCount(VOID);
int mm_time_get_ms(){
#ifdef _WIN32
	return GetTickCount();
#else
	//static struct timeval tv;
	//gettimeofday(&tv,0);
	//return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
	struct timespec ts;
	clock_gettime(1,&ts);//CLOCK_REALTIME=0=系统UTC时钟，CLOCK_MONOTONIC=1
	return ts.tv_sec;
#endif
}
//获取utc标准时间戳如 1424809727
//timezone=0返回utc时间戳，参数为时区-12~12时返回偏移后的时间戳，即该时区的当前时间戳
//:time_get_utc(0)
//:time_get_utc(8)
uint32_t mm_time_get_utc(int timezone){
	if(timezone==0)return time(0);
	uint32_t utc_time=time(0);
	utc_time+=(timezone*3600);
	return utc_time;
}
//取时间的格式化字符串，会使用本机时区设置
//mm_time_get_formatstring(0,8,out,"%Y%m%d %H:%M:%S")
//效率350万
void mm_time_get_formatstring(time_t in_utc_time,uint32_t in_time_zone,char*back_format_string,char*format_ruler){
	if(in_utc_time==0)in_utc_time=time(0);
	if(in_time_zone!=0)in_utc_time+=(in_time_zone*3600);
	//strftime(out,64,"%Y%m%d %H:%M:%S",localtime(&in_utc_time));//20150220 18:58:41  350万
	//strftime(out,64,"%Y%m%d%H%M%S",localtime(&in_utc_time));//20150220 18:58:41  400万
	strftime(back_format_string,64,format_ruler,localtime(&in_utc_time));
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------核心函数
//创建目录，不存在的父目录会自动创建。失败返回-1，成功返回1，目录名尾部可选有斜杠
//目录名中有中文需要注意编码。win下只能使用gbk，默认编码格式由本文件编码格式决定
//在win下用 /root/a/b/c 这样的目录名进行创建，将在当前分区根目录开始创建root文件夹
//目录名 ./root/a 则在可执行程序所在目录进行创建root文件夹
int mm_mkdir(char*name){
	char*nameP=name;
	int n=0,i=1;
	for(;*nameP!=0;){
		if((*nameP=='\\' || *nameP=='/') && n>0){
			*nameP=0;
			i=MKDIR(name);
			*nameP='/';
		}
		++n;
		++nameP;
	}
	if(*(--nameP)!='/'){
		i=MKDIR(name);
	}
	return ((i==0)?1:-1);
}
//取当前工作目录绝对路径，最后无反斜杠符：Z:\github\pathname
//输入缓冲区、缓冲区大小
//工作目录不等于程序所在目录，是调用该程序时的工作目录
char*mm_get_path_work(char*out,int buf_size){
	return (char*)getcwd(out,buf_size);
}
//取当前执行文件所在目录的绝对路径包含执行文件名：Z:\github\pathname\test.exe 或 /root/test/filename
//out_path空间最少256字节
void mm_get_path_exe_all(char*out_path){
#ifdef _WIN32
	GetModuleFileName(0,out_path,255);
#else
	int n=readlink("/proc/self/exe",out_path,255);
	//printf("<%d>\n",n);
	*(out_path+((n<1)?0:n))=0;
#endif
}
//完整路径取目录，最后无反斜杠符：Z:\github\pathname
//返回 out_dir 长度，失败返回0
int mm_get_path_all2dir(char*in_path,char*out_dir){
	char*p;
	p=(char*)strrchr(in_path,'\\');
	if(p==0)p=(char*)strrchr(in_path,'/');
	if(p==0)return 0;
	unsigned i=p-in_path;
	memcpy(out_dir,in_path,i);
	*(out_dir+i)=0;
	return i;
}
//取执行文件所在目录，最后无反斜杠符：Z:\github\pathname
int mm_get_path_exe_dir(char*out,int buf_size){
	char outs[255]={0};
	mm_get_path_exe_all(outs);
	return mm_get_path_all2dir(outs,out);
}
//完整路径取文件名: tcc.exe
//返回 out_dir 长度，失败返回0
int mm_get_path_filename(char*in_path,char*out_filename){
	char*p;
	p=(char*)strrchr(in_path,'\\');
	if(p==0)p=(char*)strrchr(in_path,'/');
	if(p==0)return 0;
	unsigned i=(strlen(in_path))-(p-in_path);
	memcpy(out_filename,++p,i);
	*(out_filename+i)=0;
	return i;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------核心函数
//从字符串 big 中正向寻找字符串 key 的位置，起始位置 where_start 首字符=0，未找到返回-1
//64KB字符串效率 >= 14万
//短字符串效率   >= 1200万
//测试机空转效率 >= 2000万
int mm_string_strstr(char*big,char*key,int where_start){
    char*p=big;
    unsigned len=strlen(key);
    for(;(p=(char*)strchr(p,(char)*key))!=0;p++){
			if((p-big)<where_start)continue;
			if(strncmp(p,key,len)==0)return (p-big);//return (char*)p;
    }
    return -1;
}
//从字符串 big 中正向寻找字符串 key，起始位置 where_start 首字符=0，返回结果是否排除关键字with_key=1排除/=0包含
//返回找到字符串到字符的指针，未找到返回0
char*mm_string_strstr_P(char*big,char*key,int where_start,int out_without_key){
	char*p=big;
	unsigned int len=strlen(key);
	for(;(p=(char*)strchr(p,*key))!=0;p++){
		if((p-big)<where_start)continue;
		if(strncmp(p,key,len)==0)return(out_without_key==0)?p:(p+len);//return (p-big);
	}
	return 0;
}
//从字符串 big 中逆向寻找字符串 key 。 back_type_0where_1p ==0返回偏移,否则返回指针 。未找到返回-1
//key首字母为空格会严重降低效率，200w降至60w
int mm_string_strrstr(char*big,char*key,int back_type_0where_1p){
	char*p=big;
	char*last0=0;
	uint32_t len=strlen(key);
	while((int)(p=strrchr(p,*key))!=-1){
		if(last0>0)*last0=*key;
		if(strncmp(p,key,len)==0){
			return (back_type_0where_1p==0)?(p-big):(p);
		}
		last0=p;
		*p=0;
		p=big;
	}
	return -1;
}
//删首尾空格，返回剩余字符串长度。with_0=1时结尾补'\0'
//32KB字符串效率 >= 1600万
//测试机空转效率 >= 2000万
int mm_string_trim(char*str,char*out,int with_0){
	if(str<1)return 0;
	char*str1=str;
	int64_t len=strlen(str);
	if(len<1)return 0;
	--len;
	char*str_end=str+len;
	//正向
	while(*str1 && IS_SPACE(*str1)) str1++;
	//逆向
	while(str_end>str1 && IS_SPACE(*str_end)) str_end--;
	len=str_end-str1;
	if(len<0)return 0;
	++len;
	memcpy(out,str1,len);
	return len;
}
/*
删全部多种空白符。with_0=1时结尾补'\0'。
若只删除空格或\t或\n单字符建议使用 mm_string_replace
str_replace2(buf," ","",buf2,0);
str_replace2(buf2,"\t","",buf2,0);
str_replace2(buf2,"\n","",buf2,0);
str_replace2(buf2,"\r","",buf2,0);
str_replace2(buf2,"\b","",buf2,0);
str_replace2(buf2,"\f","",buf2,0);
str_replace2(buf2,"　","",buf2,0);
*/
void mm_string_trimall(char*str,char*out,int with_0){
	char*p=str;
	//长字符效率低
	for(;*p;p++){
		if(!IS_SPACE(*p))
			*(out++)=*p;
	}
	if(with_0==1)*out=0;
}
//将>2个连续的空白符替换为1个空格。with_0=1时结尾补'\0'
void mm_string_space2one(char*s_in,char*s_out,int with_0){
	char*p=s_in;
	int i=0;
	for(;*p!=0;p++){
		if(IS_SPACE(*p)){
			if((++i)==1)*(s_out++)=' ';
		}else{
			*(s_out++)=*p;
			i=0;
		}
	}
	if(with_0==1)*s_out=0;
}
//字符串替换，被替换关键字 key 长度和 替换为关键字 new_ ，需要替换次数 times=0 时替换全部。with_0=1时结尾补'\0'
//返回成功替换的次数
//形参 char*out 需要通过 big 尺寸预估算空间尺寸
//64KB字符串:\n替换为空 >=1200万 
//64KB字符串:空格替换为空 >=1300万
//64KB字符串:A替换为空 >=2000
//64KB字符串:AA替换为空 >=2000
//64KB字符串:AAA替换为空 >=2000
//64KB字符串:.替换为| >=1100万
//64KB字符串:两个空格替换为空 >=32万
//测试机空转效率 >= 2000万
unsigned mm_string_replace(char*big,char*key,char*new_,char*out,int times,int with_0){
	register unsigned times_local=1;
	register char*s=big;
	register char*out_p=out;
	register unsigned len=0;
	unsigned len_new=strlen(new_);
	char*buf=strstr(s,key);
	while( buf != 0 ){
		*buf='\0';
		len=strlen(s);
		memcpy(out_p,s,len);
		out_p+=len;
		//写入分割字符串
		memcpy(out_p,new_,len_new);
		out_p+=len_new;
		//写入替换后字符串
		++times_local;
		s=buf+strlen(key);
		if(times!=0 && times_local>times)break;
		//printf( "%d:|%s|\n",times_local, s);
		//printf("s:|%s|\n",s);
		buf=strstr(s,key);
	}
	if((len=strlen(s))>0){
		//printf("last:%s\n",s);
		memcpy(out_p,s,len);
		out_p+=len;
	}
	if(with_0==1)*out_p='\0';
	return --times_local;
}
//从 s_in 中截取 key 至结尾的字符。失败返回负数，成功返回s_out字节数
//with_key=1时将保留key，否则不包含key
int mm_string_cut_by_keyword_to_end(const char*s_in,char*key,char*s_out,int with_key){
	char*p=(char*)strstr(s_in,key);
	if(p==0)return -1;
	int s_in_len=strlen(s_in);
	int key_len=strlen(key);
	if(1!=with_key){
		p+=key_len;
	}
	strcpy(s_out,p);
	return (s_in_len-(p-s_in));
}
//从字符串头部开始截取字符串，遇关键字后停止。失败返回负数，成功返回s_out字节数。with_0=1时结尾补'\0'
//with_key=1时将保留key，否则不包含key
int mm_string_cut_by_keyword_form_start(const char*s_in,char*key,char*s_out,int with_key,int with_0){
	char*p=(char*)strstr(s_in,key);
	if(p==0)return -1;
	int n;
	if(1!=with_key){
		n=(p-s_in);
	}else{
		n=((p-s_in)+strlen(key));
	}
	memcpy(s_out,s_in,n);
	if(with_0==1)*(s_out+n)=0;
	return n;
}
//根据开始结束位置截取字符,首位置为0。返回截取出的字符长度。with_0=1时结尾补'\0'
int mm_string_cut_by_where_start_and_end(const char*s_in,int start,int end,char*s_out,int with_0){
	int n=end-start;
	memcpy(s_out,(s_in+start),n);
	if(with_0==1)*(s_out+n)='\0';
	return n;
}
/*
 * 根据开始结束关键字截取字符
 * @param  s_in           输入字符
 * @param  key_start      起始关键字
 * @param  key_end        结束关键字
 * @param  types          就近/远匹配(-1=最远匹配，0=最近匹配)
 * @param  s_out          输出
 * @param  with_key_start 值为1时输出中包含开始关键字
 * @param  with_key_end   值为1时输出中包含结尾关键字
 * @param  with_0         值为1时输出结尾将补'\0'
 * @return                失败返回负数，成功返回截取字符长度
 */
int mm_string_cut_by_keyword_start_and_end(const char*s_in,char*key_start,char*key_end,int types,char*s_out,int with_key_start,int with_key_end,int with_0){
	char*p=(char*)strstr(s_in,key_start);//首次匹配起始关键字
	if(p==0){
		*s_out=0;
		return -1;
	}
	int len_key_start=strlen(key_start);
	int len_key_end=strlen(key_end);
	char*p2;
	int n;
	if(types==0 || types==-1){//最近匹配 || 最远匹配
		if(with_key_start==0)p+=len_key_start;//不包含起始关键字
		if(types==0){
			p2=(char*)strstr(p,key_end);//最近
		}else{
			p2=(char*)mm_string_strrstr(p,key_end,1);//最远
		}
		if(p2==0){
			*s_out=0;
			return -2;
		}
		if(with_key_end==1)p2+=len_key_end;//包含结束关键字
		n=p2-p;
		memcpy(s_out,p,n);
		if(with_0==1)*(s_out+n)='\0';
		return n;
	}else{
		return -3;
	}
}
//字符串转大写
//效率 >= 850万
//测试机空转效率 >= 2000万
void mm_string_upper(char*in,char*out){
	//*(out++)=toupper(*(in++));
	for(;*in;in++){
		*(out++)=(('a'<= *in ) && ( *in <='z'))?(*in-32):*in;
	}
	*out=0;
}
//字符串转小写
//效率 >= 850万
//测试机空转效率 >= 2000万
void mm_string_lower(char*in,char*out){
	for(;*in;in++){
		*(out++)=(('A'<= *in ) && ( *in <='Z'))?(*in+32):*in;
	}
	*out=0;
}
/**
 * 分割文本
 * @param  in       待分割字符串
 * @param  key      分割关键字
 * @param  funciton 回调处理函数指针，int do_f(char*s,int len,int times){...}，do_f返回1将停止遍历
	uint64_t var1	传递给 funciton 的参数
	uint64_t var2	传递给 funciton 的参数
	uint64_t var3	传递给 funciton 的参数
	uint64_t var4	传递给 funciton 的参数
 * @return          返回遍历的次数
int do_f(char*s,int len,int times,uint64_t var1,uint64_t var2,uint64_t var3,uint64_t var4){
	char str[1024]={0};
	memcpy(str,s,len);
	printf("%d:[%s]\n",times,str);
	return 0;
}
int main(){
	//char s[]="a|+bb|+ccc|+dddd|+|+|+z";
	char s[]="1\r\n2\r\n3\r\n4\r\n5";
	int n=mm_string_explode(s,"|+",&do_f,0,0,0,0);
	return 0;
}
 */
uint32_t mm_string_explode(char*in,char*key,void*function,uint64_t var1,uint64_t var2,uint64_t var3,uint64_t var4){
	char*P=in;
	char*P_end=P+strlen(in);
	char*buf=(char*)strstr(P,key);
	int len_key=strlen(key);
	uint32_t times=0;
	int(*f)(char*,int,int,uint64_t,uint64_t,uint64_t,uint64_t);
	f=function;
	while(buf!=0){
		++times;
		//if(1==f(P,buf-P-1,times,var1,var2,var3,var4))return times;
		//if(1==f(P,buf-P-1,times,var1,var2,var3,var4))return times;
		if(1==f(P,buf-P,times,var1,var2,var3,var4))return times;
		P=buf+len_key;
		buf=strstr(P,key);
	}
	if(times>0 && P<P_end){
		++times;
		f(P,P_end-P,times,var1,var2,var3,var4);
	}
	return times;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------核心函数
//文件是否存在，1=存在，0=不存在/无权限
//文件全路径、权限检查(0=只检查是否存在，1=执行权限，2=写权限，4=读权限，6=读写权限)
int mm_file_exists_file(char*file_name,int test_power){
	return (0==access(file_name,test_power))?1:0;
}
//目录是否存在，1=存在，0=不存在/无权限
int mm_file_exists_dir(char*dir){
#ifdef _WIN32
	struct _finddata32_t fileinfo;
	int handle=_findfirst(dir,&fileinfo);
	if(-1==handle)return -1;
	//printf("%s:%d\n",fileinfo.name,fileinfo.attrib);//1.txt:8225   aaa:8208
	_findclose(handle);
	return (0x2010==fileinfo.attrib)?1:0;
#else
	DIR*dir_tmp=opendir(dir);
	//printf("opendir:%d\n",dir_tmp);
	//printf("__size:%d\n",dir_tmp->__size);
	if(dir_tmp>0)closedir(dir_tmp);
	return (dir_tmp!=0)?1:0;
#endif
}
/*
打开文件，失败返回0
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
FILE* mm_file_open(char*path,char*type){
	return fopen(path,type);
}
//取临时文件名，返回文件句柄。临时文件在程序退出时自动删除。
FILE*mm_file_open_tmpfile(){
	return tmpfile();
}
//取文件尺寸字节数
long mm_file_size(FILE *stream){
	fseek(stream,0,SEEK_END);
	long fsize=ftell(stream);
	fseek(stream,0,SEEK_SET);
	return fsize;
}
//读入数据，返回实际读入字节数，不成功或读到文件末尾返回 0
long mm_file_read(FILE *stream,char*buf,long read_len){
	return fread(buf,read_len,1,stream);//读入数据
}
//写入数据，返回实际写入的数据块数目
long mm_file_write(FILE *stream,char*msg){
	return fwrite(msg,1,strlen(msg)+1,stream);//写入数据
}
//关闭文件，成功则返回 0
int mm_file_close(FILE *stream){
	return fclose(stream);//关闭文件
}
//文件更名重命名，成功则返回0，失败返回-1
int mm_file_rename(char*name_old,char*name_new){
	return rename(name_old,name_new);
}
//删除文件
int mm_file_delete_unlink(char*filename){
	return unlink(filename);
}
int mm_file_delete_remove(char*filename){
	return remove(filename);
}
//仅取文件尺寸，返回-1失败
long mm_file_getfilesize(char*filename){
	FILE*f=fopen(filename,"r");
	if(f==0)return -1;
	fseek(f,0,SEEK_END);
	long fsize=ftell(f);
	fseek(f,0,SEEK_SET);
	fclose(f);
	return fsize;
}
//读入文件全部内容，失败返回-1，成功返回文件字节数
//char*tmp=malloc(1024);char*out=&tmp;
//out_size为char*tmp第一次申请的尺寸
//尽可能匹配读入尺寸可以加快代码速度，否则out可能会被realloc
//释放内存: free(tmp)
/*
char*tmp=malloc(1024);
char*out=&tmp;
mm_file_readall("Z:\\editplus\\x86\\CSS2.CTL",out,1024,"rb");
printf("%s\n",tmp);
free(tmp);
*/
long mm_file_readall(char*filename,char**out,long out_size,char*type_rb_r){
	FILE*f=fopen(filename,type_rb_r);
	if(f==0)return -1;
	fseek(f,0,SEEK_END);
	long fsize=ftell(f);
	fseek(f,0,SEEK_SET);
	if(fsize>0 && out_size<=fsize){
		++fsize;
		*out=(char*)realloc(*out,fsize);//重新申请内存空间
		if(out==0)return -2;
		memset(*out,0,sizeof(char)*fsize);
	}
	//long read_size=fread(*out,fsize,1,f);//读入数据
	fread(*out,fsize,1,f);//读入数据
	fclose(f);
	return fsize;
}
//写到文件，失败返回-1，成功返回写入字节数
//文件名、写入数据、写入模式(0=追加写入,1=替换写入)
long mm_file_writeall(char*filename,char*writedata,uint32_t len,int types){
	char wtype[3]="ab+";
	if(types==1){
		wtype[0]='w';
		wtype[1]='b';
		wtype[2]='\0';
	}
	FILE*f=fopen(filename,wtype);
	if(f==0)return -1;
	//long read_size=fwrite(writedata,1,strlen(writedata)+1,f);//写入数据
	long read_size=fwrite(writedata,1,len,f);//写入数据
	fflush(f);
	fclose(f);
	return read_size;
}
//修改文件权限，改变成功返回0，否则返回-1
//常用权限: 全最高777、全读写666、全只读444、所有者最高700、所有者读写600、所有者读400
//windows下设置为 0 则只读
int mm_file_chmod(char*path,int power){
	//if(subfold==0){
		return chmod(path,power);
	//}
	//char []="chmod -R 777 /MyDir"
	//system("md D:\\MyDir");
	//return 0;
}
/**
 * 遍历目录下全部文件/子文件夹
 * @param  dir                      需遍历的目录
 * @param  get_type_0all_1file_2dir 遍历目标，0=目录及文件，1=仅文件，2=仅目录
 * @param  include_subpath          是否遍历子目录，0=no，1=yes
 * @param  function                 回调函数指针，返回1将停止遍历，目录的大小恒为-1
int f(char*path_file_name,int power,int time_create,int time_access,int time_write,int size_Byte){...}
 * @return                          根据第2参返回遍历过的文件/目录数，空目录返回0
 */
int mm_file_list_all(char*dir,int get_type_0all_1file_2dir,int include_subpath,int(*function)(char*,int,int,int,int,int)){
	//格式化为 Z:\test\ 或 /root/libs/ 去除重复斜杠，结尾补斜杠
#ifdef _WIN32
	char zz='\\';
#else
	char zz='/';
#endif
	char dir_right[1024]={0};
	char*dir_right_p=(char*)&dir_right;//结尾位置
	int i=0;
	for(;*dir!='\0';dir++){
		if( (*dir == '/' || *dir == '\\') ){
			if((++i)==1)*(dir_right_p++)=zz;
		}else{
			*(dir_right_p++)=*dir;
			i=0;
		}
	}
	if(*(dir_right_p-1)!=zz)*(dir_right_p++)=zz;
	*dir_right_p='\0';
	//printf("%s\n",dir_right);
	i=0;
#ifdef _WIN32
	strcpy(dir_right_p,"*.*");
	struct _finddata_t fileinfo;
	int handle=_findfirst(dir_right,&fileinfo);
	if(-1==handle)return 0;
	while(handle>0){
		if(-1==_findnext(handle,&fileinfo))break;
		if(strcmp(fileinfo.name,".")==0||strcmp(fileinfo.name,"..")==0)continue;
		//printf("[%x]%s\n",((fileinfo.attrib)&_A_SUBDIR),fileinfo.name);
		if(((fileinfo.attrib)&_A_SUBDIR)==0x10){//是文件夹
			strcpy(dir_right_p,fileinfo.name);
			if(get_type_0all_1file_2dir==2||get_type_0all_1file_2dir==0){//只处理目录==2     处理文件及目录==0
				++i;
				if(1==function(dir_right,fileinfo.attrib,fileinfo.time_create,fileinfo.time_access,fileinfo.time_write,-1))break;
			}
			if(include_subpath==1){
				i+=mm_file_list_all(dir_right,get_type_0all_1file_2dir,include_subpath,function);
				continue;
			}
		}else{
			if(get_type_0all_1file_2dir==1||get_type_0all_1file_2dir==0){
				++i;
				strcpy(dir_right_p,fileinfo.name);
				;//取文件大小
				if(1==function(dir_right,fileinfo.attrib,fileinfo.time_create,fileinfo.time_access,fileinfo.time_write,fileinfo.size))break;
			}
		}
	}
#else
	struct dirent*ptr;
	DIR*dir_p;
	struct stat buf;
	dir_p=opendir(dir_right);
	while((ptr=readdir(dir_p))!=0){
		//printf("%d\t[%s]\n",(ptr->d_type&DT_DIR),ptr->d_name);
		if(strcmp(ptr->d_name,".")==0||strcmp(ptr->d_name,"..")==0)continue;
		if((ptr->d_type&DT_DIR)==DT_DIR){//是子文件夹
			strcpy(dir_right_p,ptr->d_name);
			if(get_type_0all_1file_2dir==2||get_type_0all_1file_2dir==0){//只处理目录==2     处理文件及目录==0
				++i;
				lstat(dir_right,&buf);//取文件大小，修改时间，访问时间
				if(1==function(dir_right,buf.st_mode,buf.st_ctime,buf.st_atime,buf.st_mtime,-1))break;
			}
			if(include_subpath==1){
				i+=mm_file_list_all(dir_right,get_type_0all_1file_2dir,include_subpath,function);
				continue;
			}
		}else{
			if(get_type_0all_1file_2dir==1||get_type_0all_1file_2dir==0){
				++i;
				strcpy(dir_right_p,ptr->d_name);
				lstat(dir_right,&buf);//取文件大小，修改时间，访问时间
				if(1==function(dir_right,buf.st_mode,buf.st_ctime,buf.st_atime,buf.st_mtime,buf.st_size))break;
			}
		}
	}
#endif
	return i;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------核心函数


#ifdef _cplusplus
}
#endif
#endif