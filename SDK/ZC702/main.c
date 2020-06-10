/***************************************************************************//**
 *   @file   main.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "config.h"
#if HAVE_AD9361_API
#include "ad9361_api.h"
#else
#include "ad9361.h"
#endif
#include "sleep.h"
#include "parameters.h"
#include "platform.h"
#ifdef CONSOLE_COMMANDS
#ifndef TX_LO_FREQ_SWITCH
#include "command.h"
#include "console.h"
#endif
#endif
#ifdef XILINX_PLATFORM
#include <xil_cache.h>
#endif
#if defined XILINX_PLATFORM || defined LINUX_PLATFORM || defined ALTERA_PLATFORM
#include "adc_core.h"
#include "dac_core.h"
#endif

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
#ifdef CONSOLE_COMMANDS
#ifndef TX_LO_FREQ_SWITCH
extern command cmd_list[];
extern char cmd_no;
extern cmd_function cmd_functions[11];
unsigned char cmd = 0;
double param[5] = { 0, 0, 0, 0, 0 };
char param_no = 0;
int cmd_type = -1;
char invalid_cmd = 0;
char received_cmd[30] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#endif
#endif
#ifdef DDR_LESS
#ifndef CONSOLE_COMMANDS
char received_cmd[30] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint16_t value = 0;
#endif
#endif
#if HAVE_AD9361_API
AD9361_InitParam default_init_param = {
/* Device selection */
        ID_AD9361,	// dev_sel
		/* Identification number */
		0,		//id_no
		/* Reference Clock */
		40000000UL,	//reference_clk_rate
		/* Base Configuration */
		0,		//two_rx_two_tx_mode_enable *** adi,2rx-2tx-mode-enable
		1,		//one_rx_one_tx_mode_use_rx_num *** adi,1rx-1tx-mode-use-rx-num
		2,		//one_rx_one_tx_mode_use_tx_num *** adi,1rx-1tx-mode-use-tx-num
		1,//frequency_division_duplex_mode_enable *** adi,frequency-division-duplex-mode-enable
		0,//frequency_division_duplex_independent_mode_enable *** adi,frequency-division-duplex-independent-mode-enable
		0,//tdd_use_dual_synth_mode_enable *** adi,tdd-use-dual-synth-mode-enable
		0,		//tdd_skip_vco_cal_enable *** adi,tdd-skip-vco-cal-enable
		0,		//tx_fastlock_delay_ns *** adi,tx-fastlock-delay-ns
		0,		//rx_fastlock_delay_ns *** adi,rx-fastlock-delay-ns
		0,//rx_fastlock_pincontrol_enable *** adi,rx-fastlock-pincontrol-enable
		0,//tx_fastlock_pincontrol_enable *** adi,tx-fastlock-pincontrol-enable
		0,		//external_rx_lo_enable *** adi,external-rx-lo-enable
		0,		//external_tx_lo_enable *** adi,external-tx-lo-enable
		5,//dc_offset_tracking_update_event_mask *** adi,dc-offset-tracking-update-event-mask
		6,//dc_offset_attenuation_high_range *** adi,dc-offset-attenuation-high-range
		5,//dc_offset_attenuation_low_range *** adi,dc-offset-attenuation-low-range
		0x28,	//dc_offset_count_high_range *** adi,dc-offset-count-high-range
		0x32,	//dc_offset_count_low_range *** adi,dc-offset-count-low-range
		0,	//split_gain_table_mode_enable *** adi,split-gain-table-mode-enable
		MAX_SYNTH_FREF,	//trx_synthesizer_target_fref_overwrite_hz *** adi,trx-synthesizer-target-fref-overwrite-hz
		0,// qec_tracking_slow_mode_enable *** adi,qec-tracking-slow-mode-enable
		/* ENSM Control */
		0,//ensm_enable_pin_pulse_mode_enable *** adi,ensm-enable-pin-pulse-mode-enable
		0,//ensm_enable_txnrx_control_enable *** adi,ensm-enable-txnrx-control-enable
		/* LO Control */
		2400000000UL,//rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz
		2400000000UL,//tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz
		1,//tx_lo_powerdown_managed_enable *** adi,tx-lo-powerdown-managed-enable
		/* Rate & BW Control */
		{ 983040000, 245760000, 122880000, 61440000, 30720000, 30720000 },// rx_path_clock_frequencies[6] *** adi,rx-path-clock-frequencies
		{ 983040000, 122880000, 122880000, 61440000, 30720000, 30720000 },// tx_path_clock_frequencies[6] *** adi,tx-path-clock-frequencies
		18000000,				//rf_rx_bandwidth_hz *** adi,rf-rx-bandwidth-hz
		18000000,				//rf_tx_bandwidth_hz *** adi,rf-tx-bandwidth-hz
		/* RF Port Control */
		0,		//rx_rf_port_input_select *** adi,rx-rf-port-input-select
		0,		//tx_rf_port_input_select *** adi,tx-rf-port-input-select
		/* TX Attenuation Control */
		10000,	//tx_attenuation_mdB *** adi,tx-attenuation-mdB
		0,//update_tx_gain_in_alert_enable *** adi,update-tx-gain-in-alert-enable
		/* Reference Clock Control */
		0,//xo_disable_use_ext_refclk_enable *** adi,xo-disable-use-ext-refclk-enable
		{ 8, 5920 },//dcxo_coarse_and_fine_tune[2] *** adi,dcxo-coarse-and-fine-tune
		CLKOUT_DISABLE,	//clk_output_mode_select *** adi,clk-output-mode-select
		/* Gain Control */
		2,		//gc_rx1_mode *** adi,gc-rx1-mode
		2,		//gc_rx2_mode *** adi,gc-rx2-mode
		58,	//gc_adc_large_overload_thresh *** adi,gc-adc-large-overload-thresh
		4,		//gc_adc_ovr_sample_size *** adi,gc-adc-ovr-sample-size
		47,	//gc_adc_small_overload_thresh *** adi,gc-adc-small-overload-thresh
		8192,//gc_dec_pow_measurement_duration *** adi,gc-dec-pow-measurement-duration
		0,		//gc_dig_gain_enable *** adi,gc-dig-gain-enable
		800,//gc_lmt_overload_high_thresh *** adi,gc-lmt-overload-high-thresh
		704,	//gc_lmt_overload_low_thresh *** adi,gc-lmt-overload-low-thresh
		24,		//gc_low_power_thresh *** adi,gc-low-power-thresh
		15,		//gc_max_dig_gain *** adi,gc-max-dig-gain
		/* Gain MGC Control */
		2,		//mgc_dec_gain_step *** adi,mgc-dec-gain-step
		2,		//mgc_inc_gain_step *** adi,mgc-inc-gain-step
		0,		//mgc_rx1_ctrl_inp_enable *** adi,mgc-rx1-ctrl-inp-enable
		0,		//mgc_rx2_ctrl_inp_enable *** adi,mgc-rx2-ctrl-inp-enable
		0,//mgc_split_table_ctrl_inp_gain_mode *** adi,mgc-split-table-ctrl-inp-gain-mode
		/* Gain AGC Control */
		10,	//agc_adc_large_overload_exceed_counter *** adi,agc-adc-large-overload-exceed-counter
		2,//agc_adc_large_overload_inc_steps *** adi,agc-adc-large-overload-inc-steps
		0,//agc_adc_lmt_small_overload_prevent_gain_inc_enable *** adi,agc-adc-lmt-small-overload-prevent-gain-inc-enable
		10,	//agc_adc_small_overload_exceed_counter *** adi,agc-adc-small-overload-exceed-counter
		4,		//agc_dig_gain_step_size *** adi,agc-dig-gain-step-size
		3,//agc_dig_saturation_exceed_counter *** adi,agc-dig-saturation-exceed-counter
		1000,// agc_gain_update_interval_us *** adi,agc-gain-update-interval-us
		0,//agc_immed_gain_change_if_large_adc_overload_enable *** adi,agc-immed-gain-change-if-large-adc-overload-enable
		0,//agc_immed_gain_change_if_large_lmt_overload_enable *** adi,agc-immed-gain-change-if-large-lmt-overload-enable
		10,		//agc_inner_thresh_high *** adi,agc-inner-thresh-high
		1,//agc_inner_thresh_high_dec_steps *** adi,agc-inner-thresh-high-dec-steps
		12,		//agc_inner_thresh_low *** adi,agc-inner-thresh-low
		1,//agc_inner_thresh_low_inc_steps *** adi,agc-inner-thresh-low-inc-steps
		10,	//agc_lmt_overload_large_exceed_counter *** adi,agc-lmt-overload-large-exceed-counter
		2,//agc_lmt_overload_large_inc_steps *** adi,agc-lmt-overload-large-inc-steps
		10,	//agc_lmt_overload_small_exceed_counter *** adi,agc-lmt-overload-small-exceed-counter
		5,		//agc_outer_thresh_high *** adi,agc-outer-thresh-high
		2,//agc_outer_thresh_high_dec_steps *** adi,agc-outer-thresh-high-dec-steps
		18,		//agc_outer_thresh_low *** adi,agc-outer-thresh-low
		2,//agc_outer_thresh_low_inc_steps *** adi,agc-outer-thresh-low-inc-steps
		1,//agc_attack_delay_extra_margin_us; *** adi,agc-attack-delay-extra-margin-us
		0,//agc_sync_for_gain_counter_enable *** adi,agc-sync-for-gain-counter-enable
		/* Fast AGC */
		64,	//fagc_dec_pow_measuremnt_duration ***  adi,fagc-dec-pow-measurement-duration
		260,	//fagc_state_wait_time_ns ***  adi,fagc-state-wait-time-ns
		/* Fast AGC - Low Power */
		0,//fagc_allow_agc_gain_increase ***  adi,fagc-allow-agc-gain-increase-enable
		5,//fagc_lp_thresh_increment_time ***  adi,fagc-lp-thresh-increment-time
		1,//fagc_lp_thresh_increment_steps ***  adi,fagc-lp-thresh-increment-steps
		/* Fast AGC - Lock Level (Lock Level is set via slow AGC inner high threshold) */
		1,//fagc_lock_level_lmt_gain_increase_en ***  adi,fagc-lock-level-lmt-gain-increase-enable
		5,//fagc_lock_level_gain_increase_upper_limit ***  adi,fagc-lock-level-gain-increase-upper-limit
		/* Fast AGC - Peak Detectors and Final Settling */
		1,//fagc_lpf_final_settling_steps ***  adi,fagc-lpf-final-settling-steps
		1,//fagc_lmt_final_settling_steps ***  adi,fagc-lmt-final-settling-steps
		3,		//fagc_final_overrange_count ***  adi,fagc-final-overrange-count
		/* Fast AGC - Final Power Test */
		0,//fagc_gain_increase_after_gain_lock_en ***  adi,fagc-gain-increase-after-gain-lock-enable
		/* Fast AGC - Unlocking the Gain */
		0,//fagc_gain_index_type_after_exit_rx_mode ***  adi,fagc-gain-index-type-after-exit-rx-mode
		1,//fagc_use_last_lock_level_for_set_gain_en ***  adi,fagc-use-last-lock-level-for-set-gain-enable
		1,//fagc_rst_gla_stronger_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-stronger-sig-thresh-exceeded-enable
		5,		//fagc_optimized_gain_offset ***  adi,fagc-optimized-gain-offset
		10,	//fagc_rst_gla_stronger_sig_thresh_above_ll ***  adi,fagc-rst-gla-stronger-sig-thresh-above-ll
		1,//fagc_rst_gla_engergy_lost_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-exceeded-enable
		1,//fagc_rst_gla_engergy_lost_goto_optim_gain_en ***  adi,fagc-rst-gla-engergy-lost-goto-optim-gain-enable
		10,	//fagc_rst_gla_engergy_lost_sig_thresh_below_ll ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-below-ll
		8,//fagc_energy_lost_stronger_sig_gain_lock_exit_cnt ***  adi,fagc-energy-lost-stronger-sig-gain-lock-exit-cnt
		1,//fagc_rst_gla_large_adc_overload_en ***  adi,fagc-rst-gla-large-adc-overload-enable
		1,//fagc_rst_gla_large_lmt_overload_en ***  adi,fagc-rst-gla-large-lmt-overload-enable
		0,//fagc_rst_gla_en_agc_pulled_high_en ***  adi,fagc-rst-gla-en-agc-pulled-high-enable
		0,//fagc_rst_gla_if_en_agc_pulled_high_mode ***  adi,fagc-rst-gla-if-en-agc-pulled-high-mode
		64,	//fagc_power_measurement_duration_in_state5 ***  adi,fagc-power-measurement-duration-in-state5
		/* RSSI Control */
		1,		//rssi_delay *** adi,rssi-delay
		1000,	//rssi_duration *** adi,rssi-duration
		3,		//rssi_restart_mode *** adi,rssi-restart-mode
		0,//rssi_unit_is_rx_samples_enable *** adi,rssi-unit-is-rx-samples-enable
		1,		//rssi_wait *** adi,rssi-wait
		/* Aux ADC Control */
		256,	//aux_adc_decimation *** adi,aux-adc-decimation
		40000000UL,	//aux_adc_rate *** adi,aux-adc-rate
		/* AuxDAC Control */
		1,		//aux_dac_manual_mode_enable ***  adi,aux-dac-manual-mode-enable
		0,		//aux_dac1_default_value_mV ***  adi,aux-dac1-default-value-mV
		0,	//aux_dac1_active_in_rx_enable ***  adi,aux-dac1-active-in-rx-enable
		0,	//aux_dac1_active_in_tx_enable ***  adi,aux-dac1-active-in-tx-enable
		0,//aux_dac1_active_in_alert_enable ***  adi,aux-dac1-active-in-alert-enable
		0,		//aux_dac1_rx_delay_us ***  adi,aux-dac1-rx-delay-us
		0,		//aux_dac1_tx_delay_us ***  adi,aux-dac1-tx-delay-us
		0,		//aux_dac2_default_value_mV ***  adi,aux-dac2-default-value-mV
		0,	//aux_dac2_active_in_rx_enable ***  adi,aux-dac2-active-in-rx-enable
		0,	//aux_dac2_active_in_tx_enable ***  adi,aux-dac2-active-in-tx-enable
		0,//aux_dac2_active_in_alert_enable ***  adi,aux-dac2-active-in-alert-enable
		0,		//aux_dac2_rx_delay_us ***  adi,aux-dac2-rx-delay-us
		0,		//aux_dac2_tx_delay_us ***  adi,aux-dac2-tx-delay-us
		/* Temperature Sensor Control */
		256,	//temp_sense_decimation *** adi,temp-sense-decimation
		1000,//temp_sense_measurement_interval_ms *** adi,temp-sense-measurement-interval-ms
		0xCE,	//temp_sense_offset_signed *** adi,temp-sense-offset-signed
		1,//temp_sense_periodic_measurement_enable *** adi,temp-sense-periodic-measurement-enable
		/* Control Out Setup */
		0xFF,	//ctrl_outs_enable_mask *** adi,ctrl-outs-enable-mask
		0,		//ctrl_outs_index *** adi,ctrl-outs-index
		/* External LNA Control */
		0,		//elna_settling_delay_ns *** adi,elna-settling-delay-ns
		0,		//elna_gain_mdB *** adi,elna-gain-mdB
		0,		//elna_bypass_loss_mdB *** adi,elna-bypass-loss-mdB
		0,	//elna_rx1_gpo0_control_enable *** adi,elna-rx1-gpo0-control-enable
		0,	//elna_rx2_gpo1_control_enable *** adi,elna-rx2-gpo1-control-enable
		0,//elna_gaintable_all_index_enable *** adi,elna-gaintable-all-index-enable
		/* Digital Interface Control */
		0,//digital_interface_tune_skip_mode *** adi,digital-interface-tune-skip-mode
		0,//digital_interface_tune_fir_disable *** adi,digital-interface-tune-fir-disable
		1,		//pp_tx_swap_enable *** adi,pp-tx-swap-enable
		1,		//pp_rx_swap_enable *** adi,pp-rx-swap-enable
		0,		//tx_channel_swap_enable *** adi,tx-channel-swap-enable
		0,		//rx_channel_swap_enable *** adi,rx-channel-swap-enable
		1,		//rx_frame_pulse_mode_enable *** adi,rx-frame-pulse-mode-enable
		0,		//two_t_two_r_timing_enable *** adi,2t2r-timing-enable
		0,		//invert_data_bus_enable *** adi,invert-data-bus-enable
		0,		//invert_data_clk_enable *** adi,invert-data-clk-enable
		0,		//fdd_alt_word_order_enable *** adi,fdd-alt-word-order-enable
		0,		//invert_rx_frame_enable *** adi,invert-rx-frame-enable
		0,		//fdd_rx_rate_2tx_enable *** adi,fdd-rx-rate-2tx-enable
		0,		//swap_ports_enable *** adi,swap-ports-enable
		0,		//single_data_rate_enable *** adi,single-data-rate-enable
		1,		//lvds_mode_enable *** adi,lvds-mode-enable
		0,		//half_duplex_mode_enable *** adi,half-duplex-mode-enable
		0,		//single_port_mode_enable *** adi,single-port-mode-enable
		0,		//full_port_enable *** adi,full-port-enable
		0,	//full_duplex_swap_bits_enable *** adi,full-duplex-swap-bits-enable
		0,		//delay_rx_data *** adi,delay-rx-data
		0,		//rx_data_clock_delay *** adi,rx-data-clock-delay
		4,		//rx_data_delay *** adi,rx-data-delay
		7,		//tx_fb_clock_delay *** adi,tx-fb-clock-delay
		0,		//tx_data_delay *** adi,tx-data-delay
#ifdef ALTERA_PLATFORM
		300,	//lvds_bias_mV *** adi,lvds-bias-mV
#else
		150,	//lvds_bias_mV *** adi,lvds-bias-mV
#endif
		1,//lvds_rx_onchip_termination_enable *** adi,lvds-rx-onchip-termination-enable
		0,	//rx1rx2_phase_inversion_en *** adi,rx1-rx2-phase-inversion-enable
		0xFF,	//lvds_invert1_control *** adi,lvds-invert1-control
		0x0F,	//lvds_invert2_control *** adi,lvds-invert2-control
		/* GPO Control */
		0,//gpo0_inactive_state_high_enable *** adi,gpo0-inactive-state-high-enable
		0,//gpo1_inactive_state_high_enable *** adi,gpo1-inactive-state-high-enable
		0,//gpo2_inactive_state_high_enable *** adi,gpo2-inactive-state-high-enable
		0,//gpo3_inactive_state_high_enable *** adi,gpo3-inactive-state-high-enable
		0,		//gpo0_slave_rx_enable *** adi,gpo0-slave-rx-enable
		0,		//gpo0_slave_tx_enable *** adi,gpo0-slave-tx-enable
		0,		//gpo1_slave_rx_enable *** adi,gpo1-slave-rx-enable
		0,		//gpo1_slave_tx_enable *** adi,gpo1-slave-tx-enable
		0,		//gpo2_slave_rx_enable *** adi,gpo2-slave-rx-enable
		0,		//gpo2_slave_tx_enable *** adi,gpo2-slave-tx-enable
		0,		//gpo3_slave_rx_enable *** adi,gpo3-slave-rx-enable
		0,		//gpo3_slave_tx_enable *** adi,gpo3-slave-tx-enable
		0,		//gpo0_rx_delay_us *** adi,gpo0-rx-delay-us
		0,		//gpo0_tx_delay_us *** adi,gpo0-tx-delay-us
		0,		//gpo1_rx_delay_us *** adi,gpo1-rx-delay-us
		0,		//gpo1_tx_delay_us *** adi,gpo1-tx-delay-us
		0,		//gpo2_rx_delay_us *** adi,gpo2-rx-delay-us
		0,		//gpo2_tx_delay_us *** adi,gpo2-tx-delay-us
		0,		//gpo3_rx_delay_us *** adi,gpo3-rx-delay-us
		0,		//gpo3_tx_delay_us *** adi,gpo3-tx-delay-us
		/* Tx Monitor Control */
		37000,	//low_high_gain_threshold_mdB *** adi,txmon-low-high-thresh
		0,		//low_gain_dB *** adi,txmon-low-gain
		24,		//high_gain_dB *** adi,txmon-high-gain
		0,		//tx_mon_track_en *** adi,txmon-dc-tracking-enable
		0,		//one_shot_mode_en *** adi,txmon-one-shot-mode-enable
		511,	//tx_mon_delay *** adi,txmon-delay
		8192,	//tx_mon_duration *** adi,txmon-duration
		2,		//tx1_mon_front_end_gain *** adi,txmon-1-front-end-gain
		2,		//tx2_mon_front_end_gain *** adi,txmon-2-front-end-gain
		48,		//tx1_mon_lo_cm *** adi,txmon-1-lo-cm
		48,		//tx2_mon_lo_cm *** adi,txmon-2-lo-cm
		/* GPIO definitions */
		-1,		//gpio_resetb *** reset-gpios
		/* MCS Sync */
		-1,		//gpio_sync *** sync-gpios
		-1,		//gpio_cal_sw1 *** cal-sw1-gpios
		-1,		//gpio_cal_sw2 *** cal-sw2-gpios
		/* External LO clocks */
		NULL,	//(*ad9361_rfpll_ext_recalc_rate)()
		NULL,	//(*ad9361_rfpll_ext_round_rate)()
		NULL	//(*ad9361_rfpll_ext_set_rate)()
		};
#endif
//AD9361_RXFIRConfig rx_fir_config = {	// BPF PASSBAND 3/20 fs to 1/4 fs
//		3, // rx
//				0, // rx_gain
//				1, // rx_dec
//				{ -4, -6, -37, 35, 186, 86, -284, -315, 107, 219, -4, 271, 558,
//						-307, -1182, -356, 658, 157, 207, 1648, 790, -2525,
//						-2553, 748, 865, -476, 3737, 6560, -3583, -14731, -5278,
//						14819, 14819, -5278, -14731, -3583, 6560, 3737, -476,
//						865, 748, -2553, -2525, 790, 1648, 207, 157, 658, -356,
//						-1182, -307, 558, 271, -4, 219, 107, -315, -284, 86,
//						186, 35, -37, -6, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // rx_coef[128]
//				64, // rx_coef_size
//				{ 0, 0, 0, 0, 0, 0 }, //rx_path_clks[6]
//				0 // rx_bandwidth
//		};

//AD9361_TXFIRConfig tx_fir_config = {	// BPF PASSBAND 3/20 fs to 1/4 fs
//		3, // tx
//				-6, // tx_gain
//				1, // tx_int
//				{ -4, -6, -37, 35, 186, 86, -284, -315, 107, 219, -4, 271, 558,
//						-307, -1182, -356, 658, 157, 207, 1648, 790, -2525,
//						-2553, 748, 865, -476, 3737, 6560, -3583, -14731, -5278,
//						14819, 14819, -5278, -14731, -3583, 6560, 3737, -476,
//						865, 748, -2553, -2525, 790, 1648, 207, 157, 658, -356,
//						-1182, -307, 558, 271, -4, 219, 107, -315, -284, 86,
//						186, 35, -37, -6, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // tx_coef[128]
//				64, // tx_coef_size
//				{ 0, 0, 0, 0, 0, 0 }, // tx_path_clks[6]
//				0 // tx_bandwidth
//		};
struct ad9361_rf_phy *ad9361_phy;
#ifdef FMCOMMS5
struct ad9361_rf_phy *ad9361_phy_b;
#endif

#if !HAVE_AD9361_API
/* Device selection */
#define DEV_SEL 0 //ID_AD9361,	// dev_sel
/* Identification number */
#define ID_NO 0 //	0,		//id_no
/* Reference Clock */
#define REF_CLK_RATE 40000000UL //	40000000UL,	//reference_clk_rate
/* Base Configuration */
#define FREQUENCY_DIVISION_DUPLEX_MODE_ENABLE 1 //	1,		//frequency_division_duplex_mode_enable *** adi,frequency-division-duplex-mode-enable
#define FREQUENCY_DIVISION_DUPLEX_INDEPENDENT_MODE_ENABLE 0 //	0,		//frequency_division_duplex_independent_mode_enable *** adi,frequency-division-duplex-independent-mode-enable
#define TWO_RX_TWO_TX_MODE 0  //	0,		//two_rx_two_tx_mode_enable *** adi,2rx-2tx-mode-enable
#define ONE_RX_ONE_TX_MODE_USE_RX_NUM 1 //	1,		//one_rx_one_tx_mode_use_rx_num *** adi,1rx-1tx-mode-use-rx-num
#define ONE_RX_ONE_TX_MODE_USE_TX_NUM 2 //2,		//one_rx_one_tx_mode_use_tx_num *** adi,1rx-1tx-mode-use-tx-num
#define TDD_USE_DUAL_SYNTH_MODE_ENABLE 0 //	0,		//tdd_use_dual_synth_mode_enable *** adi,tdd-use-dual-synth-mode-enable
#define TDD_SKIP_VCO_CAL_ENABLE 0 //	0,		//tdd_skip_vco_cal_enable *** adi,tdd-skip-vco-cal-enable
#define TX_FASTLOCK_DELAY_NS 0 //	0,		//tx_fastlock_delay_ns *** adi,tx-fastlock-delay-ns
#define RX_FASTLOCK_DELAY_NS 0 //	0,		//rx_fastlock_delay_ns *** adi,rx-fastlock-delay-ns
#define RX_FASTLOCK_PINCONTROL_ENABLE 0 //	0,		//rx_fastlock_pincontrol_enable *** adi,rx-fastlock-pincontrol-enable
#define TX_FASTLOCK_PINCONTROL_ENABLE 0 //	0,		//tx_fastlock_pincontrol_enable *** adi,tx-fastlock-pincontrol-enable
#define EXTERNAL_RX_LO_ENABLE 0 //	0,		//external_rx_lo_enable *** adi,external-rx-lo-enable
#define EXTERNAL_TX_LO_ENABLE 0 //	0,		//external_tx_lo_enable *** adi,external-rx-lo-enable
#define DC_OFFSET_TRACKING_UPDATE_EVENT_MASK 5 //	5,		//dc_offset_tracking_update_event_mask *** adi,dc-offset-tracking-update-event-mask
#define DC_OFFSET_ATTENUATION_HIGH_RANGE 6  //	6,		//dc_offset_attenuation_high_range *** adi,dc-offset-attenuation-high-range
#define DC_OFFSET_ATTENUATION_LOW_RANGE 5 //	5,		//dc_offset_attenuation_low_range *** adi,dc-offset-attenuation-low-range
#define DC_OFFSET_COUNT_HIGH_RANGE 0X28 //	0x28,	//dc_offset_count_high_range *** adi,dc-offset-count-high-range
#define DC_OFFSET_COUNT_LOW_RANGE 0X32 //	0x32,	//dc_offset_count_low_range *** adi,dc-offset-count-low-range
#define SPLIT_GAIN_TABLE_MODE_ENABLE 0 //	0,		//split_gain_table_mode_enable *** adi,split-gain-table-mode-enable
#define TRX_SYNTHESIZER_TARGET_FREF_OVERWIRE_HZ MAX_SYNTH_FREF //	MAX_SYNTH_FREF,	//trx_synthesizer_target_fref_overwrite_hz *** adi,trx-synthesizer-target-fref-overwrite-hz
#define QEC_TRACKING_SLOW_MODE_ENABLE 0 //	0,		// qec_tracking_slow_mode_enable *** adi,qec-tracking-slow-mode-enable
/* ENSM Control */
#define ENSM_ENABLE_PIN_PULSE_MODE_ENABLE 0 //	0,		//ensm_enable_pin_pulse_mode_enable *** adi,ensm-enable-pin-pulse-mode-enable
#define ENSM_ENABLE_TXNRX_CONTROL_ENABLE 0  //	0,		//ensm_enable_txnrx_control_enable *** adi,ensm-enable-txnrx-control-enable
/* LO Control */
#define RX_SYNTHESIZER_FREQUENCY_HZ 2400000000UL  //	2400000000UL,	//rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz
#define TX_SYNTHESIZER_FREQUENCY_HZ 2400000000UL //	2400000000UL,	//tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz
#define TX_LO_POWERDOWN_MANAGED_ENABLE 1  //	1,				//tx_lo_powerdown_managed_enable *** adi,tx-lo-powerdown-managed-enable
/* Rate & BW Control */
#define RF_RX_BANDWIDTH_HZ 18000000  //	18000000,//rf_rx_bandwidth_hz *** adi,rf-rx-bandwidth-hz
#define RF_TX_BANDWIDTH_HZ 18000000 //	18000000,//rf_tx_bandwidth_hz *** adi,rf-tx-bandwidth-hz
/* RF Port Control */
#define RX_RF_PORT_INPUT_SELECT 0 //	0,		//rx_rf_port_input_select *** adi,rx-rf-port-input-select
#define TX_RF_PORT_INPUT_SELECT 0 //	0,		//tx_rf_port_input_select *** adi,tx-rf-port-input-select
/* TX Attenuation Control */
#define TX_ATTENUATION_MDB 10000 //	10000,	//tx_attenuation_mdB *** adi,tx-attenuation-mdB
#define UPDATE_TX_GAIN_IN_ALERT_ENABLE 0 //	0,		//update_tx_gain_in_alert_enable *** adi,update-tx-gain-in-alert-enable
/* Reference Clock Control */
#define XO_DISABLE_USE_EXT_REFCLK_ENABLE 0 //	0,		//xo_disable_use_ext_refclk_enable *** adi,xo-disable-use-ext-refclk-enable
#define CLK_OUTPUT_MODE_SELECT CLKOUT_DISABLE //	CLKOUT_DISABLE,	//clk_output_mode_select *** adi,clk-output-mode-select
/* Control Out Setup */
#define CTRL_OUTS_ENABLE_MASK 0XFF //	0xFF,	//ctrl_outs_enable_mask *** adi,ctrl-outs-enable-mask
#define CTRL_OUTS_INDEX 0 //	0,		//ctrl_outs_index *** adi,ctrl-outs-index
/* Digital Interface Control */
#define DIGITAL_INTERFACE_TUNE_SKIP_MODE 0 //	0,		//digital_interface_tune_skip_mode *** adi,digital-interface-tune-skip-mode
#define DIGITAL_INTERFACE_TURE_FIR_DISABLE 0 //	0,		//digital_interface_tune_fir_disable *** adi,digital-interface-tune-fir-disable
#define PP_TX_SWAP_ENABLE 1 //	1,		//pp_tx_swap_enable *** adi,pp-tx-swap-enable
#define PP_RX_SWAP_ENABLE 1 //	1,		//pp_rx_swap_enable *** adi,pp-rx-swap-enable
#define TX_CHANNEL_SWAP_ENABLE 0 //	0,		//tx_channel_swap_enable *** adi,tx-channel-swap-enable
#define RX_CHANNEL_SWAP_ENABLE 0 //	0,		//rx_channel_swap_enable *** adi,rx-channel-swap-enable
#define RX_FRAME_PULSE_MODE_ENABLE 1 //	1,		//rx_frame_pulse_mode_enable *** adi,rx-frame-pulse-mode-enable
#define TWO_T_TWO_R_TIMING_ENABLE 0 //	0,		//two_t_two_r_timing_enable *** adi,2t2r-timing-enable
#define INVERT_DATA_BUS_ENABLE 0 //	0,		//invert_data_bus_enable *** adi,invert-data-bus-enable
#define INVERT_DATA_CLK_ENABLE 0 //	0,		//invert_data_clk_enable *** adi,invert-data-clk-enable
#define FDD_ALT_WORD_ORDER_ENABLE 0 //	0,		//fdd_alt_word_order_enable *** adi,fdd-alt-word-order-enable
#define INVERT_RX_FRAME_ENABLE 0 //	0,		//invert_rx_frame_enable *** adi,invert-rx-frame-enable
#define FDD_RX_RATE_2TX_ENABLE 0 //	0,		//fdd_rx_rate_2tx_enable *** adi,fdd-rx-rate-2tx-enable
#define SWAP_PORTS_ENABLE 0 //	0,		//swap_ports_enable *** adi,swap-ports-enable
#define SINGLE_DATA_RATE_ENABLE 0  //	0,		//single_data_rate_enable *** adi,single-data-rate-enable
#define LVDS_MODE_ENABLE 1 //	1,		//lvds_mode_enable *** adi,lvds-mode-enable
#define HALF_DUPLEX_MODE_ENABLE 0 //	0,		//half_duplex_mode_enable *** adi,half-duplex-mode-enable
#define SINGLE_PORT_MODE_ENABLE 0 //	0,		//single_port_mode_enable *** adi,single-port-mode-enable
#define FULL_PORT_ENABLE 0 //	0,		//full_port_enable *** adi,full-port-enable
#define FULL_DUPLEX_SWAP_BITS_ENABLE 0 //	0,		//full_duplex_swap_bits_enable *** adi,full-duplex-swap-bits-enable
#define _DELAY_RX_DATA 0 //	0,		//delay_rx_data *** adi,delay-rx-data
#define RX_DATA_CLOCK_DELAY 0 //	0,		//rx_data_clock_delay *** adi,rx-data-clock-delay
#define _RX_DATA_DELAY 4 //	4,		//rx_data_delay *** adi,rx-data-delay
#define TX_FB_CLOCK_DELAY 7 //	7,		//tx_fb_clock_delay *** adi,tx-fb-clock-delay
#define _TX_DATA_DELAY 0 //	0,		//tx_data_delay *** adi,tx-data-delay
#define LVDS_BIAS_MV 150 //	150,	//lvds_bias_mV *** adi,lvds-bias-mV
#define LVDS_RX_ONCHIP_TERMINATION_ENABLE 1 //	1,		//lvds_rx_onchip_termination_enable *** adi,lvds-rx-onchip-termination-enable
#define RX1RX2_PHASE_INVERSION_EN 0 //	0,		//rx1rx2_phase_inversion_en *** adi,rx1-rx2-phase-inversion-enable
#define LVDS_INVERT1_CONTROL 0xFF  //	0xFF,	//lvds_invert1_control *** adi,lvds-invert1-control
#define LVDS_INVERT2_CONTROL 0x0F  //	0x0F,	//lvds_invert2_control *** adi,lvds-invert2-control
/* GPIO definitions */
#define GPIO_RESETB 100 //	-1,		//gpio_resetb *** reset-gpios
/* MCS Sync */
#define GPIO_SYNC -1 //	-1,		//gpio_sync *** sync-gpios
#define GPIO_CAL_SW1 -1 //	-1,		//gpio_cal_sw1 *** cal-sw1-gpios
#define GPIO_CAL_SW2 -1 //	-1,		//gpio_cal_sw2 *** cal-sw2-gpios

int32_t init_phy(struct ad9361_rf_phy **ad9361_phy) {
	struct ad9361_rf_phy *phy;
	phy = (struct ad9361_rf_phy *)zmalloc(sizeof(*phy));
	if (!phy) {
		return -ENOMEM;
	}

	phy->spi = (struct spi_device *)zmalloc(sizeof(*phy->spi));
	if (!phy->spi) {
		return -ENOMEM;
	}

	phy->clk_refin = (struct clk *)zmalloc(sizeof(*phy->clk_refin));
	if (!phy->clk_refin) {
		return -ENOMEM;
	}

	phy->pdata = (struct ad9361_phy_platform_data *)zmalloc(sizeof(*phy->pdata));
	if (!phy->pdata) {
		return -ENOMEM;
	}

	phy->adc_conv = (struct axiadc_converter *)zmalloc(sizeof(*phy->adc_conv));
	if (!phy->adc_conv) {
		return -ENOMEM;
	}

	phy->adc_state = (struct axiadc_state *)zmalloc(sizeof(*phy->adc_state));
	if (!phy->adc_state) {
		return -ENOMEM;
	}
	phy->adc_state->phy = phy;

	/* Device selection */
	phy->dev_sel = DEV_SEL;   //	ID_AD9361,

	/* Identification number */
	phy->spi->id_no = ID_NO;
	phy->id_no = ID_NO;

	/* Reference Clock */
	phy->clk_refin->rate = REF_CLK_RATE;

	/* Base Configuration */
	phy->pdata->fdd = FREQUENCY_DIVISION_DUPLEX_MODE_ENABLE;
	phy->pdata->fdd_independent_mode = FREQUENCY_DIVISION_DUPLEX_INDEPENDENT_MODE_ENABLE;
	phy->pdata->rx2tx2 = TWO_RX_TWO_TX_MODE;
	phy->pdata->rx1tx1_mode_use_rx_num = ONE_RX_ONE_TX_MODE_USE_RX_NUM;
	phy->pdata->rx1tx1_mode_use_tx_num = ONE_RX_ONE_TX_MODE_USE_TX_NUM;
	phy->pdata->tdd_use_dual_synth = TDD_USE_DUAL_SYNTH_MODE_ENABLE;
	phy->pdata->tdd_skip_vco_cal = TDD_SKIP_VCO_CAL_ENABLE;
	phy->pdata->rx_fastlock_delay_ns = RX_FASTLOCK_DELAY_NS;
	phy->pdata->tx_fastlock_delay_ns = TX_FASTLOCK_DELAY_NS;
	phy->pdata->trx_fastlock_pinctrl_en[0] = RX_FASTLOCK_PINCONTROL_ENABLE;
	phy->pdata->trx_fastlock_pinctrl_en[1] = TX_FASTLOCK_PINCONTROL_ENABLE;
//	if (phy->dev_sel == ID_AD9363A) {
//		phy->pdata->use_ext_rx_lo = false;
//		phy->pdata->use_ext_tx_lo = false;
//	} else {
		phy->pdata->use_ext_rx_lo = EXTERNAL_RX_LO_ENABLE;
		phy->pdata->use_ext_tx_lo = EXTERNAL_TX_LO_ENABLE;
//	}

	phy->pdata->dc_offset_update_events = DC_OFFSET_TRACKING_UPDATE_EVENT_MASK;
	phy->pdata->dc_offset_attenuation_high = DC_OFFSET_ATTENUATION_HIGH_RANGE;
	phy->pdata->dc_offset_attenuation_low = DC_OFFSET_ATTENUATION_LOW_RANGE;
	phy->pdata->rf_dc_offset_count_high = DC_OFFSET_COUNT_HIGH_RANGE;
	phy->pdata->rf_dc_offset_count_low = DC_OFFSET_COUNT_LOW_RANGE;
	phy->pdata->split_gt = SPLIT_GAIN_TABLE_MODE_ENABLE;
	phy->pdata->trx_synth_max_fref = TRX_SYNTHESIZER_TARGET_FREF_OVERWIRE_HZ;
	phy->pdata->qec_tracking_slow_mode_en = QEC_TRACKING_SLOW_MODE_ENABLE;

	/* ENSM Control */
	phy->pdata->ensm_pin_pulse_mode = ENSM_ENABLE_PIN_PULSE_MODE_ENABLE;
	phy->pdata->ensm_pin_ctrl = ENSM_ENABLE_TXNRX_CONTROL_ENABLE;

	/* LO Control */
	phy->pdata->rx_synth_freq = RX_SYNTHESIZER_FREQUENCY_HZ;
	phy->pdata->tx_synth_freq = TX_SYNTHESIZER_FREQUENCY_HZ;
	phy->pdata->lo_powerdown_managed_en = TX_LO_POWERDOWN_MANAGED_ENABLE;

	/* Rate & BW Control */
	//	{983040000, 245760000, 122880000, 61440000, 30720000, 30720000},// rx_path_clock_frequencies[6] *** adi,rx-path-clock-frequencies
	phy->pdata->rx_path_clks[0] = 983040000;
	phy->pdata->rx_path_clks[1] = 245760000;
	phy->pdata->rx_path_clks[2] = 122880000;
	phy->pdata->rx_path_clks[3] = 61440000;
	phy->pdata->rx_path_clks[4] = 30720000;
	phy->pdata->rx_path_clks[5] = 30720000;
//	for(i = 0; i < 6; i++) {
//		phy->pdata->rx_path_clks[i] = init_param->rx_path_clock_frequencies[i];
//	}
    //	{983040000, 122880000, 122880000, 61440000, 30720000, 30720000},// tx_path_clock_frequencies[6] *** adi,tx-path-clock-frequencies
	phy->pdata->tx_path_clks[0] = 983040000;
	phy->pdata->tx_path_clks[1] = 122880000;
	phy->pdata->tx_path_clks[2] = 122880000;
	phy->pdata->tx_path_clks[3] = 61440000;
	phy->pdata->tx_path_clks[4] = 30720000;
	phy->pdata->tx_path_clks[5] = 30720000;
//	for(i = 0; i < 6; i++) {
//		phy->pdata->tx_path_clks[i] = init_param->tx_path_clock_frequencies[i];
//	}
	phy->pdata->rf_rx_bandwidth_Hz = RF_RX_BANDWIDTH_HZ;;
	phy->pdata->rf_tx_bandwidth_Hz = RF_TX_BANDWIDTH_HZ;

	/* RF Port Control */
	phy->pdata->rf_rx_input_sel = RX_RF_PORT_INPUT_SELECT;
	phy->pdata->rf_tx_output_sel = TX_RF_PORT_INPUT_SELECT;

	/* TX Attenuation Control */
	phy->pdata->tx_atten = TX_ATTENUATION_MDB;
	phy->pdata->update_tx_gain_via_alert = UPDATE_TX_GAIN_IN_ALERT_ENABLE;

	/* Reference Clock Control */
//	switch (phy->dev_sel) {
//	case ID_AD9363A:
//		phy->pdata->use_extclk = true;
//		break;
//	default:
		phy->pdata->use_extclk = XO_DISABLE_USE_EXT_REFCLK_ENABLE;
//	}
	//	{8, 5920},	//dcxo_coarse_and_fine_tune[2] *** adi,dcxo-coarse-and-fine-tune
	phy->pdata->dcxo_coarse = 8;
	phy->pdata->dcxo_fine = 5920;
	phy->pdata->ad9361_clkout_mode = CLK_OUTPUT_MODE_SELECT;

	/* Control Out Setup */
	phy->pdata->ctrl_outs_ctrl.en_mask = CTRL_OUTS_ENABLE_MASK;
	phy->pdata->ctrl_outs_ctrl.index = CTRL_OUTS_INDEX;

	phy->pdata->dig_interface_tune_skipmode = DIGITAL_INTERFACE_TUNE_SKIP_MODE;
	phy->pdata->dig_interface_tune_fir_disable = DIGITAL_INTERFACE_TURE_FIR_DISABLE;
	phy->pdata->port_ctrl.pp_conf[0] = (PP_TX_SWAP_ENABLE << 7);
	phy->pdata->port_ctrl.pp_conf[0] |= (PP_RX_SWAP_ENABLE << 6);
	phy->pdata->port_ctrl.pp_conf[0] |= (TX_CHANNEL_SWAP_ENABLE << 5);
	phy->pdata->port_ctrl.pp_conf[0] |= (RX_CHANNEL_SWAP_ENABLE << 4);
	phy->pdata->port_ctrl.pp_conf[0] |= (RX_FRAME_PULSE_MODE << 3);
	phy->pdata->port_ctrl.pp_conf[0] |= (TWO_T_TWO_R_TIMING_ENABLE << 2);
	phy->pdata->port_ctrl.pp_conf[0] |= (INVERT_DATA_BUS_ENABLE << 1);
	phy->pdata->port_ctrl.pp_conf[0] |= (INVERT_DATA_CLK_ENABLE << 0);
	phy->pdata->port_ctrl.pp_conf[1] = (FDD_ALT_WORD_ORDER_ENABLE);
	phy->pdata->port_ctrl.pp_conf[1] |= (INVERT_RX_FRAME_ENABLE << 2);
	phy->pdata->port_ctrl.pp_conf[2] = (FDD_RX_RATE_2TX_ENABLE << 7);
	phy->pdata->port_ctrl.pp_conf[2] |= (SWAP_PORTS_ENABLE << 6);
	phy->pdata->port_ctrl.pp_conf[2] |= (SINGLE_DATA_RATE_ENABLE << 5);
	phy->pdata->port_ctrl.pp_conf[2] |= (LVDS_MODE_ENABLE << 4);
	phy->pdata->port_ctrl.pp_conf[2] |= (HALF_DUPLEX_MODE_ENABLE<< 3);
	phy->pdata->port_ctrl.pp_conf[2] |= (SINGLE_PORT_MODE_ENABLE << 2);
	phy->pdata->port_ctrl.pp_conf[2] |= (FULL_PORT_ENABLE << 1);
	phy->pdata->port_ctrl.pp_conf[2] |= (FULL_DUPLEX_SWAP_BITS_ENABLE << 0);
	phy->pdata->port_ctrl.pp_conf[1] |= (_DELAY_RX_DATA & 0x3);
	phy->pdata->port_ctrl.rx_clk_data_delay = DATA_CLK_DELAY(RX_DATA_CLOCK_DELAY);
	phy->pdata->port_ctrl.rx_clk_data_delay |= RX_DATA_DELAY(_RX_DATA_DELAY);
	phy->pdata->port_ctrl.tx_clk_data_delay = FB_CLK_DELAY(TX_FB_CLOCK_DELAY);
	phy->pdata->port_ctrl.tx_clk_data_delay |= TX_DATA_DELAY(_TX_DATA_DELAY);
	phy->pdata->port_ctrl.lvds_bias_ctrl = ((LVDS_BIAS_MV - 75) / 75) &0x7;
	phy->pdata->port_ctrl.lvds_bias_ctrl |= (LVDS_RX_ONCHIP_TERMINATION_ENABLE << 5);
	phy->pdata->rx1rx2_phase_inversion_en = RX1RX2_PHASE_INVERSION_EN;

	phy->pdata->debug_mode = true;
	phy->pdata->gpio_resetb = GPIO_RESETB;
	/* Optional: next three GPIOs are used for MCS synchronization */
	phy->pdata->gpio_sync = GPIO_SYNC;
	phy->pdata->gpio_cal_sw1 = GPIO_CAL_SW1;
	phy->pdata->gpio_cal_sw2 = GPIO_CAL_SW2;

	phy->pdata->port_ctrl.digital_io_ctrl = 0;
	phy->pdata->port_ctrl.lvds_invert[0] = LVDS_INVERT1_CONTROL;
	phy->pdata->port_ctrl.lvds_invert[1] = LVDS_INVERT2_CONTROL;

	//	if (AD9364_DEVICE) {
	//		phy->pdata->rx2tx2 = false;
	//		phy->pdata->rx1tx1_mode_use_rx_num = 1;
	//		phy->pdata->rx1tx1_mode_use_tx_num = 1;
	//	}

	struct axiadc_chip_info axiadc_chip_info_tbl[] = {
		{
			"4_CH_DEV",
			4
		},
		{
			"2_CH_DEV",
			2
		},
	};
	phy->adc_conv->chip_info = &axiadc_chip_info_tbl[phy->pdata->rx2tx2 ? ID_AD9361 : ID_AD9364];

	phy->rx_eq_2tx = false;

	phy->current_table = -1;
	phy->bypass_tx_fir = true;
	phy->bypass_rx_fir = true;
	phy->rate_governor = 1;
	phy->rfdc_track_en = true;
	phy->bbdc_track_en = true;
	phy->quad_track_en = true;

	phy->bist_loopback_mode = 0;
	phy->bist_config = 0;
	phy->bist_prbs_mode = BIST_DISABLE;
	phy->bist_tone_mode = BIST_DISABLE;
	phy->bist_tone_freq_Hz = 0;
	phy->bist_tone_level_dB = 0;
	phy->bist_tone_mask = 0;

	ad9361_reset(phy);

	int32_t ret = ad9361_spi_read(phy->spi, REG_PRODUCT_ID);
	if ((ret & PRODUCT_ID_MASK) != PRODUCT_ID_9361) {
		printf("%s : Unsupported PRODUCT_ID 0x%X", __func__, (unsigned int)ret);
		ret = -ENODEV;
		goto out;
	}
	int32_t rev = ret & REV_MASK;

	//	/* External LO clocks */
	//	NULL,	//(*ad9361_rfpll_ext_recalc_rate)()
	//	NULL,	//(*ad9361_rfpll_ext_round_rate)()
	//	NULL	//(*ad9361_rfpll_ext_set_rate)()
	phy->ad9361_rfpll_ext_recalc_rate = NULL;
	phy->ad9361_rfpll_ext_round_rate = NULL;
	phy->ad9361_rfpll_ext_set_rate = NULL;

	ret = register_clocks(phy);
	if (ret < 0)
		goto out;

#ifndef AXI_ADC_NOT_PRESENT
	axiadc_init(phy);
	phy->adc_state->pcore_version = axiadc_read(phy->adc_state, ADI_REG_VERSION);
#endif

	ret = ad9361_setup(phy);
	if (ret < 0)
		goto out;

#ifndef AXI_ADC_NOT_PRESENT
	/* platform specific wrapper to call ad9361_post_setup() */
	ret = axiadc_post_setup(phy);
	if (ret < 0)
		goto out;
#endif

	printf("%s : AD936x Rev %d successfully initialized\n", __func__, (int)rev);

	*ad9361_phy = phy;

	return 0;

out:
	free(phy->spi);
#ifndef AXI_ADC_NOT_PRESENT
	free(phy->adc_conv);
	free(phy->adc_state);
#endif
	free(phy->clk_refin);
	free(phy->pdata);
	free(phy);
	printf("%s : AD936x initialization error\n", __func__);

	return -ENODEV;
}
#endif

/***************************************************************************//**
 * @brief main
 *******************************************************************************/
int main(void) {
#ifdef XILINX_PLATFORM
	Xil_ICacheEnable();
	Xil_DCacheEnable();
#endif

	printf("sajad\r\n");

#ifdef ALTERA_PLATFORM
	if (altera_bridge_init()) {
		printf("Altera Bridge Init Error!\n");
		return -1;
	}
#endif

	// NOTE: The user has to choose the GPIO numbers according to desired
	// carrier board.
#if HAVE_AD9361_API
	default_init_param.gpio_resetb = GPIO_RESET_PIN;
#endif
#ifdef FMCOMMS5
	default_init_param.gpio_sync = GPIO_SYNC_PIN;
	default_init_param.gpio_cal_sw1 = GPIO_CAL_SW1_PIN;
	default_init_param.gpio_cal_sw2 = GPIO_CAL_SW2_PIN;
	default_init_param.rx1rx2_phase_inversion_en = 1;
#else
//#if HAVE_AD9361_API
//	default_init_param.gpio_sync = -1;
//	default_init_param.gpio_cal_sw1 = -1;
//	default_init_param.gpio_cal_sw2 = -1;
//#endif
#endif

#ifdef LINUX_PLATFORM
	gpio_init(default_init_param.gpio_resetb);
#else
	gpio_init(GPIO_DEVICE_ID);
#endif
#if HAVE_AD9361_API
	gpio_direction(default_init_param.gpio_resetb, 1);
#else
	gpio_direction(GPIO_RESETB, 1);
#endif

	spi_init(SPI_DEVICE_ID, 1, 0);

//#if HAVE_AD9361_API
//	if (AD9364_DEVICE)
//		default_init_param.dev_sel = ID_AD9364;
//	if (AD9363A_DEVICE)
//		default_init_param.dev_sel = ID_AD9363A;
//#endif

#if defined FMCOMMS5 || defined ADI_RF_SOM || defined ADI_RF_SOM_CMOS
	default_init_param.xo_disable_use_ext_refclk_enable = 1;
#endif

#ifdef ADI_RF_SOM_CMOS
	default_init_param.swap_ports_enable = 1;
	default_init_param.lvds_mode_enable = 0;
	default_init_param.lvds_rx_onchip_termination_enable = 0;
	default_init_param.full_port_enable = 1;
	default_init_param.digital_interface_tune_fir_disable = 1;
#endif
#if HAVE_AD9361_API
	ad9361_init(&ad9361_phy, &default_init_param);
#else
	init_phy(&ad9361_phy);
#endif

//	printf("sizeof *phy=%d\n",sizeof(ad9361_phy));

//	ad9361_set_tx_fir_config(ad9361_phy, tx_fir_config);
//	ad9361_set_rx_fir_config(ad9361_phy, rx_fir_config);

#ifdef FMCOMMS5
#ifdef LINUX_PLATFORM
	gpio_init(default_init_param.gpio_sync);
#endif
	gpio_direction(default_init_param.gpio_sync, 1);
	default_init_param.id_no = 1;
	default_init_param.gpio_resetb = GPIO_RESET_PIN_2;
#ifdef LINUX_PLATFORM
	gpio_init(default_init_param.gpio_resetb);
#endif
	default_init_param.gpio_sync = -1;
	default_init_param.gpio_cal_sw1 = -1;
	default_init_param.gpio_cal_sw2 = -1;
	default_init_param.rx_synthesizer_frequency_hz = 2300000000UL;
	default_init_param.tx_synthesizer_frequency_hz = 2300000000UL;
	gpio_direction(default_init_param.gpio_resetb, 1);
	ad9361_init(&ad9361_phy_b, &default_init_param);

//	ad9361_set_tx_fir_config(ad9361_phy_b, tx_fir_config);
//	ad9361_set_rx_fir_config(ad9361_phy_b, rx_fir_config);
#endif

#ifndef AXI_ADC_NOT_PRESENT
#if defined XILINX_PLATFORM || defined LINUX_PLATFORM || defined ALTERA_PLATFORM
#ifdef DAC_DMA_EXAMPLE
#ifdef FMCOMMS5
	dac_init(ad9361_phy_b, DATA_SEL_DMA, 0);
#endif
	dac_init(ad9361_phy, DATA_SEL_DMA, 1);
#else
#ifdef FMCOMMS5
	dac_init(ad9361_phy_b, DATA_SEL_DDS, 0);
#endif
	dac_init(ad9361_phy, DATA_SEL_DDS, 1);
#endif
#endif
#endif

#ifdef FMCOMMS5
	ad9361_do_mcs(ad9361_phy, ad9361_phy_b);
#endif

#ifndef AXI_ADC_NOT_PRESENT
#if (defined XILINX_PLATFORM || defined ALTERA_PLATFORM) && \
	(defined ADC_DMA_EXAMPLE || defined ADC_DMA_IRQ_EXAMPLE)
	// NOTE: To prevent unwanted data loss, it's recommended to invalidate
	// cache after each adc_capture() call, keeping in mind that the
	// size of the capture and the start address must be alinged to the size
	// of the cache line.
//	mdelay(1000);
//	adc_capture(16384, ADC_DDR_BASEADDR);
#ifdef XILINX_PLATFORM
#ifdef FMCOMMS5
	Xil_DCacheInvalidateRange(ADC_DDR_BASEADDR, 16384 * 16);
#else
	Xil_DCacheInvalidateRange(ADC_DDR_BASEADDR,
			ad9361_phy->pdata->rx2tx2 ? 16384 * 8 : 16384 * 4);
#endif
#endif
#endif
#endif

#ifdef ADC_CAPTURE
	mdelay(1000);
	uint8_t *adc_buf = (uint8_t*) ADC_DDR_BASEADDR;
	for(int i=0; i<16384*2; i++) {
		adc_buf[i] = 0;
	}
	adc_capture(16384, ADC_DDR_BASEADDR);
	int16_t* buf = (int16_t*) ADC_DDR_BASEADDR;
	for (int i = 0; i < 10; i++) {
		printf("buf[%d]=%d\n", i, buf[i * 4 + 1]);
	}
#endif

#ifdef TX_LO_FREQ_SWITCH
	uint64_t freq = 2400;
	int count = 1;
	while (1) {
		freq = freq + count;


		clk_set_rate(ad9361_phy, ad9361_phy->ref_clk_scale[RX_RFPLL],
										ad9361_to_clk(freq * 1000000+10000));
		usleep(90000);
		clk_set_rate(ad9361_phy, ad9361_phy->ref_clk_scale[TX_RFPLL],
						ad9361_to_clk(freq * 1000000));

		usleep(90000);
		if (freq == 2410)
			count = -1;
		if (freq == 2390)
			count = 1;
	}

//	uint64_t freq = 500;
//	int count = 50;
//	while (1) {
//		freq = freq + count;
//		clk_set_rate(ad9361_phy, ad9361_phy->ref_clk_scale[TX_RFPLL],
//				ad9361_to_clk(freq * 1000000));
//		usleep(90000);
//		if (freq == 6000)
//			count = -50;
//		if (freq == 500)
//			count = 50;
//	}

#endif

#ifdef CONSOLE_COMMANDS
#ifndef TX_LO_FREQ_SWITCH
	get_help(NULL, 0);

	while(1)
	{
		console_get_command(received_cmd);
		invalid_cmd = 0;
		for(cmd = 0; cmd < cmd_no; cmd++)
		{
			param_no = 0;
			cmd_type = console_check_commands(received_cmd, cmd_list[cmd].name,
					param, &param_no);
			if(cmd_type == UNKNOWN_CMD)
			{
				invalid_cmd++;
			}
			else
			{
				cmd_list[cmd].function(param, param_no);
			}
		}
		if(invalid_cmd == cmd_no)
		{
			console_print("Invalid command!\n");
		}
	}
#endif
#endif

#ifdef DDR_LESS
	while(1) {
		printf("ddrless@root>> ");
		char		  received_char	= 0;
		unsigned char char_number	= 0;
		while((received_char != '\n') && (received_char != '\r'))
		{
			received_char = getchar();
			printf("\n %c",received_char);
//			uart_read_char(&received_char);
			received_cmd[char_number++] = received_char;
		}
//		received_cmd[char_number] = 0;
		printf("value: 0x%x\n",value);
	}
#endif

	printf("Done.\n");

#ifdef TDD_SWITCH_STATE_EXAMPLE
	uint32_t ensm_mode;
	if (!ad9361_phy->pdata->fdd) {
		if (ad9361_phy->pdata->ensm_pin_ctrl) {
			gpio_direction(GPIO_ENABLE_PIN, 1);
			gpio_direction(GPIO_TXNRX_PIN, 1);
			gpio_set_value(GPIO_ENABLE_PIN, 0);
			gpio_set_value(GPIO_TXNRX_PIN, 0);
			udelay(10);
			ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
			printf("TXNRX control - Alert: %s\n",
					ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
			mdelay(1000);

			if (ad9361_phy->pdata->ensm_pin_pulse_mode) {
				while(1) {
					gpio_set_value(GPIO_TXNRX_PIN, 0);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - RX: %s\n",
							ensm_mode == ENSM_MODE_RX ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - Alert: %s\n",
							ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_TXNRX_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - TX: %s\n",
							ensm_mode == ENSM_MODE_TX ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 0);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX Pulse control - Alert: %s\n",
							ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					mdelay(1000);
				}
			} else {
				while(1) {
					gpio_set_value(GPIO_TXNRX_PIN, 0);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - RX: %s\n",
							ensm_mode == ENSM_MODE_RX ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_ENABLE_PIN, 0);
					udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - Alert: %s\n",
							ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_TXNRX_PIN, 1);
					udelay(10);
					gpio_set_value(GPIO_ENABLE_PIN, 1);
					udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - TX: %s\n",
							ensm_mode == ENSM_MODE_TX ? "OK" : "Error");
					mdelay(1000);

					gpio_set_value(GPIO_ENABLE_PIN, 0);
					udelay(10);
					ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
					printf("TXNRX control - Alert: %s\n",
							ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
					mdelay(1000);
				}
			}
		} else {
			while(1) {
				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_RX);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - RX: %s\n",
						ensm_mode == ENSM_MODE_RX ? "OK" : "Error");
				mdelay(1000);

				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_ALERT);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - Alert: %s\n",
						ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
				mdelay(1000);

				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_TX);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - TX: %s\n",
						ensm_mode == ENSM_MODE_TX ? "OK" : "Error");
				mdelay(1000);

				ad9361_set_en_state_machine_mode(ad9361_phy, ENSM_MODE_ALERT);
				ad9361_get_en_state_machine_mode(ad9361_phy, &ensm_mode);
				printf("SPI control - Alert: %s\n",
						ensm_mode == ENSM_MODE_ALERT ? "OK" : "Error");
				mdelay(1000);
			}
		}
	}
#endif

#ifdef XILINX_PLATFORM
	Xil_DCacheDisable();
	Xil_ICacheDisable();
#endif

#ifdef ALTERA_PLATFORM
	if (altera_bridge_uninit()) {
		printf("Altera Bridge Uninit Error!\n");
		return -1;
	}
#endif

	return 0;
}
