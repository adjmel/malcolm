#include "utils.h"

char *ft_strncpy(char *dest, const char *src, size_t n) {
    size_t i = 0;
    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

int ft_ishexdigit(char c) {
    return (c >= '0' && c <= '9') || 
           (c >= 'a' && c <= 'f') || 
           (c >= 'A' && c <= 'F');
}

int ft_hexchar_to_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

int ft_strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return (unsigned char)*str1 - (unsigned char)*str2;
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	char	*ps;
	char	*pd;

	if (!dest && !src)
		return (0);
	ps = (char *)src;
	pd = (char *)dest;
	while (n--)
		*pd++ = *ps++;
	return (dest);
}

void	*ft_memset(void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*p;

	i = 0;
	p = s;
	while (i < n)
	{
		*p = c;
		p++;
		i++;
	}
	return (s);
}
