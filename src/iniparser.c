#include "iniparser.h"
#include <stdio.h>

int parseINI(FILE* file, IniParser *parser)
{
	parse_state state;
	state = NEW_LINE;
	int read_char;
	char param[PARAM_BUF_SIZE];
	int act_param = 0;
	char value[VALUE_BUF_SIZE];
	int act_value = 0;
	int p_error = 0;
	unsigned int lineno = 1;
	
	
	while((read_char = fgetc(file)) != EOF || (state == VALUE && read_char == EOF))
	{
		switch(state)
		{
			
			case NEW_LINE:
				if(checkValidParamChar(read_char))
				{
					param[act_param++] = read_char;
					state = PARAM;
					break;
				}
				if(read_char == '[')
				{
					state = SECTION;
					break;
				}
				if(read_char == ';' || read_char == '#')
				{
					state = COMMENT;
					break;
				}
				if(read_char == '\n' || read_char == ' ')
				{
					break;
				}
				if(parser->parse_error)
					parser->parse_error(NOT_EXPECTED,read_char, lineno);
				return P_ERROR_PARAM;
				
			case SECTION:
				if(checkValidParamChar(read_char))
				{
					param[act_param++] = read_char;
					if(act_param >= PARAM_BUF_SIZE-1)
					{
						if(parser->parse_error)
							parser->parse_error(BUFFER_S,' ',lineno);
						return P_ERROR_BUFF;
					}
					break;
				}
				if(read_char == ']')
				{
					state = SEEK_S;
					param[act_param] = 0;
					act_param = 0;
					break;
				}
				if(parser->parse_error)
					parser->parse_error(NOT_EXPECTED,read_char, lineno);
				return P_ERROR_SECTION;
			case PARAM:
				if(checkValidParamChar(read_char))
				{
					param[act_param++] = read_char;
					if(act_param >= PARAM_BUF_SIZE-1)
					{
						if(parser->parse_error)
							parser->parse_error(BUFFER_P,' ',lineno);
						return P_ERROR_BUFF;
					}
					break;
				}
				if(read_char == '=')
				{
					state = VALUE;
					param[act_param] = 0;
					act_param = 0;
					break;
				}
				if(read_char == ' ')
				{
					state = SEEK_P;
					param[act_param] = 0;
					act_param = 0;
					break;
				}
				if(parser->parse_error)
							parser->parse_error(BUFFER_S,' ',lineno);
				return P_ERROR_PARAM;
			
			case VALUE:
				if((read_char >= '!' && read_char <= '~') && read_char != ';' && read_char !=  '#' && read_char != '\n')
				{
					value[act_value++] = read_char;
					if(act_value >= VALUE_BUF_SIZE-1)
					{
						if(parser->parse_error)
							parser->parse_error(BUFFER_V,' ',lineno);
						return P_ERROR_BUFF;
					}
					break;
				}
				if(read_char == ' ' && !act_value)
					break;
				if((read_char == '\n' || read_char == ';' || read_char ==  '#' || read_char == EOF) &&  act_value != 0)
				{
					value[act_value] = 0;
					act_value = 0;
					if(parser->param_handover)
						p_error = parser->param_handover(param, value, lineno);
					if(p_error)
						return p_error;
					if(read_char == '\n')
						state = NEW_LINE;
					else
						state = COMMENT;
					break;
				}
				if(read_char == ' ')
				{
					value[act_value] = 0;
					act_value = 0;
					state = SEEK_V;
					break;
				}
				return P_ERROR_VALUE;
			
			case SEEK_P:
				if(read_char == ' ')
					break;
				else if(read_char == '=')
				{
					state = VALUE;
					break;
				}
				if(parser->parse_error)
					parser->parse_error(EXPECT,'=', lineno);
				return P_ERROR_PARAM;
			break;
			
			case SEEK_V:
				if(read_char == '\n')
				{
					if(parser->param_handover)
						p_error = parser->param_handover(param, value, lineno);
					if(p_error)
						return p_error;
					state = NEW_LINE;
					break;
				}
				if(read_char == ' ')
					break;
				if(read_char == '#' || read_char == ';')
				{
					if(parser->param_handover)
						p_error = parser->param_handover(param, value, lineno);
					if(p_error)
						return p_error;
					state = COMMENT;
					break;
				}
			return P_ERROR_VALUE;
			
			case SEEK_S:
				if(read_char == ' ')
					break;
				if(read_char == '\n')
				{
					if(parser->section_handover)
						parser->section_handover(param);
					state = NEW_LINE;
					break;
				}
				if(read_char == '#' || read_char == ';')
				{
					if(parser->section_handover)
						p_error = parser->section_handover(param);
					if(p_error)
						return p_error;
					state = COMMENT;
					break;
				}
				
			return P_ERROR_SECTION;
			case COMMENT:
				if(read_char == '\n')
					state = NEW_LINE;
			break;
		}
		if(read_char == '\n')
			lineno++;
	}
	if(state != NEW_LINE && state != SEEK_S && state != SEEK_V && state != COMMENT)
	{
		if(parser->parse_error)
			parser->parse_error(FILEEND,' ', lineno);
		return P_ERROR_EOF;
	}
	return 0;
}

int checkValidParamChar(char c)
{
	return (c >= '0' && c <= '9') || \
		(c >= 'A' && c <= 'Z') || \
		(c >= 'a' && c <= 'z') || \
		(c == '_' );
}
