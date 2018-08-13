#ifndef STRING_HPP
#define STRING_HPP

size_t strlen(const char *str);
void strcat(char *to, const char *from);
int strstr(const char *text, const char *pattern);

char *getline();
char *append_char(char *str, int &len, int &cap, int c);
char *resize(const char *str, unsigned size, unsigned new_size);

#endif // STRING_HPP
