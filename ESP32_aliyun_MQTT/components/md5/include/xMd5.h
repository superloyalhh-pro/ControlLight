#ifndef _XH_MD5_H_
#define _XH_MD5_H_

#include "xMd5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"

#define XH_MD5_16 16
#define XH_MD5_32 32

typedef struct
{
	unsigned int count[2];
	unsigned int state[4];
	unsigned char buffer[64];
} XH_MD5_CTX;

#define F(x, y, z) ((x & y) | (~x & z))
#define G(x, y, z) ((x & z) | (y & ~z))
#define H(x, y, z) (x ^ y ^ z)
#define I(x, y, z) (y ^ (x | ~z))
#define ROTATE_LEFT(x, n) ((x << n) | (x >> (32 - n)))

#define FF(a, b, c, d, x, s, ac)  \
	do                            \
	{                             \
		a += F(b, c, d) + x + ac; \
		a = ROTATE_LEFT(a, s);    \
		a += b;                   \
	} while (0)

#define GG(a, b, c, d, x, s, ac)  \
	do                            \
	{                             \
		a += G(b, c, d) + x + ac; \
		a = ROTATE_LEFT(a, s);    \
		a += b;                   \
	} while (0)

#define HH(a, b, c, d, x, s, ac)  \
	do                            \
	{                             \
		a += H(b, c, d) + x + ac; \
		a = ROTATE_LEFT(a, s);    \
		a += b;                   \
	} while (0)

#define II(a, b, c, d, x, s, ac)  \
	do                            \
	{                             \
		a += I(b, c, d) + x + ac; \
		a = ROTATE_LEFT(a, s);    \
		a += b;                   \
	} while (0)

void XH_MD5Init(XH_MD5_CTX *context);
void XH_MD5Update(XH_MD5_CTX *context, unsigned char *input,
				  unsigned int inputlen);
void XH_MD5Final(XH_MD5_CTX *context, unsigned char digest[16]);
void XH_MD5Transform(unsigned int state[4], unsigned char block[64]);
void XH_MD5Encode(unsigned char *output, unsigned int *input, unsigned int len);
void XH_MD5Decode(unsigned int *output, unsigned char *input, unsigned int len);

void XH_MD5Digest(void const *strContent, uint16_t iLength, uint8_t output[16]);

/*****************************************************************************
 * 函 数 名  : XH_MD5StartDigest
 * 函数功能  : 开始 MD5加密
 * 输入参数  :
 *
 * strContent:要加密的数据
 * iLength:要加密的数据的长度
 * typeMd5: 16表示生成16位  32表示生成32位
 * outBuffer:要接收的字符串
 *
 * 返 回 值  : 无
 * 备    注  : 无
 *****************************************************************************/

void XH_MD5StartDigest(void const *strContent, uint16_t iLength, uint8_t typeMd5,
					   uint8_t *outBuffer);
/*
 * function: HMAC_XH_MD5
 * parameter: uint8_t *inBuffer - 需要加密的字符串
 *            uint16_t len - 字符串长度
 *            uint8_t *ky - 初始密钥
 *            uint8_t output[16] - 输出
 */

void HMAC_XH_MD5(uint8_t *inBuffer, uint16_t len, uint8_t *ky, char *pUserOut);

#endif /* _XH_MD5_H_ */
