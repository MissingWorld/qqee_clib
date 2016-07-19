#ifndef LIB_SHA1_H
#define LIB_SHA1_H
#ifdef _cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define SHA1_T8(x) ((x)&0xffU)
#define SHA1_INIT() {.buf={0x67452301UL,0xefcdab89UL,0x98badcfeUL,0x10325476UL,0xc3d2e1f0UL},.bits={0,0},.in={0},.b=0}
#define SHA1_F1(x, y, z) ((x & y) | ((~x) & z))
#define SHA1_F2(x, y, z) (x ^ y ^ z)
#define SHA1_F3(x, y, z) ((x & y) | (x & z) | (y & z))
#define SHA1_F4(x, y, z) (x ^ y ^ z)
#define SHA1STEP(f, data){ \
	register uint32_t tmp = e + f(b, c, d) + data + ((a<<5) | (a>>27)); \
	e = d ; \
	d = c ; \
	c = (b<<30) | (b>>2) ; \
	b = a ; \
	a = tmp ; \
}
void uint32_pack_big (char *s, uint32_t u){
	((unsigned char *)s)[3] = SHA1_T8(u) ; u >>= 8 ;
	((unsigned char *)s)[2] = SHA1_T8(u) ; u >>= 8 ;
	((unsigned char *)s)[1] = SHA1_T8(u) ; u >>= 8 ;
	((unsigned char *)s)[0] = SHA1_T8(u) ;
}
typedef struct SHA1Schedule{
	uint32_t buf[5] ;
	uint32_t bits[2] ;
	uint32_t in[16] ;
	unsigned int b ;
}SHA1Schedule,*SHA1Schedule_ref;
//extern void sha1_init (SHA1Schedule *) ;
//extern void sha1_update (SHA1Schedule *, char const *, unsigned int) ;
//extern void sha1_final (SHA1Schedule *, char * /* 20 chars */) ;
//extern void sha1_feed (SHA1Schedule *, unsigned char) ;
//extern void sha1_transform (uint32_t * /* 5 uint32s */, uint32_t const * /* 16 uint32s */) ;
void sha1_transform(uint32_t *buf, uint32_t const *in){
	register uint32_t a = buf[0], b = buf[1], c = buf[2], d = buf[3], e = buf[4] ;
	uint32_t w[80] ;
	register unsigned int i = 0 ;
	for (; i < 16 ; i++) w[i] = in[i] ;
	for (; i < 80 ; i++){
		w[i] = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16] ;
		w[i] = (w[i]<<1) | (w[i]>>31) ;
	}
	for (i = 0 ; i < 20 ; i++)SHA1STEP(SHA1_F1, w[i] + 0x5a827999UL) ;
	for (; i < 40 ; i++)SHA1STEP(SHA1_F2, w[i] + 0x6ed9eba1UL) ;
	for (; i < 60 ; i++)SHA1STEP(SHA1_F3, w[i] + 0x8f1bbcdcUL) ;
	for (; i < 80 ; i++)SHA1STEP(SHA1_F4, w[i] + 0xca62c1d6UL) ;
	buf[0] += a ; buf[1] += b ; buf[2] += c ; buf[3] += d ; buf[4] += e ;
}
void sha1_feed(SHA1Schedule *ctx, unsigned char inb){
	register uint32_t tmp ;
	ctx->in[ctx->b>>2] <<= 8 ;
	ctx->in[ctx->b>>2] |= SHA1_T8(inb) ;
	if (++ctx->b >= 64){
		register unsigned int i = 0 ;
		sha1_transform(ctx->buf, ctx->in) ;
		ctx->b = 0 ;
		for (i = 0 ; i < 16 ; i++) ctx->in[i] = 0 ;
	}
	tmp = ctx->bits[0] ;
	ctx->bits[0] += 8 ;
	if (tmp > ctx->bits[0]) ctx->bits[1]++ ;
}
void sha1_init(SHA1Schedule *ctx){
	register unsigned int i = 0 ;
	ctx->buf[0] = 0x67452301UL ;
	ctx->buf[1] = 0xefcdab89UL ;
	ctx->buf[2] = 0x98badcfeUL ;
	ctx->buf[3] = 0x10325476UL ;
	ctx->buf[4] = 0xc3d2e1f0UL ;
	ctx->bits[0] = ctx->bits[1] = 0 ;
	for(; i < 16 ; i++) ctx->in[i] = 0 ;
	ctx->b = 0 ;
}
void sha1_update(SHA1Schedule *ctx, char const *buf, unsigned int len){
	register unsigned int i = 0 ;
	for (; i < len ; i++) sha1_feed(ctx, (unsigned char)buf[i]);
}
void sha1_final(SHA1Schedule *ctx, char *digest){
	char pack[8] ;
	register unsigned int i = 0 ;
	uint32_pack_big(pack, ctx->bits[1]) ;
	uint32_pack_big(pack+4, ctx->bits[0]) ;
	sha1_feed(ctx, 0x80) ;
	while (ctx->b != 56) sha1_feed(ctx, 0) ;
	sha1_update(ctx, pack, 8) ;
	for (; i < 5 ; i++)uint32_pack_big(digest + (i<<2), ctx->buf[i]);
}
void sha1(char*message,unsigned len,char*out){
	static char digest[20];
	SHA1Schedule ctx;
	sha1_init(&ctx);
	sha1_update(&ctx, message, len);
	sha1_final(&ctx, digest);
	const char table[]="0123456789abcdef";
	register uint32_t value;
	char*p_md5_2char=(out-2);
	int j;
	for(j=0;j<20;j++){
		value=( (uint8_t)(digest[j])&0xff );//计算出10进制数
		p_md5_2char+=2;
		*(p_md5_2char+1)=table[value&0xf];
		value=(value>>4);//value/=16;
		*p_md5_2char=table[value&0xf];
	}
}

/*int main(int argc, char const *argv[]){
	char in[]="abc";
	char out[41];out[40]=0;
	sha1(in, strlen(in), out);
	printf("|%s|\n",out);//输出 A9993E364706816ABA3E25717850C26C9CD0D89D
	return 0;
}*/

#ifdef _cplusplus
}
#endif
#endif