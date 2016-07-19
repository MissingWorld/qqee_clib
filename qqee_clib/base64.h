#ifndef BASE64_H
#define BASE64_H
#ifdef _cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
static const char base64_table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int base64encode(char*in,unsigned in_len,char*out){
	register unsigned X1,X2,X3;
	char*in_end=in+in_len;
	if(in_len<1)return -1;
	for(;in<in_end;in++){//for(;in<in_end;in++){
		X1=(in>in_end)?(0):(*(in++));
		X2=(in>in_end)?(0):(*(in++));
		X3=(in>in_end)?(0):(*in);
		*(out++)=base64_table[(X1&252)>>2];//*(out++)=base64_table[(X1&252)/4];
		*(out++)=base64_table[(((X2&240)>>4)|((X1&3)<<4))&255];//*(out++)=base64_table[(((X2&240)/16)|((X1&3)*16))&255];
		*(out++)=base64_table[(((X3&192)>>6)|((X2&15)<<2))&255];//*(out++)=base64_table[(((X3&192)/64)|((X2&15)*4))&255];
		*(out++)=base64_table[X3&63];
	}
	*out=0;
	unsigned len_buf=in_len%3;//in_len%3
	if(len_buf>0){
		*(--out)='=';
		if(len_buf==1)*(--out)='=';
	}
	return 1;
}
int base64decode(char*in,unsigned in_len,char*out){
	//static const char base64_table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//register unsigned char len_buf=in_len&3;//in_len%4;
	register unsigned char i,s,len_buf=in_len&3;
	if(in_len<1 || len_buf>0)return -1;
	if(in[in_len-1]=='='){
		len_buf=3;
		in[in_len-1]='A';
		if(in[in_len-2]=='='){
			len_buf=2;
			in[in_len-2]='A';
		}
	}
	//char*in_end=in+in_len;
	register int N1,N2,N3,N4;
	for(;*in;in++){//for(;in<in_end;in++){
		//N1=base64_loop(*(in++));
		s=*(in++);i=63;for(;i--;)if(s==base64_table[i]){N1=i;break;}
		//N2=base64_loop(*(in++));
		s=*(in++);i=63;for(;i--;)if(s==base64_table[i]){N2=i;break;}
		//N3=base64_loop(*(in++));
		s=*(in++);i=63;for(;i--;)if(s==base64_table[i]){N3=i;break;}
		//N4=base64_loop(*in);
		s=*in;i=63;for(;i--;)if(s==base64_table[i]){N4=i;break;}
		*(out++)=(((N2&48)>>4|N1<<2)&255);//*(out++)=(((N2&48)/16|N1*4)&255);
		*(out++)=(((N3&60)>>2|N2<<4)&255);//*(out++)=(((N3&60)/4|N2*16)&255);
		*(out++)=(((N3&3)*64&255)|(N4&63));
	}
	*out=0;
	return 1;
}

#ifdef _cplusplus
}
#endif
#endif