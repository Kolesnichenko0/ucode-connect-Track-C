char *mx_strcpy(char *dst, const char *src);
int mx_strlen(const char *s);
char *mx_strnew(const int size);

char *mx_strdup(const char *str) {
    char *dup_str = mx_strnew(mx_strlen(str));
    mx_strcpy(dup_str, str);
    return dup_str;
}
