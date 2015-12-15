#ifndef _POWERSTEP_H
#define _POWERSTEP_H

#include <stdint.h>
#include "iniparser.h"

/*address map*/
#define PS_ABS_POS        0x01
#define PS_ABS_POS_LEN    22
#define PS_EL_POS         0x02
#define PS_EL_POS_LEN     9
#define PS_MARK           0x03
#define PS_MARK_LEN       22
#define PS_SPEED          0x04
#define PS_SPEED_LEN      20
#define PS_ACC            0x05
#define PS_ACC_LEN        12
#define PS_DEC            0x06
#define PS_DEC_LEN        12
#define PS_MAX_SPEED      0x07
#define PS_MAX_SPEED_LEN  10
#define PS_MIN_SPEED      0x08
#define PS_MIN_SPEED_LEN  12
#define PS_KVAL_HOLD      0x09
#define PS_KVAL_HOLD_LEN  8
#define PS_KVAL_RUN       0x0A
#define PS_KVAL_RUN_LEN   8
#define PS_KVAL_ACC       0x0B
#define PS_KVAL_ACC_LEN   8
#define PS_KVAL_DEC       0x0C
#define PS_KVAL_DEC_LEN   8
#define PS_INT_SPEED      0x0D
#define PS_INT_SPEED_LEN  14
#define PS_ST_SLP         0x0E
#define PS_ST_SLP_LEN     8
#define PS_FN_SLP_ACC     0x0F
#define PS_FN_SLP_ACC_LEN 8
#define PS_FN_SLP_DEC     0x10
#define PS_FN_SLP_DEC_LEN 8
#define PS_K_THERM        0x11
#define PS_K_THERM_LEN    4
#define PS_STALL_TH       0x14
#define PS_STALL_TH_LEN   5
#define PS_CONFIG         0x1A
#define PS_CONFIG_LEN     16
#define PS_TVAL_HOLD      0x09
#define PS_TVAL_HOLD_LEN  8
#define PS_TVAL_RUN       0x0A
#define PS_TVAL_RUN_LEN   8
#define PS_TVAL_ACC       0x0B
#define PS_TVAL_ACC_LEN   8
#define PS_TVAL_DEC       0x0C
#define PS_TVAL_DEC_LEN   8
#define PS_T_FAST         0x0E
#define PS_T_FAST_LEN     8
#define PS_TON_MIN        0x0F
#define PS_TON_MIN_LEN    8
#define PS_TOFF_MIN       0x10
#define PS_TOFF_MIN_LEN   8
#define PS_ADC_OUT        0x12
#define PS_ADC_OUT_LEN    5
#define PS_OCD_TH         0x13
#define PS_OCD_TH_LEN     5
#define PS_FS_SPD         0x15
#define PS_FS_SPD_LEN     11
#define PS_STEP_MODE      0x16
#define PS_STEP_MODE_LEN  8
#define PS_ALARM_EN       0x17
#define PS_ALARM_EN_LEN   8
#define PS_GATECFG1       0x18
#define PS_GATECFG1_LEN   11
#define PS_GATECFG2       0x19
#define PS_GATECFG2_LEN   8
#define PS_STATUS         0x1B
#define PS_STATUS_LEN     16

typedef struct 
{
	uint8_t addr;
	uint8_t len;
}powerstep_register;

typedef struct
{
	char* name;
	powerstep_register* reg;
	int offset;
	int length;
} powerstep_parameter;

typedef struct _powerstep_send_queue_
{
	powerstep_register* reg;
	uint32_t value;
	uint32_t mask;
	struct _powerstep_send_queue_* next;
} powerstep_send_queue;

typedef struct _powerstep_parameter_queue_
{
	powerstep_parameter* parameter;
	struct _powerstep_parameter_queue_* next;
} powerstep_parameter_queue;

extern powerstep_parameter powerstep_parameter_list[];

extern int parse_param_ini(char* , char* , unsigned int );
extern int new_section(char*);
extern void parse_error_function(p_errtype, char, int);

extern int register_set(unsigned int argc, char* argv[]);
extern int ini_command(unsigned int argc, char* argv[]);
extern int run_command(unsigned int argc, char* argv[]);
extern int stepclock_command(unsigned int argc, char* argv[]);
extern int move_command(unsigned int argc, char* argv[]);
extern int goto_command(unsigned int argc, char* argv[]);
extern int gountil_command(unsigned int argc, char* argv[]);
extern int releasesw_command(unsigned int argc, char* argv[]);
extern int gohome_command(unsigned int argc, char* argv[]);
extern int gomark_command(unsigned int argc, char* argv[]);
extern int resetpos_command(unsigned int argc, char* argv[]);
extern int resetdev_command(unsigned int argc, char* argv[]);
extern int softstop_command(unsigned int argc, char* argv[]);
extern int hardstop_command(unsigned int argc, char* argv[]);
extern int softhiz_command(unsigned int argc, char* argv[]);
extern int hardhiz_command(unsigned int argc, char* argv[]);
extern int getStatus_command(unsigned int argc, char* argv[]);
extern int printregisters_command(unsigned int argc, char* argv[]);

#endif
