#pragma once
bool M_StringCopy(char *dest, const char *src, size_t dest_size);
bool M_StringConcat(char *dest, const char *src, size_t dest_size);
char *M_StringJoin(const char *s, ...);
int M_snprintf(char *buf, size_t buf_len, const char *s, ...);
