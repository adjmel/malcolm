#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <netinet/if_ether.h>
#include <ifaddrs.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>

char *ft_strncpy(char *dest, const char *src, size_t n);
int ft_strcmp(const char *str1, const char *str2);
void *ft_memcpy(void *dst, const void *src, size_t n);
void *ft_memset(void *b, int c, size_t len);
int ft_ishexdigit(char c);
int ft_hexchar_to_value(char c);

#endif 
