#pragma once
#include <string>

#if defined (_UNICODE)
#define SIMPLEGUI_STRING std::wstring 
#define SIMPLEGUI_STRSTREAM std::wstringstream;
#define SIMPLEGUI_CHAR wchar_t
#define _UI(x) L ## x
#define SIMPLEGUI_STRLEN wcslen
#define SIMPLEGUI_STRCMP wcscmp
#define SIMPLEGUI_STRCPY wcscpy
#define SIMPLEGUI_STRCPY_S wcscpy_s
#define SIMPLEGUI_MEMCPY wmemcpy
#define SIMPLEGUI_STRCAT wcscat
#define SIMPLEGUI_UTOI _wtoi
#define SIMPLEGUI_UTOF _wtof
#define SIMPLEGUI_FOPEN _wfopen
#define SIMPLEGUI_FPRINTF fwprintf
#define SIMPLEGUI_VFPRINTF vfwprintf
#define SIMPLEGUI_FPUTS fputws
#define SIMPLEGUI_FTIME wcsftime
#define SIMPLEGUI_SPRINTF swprinft
#else
#define SIMPLEGUI_STRING std::string 
#define SIMPLEGUI_STRSTREAM std::stringstream;
#define SIMPLEGUI_CHAR char
#define _UI(x) x
#define SIMPLEGUI_STRLEN strlen
#define SIMPLEGUI_STRCMP strcmp
#define SIMPLEGUI_STRCPY strcpy
#define SIMPLEGUI_STRCPY_S strcpy_s
#define SIMPLEGUI_STRCAT strcat
#define SIMPLEGUI_MEMCPY memcpy
#define SIMPLEGUI_UTOI atoi
#define SIMPLEGUI_UTOF atof
#define SIMPLEGUI_FOPEN fopen
#define SIMPLEGUI_FPRINTF fprintf
#define SIMPLEGUI_VFPRINTF vfprintf
#define SIMPLEGUI_FPUTS fputs
#define SIMPLEGUI_FTIME strftime
#define SIMPLEGUI_SPRINTF sprintf
#endif