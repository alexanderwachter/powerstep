#include "powerstep.h"
powerstep_register ps_abs_pos_ = {
	.addr = PS_ABS_POS,
	.len = PS_ABS_POS_LEN
};

powerstep_register ps_el_pos_ = {
	.addr = PS_EL_POS,
	.len = PS_EL_POS_LEN
};

powerstep_register ps_mark_ = {
	.addr = PS_MARK,
	.len = PS_EL_POS_LEN
};

powerstep_register ps_speed_ = {
	.addr = PS_SPEED,
	.len = PS_SPEED_LEN
};

powerstep_register ps_acc_ = {
	.addr = PS_ACC,
	.len = PS_ACC_LEN
};

powerstep_register ps_dec_ = {
	.addr = PS_DEC,
	.len = PS_DEC_LEN
};

powerstep_register ps_max_speed_ = {
	.addr = PS_MAX_SPEED,
	.len = PS_MAX_SPEED_LEN
};

powerstep_register ps_min_speed_ = {
	.addr = PS_MIN_SPEED,
	.len = PS_MIN_SPEED_LEN
};

powerstep_register ps_kval_hold_ = {
	.addr = PS_KVAL_HOLD,
	.len = PS_KVAL_HOLD_LEN
};

powerstep_register ps_kval_run_ = {
	.addr = PS_KVAL_RUN,
	.len = PS_KVAL_RUN_LEN
};

powerstep_register ps_kval_acc_ = {
	.addr = PS_KVAL_ACC,
	.len = PS_KVAL_ACC_LEN
};

powerstep_register ps_kval_dec_ = {
	.addr = PS_KVAL_DEC,
	.len = PS_KVAL_DEC_LEN
};

powerstep_register ps_init_speed_ = {
	.addr = PS_INT_SPEED,
	.len = PS_INT_SPEED_LEN
};

powerstep_register ps_st_spl_ = {
	.addr = PS_ST_SLP,
	.len = PS_ST_SLP_LEN
};

powerstep_register ps_fn_spl_acc_ = {
	.addr = PS_FN_SLP_ACC,
	.len = PS_FN_SLP_ACC_LEN
};

powerstep_register ps_fn_spl_dec_ = {
	.addr = PS_FN_SLP_DEC,
	.len = PS_FN_SLP_DEC_LEN
};

powerstep_register ps_k_therm_ = {
	.addr = PS_K_THERM,
	.len = PS_K_THERM_LEN
};

powerstep_register ps_stall_th_ = {
	.addr = PS_STALL_TH,
	.len = PS_STALL_TH_LEN
};

powerstep_register ps_config_ = {
	.addr = PS_CONFIG,
	.len = PS_CONFIG_LEN
};

powerstep_register ps_tval_hold_ = {
	.addr = PS_TVAL_HOLD,
	.len = PS_TVAL_HOLD_LEN
};

powerstep_register ps_tval_run_ = {
	.addr = PS_TVAL_RUN,
	.len = PS_TVAL_RUN_LEN
};

powerstep_register ps_tval_acc_ = {
	.addr = PS_TVAL_ACC,
	.len = PS_TVAL_ACC_LEN
};

powerstep_register ps_tval_dec_ = {
	.addr = PS_TVAL_DEC,
	.len = PS_TVAL_DEC_LEN
};

powerstep_register ps_t_fast_ = {
	.addr = PS_T_FAST,
	.len = PS_T_FAST_LEN
};

powerstep_register ps_ton_min_ = {
	.addr = PS_TON_MIN,
	.len = PS_TON_MIN_LEN
};

powerstep_register ps_toff_min_ = {
	.addr = PS_TOFF_MIN,
	.len = PS_TOFF_MIN_LEN
};

powerstep_register ps_adc_out_ = {
	.addr = PS_ADC_OUT,
	.len = PS_ADC_OUT_LEN
};

powerstep_register ps_ocd_th_ = {
	.addr = PS_OCD_TH,
	.len = PS_OCD_TH_LEN
};

powerstep_register ps_fs_spd_ = {
	.addr = PS_FS_SPD,
	.len = PS_FS_SPD_LEN
};

powerstep_register ps_step_mode_ = {
	.addr = PS_STEP_MODE,
	.len = PS_STEP_MODE_LEN
};

powerstep_register ps_alarm_en_ = {
	.addr = PS_ALARM_EN,
	.len = PS_ALARM_EN_LEN
};

powerstep_register ps_gatecfg1_ = {
	.addr = PS_GATECFG1,
	.len = PS_GATECFG1_LEN
};

powerstep_register ps_gatecfg2_ = {
	.addr = PS_GATECFG2,
	.len = PS_GATECFG2_LEN
};

powerstep_register ps_status_ = {
	.addr = PS_STATUS,
	.len = PS_STATUS_LEN
};

powerstep_parameter powerstep_parameter_list[] = {
{
	.name = "abs_pos",
	.reg = &ps_abs_pos_,
	.offset = 0,
	.length = PS_ABS_POS_LEN,
},
{
	.name = "el_pos_micro",
	.reg = &ps_el_pos_,
	.offset = 0,
	.length = PS_EL_POS_LEN-2,
},
{
	.name = "el_pos",
	.reg = &ps_el_pos_,
	.offset = 7,
	.length = 2,
},
{
	.name = "mark",
	.reg = &ps_mark_,
	.offset = 0,
	.length = PS_MARK_LEN,
},
{
	.name = "speed",
	.reg = &ps_speed_,
	.offset = 0,
	.length = PS_SPEED_LEN,
},
{
	.name = "accel",
	.reg = &ps_acc_,
	.offset = 0,
	.length = PS_ACC_LEN,
},
{
	.name = "decel",
	.reg = &ps_dec_,
	.offset = 0,
	.length = PS_DEC_LEN,
},
{
	.name = "max_speed",
	.reg = &ps_max_speed_,
	.offset = 0,
	.length = PS_MAX_SPEED_LEN,
},
{
	.name = "min_speed",
	.reg = &ps_min_speed_,
	.offset = 0,
	.length = PS_MIN_SPEED_LEN-1,
},
{
	.name = "lspd_opt",
	.reg = &ps_min_speed_,
	.offset = 11,
	.length = 1,
},
{
	.name = "fs_spd",
	.reg = &ps_fs_spd_,
	.offset = 0,
	.length = PS_FS_SPD_LEN-1,
},
{
	.name = "boost_mode",
	.reg = &ps_fs_spd_,
	.offset = 10,
	.length = 1,
},
{
	.name = "kval_hold",
	.reg = &ps_kval_hold_,
	.offset = 0,
	.length = PS_KVAL_HOLD_LEN,
},
{
	.name = "kval_run",
	.reg = &ps_kval_run_,
	.offset = 0,
	.length = PS_KVAL_RUN_LEN,
},
{
	.name = "kval_acc",
	.reg = &ps_kval_acc_,
	.offset = 0,
	.length = PS_KVAL_ACC_LEN,
},
{
	.name = "kval_dec",
	.reg = &ps_kval_dec_,
	.offset = 0,
	.length = PS_KVAL_DEC_LEN,
},
{
	.name = "int_speed",
	.reg = &ps_init_speed_,
	.offset = 0,
	.length = PS_INT_SPEED_LEN,
},
{
	.name = "st_spl",
	.reg = &ps_st_spl_,
	.offset = 0,
	.length = PS_ST_SLP_LEN,
},
{
	.name = "fn_slp_acc",
	.reg = &ps_fn_spl_acc_,
	.offset = 0,
	.length = PS_FN_SLP_ACC_LEN,
},
{
	.name = "fn_slp_dec",
	.reg = &ps_fn_spl_dec_,
	.offset = 0,
	.length = PS_FN_SLP_DEC_LEN,
},
{
	.name = "k_therm",
	.reg = &ps_k_therm_,
	.offset = 0,
	.length = PS_K_THERM_LEN,
},
{
	.name = "tval_hold",
	.reg = &ps_tval_hold_,
	.offset = 0,
	.length = PS_TVAL_HOLD_LEN,
},
{
	.name = "tval_run",
	.reg = &ps_tval_run_,
	.offset = 0,
	.length = PS_TVAL_RUN_LEN,
},
{
	.name = "tval_acc",
	.reg = &ps_tval_acc_,
	.offset = 0,
	.length = PS_TVAL_ACC_LEN,
},
{
	.name = "tval_dec",
	.reg = &ps_tval_dec_,
	.offset = 0,
	.length = PS_TVAL_DEC_LEN,
},
{
	.name = "fast_step",
	.reg = &ps_t_fast_,
	.offset = 0,
	.length = 4,
},
{
	.name = "toff_fast",
	.reg = &ps_t_fast_,
	.offset = 4,
	.length = 4,
},
{
	.name = "ton_min",
	.reg = &ps_ton_min_,
	.offset = 0,
	.length = PS_TON_MIN_LEN,
},
{
	.name = "toff_min",
	.reg = &ps_toff_min_,
	.offset = 0,
	.length = PS_TOFF_MIN_LEN,
},
{
	.name = "adc_out",
	.reg = &ps_adc_out_,
	.offset = 0,
	.length = PS_ADC_OUT_LEN,
},
{
	.name = "ocd_th",
	.reg = &ps_ocd_th_,
	.offset = 0,
	.length = PS_OCD_TH_LEN,
},
{
	.name = "stall_th",
	.reg = &ps_stall_th_,
	.offset = 0,
	.length = PS_STALL_TH_LEN,
},
{
	.name = "step_sel",
	.reg = &ps_step_mode_,
	.offset = 0,
	.length = 3,
},
{
	.name = "cm_vm",
	.reg = &ps_step_mode_,
	.offset = 3,
	.length = 1,
},
{
	.name = "sync_sel",
	.reg = &ps_step_mode_,
	.offset = 4,
	.length = 3,
},
{
	.name = "sync_en",
	.reg = &ps_step_mode_,
	.offset = 7,
	.length = 1,
},
{
	.name = "alarm_en_overcurrent",
	.reg = &ps_alarm_en_,
	.offset = 0,
	.length = 1,
},
{
	.name = "alarm_en_therm_sd",
	.reg = &ps_alarm_en_,
	.offset = 1,
	.length = 1,
},
{
	.name = "alarm_en_therm_warn",
	.reg = &ps_alarm_en_,
	.offset = 2,
	.length = 1,
},
{
	.name = "alarm_en_uvlo",
	.reg = &ps_alarm_en_,
	.offset = 3,
	.length = 1,
},
{
	.name = "alarm_en_adc_uvlo",
	.reg = &ps_alarm_en_,
	.offset = 4,
	.length = 1,
},
{
	.name = "alarm_en_stall",
	.reg = &ps_alarm_en_,
	.offset = 5,
	.length = 1,
},
{
	.name = "alarm_en_switch",
	.reg = &ps_alarm_en_,
	.offset = 6,
	.length = 1,
},
{
	.name = "alarm_en_cmd",
	.reg = &ps_alarm_en_,
	.offset = 7,
	.length = 1,
},
{
	.name = "tcc",
	.reg = &ps_gatecfg1_,
	.offset = 0,
	.length = 4,
},
{
	.name = "igate",
	.reg = &ps_gatecfg1_,
	.offset = 5,
	.length = 3,
},
{
	.name = "tboost",
	.reg = &ps_gatecfg1_,
	.offset = 8,
	.length = 3,
},
{
	.name = "wd_en",
	.reg = &ps_gatecfg1_,
	.offset = 11,
	.length = 1,
},
{
	.name = "tdt",
	.reg = &ps_gatecfg2_,
	.offset = 0,
	.length = 5,
},
{
	.name = "tblank",
	.reg = &ps_gatecfg2_,
	.offset = 5,
	.length = 3,
},
{
	.name = "osc_sel",
	.reg = &ps_config_,
	.offset = 0,
	.length = 3,
},
{
	.name = "ext_clk",
	.reg = &ps_config_,
	.offset = 3,
	.length = 1,
},
{
	.name = "sw_mode",
	.reg = &ps_config_,
	.offset = 4,
	.length = 1,
},
{
	.name = "en_vscomp",
	.reg = &ps_config_,
	.offset = 5,
	.length = 1,
},
{
	.name = "oc_sd",
	.reg = &ps_config_,
	.offset = 7,
	.length = 1,
},
{
	.name = "uvloval",
	.reg = &ps_config_,
	.offset = 8,
	.length = 1,
},
{
	.name = "vccval",
	.reg = &ps_config_,
	.offset = 9,
	.length = 1,
},
{
	.name = "fpwm_dec",
	.reg = &ps_config_,
	.offset = 10,
	.length = 3,
},
{
	.name = "fpwm_int",
	.reg = &ps_config_,
	.offset = 13,
	.length = 3,
},
{
	.name = "en_tqreg",
	.reg = &ps_config_,
	.offset = 5,
	.length = 1,
},
{
	.name = "tsw",
	.reg = &ps_config_,
	.offset = 10,
	.length = 4,
},
{0}
};
