/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2021 MediaTek Inc.
*/
#ifndef __MTK_CHARGER_INTF_H__
#define __MTK_CHARGER_INTF_H__

#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/mutex.h>
#include <linux/notifier.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/alarmtimer.h>
#include <mt-plat/v1/charger_type.h>
#include <mt-plat/v1/mtk_charger.h>
#include <mt-plat/v1/mtk_battery.h>

#include <mtk_gauge_time_service.h>

#include <mt-plat/v1/charger_class.h>

struct charger_manager;
struct charger_data;
#ifdef CONFIG_AFC_CHARGER
//yuanjian.wt add for AFC charge
#include "afc_charger_intf.h"
#endif
#ifdef CONFIG_W2_CHARGER_PRIVATE
#include "mtk_pdc.h" //Bug790556,churui1.wt,ADD,20220808,charging pd flag
extern int batt_slate_mode; //Bug773947,churui1.wt,set mode 2 for batt_slate_mode node
#endif

#include "mtk_pe_intf.h"
#include "mtk_pe20_intf.h"
#include "mtk_pe40_intf.h"
#include "mtk_pe50_intf.h"
#include "mtk_pdc_intf.h"
#include "adapter_class.h"
#include "mtk_smartcharging.h"

#define CHARGING_INTERVAL 10
#define CHARGING_FULL_INTERVAL 20

#define CHRLOG_ERROR_LEVEL   1
#define CHRLOG_DEBUG_LEVEL   2

extern int chr_get_debug_level(void);

#define chr_err(fmt, args...)					\
do {								\
	if (chr_get_debug_level() >= CHRLOG_ERROR_LEVEL) {	\
		pr_notice(fmt, ##args);				\
	}							\
} while (0)

#define chr_info(fmt, args...)					\
do {								\
	if (chr_get_debug_level() >= CHRLOG_ERROR_LEVEL) {	\
		pr_notice_ratelimited(fmt, ##args);		\
	}							\
} while (0)

#define chr_debug(fmt, args...)					\
do {								\
	if (chr_get_debug_level() >= CHRLOG_DEBUG_LEVEL) {	\
		pr_notice(fmt, ##args);				\
	}							\
} while (0)

#define CHR_CC		(0x0001)
#define CHR_TOPOFF	(0x0002)
#define CHR_TUNING	(0x0003)
#define CHR_POSTCC	(0x0004)
#define CHR_BATFULL	(0x0005)
#define CHR_ERROR	(0x0006)
#define	CHR_PE40_INIT	(0x0007)
#define	CHR_PE40_CC	(0x0008)
#define	CHR_PE40_TUNING	(0x0009)
#define	CHR_PE40_POSTCC	(0x000A)
#define CHR_PE30	(0x000B)
#define CHR_PE40	(0x000C)
#define CHR_PDC		(0x000D)
#define CHR_PE50_READY	(0x000E)
#define CHR_PE50_RUNNING	(0x000F)
#define CHR_PE50	(0x0010)

/* charging abnormal status */
#define CHG_VBUS_OV_STATUS	(1 << 0)
#define CHG_BAT_OT_STATUS	(1 << 1)
#define CHG_OC_STATUS		(1 << 2)
#define CHG_BAT_OV_STATUS	(1 << 3)
#define CHG_ST_TMO_STATUS	(1 << 4)
#define CHG_BAT_LT_STATUS	(1 << 5)
#define CHG_TYPEC_WD_STATUS	(1 << 6)

/* charger_algorithm notify charger_dev */
enum {
	EVENT_EOC,
	EVENT_RECHARGE,
};

/* charger_dev notify charger_manager */
enum {
	CHARGER_DEV_NOTIFY_VBUS_OVP,
	CHARGER_DEV_NOTIFY_BAT_OVP,
	CHARGER_DEV_NOTIFY_EOC,
	CHARGER_DEV_NOTIFY_RECHG,
	CHARGER_DEV_NOTIFY_SAFETY_TIMEOUT,
	CHARGER_DEV_NOTIFY_VBATOVP_ALARM,
	CHARGER_DEV_NOTIFY_VBUSOVP_ALARM,
	CHARGER_DEV_NOTIFY_IBATOCP,
	CHARGER_DEV_NOTIFY_IBUSOCP,
	CHARGER_DEV_NOTIFY_IBUSUCP_FALL,
	CHARGER_DEV_NOTIFY_VOUTOVP,
	CHARGER_DEV_NOTIFY_VDROVP,
};

/*
 * Software JEITA
 * T0: -10 degree Celsius
 * T1: 0 degree Celsius
 * T2: 10 degree Celsius
 * T3: 45 degree Celsius
 * T4: 50 degree Celsius
 */
enum sw_jeita_state_enum {
	TEMP_BELOW_T0 = 0,
	TEMP_T0_TO_T1,
	TEMP_T1_TO_T2,
	TEMP_T2_TO_T3,
	TEMP_T3_TO_T4,
	TEMP_ABOVE_T4
};

struct sw_jeita_data {
	int sm;
	int pre_sm;
	int cv;
	int cc;//Bug493176,zhaosidong.wt,MODIFY,20191017,SW JEITA configuration
	bool charging;
	bool error_recovery_flag;
};

/* battery thermal protection */
enum bat_temp_state_enum {
	BAT_TEMP_LOW = 0,
	BAT_TEMP_NORMAL,
	BAT_TEMP_HIGH
};

struct battery_thermal_protection_data {
	int sm;
	bool enable_min_charge_temp;
	int min_charge_temp;
	int min_charge_temp_plus_x_degree;
	int max_charge_temp;
	int max_charge_temp_minus_x_degree;
};

struct charger_custom_data {
	int battery_cv;	/* uv */
	int max_charger_voltage;
	int max_charger_voltage_setting;
	int min_charger_voltage;

	int usb_charger_current_suspend;
	int usb_charger_current_unconfigured;
	int usb_charger_current_configured;
	int usb_charger_current;
	int ac_charger_current;
	int ac_charger_input_current;
//zhaosidong.wt, pe20 icl
	int pe20_charger_input_current;
	int non_std_ac_charger_current;
	int charging_host_charger_current;
	int apple_1_0a_charger_current;
	int apple_2_1a_charger_current;
	int usb_unlimited_current;
	int ta_ac_charger_current;
	int pd_charger_current;

	/* dynamic mivr */
	int min_charger_voltage_1;
	int min_charger_voltage_2;
	int max_dmivr_charger_current;

	/* sw jeita */
	int jeita_temp_above_t4_cv;
	int jeita_temp_t3_to_t4_cv;
	int jeita_temp_t2_to_t3_cv;
	int jeita_temp_t1_to_t2_cv;
	int jeita_temp_t0_to_t1_cv;
	int jeita_temp_below_t0_cv;
	int jeita_temp_above_t4_cc;//Bug493176,zhaosidong.wt,ADD,20191017,SW JEITA configuration
	int jeita_temp_t3_to_t4_cc;
	int jeita_temp_t2_to_t3_cc;
	int jeita_temp_t1_to_t2_cc;
	int jeita_temp_t0_to_t1_cc;
	int jeita_temp_below_t0_cc;
	int temp_t4_thres;
	int temp_t4_thres_minus_x_degree;
	int temp_t3_thres;
	int temp_t3_thres_minus_x_degree;
	int temp_t2_thres;
	int temp_t2_thres_plus_x_degree;
	int temp_t1_thres;
	int temp_t1_thres_plus_x_degree;
	int temp_t0_thres;
	int temp_t0_thres_plus_x_degree;
	int temp_neg_10_thres;

//+Bug774000,gudi.wt,ADD,20191126,charge current limit for AP overheat
//+Bug678504,lvyuanchuan.wt,modify,20210813,T =25 charging , the back exceeds 1.29
	int ap_temp_above_t3_cc;
	int ap_temp_t2_to_t3_cc;
//-Bug678504,lvyuanchuan.wt,modify,20210813,T =25 charging , the back exceeds 1.29	
	int ap_temp_t1_to_t2_cc;
	int ap_temp_t0_to_t1_cc;
	int ap_temp_below_t0_cc;
	int ap_temp_high_lcmon_cc;
	int ap_temp_low_lcmon_cc;
//+Bug678504,lvyuanchuan.wt,modify,20210813,T =25 charging , the back exceeds 1.29
	int ap_temp_t3_thres;
	int ap_temp_t3_thres_minus_x_degree;
//-Bug678504,lvyuanchuan.wt,modify,20210813,T =25 charging , the back exceeds 1.29	
	int ap_temp_t2_thres;
	int ap_temp_t2_thres_minus_x_degree;
	int ap_temp_t1_thres;
	int ap_temp_t1_thres_minus_x_degree;
	int ap_temp_t0_thres;
	int ap_temp_t0_thres_minus_x_degree;
	int ap_temp_thres_lcmon;
	int ap_temp_thres_minus_x_degree_lcmon;
//-Bug774000,gudi.wt,ADD,20191126,charge current limit for AP overheat
//+P240111-05098   guhan01.wt 2024031820,Modify the maximum current limit for bright screen charging
#if defined(CONFIG_WT_PROJECT_S96902AA1) || defined(CONFIG_WT_PROJECT_S96901AA1) || defined(CONFIG_WT_PROJECT_S96901WA1)
	int ap_temp_lcmon_above_t4;
	int ap_temp_lcmon_t3_to_t4;
	int ap_temp_lcmon_t2_to_t3;
	int ap_temp_lcmon_t1_to_t2;
	int ap_temp_lcmon_t0_to_t1;
	int ap_temp_lcmon_below_t0;

	int ap_temp_lcmon_t4;
	int ap_temp_lcmon_t3;
	int ap_temp_lcmon_t2;
	int ap_temp_lcmon_t1;
	int ap_temp_lcmon_t0;

	int ap_temp_lcmon_t4_anti_shake;
	int ap_temp_lcmon_t3_anti_shake;
	int ap_temp_lcmon_t2_anti_shake;
	int ap_temp_lcmon_t1_anti_shake;
	int ap_temp_lcmon_t0_anti_shake;
#endif
//-P240111-05098   guhan01.wt 2024031820,Modify the maximum current limit for bright screen charging
/* +churui1.wt, ADD, 20230603, cp charging current limit for AP overheat */
#ifdef CONFIG_N28_CHARGER_PRIVATE
	int ap_temp_above_t4_cp_cc;
	int ap_temp_t3_to_t4_cp_cc;
	int ap_temp_t2_to_t3_cp_cc;
	int ap_temp_t1_to_t2_cp_cc;
	int ap_temp_t0_to_t1_cp_cc;
	int ap_temp_below_t0_cp_cc;
	int ap_temp_high_lcmon_cp_cc;
	int ap_temp_low_lcmon_cp_cc;
//+P240111-05098   guhan01.wt 2024031820,Modify the maximum current limit for bright screen charging
	int ap_temp_lcmon_above_t4;
	int ap_temp_lcmon_t3_to_t4;
	int ap_temp_lcmon_t2_to_t3;
	int ap_temp_lcmon_t1_to_t2;
	int ap_temp_lcmon_t0_to_t1;
	int ap_temp_lcmon_below_t0;
//-P240111-05098   guhan01.wt 2024031820,Modify the maximum current limit for bright screen charging
	int ap_temp_t4_cp_thres;
	int ap_temp_t4_cp_thres_minus_x_degree;
	int ap_temp_t3_cp_thres;
	int ap_temp_t3_cp_thres_minus_x_degree;
	int ap_temp_t2_cp_thres;
	int ap_temp_t2_cp_thres_minus_x_degree;
	int ap_temp_t1_cp_thres;
	int ap_temp_t1_cp_thres_minus_x_degree;
	int ap_temp_t0_cp_thres;
	int ap_temp_t0_cp_thres_minus_x_degree;
	int ap_temp_cp_thres_lcmon;
	int ap_temp_cp_thres_minus_x_degree_lcmon;

//+P240111-05098   guhan01.wt 2024031820,Modify the maximum current limit for bright screen charging
	int ap_temp_lcmon_t4;
	int ap_temp_lcmon_t3;
	int ap_temp_lcmon_t2;
	int ap_temp_lcmon_t1;
	int ap_temp_lcmon_t0;

	int ap_temp_lcmon_t4_anti_shake;
	int ap_temp_lcmon_t3_anti_shake;
	int ap_temp_lcmon_t2_anti_shake;
	int ap_temp_lcmon_t1_anti_shake;
	int ap_temp_lcmon_t0_anti_shake;
//-P240111-05098   guhan01.wt 2024031820,Modify the maximum current limit for bright screen charging
#endif
/* -churui1.wt, ADD, 20230603, cp charging current limit for AP overheat */

	/* battery temperature protection */
	int mtk_temperature_recharge_support;
	int max_charge_temp;
	int max_charge_temp_minus_x_degree;
	int min_charge_temp;
	int min_charge_temp_plus_x_degree;

	/* pe */
	int pe_ichg_level_threshold;	/* ma */
	int ta_ac_12v_input_current;
	int ta_ac_9v_input_current;
	int ta_ac_7v_input_current;
	bool ta_12v_support;
	bool ta_9v_support;

/*yuanjian.wt, start 20191123, add for AFC charge*/
	/*AFC*/
	int afc_start_battery_soc;
	int afc_stop_battery_soc;
	int afc_ichg_level_threshold;
	int afc_pre_input_current;
	int afc_charger_input_current;
	int afc_charger_current;
	int afc_min_charger_voltage;
	int afc_max_charger_voltage;
/*yuanjian.wt, End 20191123, add for AFC charge*/

	/* pe2.0 */
	int pe20_ichg_level_threshold;	/* ma */
	int ta_start_battery_soc;
	int ta_stop_battery_soc;

	/* pe4.0 */
	int pe40_single_charger_input_current;	/* ma */
	int pe40_single_charger_current;
	int pe40_dual_charger_input_current;
	int pe40_dual_charger_chg1_current;
	int pe40_dual_charger_chg2_current;
	int pe40_stop_battery_soc;
	int pe40_max_vbus;
	int pe40_max_ibus;
	int high_temp_to_leave_pe40;
	int high_temp_to_enter_pe40;
	int low_temp_to_leave_pe40;
	int low_temp_to_enter_pe40;

	/* pe4.0 cable impedance threshold (mohm) */
	u32 pe40_r_cable_1a_lower;
	u32 pe40_r_cable_2a_lower;
	u32 pe40_r_cable_3a_lower;

	/* dual charger */
	u32 chg1_ta_ac_charger_current;
	u32 chg2_ta_ac_charger_current;
	int slave_mivr_diff;
	u32 dual_polling_ieoc;

	/* slave charger */
	int chg2_eff;
	bool parallel_vbus;

	/* cable measurement impedance */
	int cable_imp_threshold;
	int vbat_cable_imp_threshold;

	/* bif */
	int bif_threshold1;	/* uv */
	int bif_threshold2;	/* uv */
	int bif_cv_under_threshold2;	/* uv */

	/* power path */
	bool power_path_support;

	int max_charging_time; /* second */

	int bc12_charger;

	/* pd */
	int pd_vbus_upper_bound;
	int pd_vbus_low_bound;
	int pd_ichg_level_threshold;
	int pd_stop_battery_soc;

	int vsys_watt;
	int ibus_err;
};

struct charger_data {
	int force_charging_current;
	int thermal_input_current_limit;
	int thermal_charging_current_limit;
	int input_current_limit;
	int charging_current_limit;
	int disable_charging_count;
	int input_current_limit_by_aicl;
	int junction_temp_min;
	int junction_temp_max;
};

struct charger_manager {
	bool init_done;
	const char *algorithm_name;
	struct platform_device *pdev;
	void	*algorithm_data;
	int usb_state;
	bool usb_unlimited;
	bool disable_charger;

	struct charger_device *chg1_dev;
	struct notifier_block chg1_nb;
	struct charger_data chg1_data;
	struct charger_consumer *chg1_consumer;

	struct charger_device *chg2_dev;
	struct notifier_block chg2_nb;
	struct charger_data chg2_data;

	struct charger_device *dvchg1_dev;
	struct notifier_block dvchg1_nb;
	struct charger_data dvchg1_data;

	struct charger_device *dvchg2_dev;
	struct notifier_block dvchg2_nb;
	struct charger_data dvchg2_data;

	struct adapter_device *pd_adapter;


	enum charger_type chr_type;
	bool can_charging;
	int cable_out_cnt;

	int (*do_algorithm)(struct charger_manager *cm);
	int (*plug_in)(struct charger_manager *cm);
	int (*plug_out)(struct charger_manager *cm);
	int (*do_charging)(struct charger_manager *cm, bool en);
	int (*do_event)(struct notifier_block *nb, unsigned long ev, void *v);
	int (*change_current_setting)(struct charger_manager *cm);

	/* notify charger user */
	struct srcu_notifier_head evt_nh;
	/* receive from battery */
	struct notifier_block psy_nb;
	//+Bug492299,lili5.wt,ADD,20191021,battery Current event and slate mode
	struct notifier_block charger_nb;
	//-Bug492299,lili5.wt,ADD,20191021,battery Current event and slate mode

	/* common info */
	int battery_temp;
	//+Bug516174,zhaosidong.wt,ADD,20191126,charge current limit for AP overheat
	int ap_temp;
	bool lcmoff;
	struct sw_jeita_data ap_thermal_lcmoff;
	struct sw_jeita_data ap_thermal_lcmon;
	//-Bug516174,zhaosidong.wt,ADD,20191126,charge current limit for AP overheat
	/* sw jeita */
	bool enable_sw_jeita;
	struct sw_jeita_data sw_jeita;

	/* dynamic_cv */
	bool enable_dynamic_cv;

	bool cmd_discharging;
	bool safety_timeout;
	bool vbusov_stat;

	/* battery warning */
	unsigned int notify_code;
	unsigned int notify_test_mode;

	/* battery thermal protection */
	struct battery_thermal_protection_data thermal;

	/* dtsi custom data */
	struct charger_custom_data data;

	bool enable_sw_safety_timer;
	bool sw_safety_timer_setting;

	/* High voltage charging */
	bool enable_hv_charging;

#ifdef CONFIG_AFC_CHARGER
	/*AFC*/
	bool enable_afc;
	struct afc_dev afc;
#endif
	bool is_camera_on;
	/* pe */
	bool enable_pe_plus;
	struct mtk_pe pe;

	/* pe 2.0 */
	bool enable_pe_2;
	struct mtk_pe20 pe2;

	/* pe 4.0 */
	bool enable_pe_4;
	bool leave_pe4;
	struct mtk_pe40 pe4;

	/* pe 5.0 */
	bool enable_pe_5;
	bool leave_pe5;
	struct mtk_pe50 pe5;

	/* type-C*/
	bool enable_type_c;

	/* water detection */
	bool water_detected;

	/* pd */
	bool leave_pdc;
	struct mtk_pdc pdc;
	bool disable_pd_dual;
	bool is_pdc_run;

	int pd_type;
	bool pd_reset;

	/* thread related */
	struct hrtimer charger_kthread_timer;

	/* alarm timer */
	struct alarm charger_timer;
	struct timespec endtime;
	bool is_suspend;

	struct wakeup_source *charger_wakelock;
	struct mutex charger_lock;
	struct mutex charger_pd_lock;
	struct mutex cable_out_lock;
	spinlock_t slock;
	unsigned int polling_interval;
	bool charger_thread_timeout;
	wait_queue_head_t  wait_que;
	bool charger_thread_polling;

	/* kpoc */
	atomic_t enable_kpoc_shdn;

	/* ATM */
	bool atm_enabled;

	/* dynamic mivr */
	bool enable_dynamic_mivr;

	struct smartcharging sc;


	/*daemon related*/
	struct sock *daemo_nl_sk;
	u_int g_scd_pid;
	struct scd_cmd_param_t_1 sc_data;

	bool force_disable_pp[TOTAL_CHARGER];
	bool enable_pp[TOTAL_CHARGER];
	struct mutex pp_lock[TOTAL_CHARGER];
};

/* charger related module interface */
extern int charger_manager_notifier(struct charger_manager *info, int event);
extern int mtk_switch_charging_init(struct charger_manager *info);
extern int mtk_switch_charging_init2(struct charger_manager *info);
extern int mtk_dual_switch_charging_init(struct charger_manager *info);
extern int mtk_linear_charging_init(struct charger_manager *info);
extern void _wake_up_charger(struct charger_manager *info);
extern int mtk_get_dynamic_cv(struct charger_manager *info, unsigned int *cv);
extern bool is_dual_charger_supported(struct charger_manager *info);
extern int charger_enable_vbus_ovp(struct charger_manager *pinfo, bool enable);
extern bool is_typec_adapter(struct charger_manager *info);

/* pmic API */
extern unsigned int upmu_get_rgs_chrdet(void);
extern int pmic_get_vbus(void);
extern int pmic_get_charging_current(void);
extern int pmic_get_battery_voltage(void);
extern int pmic_get_bif_battery_voltage(int *vbat);
extern int pmic_is_bif_exist(void);
extern int pmic_enable_hw_vbus_ovp(bool enable);
extern bool pmic_is_battery_exist(void);


extern void notify_adapter_event(enum adapter_type type, enum adapter_event evt,
	void *val);


/* FIXME */
enum usb_state_enum {
	USB_SUSPEND = 0,
	USB_UNCONFIGURED,
	USB_CONFIGURED
};

#if defined(CONFIG_MACH_MT6877) || defined(CONFIG_MACH_MT6893) \
	|| defined(CONFIG_MACH_MT6885) || defined(CONFIG_MACH_MT6785)
bool is_usb_rdy(struct device *dev);
#else
bool __attribute__((weak)) is_usb_rdy(void)
{
	pr_info("%s is not defined\n", __func__);
	return false;
}
#endif

/* procfs */
#define PROC_FOPS_RW(name)						\
static int mtk_chg_##name##_open(struct inode *node, struct file *file)	\
{									\
	return single_open(file, mtk_chg_##name##_show, PDE_DATA(node));\
}									\
static const struct file_operations mtk_chg_##name##_fops = {		\
	.owner = THIS_MODULE,						\
	.open = mtk_chg_##name##_open,					\
	.read = seq_read,						\
	.llseek = seq_lseek,						\
	.release = single_release,					\
	.write = mtk_chg_##name##_write,				\
}

#endif /* __MTK_CHARGER_INTF_H__ */
