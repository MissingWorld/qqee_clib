#ifndef MM_UTF8_GBK_WINDOWS_H
#define MM_UTF8_GBK_WINDOWS_H
#ifdef _cplusplus
extern "C"{
#endif

/**
 * utf8转gbk
 * @param  in_utf8     输入utf字节
 * @param  out_gbk     输出gbk字节
 * @param  out_gbk_len 缓冲区长度
 * @return             返回实际输出字节长度
 */
int utf8_gbk(char*in_utf8,char*out_gbk,unsigned out_gbk_len);

/**
 * gbk转utf8
 * @param  in_gbk       输入gbk字节
 * @param  out_utf8     输出utf8字节
 * @param  out_utf8_len 缓冲区长度
 * @return              返回实际输出字节长度
 */
int gbk_utf8(char*in_gbk,char*out_utf8,unsigned out_utf8_len);

//-------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <locale.h>
#include <tchar.h>
//int __stdcall MultiByteToWideChar(int,int,LPCSTR,int,LPWSTR,int);
//int __stdcall WideCharToMultiByte(int,int,LPCWSTR,int,LPSTR,int,LPCSTR,int);
int utf8_gbk(char*in_utf8,char*out_gbk,unsigned out_gbk_len){
	//utf8==>unicode
	unsigned len=strlen(in_utf8);//wcslen
	len=MultiByteToWideChar(65001, 0, in_utf8, len*2, 0, 0);
	char*out_unicode=malloc(len*2);
	len=MultiByteToWideChar(65001, 0, in_utf8, len, (LPWSTR)out_unicode, len*2);
	//unicode==>gbk
	_tsetlocale(LC_ALL,_T(""));
	len=wcstombs(out_gbk,(wchar_t*)out_unicode,out_gbk_len);
	free(out_unicode);
	return len;
}
int gbk_utf8(char*in_gbk,char*out_utf8,unsigned out_utf8_len){
	//gbk==>unicode0
	unsigned len=strlen(in_gbk);//wcslen
	char*wcstr=malloc(len*2);
	len=mbstowcs((wchar_t*)wcstr,in_gbk,len*2);
	//unicode==>utf8
	len=WideCharToMultiByte(65001,0,(LPCWSTR)wcstr,len*2,0,0,0,0);
	len=WideCharToMultiByte(65001,0,(LPCWSTR)wcstr,len*2,out_utf8,out_utf8_len,0,0);
	free(wcstr);
	return strlen(out_utf8);
}

#ifdef _cplusplus
}
#endif
#endif