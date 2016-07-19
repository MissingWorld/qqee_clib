#ifndef URL_ENCODE_H
#define URL_ENCODE_H
#ifdef _cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//除了  0~9 a~z A~Z . - _  之外的符号都进行编码
//char*out 需要准备3倍待转码字符串的长度
void urlencode(char*in,char*out);

//char*out 需要准备与待转码字符串的长度相同
void urldecode(char*in,char*out);
//----------------------------------------------------
//除了  0~9 a~z A~Z . - _  之外的符号都进行编码
//char*out 需要准备3倍待转码字符串的长度
void urlencode(char*in,char*out){
	//unsigned int len_in=strlen(in);
	const char table[]="0123456789abcdef";
	register uint32_t value;
	for(;*in;in++){
		if(*in<127){
			if(*in==0x20){//空格变+符号
				*(out++)='+';
				continue;
			}
			if( (*in>='0' && *in<='9') || (*in>='a' && *in<='z') || (*in>='A' && *in<='Z') || *in=='.' || *in=='-' || *in=='_' ){
				*(out++)=*in;
			}else{
				*(out++)='%';
				value=( (*in) & 255 );
				*(++out)=table[value&0xf];
				value=(value>>4);//value/=16;
				*(--out)=table[value&0xf];
				out+=2;
			}
		}else{
			*(out++)='%';
			value=( (*in) & 255 );
			*(++out)=table[value&0xf];
			value=(value>>4);//value/=16;
			*(--out)=table[value&0xf];
			out+=2;
			++in;
			*(out++)='%';
			value=( (*in) & 255 );
			*(++out)=table[value&0xf];
			value=(value>>4);//value/=16;
			*(--out)=table[value&0xf];
			out+=2;
			++in;
		}
	}
	*out=0;
}
//utf8需要预留4字节空间
uint8_t ucs2_to_utf8(uint32_t c,char*utf){
	if(c<0x80){
		utf[0]=c;
		utf[1]=0;
		return 2;
	}else if(c<0x800){
		utf[0] = 0xc0 | (c >> 6);
		utf[1] = 0x80 | (c & 0x3f);
		utf[2] = 0;
		return 3;
	}
	utf[0] = 0xe0 | (c >> 12);
	utf[1] = 0x80 | ((c >> 6) & 0x3f);
	utf[2] = 0x80 | (c & 0x3f);
	utf[3] = 0;
	return 4;
}
void urldecode(char*in,char*out){
	char c5[5]={0};
	register uint32_t value,value2,value3,value4;
	for(;*in;in++){
		//处理unicode
		if( (*in=='%'||*in=='\\') && (*(in+1)=='u'||*(in+1)=='U') ){
			//%u6b22%u8fce%u4f7f
			//memcpy(c5,in+2,4);
			in+=2;
			//printf("%c\n",*in);
			value = *(in++);
			if(value>='a' && value<='f'){
				value-=87;
			}else if(value>='A' && value<='F'){
				value-=55;
			}else{
				value-=48;
			}
			value2 = *(in++);
			if(value2>='a' && value2<='f'){
				value2-=87;
			}else if(value2>='A' && value2<='F'){
				value2-=55;
			}else{
				value2-=48;
			}
			value3 = *(in++);
			if(value3>='a' && value3<='f'){
				value3-=87;
			}else if(value3>='A' && value3<='F'){
				value3-=55;
			}else{
				value3-=48;
			}
			value4 = *in;
			if(value4>='a' && value4<='f'){
				value4-=87;
			}else if(value4>='A' && value4<='F'){
				value4-=55;
			}else{
				value4-=48;
			}
			uint32_t u32=(((value<<4)+value2)<<8)+((value3<<4)+value4);
			int x=ucs2_to_utf8(u32,out);
			out+=x;
			--out;
		}else if(*in=='%'){
			//%E2%9C%93
			++in;
			value = *(in++);
			if(value>='a' && value<='f'){
				value-=87;
			}else if(value>='A' && value<='F'){
				value-=55;
			}else{
				value-=48;
			}
			value2 = *(in);
			if(value2>='a' && value2<='f'){
				value2-=87;
			}else if(value2>='A' && value2<='F'){
				value2-=55;
			}else{
				value2-=48;
			}
			*(out++) = (value<<4)+value2;
		}else{
			*(out++) = *in;
		}
	}
	*out=0;
}

#ifdef _cplusplus
}
#endif
#endif