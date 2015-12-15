#include "prompt.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <assert.h>
#include "powerstep.h"

extern COMMAND commands[];  //implemented by userprog

void* xmalloc(size_t size)
{
	register void* tmp = malloc(size);
	if(tmp)
		return tmp;
	printf("Out of Mem!");
	exit(-1);
}

void* xrealloc(void* ptr,size_t size)
{
	register void* tmp = realloc(ptr, size);
	if(tmp)
		return tmp;
	printf("Out of Mem!");
	exit(-1);
}

char* dupstr (char* s)
{
  char *r;
  r = xmalloc (strlen (s) + 1);
  strcpy (r, s);
  return (r);
}

char* skip_lead_white(char* string)
{
	register char* s;
	for (s = string; *s == ' '; s++);
	return s;
}

char* skip_tail_white(char* string)
{
	register char* s;
	for (s = string; *s != ' ' && *s != '\0'; s++);
	return s;
}

COMMAND* find_command(char* name)
{
	assert(name);
	register int i;
	for(i = 0; commands[i].name; i++)
	{
		if (strcmp(name, commands[i].name) == 0)
			return (&commands[i]);
	}
	return ((COMMAND *)NULL);
}

char* command_generator(const char* text, int state)
{
	static int list_index, list_index_param ,len, list_cnt;
	char *name;
	/* new completion */
	if (!state)
	{
		list_cnt = 0;
		list_index = 0;
		list_index_param = 0; 
		len = strlen (text);
	}

  /* Return the next name which partially matches from the command list. */
	while(list_cnt == 0 && (name = commands[list_index].name))
	{
		list_index++;
		if (strncmp (name, text, len) == 0)
			return dupstr(name);
	}
	list_cnt = 1;
	while(list_cnt == 1 && (name = powerstep_parameter_list[list_index_param++].name))
	{
		if(strncmp(name, text, len) == 0)
			return dupstr(name);
	}
	return ((char *)NULL);
}

char** command_completion(const char* text, int start, int end)
{
    return rl_completion_matches (text, command_generator);

}

int strip_parameters(char* line, char*** argv)
{
	int size = 3;
	int argc = 0;
	*argv = (char**)xmalloc(size * sizeof(char**));
	line = skip_lead_white(line);
	while(*line)
	{
		if(argc >= size)
		{
			size += 3;
			*argv = (char**)xrealloc(*argv, size * sizeof(char**));
		}
		(*argv)[argc++] = line;
		line = skip_tail_white(line);
		if(*line)
		{
			*line = '\0';
			line++;
		}
		line = skip_lead_white(line);
	}
	return argc;
}

int execute(char* line)
{
	COMMAND* cmd;
	int ret = -1;
	char** argv;
	int argc;
	
	argc = strip_parameters(line, &argv);
	if((cmd = find_command(argv[0])) != NULL)
	{
		ret = cmd->func(argc,argv);
		free(argv);
		return ret;
	}
	
	printf("command not found!\n");
	return ret;
}

void commandPrompt()
{
	char* buf;
	rl_initialize();
	rl_attempted_completion_function = command_completion;
	while((buf = readline("PSTP> ")) != NULL) 
	{
		char* line = skip_lead_white(buf);
		if(*line) //Only save non-empty commands
		{ 
			add_history(buf);
			execute(line);
		}
		free(buf);
		buf = NULL;
	}
	free(buf);
}

int help(unsigned int argc, char* argv[])
{
	if(argc < 2)
	{
		COMMAND* command_ptr = commands;
		printf("Available commands:\n");
		while(command_ptr->name)
		{
			printf("%s\n", command_ptr->name);
			command_ptr++;
		}
		printf("for more inforamtion, enter \"help <command>\"\n");
		return 0;
	}
	COMMAND* cmd = find_command(argv[1]);
	if(cmd)
		printf("%s\n", cmd->doc);
	else 
		printf("Unknown command %s\n", argv[1]);
	return 0;
}

int exit_prog(unsigned int argc, char* argv[])
{
	exit(0);
}
