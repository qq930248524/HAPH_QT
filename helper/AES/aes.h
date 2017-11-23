#ifndef AES_H
#define AES_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * 参数 p: 明文的字符串数组。
 * 参数 plen: 明文的长度,长度必须为16的倍数。
 * 参数 key: 密钥的字符串数组。
 */
bool aes(char *p, int plen,  char * key);

/**
 * 参数 c: 密文的字符串数组。
 * 参数 clen: 密文的长度,长度必须为16的倍数。
 * 参数 key: 密钥的字符串数组。
 */
bool deAes(char *c, int clen, char * key);

/**
  * 天啊噜，这个就是秘钥
  */
static char *key = "1234567890123456";

/**
 * @brief getAesLen 获取加密字符串需要的空间大小，也就是16的整数倍
 * @param str 需要加密的字符串，必须是\0结尾
 * @return      存储字符串的空间大小，16的整数倍
 */
int getAesLen(char *str);

#ifdef __cplusplus
}
#endif


#endif
