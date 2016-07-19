#ifndef MD5_H
#define MD5_H
#ifdef _cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * 获取字符串MD5
 * @param message 字符串
 * @param len     字符串长度
 * @param out     缓冲区指针，返回32位小写哈希值，char out[33];
 */
void md5(char*message,unsigned len,char*out);

/*
int main(){
	char in[]="A中国bcDefgHijKlMn";
	char out[33]={0};
	md5(in, strlen(in), out);
	printf("|%s|\n",out);
	return 0;
}
*/

//大小端处理用
#define uint32_little_endian(s,n)    
//#define T(x) (((x&0xff)<<24)|((x&0xff00)<<8)|((x&0xff0000)>>8)|((x&0xff000000)>>24))

typedef struct MD5Schedule{
	uint32_t buf[4];
	uint32_t bits[2];
	unsigned char in[64];
}MD5Schedule,*MD5Schedule_ref;

#define MD5_F1(x, y, z) (z ^ (x & (y ^ z)))
#define MD5_F2(x, y, z) MD5_F1(z, x, y)
#define MD5_F3(x, y, z) (x ^ y ^ z)
#define MD5_F4(x, y, z) (y ^ (x | ~z))
#define MD5STEP(f, w, x, y, z, data, s) ( w+=f(x,y,z)+data,  w=w<<s | w>>(32-s),  w+=x )

void md5_transform(uint32_t*buf,uint32_t const *in){
	register uint32_t a=buf[0], b=buf[1], c=buf[2], d=buf[3];

	MD5STEP(MD5_F1, a, b, c, d, in[0] + 0xd76aa478, 7);
	MD5STEP(MD5_F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
	MD5STEP(MD5_F1, c, d, a, b, in[2] + 0x242070db, 17);
	MD5STEP(MD5_F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
	MD5STEP(MD5_F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
	MD5STEP(MD5_F1, d, a, b, c, in[5] + 0x4787c62a, 12);
	MD5STEP(MD5_F1, c, d, a, b, in[6] + 0xa8304613, 17);
	MD5STEP(MD5_F1, b, c, d, a, in[7] + 0xfd469501, 22);
	MD5STEP(MD5_F1, a, b, c, d, in[8] + 0x698098d8, 7);
	MD5STEP(MD5_F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
	MD5STEP(MD5_F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
	MD5STEP(MD5_F1, b, c, d, a, in[11] + 0x895cd7be, 22);
	MD5STEP(MD5_F1, a, b, c, d, in[12] + 0x6b901122, 7);
	MD5STEP(MD5_F1, d, a, b, c, in[13] + 0xfd987193, 12);
	MD5STEP(MD5_F1, c, d, a, b, in[14] + 0xa679438e, 17);
	MD5STEP(MD5_F1, b, c, d, a, in[15] + 0x49b40821, 22);

	MD5STEP(MD5_F2, a, b, c, d, in[1] + 0xf61e2562, 5);
	MD5STEP(MD5_F2, d, a, b, c, in[6] + 0xc040b340, 9);
	MD5STEP(MD5_F2, c, d, a, b, in[11] + 0x265e5a51, 14);
	MD5STEP(MD5_F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
	MD5STEP(MD5_F2, a, b, c, d, in[5] + 0xd62f105d, 5);
	MD5STEP(MD5_F2, d, a, b, c, in[10] + 0x02441453, 9);
	MD5STEP(MD5_F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
	MD5STEP(MD5_F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
	MD5STEP(MD5_F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
	MD5STEP(MD5_F2, d, a, b, c, in[14] + 0xc33707d6, 9);
	MD5STEP(MD5_F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
	MD5STEP(MD5_F2, b, c, d, a, in[8] + 0x455a14ed, 20);
	MD5STEP(MD5_F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
	MD5STEP(MD5_F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
	MD5STEP(MD5_F2, c, d, a, b, in[7] + 0x676f02d9, 14);
	MD5STEP(MD5_F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

	MD5STEP(MD5_F3, a, b, c, d, in[5] + 0xfffa3942, 4);
	MD5STEP(MD5_F3, d, a, b, c, in[8] + 0x8771f681, 11);
	MD5STEP(MD5_F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
	MD5STEP(MD5_F3, b, c, d, a, in[14] + 0xfde5380c, 23);
	MD5STEP(MD5_F3, a, b, c, d, in[1] + 0xa4beea44, 4);
	MD5STEP(MD5_F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
	MD5STEP(MD5_F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
	MD5STEP(MD5_F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
	MD5STEP(MD5_F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
	MD5STEP(MD5_F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
	MD5STEP(MD5_F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
	MD5STEP(MD5_F3, b, c, d, a, in[6] + 0x04881d05, 23);
	MD5STEP(MD5_F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
	MD5STEP(MD5_F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
	MD5STEP(MD5_F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
	MD5STEP(MD5_F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

	MD5STEP(MD5_F4, a, b, c, d, in[0] + 0xf4292244, 6);
	MD5STEP(MD5_F4, d, a, b, c, in[7] + 0x432aff97, 10);
	MD5STEP(MD5_F4, c, d, a, b, in[14] + 0xab9423a7, 15);
	MD5STEP(MD5_F4, b, c, d, a, in[5] + 0xfc93a039, 21);
	MD5STEP(MD5_F4, a, b, c, d, in[12] + 0x655b59c3, 6);
	MD5STEP(MD5_F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
	MD5STEP(MD5_F4, c, d, a, b, in[10] + 0xffeff47d, 15);
	MD5STEP(MD5_F4, b, c, d, a, in[1] + 0x85845dd1, 21);
	MD5STEP(MD5_F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
	MD5STEP(MD5_F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
	MD5STEP(MD5_F4, c, d, a, b, in[6] + 0xa3014314, 15);
	MD5STEP(MD5_F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
	MD5STEP(MD5_F4, a, b, c, d, in[4] + 0xf7537e82, 6);
	MD5STEP(MD5_F4, d, a, b, c, in[11] + 0xbd3af235, 10);
	MD5STEP(MD5_F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
	MD5STEP(MD5_F4, b, c, d, a, in[9] + 0xeb86d391, 21);

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}
void md5_init(MD5Schedule *ctx){
	ctx->buf[0] = 0x67452301UL;
	ctx->buf[1] = 0xefcdab89UL;
	ctx->buf[2] = 0x98badcfeUL;
	ctx->buf[3] = 0x10325476UL;
	ctx->bits[0] = 0;
	ctx->bits[1] = 0;
}
void md5_update(MD5Schedule *ctx, char const *s, unsigned int len){
	register uint32_t t = ctx->bits[0];
	if ((ctx->bits[0] = t + (len << 3)) < t)ctx->bits[1]++;
	ctx->bits[1] += len >> 29;
	t=(t>>3)&0x3f;
	if(t){
		unsigned char*p=ctx->in+t;
		t=64-t;
		if(len<t){
			memmove((char*)p,s,len);
			return;
		}
		memmove((char*)p,s,t);
		uint32_little_endian((char*)ctx->in,16);
		md5_transform(ctx->buf, (uint32_t*)ctx->in);
		s+=t;len-=t;
	}
	while(len>=64){
		memmove((char*)ctx->in,s,64);
		uint32_little_endian((char*)ctx->in,16);
		md5_transform(ctx->buf,(uint32_t*)ctx->in);
		s+=64;len-=64;
	}
	memmove((char*)ctx->in,s,len);
}
void md5_final(MD5Schedule*ctx, char*digest){
	register unsigned int count = (ctx->bits[0] >> 3) & 0x3F;
	register unsigned char*p = ctx->in + count;
	*p++ = 0x80;
	count = 63-count;
	if (count < 8){
		memset(p,0, count);
		uint32_little_endian((char*)ctx->in, 16);
		md5_transform(ctx->buf, (uint32_t*)ctx->in);
		memset(ctx->in,0, 56);
	}
	else memset(p,0, count - 8);
	uint32_little_endian((char*)ctx->in, 14);
	memmove(ctx->in + 56, (char*)&ctx->bits[0], 4);
	memmove(ctx->in + 60, (char*)&ctx->bits[1], 4);
	md5_transform(ctx->buf, (uint32_t*)ctx->in);
	uint32_little_endian((char*)ctx->buf, 4);
	memmove(digest, (char*)ctx->buf, 16);
}
void md5(char*message,unsigned in_len,char*out){
	char digest[16];
	static const char table[]="0123456789abcdef";
	MD5Schedule ctx;
	md5_init(&ctx);
	md5_update(&ctx, message, in_len);
	md5_final(&ctx, digest);
	register uint32_t value;
	char*p_md5_2char=(out-2);
	int j;
	for(j=0;j<16;j++){
		//printf("%x\n",T(digest[j]));//大小端转换
		value=((uint8_t)(digest[j])&0xff);
		p_md5_2char+=2;
		*(p_md5_2char+1)=table[value&0xf];
		value=(value>>4);//value/=16;
		*p_md5_2char=table[value&0xf];
	}
}

#ifdef _cplusplus
}
#endif
#endif