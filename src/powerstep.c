/*
 * Test utility for STMicroelectronics powerSTEP01 (using spidev driver)
 *
 * Copyright (c) 2015 Alexander Wachter <alexander.wachter@student.tugraz.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 * There is no waranty for this programm
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "powerstep.h"
#include "iniparser.h"
#include <assert.h>
#include "prompt.h"


COMMAND commands[] = {
	{
		.name = "exit",
		.func = exit_prog,
		.doc = "Exit this programm\nexit"
	},
	{
		.name = "register",
		.func = register_set,
		.doc = "Read or Set the registers value\nregister <name> [value]"
               "\nif a value is presen, the register is set to it."
	},
	{
		.name = "help",
		.func = help,
		.doc = "Print help for command\nhelp [command]"
	},
	{
		.name = "ini",
		.func = ini_command,
		.doc = "Process ini file\nini <file name>"
	},
	{
		.name = "run",
		.func = run_command,
		.doc = "Start turning with specified speed\nrun <speed>[dir]\n"
			   "dir: 0,1"
	},
	{
		.name = "stepClock",
		.func = stepclock_command,
		.doc = "Puts the device in Step-clock mode and imposes DIR "
		       "direction\nstepClock <dir>\n"
			   "dir: 0,1"
	},
	{
		.name = "move",
		.func = move_command,
		.doc = "Makes N_STEP (micro)steps in DIR direction "
		       "(Not performable when motor is running)"
		       "\nmove <steps>[dir]\n"
			   "dir: 0,1"
	},
	{
		.name = "goTo",
		.func = goto_command,
		.doc = "Brings motor in ABS_POS position "
		       "(without dir, minimum path)"
		       "\ngoTo <abs pos>[dir]\n"
			   "dir: 0,1"
	},
	{
		.name = "goUntil",
		.func = gountil_command,
		.doc = "Performs a motion in dir direction with given speed\n"
		       "until SW is closed, the ACT action is executed\n"
		       "and a SoftStop takes place."
		       "\nIf act is 0, the ABS_POS is set to zero."
		       "\nIf act is 1, the ABS_POS register is copied into MARK register"
		       "\ngoUntil <speed dir act>\n"
			   "dir: 0,1\n"
			   "act: 0,1"
	},
	{
		.name = "releseSW",
		.func = releasesw_command,
		.doc = "Performs a motion in DIR direction at minimum speed\n"
		       "until the SW is released (open), the ACT action is executed\n"
		       "then a HardStop takes place"
		       "\nIf act is 0, the ABS_POS is set to zero."
		       "\nIf act is 1, the ABS_POS register is copied into MARK register"
		       "\nreleseSW <dir act>\n"
			   "dir: 0,1\n"
			   "act: 0,1"
	},
	{
		.name = "goHome",
		.func = gohome_command,
		.doc = "The GoHome command produces a motion to the HOME position\n"
		       "(zero position) via the shortest path."
		       "\ngoHome"
	},
	{
		.name = "goMark",
		.func = gomark_command,
		.doc = "The GoHome command produces a motion to the MARK position\n"
		       "via the shortest path."
		       "\ngoMark"
	},
	{
		.name = "resetPos",
		.func = resetpos_command,
		.doc = "The ResetPos command resets the ABS_POS register to zero.\n"
		       "\nresetPos"
	},
	{
		.name = "resetDevice",
		.func = resetdev_command,
		.doc = "The ResetDevice command resets the device to power-up conditions\n"
		       "\nresetDevice"
	},
	{
		.name = "softStop",
		.func = softstop_command,
		.doc = "Stops motor with a deceleration phase\n"
			   "When the motor is in high impedance state, a SoftStop "
			   "command\nforces the bridges to exit from high impedance state\n"
			   "No motion is performed."
		       "\nsoftStop"
	},
	{
		.name = "hardStop",
		.func = hardstop_command,
		.doc = "The HardStop command causes an immediate motor stop with"
			   "infinite deceleration.\n"
			   "When the motor is in high impedance state, a SoftStop "
			   "command\nforces the bridges to exit from high impedance state\n"
			   "No motion is performed."
		       "\nhardStop"
	},
	{
		.name = "softHiZ",
		.func = softhiz_command,
		.doc = "Puts the bridges in high impedance status after a deceleration phase"
		       "\nsoftHiZ"
	},
	{
		.name = "hardHiZ",
		.func = hardhiz_command,
		.doc = "Puts the bridges in high impedance status immediately"
		       "\nhardHiZ"
	},
	{
		.name = "getStatus",
		.func = getStatus_command,
		.doc = "Prints the status flags and resets the STATUS register"
		       " warning flags"
		       "\ngetStatus"
	},
	{
		.name = "printRegisters",
		.func = printregisters_command,
		.doc = "Print the actual content of all registers"
		       "\nprintRegisters"
	},
	{NULL, NULL, NULL}
};

static IniParser parser = {
		.param_handover = parse_param_ini,
		.section_handover = new_section,
		.parse_error = parse_error_function
	};

static const char* device;
static const char* ini_filename; 
static uint32_t mode = SPI_CPOL | SPI_CPHA;
static uint8_t bits = 8;
static uint32_t speed = 1000000;
static uint16_t delay = 10;
int fd;

static powerstep_send_queue* send_queue;

int transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len)
{
	int ret = 1;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)(tx + (len - 1)),
		.rx_buf = (unsigned long)(rx + (len - 1)),
		.len = 1,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
		.cs_change = 0
	};
	
	while(len-- && ret > 0)
	{
		printf("Send to spi: 0x%x\n", (*(char*)tr.tx_buf) & 0xFF);
		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
		printf("Get 0x%x back\n", (*(char*)tr.rx_buf) & 0xFF);
		tr.tx_buf --;
		tr.rx_buf --;
	}
	return ret;
}

int read_reg(int fd, powerstep_register* reg, uint32_t* rx)
{
	int tr_count = (reg->len + 7) >> 3;
	assert(tr_count < 4);
	uint8_t tx[4] = {0};
	tx[tr_count] = reg->addr | 0x20;
	if(transfer(fd, (uint8_t*)&tx, (uint8_t*)rx, tr_count + 1) < 1)
		return -1;
	return tr_count;
}

int write_reg(int fd, powerstep_register* reg, uint32_t value)
{
	int tr_count = (reg->len + 7) >> 3;
	assert(tr_count < 4);
	value |= (reg->addr << (tr_count << 3));
	uint8_t rx[4];
	if(transfer(fd, (uint8_t*)&value, rx, tr_count + 1) < 1)
		return -1;
	return tr_count;
}

void send_parameter(int fd, powerstep_send_queue* parameter)
{
	uint32_t value = 0;
	if(parameter->mask == (1 << parameter->reg->len) - 1)
		value = parameter->value;
	else
	{
		printf("Read reg %d before Write\n", parameter->reg->addr);	
		if(read_reg(fd, parameter->reg, &value) < 0)
		{
			printf("Error reading Register %d\n", parameter->reg->addr);
			return;
		}
		value &= ~parameter->mask;
		value |= parameter->value;
	}
	if(write_reg(fd, parameter->reg, value) < 0)
	{
		printf("Error wwriting Register %d\n", parameter->reg->addr);
		return;
	}
}

uint32_t read_parameter(int fd, powerstep_send_queue* parameter)
{
	uint32_t value = 0;
	if(read_reg(fd, parameter->reg, &value) < 0)
	{
		printf("Error reading Register %d\n", parameter->reg->addr);
		return -1;
	}
	printf("value: %d, mask: %x\n",value, parameter->mask);
	parameter->value = value & parameter->mask;
	return 0;
}

void send_parameterlist(powerstep_send_queue* parameter, int fd)
{
	while(parameter)
	{
		send_parameter(fd, parameter);
		parameter = parameter->next;
	}
}

void read_parameterlist(powerstep_send_queue* parameter, int fd)
{
	while(parameter)
	{
		if(read_parameter(fd, parameter))
		{
			printf("Error reading register %d\n", parameter->reg->addr);
			return;
		}
		parameter = parameter->next;
	}
}

powerstep_parameter* get_parameter(char* name)
{
	powerstep_parameter* param = powerstep_parameter_list;
	
	while(param->name)
	{
		if(!strcmp(name, param->name))
			return param;
		++param;
	}
	return NULL;
}

powerstep_send_queue* get_new_element(uint32_t value, uint32_t mask, powerstep_register* reg)
{
	powerstep_send_queue* element = malloc(sizeof(powerstep_send_queue));
	element->next = NULL;
	element->reg = reg;
	element->value = value;
	element->mask = mask;
	return element;
}

void free_send_queue(powerstep_send_queue* queue)
{
	while(queue)
	{
		powerstep_send_queue* tmp = queue;
		queue = queue->next;
		free(tmp);
	}
}

int add_to_send_queue(uint32_t value, uint32_t mask, powerstep_register* reg, powerstep_send_queue** send_queue)
{
	powerstep_send_queue* element;
	int ret = 0;
	if(!*send_queue)
	{
		*send_queue = get_new_element(value,mask,reg);
		element = *send_queue;
	}
	else
	{
		element = *send_queue;
		while(element->reg != reg)
		{	
			//append a new element
			if(!element->next)
			{
				element->next = get_new_element(value,mask,reg);
				element = element->next;
				return ret;
			}
			element = element->next;
		}
		//use existing
		if(element->mask & mask)
			ret = -1;
		element->value |= value;
		element->mask |= mask;
	}
	return ret;
}

void add_to_param_queue(powerstep_parameter_queue** queue, powerstep_parameter* param)
{
	powerstep_parameter_queue* new_element = malloc(sizeof(powerstep_parameter_queue));
	new_element->next = NULL;
	new_element->parameter = param;
	powerstep_parameter_queue* queue_ptr = *queue;
	if(queue_ptr)
	{
		while(queue_ptr->next)
			queue_ptr = queue_ptr->next;
		queue_ptr->next = new_element;
	}
	else
		*queue = new_element;
}

void free_parameter_queue(powerstep_parameter_queue* queue)
{
	while(queue)
	{
		powerstep_parameter_queue* tmp = queue;
		queue = queue->next;
		free(tmp);
	}
}

int parse_param(char* param_name, char* value_string, unsigned int linenr, powerstep_send_queue** send_queue)
{
	powerstep_parameter* param = get_parameter(param_name);
	if(!param)
	{
		if(linenr)
			printf("Error: unknown parameter %s on line %d\n", param_name, linenr);
		else
			printf("Error: unknown parameter %s\n", param_name);
		return -1;
	}
	int value_len = strlen(value_string);
	char* endptr;
	uint32_t value  = strtol(value_string, &endptr, 0);
	if(endptr != value_string + value_len)
	{
		if(linenr)
			printf("Error: value should be a number on line %d\n", linenr);
		else
			printf("Error: value should be a number\n");
		return -1;
	}
	
	uint32_t bit_len = (1 << param->length) - 1;
	if(value > bit_len)
	{
		if(linenr)
			printf("Error: value of param %s exceeded reg maximum on line %d\n", param_name, linenr);
		else
			printf("Error: value of param %s exceeded reg maximum\n", param_name);
		return -1;
	}
	uint32_t maks = bit_len << param->offset;
	value = value << param->offset;
	if(add_to_send_queue(value, maks, param->reg, send_queue))
	{
		if(linenr)
			printf("Warning: parameter %s is overwritten on line %d\n", param_name, linenr);
		else
			printf("Warning: parameter %s is overwritten\n", param_name);
	}
	return 0;
}

int parse_param_ini(char* param_name, char* value_string, unsigned int linenr)
{
	return parse_param(param_name, value_string, linenr, &send_queue);
}

void print_parameter_queue(powerstep_parameter_queue* param_queue, powerstep_send_queue* results)
{
	
	while(param_queue)
	{
		powerstep_send_queue* results_ptr = results;
		while(results_ptr)
		{
			if(results_ptr->reg == param_queue->parameter->reg)
				break;
			results_ptr = results_ptr->next;
		}
		assert(results_ptr);
		uint32_t content = results_ptr->value >> param_queue->parameter->offset;
		content &= (1 << param_queue->parameter->length) - 1;
		printf("%s: %d\n", param_queue->parameter->name, content);
		param_queue = param_queue->next;
	}
}

void printSendqueue(powerstep_send_queue* queue_prt)
{
	
	if(!queue_prt)
	{
		printf("Send queue is empty!\n");
		return;
	}
	do{
		printf("======================================\n");
		printf("Address: %d\n", queue_prt->reg->addr);
		printf("Length: %d\n", queue_prt->reg->len);
		printf("Mask: %x\n", queue_prt->mask);
		printf("Value: %d\n", queue_prt->value);
		printf("======================================\n");
		queue_prt = queue_prt->next;
	}while(queue_prt);
}

int new_section(char* s)
{
	printf("New Section: %s\n", s);
	printf("Currently there is no support for sections. The will be ignored\n");
	return 0;
}

void parse_error_function(p_errtype e,char c ,int lineno)
{
	printf("Error: ");
	switch(e)
	{
		case NOT_EXPECTED:
			printf("unexpected char %c", c);
		break;
		
		case EXPECT:
			printf("expect %c", c);
		break;
		
		case FILEEND:
			printf("unexpected end of file");
		break;
			
		case BUFFER_P:
			printf("Parameter is to long");
		break;
		
		case BUFFER_V:
			printf("Value is to long");
		break;
		
		case BUFFER_S:
			printf("Section name is to long");
		break;
	}
	printf(" on line %d\n", lineno);
}

int register_set(unsigned int argc, char* argv[])
{
	int i;
	char* value_ptr;
	int char_index;
	powerstep_send_queue* send_queue = NULL;
	powerstep_send_queue* read_queue = NULL;
	powerstep_parameter_queue* param_queue = NULL;
	
	for(i=1; i < argc; i++)
	{
		char_index = 0;
		value_ptr = 0;
		while(argv[i][char_index++])
		{
			if(argv[i][char_index] == '=')
			{
				argv[i][char_index] = '\0';
				value_ptr = &argv[i][++char_index];
				if(!*value_ptr)
				{
					printf("value for param %s is missing\n", argv[i]);
					goto error;
				}
				break;
			}
		}
		if(value_ptr) //write to reg
		{
			if(parse_param(argv[i], value_ptr, 0, &send_queue))
				goto error;
		}
		else //just read the reg
		{
			powerstep_parameter* param = get_parameter(argv[i]);
			if(!param)
			{
				printf("Error: unknown parameter %s\n", argv[i]);
				goto error;
			}
			add_to_param_queue(&param_queue, param);
			uint32_t bit_len = (1 << param->length) - 1;
			uint32_t maks = bit_len << param->offset;
			add_to_send_queue(0, maks, param->reg, &read_queue);
		}

	}
	send_parameterlist(send_queue, fd);
	read_parameterlist(read_queue, fd);
	print_parameter_queue(param_queue, read_queue);
	
error:	
	free_send_queue(send_queue);
	free_send_queue(read_queue);
	free_parameter_queue(param_queue);
	return 0;
}

int run_command(unsigned int argc, char* argv[])
{
	int dir = 0;
	if(argc < 2)
	{
		printf("No speed spec. (run <speed>[dir])\n");
		return -1;
	}
	if(argc > 3)
	{
		printf("to many arguments\n");
		return -1;
	}
	char* endptr;
	uint32_t speed  = strtol(argv[1], &endptr, 0);
	if(endptr != argv[1] + strlen(argv[1]))
	{
		printf("speed is not a number\n");
		return -1;
	}
	if(argc == 3)
	{
		if(argv[2][0] == '1' && !argv[2][1])
			dir = 1;
		else if(argv[2][0] != '0' || argv[2][1])
		{
			printf("dir must be either 0 or 1\n");
			return -1;
		}
	}
	if(speed > (1 << PS_SPEED_LEN) -1)
	{
		printf("speed exceeds maximum of 1048575\n");
		return -1;
	}
	uint32_t tx = speed | ((0x50 | dir) << 24);
	uint8_t rx[4];
	transfer(fd, (uint8_t*)&tx, rx, 4);
	return 0;
}

int stepclock_command(unsigned int argc, char* argv[])
{
	uint8_t dir = 0;
	if(argc < 2)
	{
		printf("dir is missing\n");
		return -1;
	}
	if(argc > 2)
	{
		printf("to many arguments\n");
		return -1;
	}
	if(argv[1][0] == '1' && ! argv[1][1])
			dir = 1;
	else if(argv[1][0] != '0' || argv[1][1])
	{
		printf("dir must be either 0 or 1\n");
		return -1;
	}
	uint8_t tx = 0x58 | dir;
	uint8_t rx;
	transfer(fd, &tx, &rx, 1);
	return 0;
}

int move_command(unsigned int argc, char* argv[])
{
	uint32_t dir = 0;
	if(argc < 2)
	{
		printf("No steps specified (move <steps>[dir])\n");
		return -1;
	}
	if(argc > 3)
	{
		printf("to many arguments\n");
		return -1;
	}
	char* endptr;
	uint32_t steps  = strtol(argv[1], &endptr, 0);
	if(endptr != argv[1] + strlen(argv[1]))
	{
		printf("steps is not a number\n");
		return -1;
	}
	if(argc == 3)
	{
		if(argv[2][0] == '1' && !argv[2][1])
			dir = 1;
		else if(argv[2][0] != '0' || argv[2][1])
		{
			printf("dir must be either 0 or 1\n");
			return -1;
		}
	}
	if(steps > (1 << PS_ABS_POS_LEN) -1)
	{
		printf("steps exceeds maximum of 4194303\n");
		return -1;
	}
	uint32_t tx = steps | ((0x40 | dir) << 24);
	uint8_t rx[4];
	transfer(fd, (uint8_t*)&tx, rx, 4);
	return 0;
}

int goto_command(unsigned int argc, char* argv[])
{
	uint32_t dir = 0;
	uint32_t tx;
	uint8_t rx[4];
	uint32_t pos;
	
	if(argc < 2)
	{
		printf("No abs pos specified (goto <abs pos>[dir])\n");
		return -1;
	}
	if(argc > 3)
	{
		printf("to many arguments\n");
		return -1;
	}
	char* endptr;
	pos = strtol(argv[1], &endptr, 0);

	if(endptr != argv[1] + strlen(argv[1]))
	{
		printf("abs pos is not a number\n");
		return -1;
	}	
	if(pos > (1 << PS_ABS_POS_LEN) -1)
	{
		printf("abs pos exceeds maximum of 4194303\n");
		return -1;
	}
	if(argc == 3)
	{
		if(argv[2][0] == '1' && !argv[2][1])
			dir = 1;
		else if(argv[2][0] != '0' || argv[2][1])
		{
			printf("dir must be either 0 or 1\n");
			return -1;
		}
		tx = pos | ((0x68 | dir) << 24);
	}
	else
		tx = pos | (0x60 << 24);
	transfer(fd, (uint8_t*)&tx, rx, 4);
	return 0;
}

int gountil_command(unsigned int argc, char* argv[])
{
	uint32_t flags = 0;
	uint32_t tx;
	uint8_t rx[4];
	uint32_t speed;
	
	if(argc < 4)
	{
		printf("Wrong arg count(gountil <speed dir act>)\n");
		return -1;
	}
	if(argc > 4)
	{
		printf("to many arguments\n");
		return -1;
	}
	char* endptr;
	speed = strtol(argv[1], &endptr, 0);

	if(endptr != argv[1] + strlen(argv[1]))
	{
		printf("speed is not a number\n");
		return -1;
	}	
	if(speed > (1 << PS_SPEED_LEN) -1)
	{
		printf("speed exceeds maximum of 1048575\n");
		return -1;
	}
	if(argv[2][0] == '1' && !argv[2][1])
			flags |= 1;
	else if(argv[2][0] != '0' || argv[2][1])
	{
		printf("dir must be either 0 or 1\n");
		return -1;
	}
	if(argv[3][0] == '1' && !argv[3][1])
			flags |= (1 << 3);
	else if(argv[3][0] != '0' || argv[3][1])
	{
		printf("act must be either 0 or 1\n");
		return -1;
	}
	tx = speed | ((0x80 | flags) << 24);
	transfer(fd, (uint8_t*)&tx, rx, 4);
	return 0;
}

int releasesw_command(unsigned int argc, char* argv[])
{
	uint32_t flags = 0;
	uint8_t tx;
	uint8_t rx;
	
	if(argc < 3)
	{
		printf("Wrong arg count(releasesw <dir act>)\n");
		return -1;
	}
	if(argc > 3)
	{
		printf("to many arguments\n");
		return -1;
	}
	
	if(argv[1][0] == '1' && !argv[1][1])
			flags |= 1;
	else if(argv[1][0] != '0' || argv[1][1])
	{
		printf("dir must be either 0 or 1\n");
		return -1;
	}
	if(argv[2][0] == '1' && !argv[2][1])
			flags |= (1 << 3);
	else if(argv[2][0] != '0' || argv[2][1])
	{
		printf("act must be either 0 or 1\n");
		return -1;
	}
	tx = (0x90 | flags);
	transfer(fd, &tx, &rx, 1);
	return 0;
}

int gohome_command(unsigned int argc, char* argv[])
{
	uint8_t tx = 0x70;
	uint8_t rx;
	transfer(fd, &tx, &rx, 1);
	return 0;
}

int gomark_command(unsigned int argc, char* argv[])
{
	uint8_t tx = 0x78;
	uint8_t rx;
	transfer(fd, &tx, &rx, 1);
	return 0;
}

int resetpos_command(unsigned int argc, char* argv[])
{
	uint8_t tx = 0xD8;
	uint8_t rx;
	transfer(fd, &tx, &rx, 1);
	return 0;
}

int resetdev_command(unsigned int argc, char* argv[])
{
	uint8_t tx = 0xC0;
	uint8_t rx;
	transfer(fd, &tx, &rx, 1);
	return 0;
}

int softstop_command(unsigned int argc, char* argv[])
{
	uint8_t tx = 0xB0;
	uint8_t rx;
	transfer(fd, &tx, &rx, 1);
	return 0;
}

int hardstop_command(unsigned int argc, char* argv[])
{
	uint8_t tx = 0xB8;
	uint8_t rx;
	transfer(fd, &tx, &rx, 1);
	return 0;
}

int softhiz_command(unsigned int argc, char* argv[])
{
	uint8_t tx = 0xA0;
	uint8_t rx;
	transfer(fd, &tx, &rx, 1);
	return 0;
}

int hardhiz_command(unsigned int argc, char* argv[])
{
	uint8_t tx = 0xA8;
	uint8_t rx;
	transfer(fd, &tx, &rx, 1);
	return 0;
}
int getStatus_command(unsigned int argc, char* argv[])
{
	uint8_t tx[] = {0, 0, 0xD0};
	uint8_t rx[3];
	transfer(fd, tx, rx, 3);
	
	uint16_t status = rx[2] | (rx[1] << 8);
	uint8_t th_status = (status >> 11) & 0x03;
	uint8_t mot_status = (status >> 5) & 0x03;
	
	printf("Status register: 0x%04x\n", status);
	
	if(!(status & (1 << 15)))
		printf("Stall on bridge A\n");
		
	if(!(status & (1 << 14)))
		printf("Stall on bridge B\n");

	if(!(status & (1 << 13)))
		printf("Overcurrent detected\n");

	printf("Thermal status: ");
	switch (th_status)
	{
		case 0:
			printf("Normal\n");
			break;
		case 1:
			printf("Warning\n");
			break;
		case 2:
			printf("Bridge shutdown\n");
			break;
		case 3:
			printf("Device shutdown\n");
			break;
		
	}
	if(!(status & (1 << 10)))
		printf("Undevoltage on ADC\n");
		
	if(!(status & (1 << 9)))
		printf("Reset or undervoltage lockout\n");
		
	if(!(status & (1 << 8)))
		printf("Device ist in step clock mode\n");
		
	if(status & (1 << 7))
		printf("Command Error\n");
	
	printf("Motor status: ");
	switch (mot_status)
	{
		case 0:
			printf("Stopped\n");
			break;
		case 1:
			printf("Acceleration\n");
			break;
		case 2:
			printf("Deceleration\n");
			break;
		case 3:
			printf("Constant speed\n");
			break;
	}
	
	printf("Direction: ");
	if(status & (1 << 4))
		printf("forward\n");
	else
		printf("reverse\n");
		
	if(status & (1 << 3))
		printf("Switch turn on event has occured\n");
		
	if(status & (1 << 2))
		printf("Switch is closed\n");
	else
		printf("Switch is open\n");
		
	if(status & (1 << 1))
		printf("Motion is performing\n");
	if(status & 1)
		printf("Bridges are in HighZ mode\n");
	else
		printf("Bridges are active\n");
	return 0;
}

int printregisters_command(unsigned int argc, char* argv[])
{
	powerstep_send_queue* read_queue = NULL;
	powerstep_parameter* param = powerstep_parameter_list;
	powerstep_parameter_queue* param_queue = NULL;
	uint32_t bit_len;
	uint32_t maks;
	while(param->name)
	{
		add_to_param_queue(&param_queue, param);
		bit_len = (1 << param->length) - 1;
		maks = bit_len << param->offset;
		add_to_send_queue(0, maks, param->reg, &read_queue);
		param++;
	}
	read_parameterlist(read_queue, fd);
	print_parameter_queue(param_queue, read_queue);
	free_send_queue(read_queue);
	free_parameter_queue(param_queue);
	return 0;
}

static void print_usage(const char *prog)
{
	printf("Usage: %s <-D>[-sdi]\n", prog);
	puts("  -D --device   device to use (default /dev/spidev1.1)\n"
	     "  -s --speed    max speed (Hz)\n"
	     "  -d --delay    delay (usec)\n"
	     "  -i --ini      ini file\n");
	exit(1);
}

void parse_opts(int argc, char *argv[])
{		
	static const struct option lopts[] = {
		{ "device",  1, 0, 'D' },
		{ "speed",   1, 0, 's' },
		{ "delay",   1, 0, 'd' },
		{ "ini",     1, 0, 'i' },
		{ NULL, 0, 0, 0 },
	};
	int c;
	while ((c = getopt_long(argc, argv, "D:s:d:i:", lopts, NULL)) != -1) 
	{
		switch (c) {
		case 'D':
			device = optarg;
			break;
		case 's':
			speed = atoi(optarg);
			break;
		case 'd':
			delay = atoi(optarg);
			break;
		case 'i':
			ini_filename = optarg;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

void process_ini(const char* file_name)
{
	int perror;
	FILE* ini_file = fopen(file_name, "r");
	if(!ini_file)
	{
		printf("cannot open file %s\n", file_name);
		return;
	}
	if((perror = parseINI(ini_file, &parser)))
		printf("parse error %d \n", perror);
	else
	{
		//printSendqueue(send_queue);
		send_parameterlist(send_queue,fd);
	}
	fclose(ini_file);
	free_send_queue(send_queue);
	send_queue = NULL;
}

int ini_command(unsigned int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("filename is missing (ini <file name>)\n");
		return -1;
	}
	process_ini(argv[1]);
	return 0;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	fd = 0;
	
	parse_opts(argc, argv);
	
	if(!device)
	{
		printf("No device selected!\n");
		print_usage(argv[0]);
		exit(-1);
	}

	fd = open(device, O_RDWR);
	if (fd < 0)
	{
		printf("can't open device");
		exit(-1);
	}

	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
	{
		printf("can't set spi mode");
		exit(-1);
	}

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
	{
		printf("can't set bits per word");
		exit(-1);
	}

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
	{
		printf("can't set max speed hz");
		exit(-1);
	}

	if(ini_filename)
	{
		process_ini(ini_filename);
	}
	commandPrompt();
	//close(fd);
	return ret;
}
