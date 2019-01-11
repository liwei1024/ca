#ifndef _XEDPARSE_H
#define _XEDPARSE_H

#include <windows.h>

//XEDParse defines
#ifdef XEDPARSE_STATIC
#define XEDPARSE_EXPORT
#else
#ifdef XEDPARSE_BUILD
#define XEDPARSE_EXPORT __declspec(dllexport)
#else
#define XEDPARSE_EXPORT __declspec(dllimport)
#endif //XEDPARSE_BUILD
#endif

#define XEDPARSE_CALL //calling convention

#define XEDPARSE_MAXBUFSIZE 256
#define XEDPARSE_MAXASMSIZE 16

//typedefs
typedef bool (XEDPARSE_CALL* CBXEDPARSE_UNKNOWN)(const char* text, ULONGLONG* value);

//XEDParse enums
enum XEDPARSE_STATUS
{
    XEDPARSE_ERROR = 0,
    XEDPARSE_OK = 1
};

//XEDParse structs
#pragma pack(push,8)
struct XEDPARSE
{
    bool x64; // use 64-bit instructions 使用64位指令
    ULONGLONG cip; //instruction pointer (for relative addressing) 指令指针(用于相对寻址)
    unsigned int dest_size; //destination size (returned by XEDParse) 目标大小(由XEDParse返回)
    CBXEDPARSE_UNKNOWN cbUnknown; //unknown operand callback 未知的操作数的回调
    unsigned char dest[XEDPARSE_MAXASMSIZE]; //destination buffer 目的地缓冲区(返回的shell_code)
    char instr[XEDPARSE_MAXBUFSIZE]; //instruction text 指令文本
    char error[XEDPARSE_MAXBUFSIZE]; //error text (in case of an error) 错误文本(如果出现错误)
};
#pragma pack(pop)

#ifdef __cplusplus
extern "C"
{
#endif

XEDPARSE_EXPORT XEDPARSE_STATUS XEDPARSE_CALL XEDParseAssemble(XEDPARSE* XEDParse);

#ifdef __cplusplus
}
#endif

#endif // _XEDPARSE_H
