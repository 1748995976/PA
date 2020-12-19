#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int cnt;
	for(cnt = 0; s[cnt] != '\0'; cnt++);
	return cnt;
}

char *strcpy(char* dst,const char* src) {
  int cnt;
	for(cnt = 0; src[cnt] != '\0'; cnt++){
		dst[cnt] = src[cnt];
	}
	dst[cnt] = '\0';
	return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  int cnt;
	for(cnt = 0; cnt < n && src[cnt] != '\0'; cnt++){
		dst[cnt] = src[cnt];
	}
	while(cnt < n){
		dst[cnt] = '\0';
	}
	return dst;
}

char* strcat(char* dst, const char* src) {
  int n = strlen(dst);
	int cnt;
	for(cnt = 0; src[cnt] != '\0'; cnt++){
		dst[n+cnt] = src[cnt];
	}
	dst[n+cnt] = '\0';
	return dst;
}

int strcmp(const char* s1, const char* s2) {
  int n1 = strlen(s1);
	int n2 = strlen(s2);
	int cnt;
	for(cnt = 0; cnt < n1 && cnt < n2; cnt++){
		if(s1[cnt] < s2[cnt]){
			return -1;
		}
		else if(s1[cnt] > s2[cnt]){
			return 1;
		}
	}
	if(cnt == n1 && cnt == n2){
		return 0;
	}
	else if(cnt < n1){
		return 1;
	}else{
    return -1;
  }
}

int strncmp(const char* s1, const char* s2, size_t n) {
  int n1 = strlen(s1);
	int n2 = strlen(s2);
	int cnt;
	for(cnt = 0; cnt < n1 && cnt < n2 && cnt < n; cnt++){
		if(s1[cnt] < s2[cnt]){
			return -1;
		}
		else if(s1[cnt] > s2[cnt]){
			return 1;
		}
	}
	if(cnt == n || (cnt == n1 && cnt == n2)){
		return 0;
	}
	else if(cnt < n1){
		return 1;
	}else{
    return -1;
  }
}

void* memset(void* v,int c,size_t n) {
  unsigned char ch = (unsigned char)c;
	unsigned char *p = (unsigned char *)v;
	int cnt;
	for(cnt = 0; cnt < n; cnt++){
		p[cnt] = ch;
	}
	return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  char *a = (char *)out;
	char *b = (char *)in;
	int cnt;
	for(cnt = 0; cnt < n; cnt ++){
		a[cnt] = b[cnt];
	}
	return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  int cnt;
	unsigned char *p = (unsigned char *)s1, *q = (unsigned char *)s2;
	for(cnt = 0; cnt < n; cnt++){
		if(p[cnt] < q[cnt]){
			return -1;
		}
		else if(p[cnt] > q[cnt]){
			return 1;
		}
	}
	return 0;
}

#endif
