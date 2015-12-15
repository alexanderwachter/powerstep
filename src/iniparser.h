#ifndef _INIPARSER_H
#define _INIPARSER_H
#include <stdio.h>

#define PARAM_BUF_SIZE 50
#define VALUE_BUF_SIZE 50
#define P_ERROR_BUFF -1
#define P_ERROR_PARAM -2
#define P_ERROR_VALUE -3
#define P_ERROR_SECTION -4
#define P_ERROR_EOF -5

typedef enum  {NEW_LINE, SECTION ,PARAM, VALUE,SEEK_P, SEEK_V, SEEK_S, COMMENT} parse_state;
typedef enum {NOT_EXPECTED, EXPECT, FILEEND, BUFFER_P, BUFFER_V, BUFFER_S} p_errtype;

typedef struct
{
	int (*param_handover)(char* param, char* value, unsigned int linenr);
	int (*section_handover)(char* name);
	void (*parse_error)(p_errtype e,char c ,int linenr);
}
IniParser;

extern int checkValidParamChar(char c);
extern int parseINI(FILE* file, IniParser *parser);

#endif
