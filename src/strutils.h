#ifndef STRUTILS_H
#define STRUTILS_H

char** strSplit (char* src, char* delim, int* strCount);

int strCountChar (char* src, char ch);
int strFindFirstOf (char* src, char* targets, int start);
int strFindFirstNotOf (char* src, char* targets, int start);

char* strSubstr (char* src, int start, int end);
char* strDup (char* src);
void strToUpper (char* src);
void strCat (char* dst, char* src);

#endif
