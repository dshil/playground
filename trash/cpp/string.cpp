#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "string.hpp"

size_t strlen(const char *str)
{
    const char *ret = str;
    while(*ret++);
    return ret - str - 1;
}

void strcat(char *to, const char *from)
{
    const int tosz = strlen(to);
    const int fromsz = strlen(from);

    if (fromsz == 0)
        return;

    to += tosz;
    for (int n = 0; n < fromsz; n++)
        *to++ = *from++;
    *to = '\0';
}

int strstr(const char *text, const char *pattern)
{
    const int psz = strlen(pattern);
    const int tsz = strlen(text);

    if (!psz)
        return 0;

    if (tsz < psz)
        return -1;

    int si = 0;
    int t = 0;
    int p = 0;

    for (;;) {
        if (p == psz)
            return t - psz;

        if (t == tsz)
            return -1;

        if (text[t] == pattern[p]) {
            t++;
            p++;
            continue;
        }

        if (p != 0) {
            t = ++si;
            p = 0;
            continue;
        }

        p = 0;
        t++;
    }
}

struct test{
    int ret_value;
    const char *text;
    const char *pattern;
};
test tests[] = {
        {0, "", ""}, //0
        {0, "a", ""}, //1
        {0, "a", "a"}, //2
        {-1, "a", "b"}, //3
        {0, "aa", ""}, //4
        {0, "aa", "a"}, //5
        {0, "ab", "a"}, //6
        {1, "ba", "a"}, //7
        {-1, "bb", "a"}, //8
        {0, "aaa", ""}, //9
        {0, "aaa", "a"}, //10
        {1, "abc", "b"}, //11
        {2, "abc", "c"}, //12
        {-1, "abc", "d"}, //13
        {-1, "a", "aa"}, //14
        {-1, "a", "ba"}, //15
        {-1, "a", "ab"}, //16
        {-1, "a", "bb"}, //17
        {-1, "a", "aaa"}, //18
        {-1, "aa", "aaa"}, //19
        {0, "aaa", "aaa"}, //20
        {0, "aaab", "aaa"}, //21
        {1, "baaa", "aaa"}, //22
        {1, "baaaa", "aaa"}, //23
        {1, "baaab", "aaa"}, //24
        {-1, "abd", "abc"}, //25
        {2, "ababc", "abc"}, //26
        {3, "abdabc", "abc"}, //27
        {-1, "", "a"}, //28
        {2, "asasaf", "asaf"}, //29
        {2, "ababac", "abac"} //30
};

char *getline()
{
    char *ret = NULL;
    int cap = 0;
    int len = 0;
    int c = 0;

    for (;;) {
        c = getchar();
        if (ferror(stdin)) {
            if (ret)
                free(ret);
            return NULL;
        }

        if (c == EOF || c == '\n')
            break;

        if (!(ret = append_char(ret, len, cap, c)))
            return NULL;
    }

    if (!(ret = append_char(ret, len, cap, '\0')))
        return NULL;

    return ret;
}

char *append_char(char *str, int &len, int &cap, int c)
{
    char *ret = str;

    if (len == cap) {
        cap *= 2;

        if (cap == 0)
            cap = 1;

        ret = resize(str, len, cap);
        if (!ret)
            return NULL;
    }

    ret[len++] = c;

    return ret;
}

char *resize(const char *str, unsigned size, unsigned new_size)
{
    if (new_size == 0) {
        if (size)
            delete []str;
        return NULL;
    }

    if (new_size == size)
        return (char *)str;

    char *ret = new char[new_size];
    if (!ret)
        return NULL;

    if (str) {
        int itnum = (new_size > size) ? size : new_size;
        for (int n = 0; n < itnum; n++)
            ret[n] = str[n];

        delete []str;
    }

    return ret;
}
