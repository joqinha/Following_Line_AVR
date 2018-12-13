#include <stddef.h>
#include <stdint.h>

#include "m_string.h"

void m_itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

size_t strlen(const char *str)
{
         const char *s;

         for (s = str; *s; ++s)
                 ;
         return (s - str);
}

 void *memcpy (void *dest, const void *src, size_t len)
 {
   char *d = dest;
   const char *s = src;
   while (len--)
     *d++ = *s++;
   return dest;
 }

 uint8_t m_STRCMP (const char *str1, const char *str2)
 {
       int ctr=0;

       while(str1[ctr]==str2[ctr])
       {
           if(str1[ctr]=='\0'||str2[ctr]=='\0')
               break;
           ctr++;
       }
       if(str1[ctr]=='\0' && str2[ctr]=='\0')
           return 0;
       else
           return -1;
 }
