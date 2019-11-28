
#include "my_code.h"
#include "ad9361.h"
#include "common.h"
#include "util.h"

//AD9361_InitParam default_init_param = {
//	/* Device selection */
//	ID_AD9361,	// dev_sel
//	/* Identification number */
//	0,		//id_no
//	/* Reference Clock */
//	40000000UL,	//reference_clk_rate
//	/* Base Configuration */
//	1,		//two_rx_two_tx_mode_enable *** adi,2rx-2tx-mode-enable
//	1,		//one_rx_one_tx_mode_use_rx_num *** adi,1rx-1tx-mode-use-rx-num
//	1,		//one_rx_one_tx_mode_use_tx_num *** adi,1rx-1tx-mode-use-tx-num
//	1,		//frequency_division_duplex_mode_enable *** adi,frequency-division-duplex-mode-enable
//	0,		//frequency_division_duplex_independent_mode_enable *** adi,frequency-division-duplex-independent-mode-enable
//	0,		//tdd_use_dual_synth_mode_enable *** adi,tdd-use-dual-synth-mode-enable
//	0,		//tdd_skip_vco_cal_enable *** adi,tdd-skip-vco-cal-enable
//	0,		//tx_fastlock_delay_ns *** adi,tx-fastlock-delay-ns
//	0,		//rx_fastlock_delay_ns *** adi,rx-fastlock-delay-ns
//	0,		//rx_fastlock_pincontrol_enable *** adi,rx-fastlock-pincontrol-enable
//	0,		//tx_fastlock_pincontrol_enable *** adi,tx-fastlock-pincontrol-enable
//	0,		//external_rx_lo_enable *** adi,external-rx-lo-enable
//	0,		//external_tx_lo_enable *** adi,external-tx-lo-enable
//	5,		//dc_offset_tracking_update_event_mask *** adi,dc-offset-tracking-update-event-mask
//	6,		//dc_offset_attenuation_high_range *** adi,dc-offset-attenuation-high-range
//	5,		//dc_offset_attenuation_low_range *** adi,dc-offset-attenuation-low-range
//	0x28,	//dc_offset_count_high_range *** adi,dc-offset-count-high-range
//	0x32,	//dc_offset_count_low_range *** adi,dc-offset-count-low-range
//	0,		//split_gain_table_mode_enable *** adi,split-gain-table-mode-enable
//	MAX_SYNTH_FREF,	//trx_synthesizer_target_fref_overwrite_hz *** adi,trx-synthesizer-target-fref-overwrite-hz
//	0,		// qec_tracking_slow_mode_enable *** adi,qec-tracking-slow-mode-enable
//	/* ENSM Control */
//	0,		//ensm_enable_pin_pulse_mode_enable *** adi,ensm-enable-pin-pulse-mode-enable
//	0,		//ensm_enable_txnrx_control_enable *** adi,ensm-enable-txnrx-control-enable
//	/* LO Control */
//	2400000000UL,	//rx_synthesizer_frequency_hz *** adi,rx-synthesizer-frequency-hz
//	2400000000UL,	//tx_synthesizer_frequency_hz *** adi,tx-synthesizer-frequency-hz
//	1,				//tx_lo_powerdown_managed_enable *** adi,tx-lo-powerdown-managed-enable
//	/* Rate & BW Control */
//	{983040000, 245760000, 122880000, 61440000, 30720000, 30720000},// rx_path_clock_frequencies[6] *** adi,rx-path-clock-frequencies
//	{983040000, 122880000, 122880000, 61440000, 30720000, 30720000},// tx_path_clock_frequencies[6] *** adi,tx-path-clock-frequencies
//	18000000,//rf_rx_bandwidth_hz *** adi,rf-rx-bandwidth-hz
//	18000000,//rf_tx_bandwidth_hz *** adi,rf-tx-bandwidth-hz
//	/* RF Port Control */
//	0,		//rx_rf_port_input_select *** adi,rx-rf-port-input-select
//	0,		//tx_rf_port_input_select *** adi,tx-rf-port-input-select
//	/* TX Attenuation Control */
//	10000,	//tx_attenuation_mdB *** adi,tx-attenuation-mdB
//	0,		//update_tx_gain_in_alert_enable *** adi,update-tx-gain-in-alert-enable
//	/* Reference Clock Control */
//	0,		//xo_disable_use_ext_refclk_enable *** adi,xo-disable-use-ext-refclk-enable
//	{8, 5920},	//dcxo_coarse_and_fine_tune[2] *** adi,dcxo-coarse-and-fine-tune
//	CLKOUT_DISABLE,	//clk_output_mode_select *** adi,clk-output-mode-select
//	/* Gain Control */
//	2,		//gc_rx1_mode *** adi,gc-rx1-mode
//	2,		//gc_rx2_mode *** adi,gc-rx2-mode
//	58,		//gc_adc_large_overload_thresh *** adi,gc-adc-large-overload-thresh
//	4,		//gc_adc_ovr_sample_size *** adi,gc-adc-ovr-sample-size
//	47,		//gc_adc_small_overload_thresh *** adi,gc-adc-small-overload-thresh
//	8192,	//gc_dec_pow_measurement_duration *** adi,gc-dec-pow-measurement-duration
//	0,		//gc_dig_gain_enable *** adi,gc-dig-gain-enable
//	800,	//gc_lmt_overload_high_thresh *** adi,gc-lmt-overload-high-thresh
//	704,	//gc_lmt_overload_low_thresh *** adi,gc-lmt-overload-low-thresh
//	24,		//gc_low_power_thresh *** adi,gc-low-power-thresh
//	15,		//gc_max_dig_gain *** adi,gc-max-dig-gain
//	/* Gain MGC Control */
//	2,		//mgc_dec_gain_step *** adi,mgc-dec-gain-step
//	2,		//mgc_inc_gain_step *** adi,mgc-inc-gain-step
//	0,		//mgc_rx1_ctrl_inp_enable *** adi,mgc-rx1-ctrl-inp-enable
//	0,		//mgc_rx2_ctrl_inp_enable *** adi,mgc-rx2-ctrl-inp-enable
//	0,		//mgc_split_table_ctrl_inp_gain_mode *** adi,mgc-split-table-ctrl-inp-gain-mode
//	/* Gain AGC Control */
//	10,		//agc_adc_large_overload_exceed_counter *** adi,agc-adc-large-overload-exceed-counter
//	2,		//agc_adc_large_overload_inc_steps *** adi,agc-adc-large-overload-inc-steps
//	0,		//agc_adc_lmt_small_overload_prevent_gain_inc_enable *** adi,agc-adc-lmt-small-overload-prevent-gain-inc-enable
//	10,		//agc_adc_small_overload_exceed_counter *** adi,agc-adc-small-overload-exceed-counter
//	4,		//agc_dig_gain_step_size *** adi,agc-dig-gain-step-size
//	3,		//agc_dig_saturation_exceed_counter *** adi,agc-dig-saturation-exceed-counter
//	1000,	// agc_gain_update_interval_us *** adi,agc-gain-update-interval-us
//	0,		//agc_immed_gain_change_if_large_adc_overload_enable *** adi,agc-immed-gain-change-if-large-adc-overload-enable
//	0,		//agc_immed_gain_change_if_large_lmt_overload_enable *** adi,agc-immed-gain-change-if-large-lmt-overload-enable
//	10,		//agc_inner_thresh_high *** adi,agc-inner-thresh-high
//	1,		//agc_inner_thresh_high_dec_steps *** adi,agc-inner-thresh-high-dec-steps
//	12,		//agc_inner_thresh_low *** adi,agc-inner-thresh-low
//	1,		//agc_inner_thresh_low_inc_steps *** adi,agc-inner-thresh-low-inc-steps
//	10,		//agc_lmt_overload_large_exceed_counter *** adi,agc-lmt-overload-large-exceed-counter
//	2,		//agc_lmt_overload_large_inc_steps *** adi,agc-lmt-overload-large-inc-steps
//	10,		//agc_lmt_overload_small_exceed_counter *** adi,agc-lmt-overload-small-exceed-counter
//	5,		//agc_outer_thresh_high *** adi,agc-outer-thresh-high
//	2,		//agc_outer_thresh_high_dec_steps *** adi,agc-outer-thresh-high-dec-steps
//	18,		//agc_outer_thresh_low *** adi,agc-outer-thresh-low
//	2,		//agc_outer_thresh_low_inc_steps *** adi,agc-outer-thresh-low-inc-steps
//	1,		//agc_attack_delay_extra_margin_us; *** adi,agc-attack-delay-extra-margin-us
//	0,		//agc_sync_for_gain_counter_enable *** adi,agc-sync-for-gain-counter-enable
//	/* Fast AGC */
//	64,		//fagc_dec_pow_measuremnt_duration ***  adi,fagc-dec-pow-measurement-duration
//	260,	//fagc_state_wait_time_ns ***  adi,fagc-state-wait-time-ns
//	/* Fast AGC - Low Power */
//	0,		//fagc_allow_agc_gain_increase ***  adi,fagc-allow-agc-gain-increase-enable
//	5,		//fagc_lp_thresh_increment_time ***  adi,fagc-lp-thresh-increment-time
//	1,		//fagc_lp_thresh_increment_steps ***  adi,fagc-lp-thresh-increment-steps
//	/* Fast AGC - Lock Level (Lock Level is set via slow AGC inner high threshold) */
//	1,		//fagc_lock_level_lmt_gain_increase_en ***  adi,fagc-lock-level-lmt-gain-increase-enable
//	5,		//fagc_lock_level_gain_increase_upper_limit ***  adi,fagc-lock-level-gain-increase-upper-limit
//	/* Fast AGC - Peak Detectors and Final Settling */
//	1,		//fagc_lpf_final_settling_steps ***  adi,fagc-lpf-final-settling-steps
//	1,		//fagc_lmt_final_settling_steps ***  adi,fagc-lmt-final-settling-steps
//	3,		//fagc_final_overrange_count ***  adi,fagc-final-overrange-count
//	/* Fast AGC - Final Power Test */
//	0,		//fagc_gain_increase_after_gain_lock_en ***  adi,fagc-gain-increase-after-gain-lock-enable
//	/* Fast AGC - Unlocking the Gain */
//	0,		//fagc_gain_index_type_after_exit_rx_mode ***  adi,fagc-gain-index-type-after-exit-rx-mode
//	1,		//fagc_use_last_lock_level_for_set_gain_en ***  adi,fagc-use-last-lock-level-for-set-gain-enable
//	1,		//fagc_rst_gla_stronger_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-stronger-sig-thresh-exceeded-enable
//	5,		//fagc_optimized_gain_offset ***  adi,fagc-optimized-gain-offset
//	10,		//fagc_rst_gla_stronger_sig_thresh_above_ll ***  adi,fagc-rst-gla-stronger-sig-thresh-above-ll
//	1,		//fagc_rst_gla_engergy_lost_sig_thresh_exceeded_en ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-exceeded-enable
//	1,		//fagc_rst_gla_engergy_lost_goto_optim_gain_en ***  adi,fagc-rst-gla-engergy-lost-goto-optim-gain-enable
//	10,		//fagc_rst_gla_engergy_lost_sig_thresh_below_ll ***  adi,fagc-rst-gla-engergy-lost-sig-thresh-below-ll
//	8,		//fagc_energy_lost_stronger_sig_gain_lock_exit_cnt ***  adi,fagc-energy-lost-stronger-sig-gain-lock-exit-cnt
//	1,		//fagc_rst_gla_large_adc_overload_en ***  adi,fagc-rst-gla-large-adc-overload-enable
//	1,		//fagc_rst_gla_large_lmt_overload_en ***  adi,fagc-rst-gla-large-lmt-overload-enable
//	0,		//fagc_rst_gla_en_agc_pulled_high_en ***  adi,fagc-rst-gla-en-agc-pulled-high-enable
//	0,		//fagc_rst_gla_if_en_agc_pulled_high_mode ***  adi,fagc-rst-gla-if-en-agc-pulled-high-mode
//	64,		//fagc_power_measurement_duration_in_state5 ***  adi,fagc-power-measurement-duration-in-state5
//	/* RSSI Control */
//	1,		//rssi_delay *** adi,rssi-delay
//	1000,	//rssi_duration *** adi,rssi-duration
//	3,		//rssi_restart_mode *** adi,rssi-restart-mode
//	0,		//rssi_unit_is_rx_samples_enable *** adi,rssi-unit-is-rx-samples-enable
//	1,		//rssi_wait *** adi,rssi-wait
//	/* Aux ADC Control */
//	256,	//aux_adc_decimation *** adi,aux-adc-decimation
//	40000000UL,	//aux_adc_rate *** adi,aux-adc-rate
//	/* AuxDAC Control */
//	1,		//aux_dac_manual_mode_enable ***  adi,aux-dac-manual-mode-enable
//	0,		//aux_dac1_default_value_mV ***  adi,aux-dac1-default-value-mV
//	0,		//aux_dac1_active_in_rx_enable ***  adi,aux-dac1-active-in-rx-enable
//	0,		//aux_dac1_active_in_tx_enable ***  adi,aux-dac1-active-in-tx-enable
//	0,		//aux_dac1_active_in_alert_enable ***  adi,aux-dac1-active-in-alert-enable
//	0,		//aux_dac1_rx_delay_us ***  adi,aux-dac1-rx-delay-us
//	0,		//aux_dac1_tx_delay_us ***  adi,aux-dac1-tx-delay-us
//	0,		//aux_dac2_default_value_mV ***  adi,aux-dac2-default-value-mV
//	0,		//aux_dac2_active_in_rx_enable ***  adi,aux-dac2-active-in-rx-enable
//	0,		//aux_dac2_active_in_tx_enable ***  adi,aux-dac2-active-in-tx-enable
//	0,		//aux_dac2_active_in_alert_enable ***  adi,aux-dac2-active-in-alert-enable
//	0,		//aux_dac2_rx_delay_us ***  adi,aux-dac2-rx-delay-us
//	0,		//aux_dac2_tx_delay_us ***  adi,aux-dac2-tx-delay-us
//	/* Temperature Sensor Control */
//	256,	//temp_sense_decimation *** adi,temp-sense-decimation
//	1000,	//temp_sense_measurement_interval_ms *** adi,temp-sense-measurement-interval-ms
//	0xCE,	//temp_sense_offset_signed *** adi,temp-sense-offset-signed
//	1,		//temp_sense_periodic_measurement_enable *** adi,temp-sense-periodic-measurement-enable
//	/* Control Out Setup */
//	0xFF,	//ctrl_outs_enable_mask *** adi,ctrl-outs-enable-mask
//	0,		//ctrl_outs_index *** adi,ctrl-outs-index
//	/* External LNA Control */
//	0,		//elna_settling_delay_ns *** adi,elna-settling-delay-ns
//	0,		//elna_gain_mdB *** adi,elna-gain-mdB
//	0,		//elna_bypass_loss_mdB *** adi,elna-bypass-loss-mdB
//	0,		//elna_rx1_gpo0_control_enable *** adi,elna-rx1-gpo0-control-enable
//	0,		//elna_rx2_gpo1_control_enable *** adi,elna-rx2-gpo1-control-enable
//	0,		//elna_gaintable_all_index_enable *** adi,elna-gaintable-all-index-enable
//	/* Digital Interface Control */
//	0,		//digital_interface_tune_skip_mode *** adi,digital-interface-tune-skip-mode
//	0,		//digital_interface_tune_fir_disable *** adi,digital-interface-tune-fir-disable
//	1,		//pp_tx_swap_enable *** adi,pp-tx-swap-enable
//	1,		//pp_rx_swap_enable *** adi,pp-rx-swap-enable
//	0,		//tx_channel_swap_enable *** adi,tx-channel-swap-enable
//	0,		//rx_channel_swap_enable *** adi,rx-channel-swap-enable
//	1,		//rx_frame_pulse_mode_enable *** adi,rx-frame-pulse-mode-enable
//	0,		//two_t_two_r_timing_enable *** adi,2t2r-timing-enable
//	0,		//invert_data_bus_enable *** adi,invert-data-bus-enable
//	0,		//invert_data_clk_enable *** adi,invert-data-clk-enable
//	0,		//fdd_alt_word_order_enable *** adi,fdd-alt-word-order-enable
//	0,		//invert_rx_frame_enable *** adi,invert-rx-frame-enable
//	0,		//fdd_rx_rate_2tx_enable *** adi,fdd-rx-rate-2tx-enable
//	0,		//swap_ports_enable *** adi,swap-ports-enable
//	0,		//single_data_rate_enable *** adi,single-data-rate-enable
//	1,		//lvds_mode_enable *** adi,lvds-mode-enable
//	0,		//half_duplex_mode_enable *** adi,half-duplex-mode-enable
//	0,		//single_port_mode_enable *** adi,single-port-mode-enable
//	0,		//full_port_enable *** adi,full-port-enable
//	0,		//full_duplex_swap_bits_enable *** adi,full-duplex-swap-bits-enable
//	0,		//delay_rx_data *** adi,delay-rx-data
//	0,		//rx_data_clock_delay *** adi,rx-data-clock-delay
//	4,		//rx_data_delay *** adi,rx-data-delay
//	7,		//tx_fb_clock_delay *** adi,tx-fb-clock-delay
//	0,		//tx_data_delay *** adi,tx-data-delay
//	150,	//lvds_bias_mV *** adi,lvds-bias-mV
//	1,		//lvds_rx_onchip_termination_enable *** adi,lvds-rx-onchip-termination-enable
//	0,		//rx1rx2_phase_inversion_en *** adi,rx1-rx2-phase-inversion-enable
//	0xFF,	//lvds_invert1_control *** adi,lvds-invert1-control
//	0x0F,	//lvds_invert2_control *** adi,lvds-invert2-control
//	/* GPO Control */
//	0,		//gpo0_inactive_state_high_enable *** adi,gpo0-inactive-state-high-enable
//	0,		//gpo1_inactive_state_high_enable *** adi,gpo1-inactive-state-high-enable
//	0,		//gpo2_inactive_state_high_enable *** adi,gpo2-inactive-state-high-enable
//	0,		//gpo3_inactive_state_high_enable *** adi,gpo3-inactive-state-high-enable
//	0,		//gpo0_slave_rx_enable *** adi,gpo0-slave-rx-enable
//	0,		//gpo0_slave_tx_enable *** adi,gpo0-slave-tx-enable
//	0,		//gpo1_slave_rx_enable *** adi,gpo1-slave-rx-enable
//	0,		//gpo1_slave_tx_enable *** adi,gpo1-slave-tx-enable
//	0,		//gpo2_slave_rx_enable *** adi,gpo2-slave-rx-enable
//	0,		//gpo2_slave_tx_enable *** adi,gpo2-slave-tx-enable
//	0,		//gpo3_slave_rx_enable *** adi,gpo3-slave-rx-enable
//	0,		//gpo3_slave_tx_enable *** adi,gpo3-slave-tx-enable
//	0,		//gpo0_rx_delay_us *** adi,gpo0-rx-delay-us
//	0,		//gpo0_tx_delay_us *** adi,gpo0-tx-delay-us
//	0,		//gpo1_rx_delay_us *** adi,gpo1-rx-delay-us
//	0,		//gpo1_tx_delay_us *** adi,gpo1-tx-delay-us
//	0,		//gpo2_rx_delay_us *** adi,gpo2-rx-delay-us
//	0,		//gpo2_tx_delay_us *** adi,gpo2-tx-delay-us
//	0,		//gpo3_rx_delay_us *** adi,gpo3-rx-delay-us
//	0,		//gpo3_tx_delay_us *** adi,gpo3-tx-delay-us
//	/* Tx Monitor Control */
//	37000,	//low_high_gain_threshold_mdB *** adi,txmon-low-high-thresh
//	0,		//low_gain_dB *** adi,txmon-low-gain
//	24,		//high_gain_dB *** adi,txmon-high-gain
//	0,		//tx_mon_track_en *** adi,txmon-dc-tracking-enable
//	0,		//one_shot_mode_en *** adi,txmon-one-shot-mode-enable
//	511,	//tx_mon_delay *** adi,txmon-delay
//	8192,	//tx_mon_duration *** adi,txmon-duration
//	2,		//tx1_mon_front_end_gain *** adi,txmon-1-front-end-gain
//	2,		//tx2_mon_front_end_gain *** adi,txmon-2-front-end-gain
//	48,		//tx1_mon_lo_cm *** adi,txmon-1-lo-cm
//	48,		//tx2_mon_lo_cm *** adi,txmon-2-lo-cm
//	/* GPIO definitions */
//	-1,		//gpio_resetb *** reset-gpios
//	/* MCS Sync */
//	-1,		//gpio_sync *** sync-gpios
//	-1,		//gpio_cal_sw1 *** cal-sw1-gpios
//	-1,		//gpio_cal_sw2 *** cal-sw2-gpios
//	/* External LO clocks */
//	NULL,	//(*ad9361_rfpll_ext_recalc_rate)()
//	NULL,	//(*ad9361_rfpll_ext_round_rate)()
//	NULL	//(*ad9361_rfpll_ext_set_rate)()
//};


int32_t ad9361_init (struct ad9361_rf_phy **ad9361_phy)
{
	struct ad9361_rf_phy *phy;
	int32_t ret = 0;
	int32_t rev = 0;
	int32_t i   = 0;

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
	phy->dev_sel = ID_AD9361;

	/* Identification number */
	phy->spi->id_no = 0;
	phy->id_no = 0;

	/* Reference Clock */
	phy->clk_refin->rate = 40000000UL;

	/* Base Configuration */
	phy->pdata->fdd = 1;
	phy->pdata->fdd_independent_mode = 0;
	phy->pdata->rx2tx2 = 1;
	phy->pdata->rx1tx1_mode_use_rx_num = 1;
	phy->pdata->rx1tx1_mode_use_tx_num = 1;
	phy->pdata->tdd_use_dual_synth = 0;
	phy->pdata->tdd_skip_vco_cal = 0;
	phy->pdata->rx_fastlock_delay_ns = 0;
	phy->pdata->tx_fastlock_delay_ns = 0;
	phy->pdata->trx_fastlock_pinctrl_en[0] = 0;
	phy->pdata->trx_fastlock_pinctrl_en[1] = 0;
	phy->pdata->use_ext_rx_lo = 0;
	phy->pdata->use_ext_tx_lo = 0;
	phy->pdata->dc_offset_update_events = 5;
	phy->pdata->dc_offset_attenuation_high = 6;
	phy->pdata->dc_offset_attenuation_low = 5;
	phy->pdata->rf_dc_offset_count_high = 0x28;
	phy->pdata->rf_dc_offset_count_low = 0x32;
	phy->pdata->split_gt = 0;
	phy->pdata->trx_synth_max_fref = MAX_SYNTH_FREF;
	phy->pdata->qec_tracking_slow_mode_en = 0;

	/* ENSM Control */
	phy->pdata->ensm_pin_pulse_mode = 0;
	phy->pdata->ensm_pin_ctrl = 0;

	/* LO Control */
	phy->pdata->rx_synth_freq = 2400000000UL;
	phy->pdata->tx_synth_freq = 2400000000UL;
	phy->pdata->lo_powerdown_managed_en = 1;

	/* Rate & BW Control */
//	{983040000, 245760000, 122880000, 61440000, 30720000, 30720000}
	for(i = 0; i < 6; i++) {
		phy->pdata->rx_path_clks[i] = init_param->rx_path_clock_frequencies[i];
	}
//	{983040000, 122880000, 122880000, 61440000, 30720000, 30720000}
	for(i = 0; i < 6; i++) {
		phy->pdata->tx_path_clks[i] = init_param->tx_path_clock_frequencies[i];
	}
	phy->pdata->rf_rx_bandwidth_Hz = 18000000;
	phy->pdata->rf_tx_bandwidth_Hz = 18000000;

	/* RF Port Control */
	phy->pdata->rf_rx_input_sel = 0;
	phy->pdata->rf_tx_output_sel = 0;

	/* TX Attenuation Control */
	phy->pdata->tx_atten = init_param->tx_attenuation_mdB;
	phy->pdata->update_tx_gain_via_alert =
		init_param->update_tx_gain_in_alert_enable;

	/* Reference Clock Control */
	switch (phy->dev_sel) {
	case ID_AD9363A:
		phy->pdata->use_extclk = true;
		break;
	default:
		phy->pdata->use_extclk = init_param->xo_disable_use_ext_refclk_enable;
	}
	phy->pdata->dcxo_coarse = init_param->dcxo_coarse_and_fine_tune[0];
	phy->pdata->dcxo_fine = init_param->dcxo_coarse_and_fine_tune[1];
	phy->pdata->ad9361_clkout_mode = (enum ad9361_clkout)
					 init_param->clk_output_mode_select;

	/* Gain Control */
	phy->pdata->gain_ctrl.rx1_mode = (enum rf_gain_ctrl_mode)
					 init_param->gc_rx1_mode;
	phy->pdata->gain_ctrl.rx2_mode = (enum rf_gain_ctrl_mode)
					 init_param->gc_rx2_mode;
	phy->pdata->gain_ctrl.adc_large_overload_thresh =
		init_param->gc_adc_large_overload_thresh;
	phy->pdata->gain_ctrl.adc_ovr_sample_size = init_param->gc_adc_ovr_sample_size;
	phy->pdata->gain_ctrl.adc_small_overload_thresh =
		init_param->gc_adc_small_overload_thresh;
	phy->pdata->gain_ctrl.dec_pow_measuremnt_duration =
		init_param->gc_dec_pow_measurement_duration;
	phy->pdata->gain_ctrl.dig_gain_en = init_param->gc_dig_gain_enable;
	phy->pdata->gain_ctrl.lmt_overload_high_thresh =
		init_param->gc_lmt_overload_high_thresh;
	phy->pdata->gain_ctrl.lmt_overload_low_thresh =
		init_param->gc_lmt_overload_low_thresh;
	phy->pdata->gain_ctrl.low_power_thresh = init_param->gc_low_power_thresh;
	phy->pdata->gain_ctrl.max_dig_gain = init_param->gc_max_dig_gain;

	/* Gain MGC Control */
	phy->pdata->gain_ctrl.mgc_dec_gain_step = init_param->mgc_dec_gain_step;
	phy->pdata->gain_ctrl.mgc_inc_gain_step = init_param->mgc_inc_gain_step;
	phy->pdata->gain_ctrl.mgc_rx1_ctrl_inp_en = init_param->mgc_rx1_ctrl_inp_enable;
	phy->pdata->gain_ctrl.mgc_rx2_ctrl_inp_en = init_param->mgc_rx2_ctrl_inp_enable;
	phy->pdata->gain_ctrl.mgc_split_table_ctrl_inp_gain_mode =
		init_param->mgc_split_table_ctrl_inp_gain_mode;

	/* Gain AGC Control */
	phy->pdata->gain_ctrl.adc_large_overload_exceed_counter =
		init_param->agc_adc_large_overload_exceed_counter;
	phy->pdata->gain_ctrl.adc_large_overload_inc_steps =
		init_param->agc_adc_large_overload_inc_steps;
	phy->pdata->gain_ctrl.adc_lmt_small_overload_prevent_gain_inc =
		init_param->agc_adc_lmt_small_overload_prevent_gain_inc_enable;
	phy->pdata->gain_ctrl.adc_small_overload_exceed_counter =
		init_param->agc_adc_small_overload_exceed_counter;
	phy->pdata->gain_ctrl.dig_gain_step_size = init_param->agc_dig_gain_step_size;
	phy->pdata->gain_ctrl.dig_saturation_exceed_counter =
		init_param->agc_dig_saturation_exceed_counter;
	phy->pdata->gain_ctrl.gain_update_interval_us =
		init_param->agc_gain_update_interval_us;
	phy->pdata->gain_ctrl.immed_gain_change_if_large_adc_overload =
		init_param->agc_immed_gain_change_if_large_adc_overload_enable;
	phy->pdata->gain_ctrl.immed_gain_change_if_large_lmt_overload =
		init_param->agc_immed_gain_change_if_large_lmt_overload_enable;
	phy->pdata->gain_ctrl.agc_inner_thresh_high = init_param->agc_inner_thresh_high;
	phy->pdata->gain_ctrl.agc_inner_thresh_high_dec_steps =
		init_param->agc_inner_thresh_high_dec_steps;
	phy->pdata->gain_ctrl.agc_inner_thresh_low = init_param->agc_inner_thresh_low;
	phy->pdata->gain_ctrl.agc_inner_thresh_low_inc_steps =
		init_param->agc_inner_thresh_low_inc_steps;
	phy->pdata->gain_ctrl.lmt_overload_large_exceed_counter =
		init_param->agc_lmt_overload_large_exceed_counter;
	phy->pdata->gain_ctrl.lmt_overload_large_inc_steps =
		init_param->agc_lmt_overload_large_inc_steps;
	phy->pdata->gain_ctrl.lmt_overload_small_exceed_counter =
		init_param->agc_lmt_overload_small_exceed_counter;
	phy->pdata->gain_ctrl.agc_outer_thresh_high = init_param->agc_outer_thresh_high;
	phy->pdata->gain_ctrl.agc_outer_thresh_high_dec_steps =
		init_param->agc_outer_thresh_high_dec_steps;
	phy->pdata->gain_ctrl.agc_outer_thresh_low = init_param->agc_outer_thresh_low;
	phy->pdata->gain_ctrl.agc_outer_thresh_low_inc_steps =
		init_param->agc_outer_thresh_low_inc_steps;
	phy->pdata->gain_ctrl.agc_attack_delay_extra_margin_us =
		init_param->agc_attack_delay_extra_margin_us;
	phy->pdata->gain_ctrl.sync_for_gain_counter_en =
		init_param->agc_sync_for_gain_counter_enable;

	/* Fast AGC */
	phy->pdata->gain_ctrl.f_agc_dec_pow_measuremnt_duration =
		init_param->fagc_dec_pow_measuremnt_duration;
	phy->pdata->gain_ctrl.f_agc_state_wait_time_ns =
		init_param->fagc_state_wait_time_ns;
	/* Fast AGC - Low Power */
	phy->pdata->gain_ctrl.f_agc_allow_agc_gain_increase =
		init_param->fagc_allow_agc_gain_increase;
	phy->pdata->gain_ctrl.f_agc_lp_thresh_increment_time =
		init_param->fagc_lp_thresh_increment_time;
	phy->pdata->gain_ctrl.f_agc_lp_thresh_increment_steps =
		init_param->fagc_lp_thresh_increment_steps;
	/* Fast AGC - Lock Level (Lock Level is set via slow AGC inner high threshold) */
	phy->pdata->gain_ctrl.f_agc_lock_level_lmt_gain_increase_en =
		init_param->fagc_lock_level_lmt_gain_increase_en;
	phy->pdata->gain_ctrl.f_agc_lock_level_gain_increase_upper_limit =
		init_param->fagc_lock_level_gain_increase_upper_limit;
	/* Fast AGC - Peak Detectors and Final Settling */
	phy->pdata->gain_ctrl.f_agc_lpf_final_settling_steps =
		init_param->fagc_lpf_final_settling_steps;
	phy->pdata->gain_ctrl.f_agc_lmt_final_settling_steps =
		init_param->fagc_lmt_final_settling_steps;
	phy->pdata->gain_ctrl.f_agc_final_overrange_count =
		init_param->fagc_final_overrange_count;
	/* Fast AGC - Final Power Test */
	phy->pdata->gain_ctrl.f_agc_gain_increase_after_gain_lock_en =
		init_param->fagc_gain_increase_after_gain_lock_en;
	/* Fast AGC - Unlocking the Gain */
	phy->pdata->gain_ctrl.f_agc_gain_index_type_after_exit_rx_mode =
		(enum f_agc_target_gain_index_type)
		init_param->fagc_gain_index_type_after_exit_rx_mode;
	phy->pdata->gain_ctrl.f_agc_use_last_lock_level_for_set_gain_en =
		init_param->fagc_use_last_lock_level_for_set_gain_en;
	phy->pdata->gain_ctrl.f_agc_rst_gla_stronger_sig_thresh_exceeded_en =
		init_param->fagc_rst_gla_stronger_sig_thresh_exceeded_en;
	phy->pdata->gain_ctrl.f_agc_optimized_gain_offset =
		init_param->fagc_optimized_gain_offset;
	phy->pdata->gain_ctrl.f_agc_rst_gla_stronger_sig_thresh_above_ll =
		init_param->fagc_rst_gla_stronger_sig_thresh_above_ll;
	phy->pdata->gain_ctrl.f_agc_rst_gla_engergy_lost_sig_thresh_exceeded_en =
		init_param->fagc_rst_gla_engergy_lost_sig_thresh_exceeded_en;
	phy->pdata->gain_ctrl.f_agc_rst_gla_engergy_lost_goto_optim_gain_en =
		init_param->fagc_rst_gla_engergy_lost_goto_optim_gain_en;
	phy->pdata->gain_ctrl.f_agc_rst_gla_engergy_lost_sig_thresh_below_ll =
		init_param->fagc_rst_gla_engergy_lost_sig_thresh_below_ll;
	phy->pdata->gain_ctrl.f_agc_energy_lost_stronger_sig_gain_lock_exit_cnt =
		init_param->fagc_energy_lost_stronger_sig_gain_lock_exit_cnt;
	phy->pdata->gain_ctrl.f_agc_rst_gla_large_adc_overload_en =
		init_param->fagc_rst_gla_large_adc_overload_en;
	phy->pdata->gain_ctrl.f_agc_rst_gla_large_lmt_overload_en =
		init_param->fagc_rst_gla_large_lmt_overload_en;
	phy->pdata->gain_ctrl.f_agc_rst_gla_en_agc_pulled_high_en =
		init_param->fagc_rst_gla_en_agc_pulled_high_en;
	phy->pdata->gain_ctrl.f_agc_rst_gla_if_en_agc_pulled_high_mode =
		(enum f_agc_target_gain_index_type)
		init_param->fagc_rst_gla_if_en_agc_pulled_high_mode;
	phy->pdata->gain_ctrl.f_agc_power_measurement_duration_in_state5 =
		init_param->fagc_power_measurement_duration_in_state5;

	/* RSSI Control */
	phy->pdata->rssi_ctrl.rssi_delay = init_param->rssi_delay;
	phy->pdata->rssi_ctrl.rssi_duration = init_param->rssi_duration;
	phy->pdata->rssi_ctrl.restart_mode = (enum rssi_restart_mode)
					     init_param->rssi_restart_mode;
	phy->pdata->rssi_ctrl.rssi_unit_is_rx_samples =
		init_param->rssi_unit_is_rx_samples_enable;
	phy->pdata->rssi_ctrl.rssi_wait = init_param->rssi_wait;

	/* Aux ADC Control */
	phy->pdata->auxadc_ctrl.auxadc_decimation = init_param->aux_adc_decimation;
	phy->pdata->auxadc_ctrl.auxadc_clock_rate = init_param->aux_adc_rate;

	/* AuxDAC Control */
	phy->pdata->auxdac_ctrl.auxdac_manual_mode_en =
		init_param->aux_dac_manual_mode_enable;
	phy->pdata->auxdac_ctrl.dac1_default_value =
		init_param->aux_dac1_default_value_mV;
	phy->pdata->auxdac_ctrl.dac1_in_rx_en =
		init_param->aux_dac1_active_in_rx_enable;
	phy->pdata->auxdac_ctrl.dac1_in_tx_en =
		init_param->aux_dac1_active_in_tx_enable;
	phy->pdata->auxdac_ctrl.dac1_in_alert_en =
		init_param->aux_dac1_active_in_alert_enable;
	phy->pdata->auxdac_ctrl.dac1_rx_delay_us = init_param->aux_dac1_rx_delay_us;
	phy->pdata->auxdac_ctrl.dac1_tx_delay_us = init_param->aux_dac1_tx_delay_us;
	phy->pdata->auxdac_ctrl.dac2_default_value =
		init_param->aux_dac2_default_value_mV;
	phy->pdata->auxdac_ctrl.dac2_in_rx_en =
		init_param->aux_dac2_active_in_rx_enable;
	phy->pdata->auxdac_ctrl.dac2_in_tx_en =
		init_param->aux_dac2_active_in_tx_enable;
	phy->pdata->auxdac_ctrl.dac2_in_alert_en =
		init_param->aux_dac2_active_in_alert_enable;
	phy->pdata->auxdac_ctrl.dac2_rx_delay_us = init_param->aux_dac2_rx_delay_us;
	phy->pdata->auxdac_ctrl.dac2_tx_delay_us = init_param->aux_dac2_tx_delay_us;

	/* Temperature Sensor Control */
	phy->pdata->auxadc_ctrl.temp_sensor_decimation =
		init_param->temp_sense_decimation;
	phy->pdata->auxadc_ctrl.temp_time_inteval_ms =
		init_param->temp_sense_measurement_interval_ms;
	phy->pdata->auxadc_ctrl.offset = init_param->temp_sense_offset_signed;
	phy->pdata->auxadc_ctrl.periodic_temp_measuremnt =
		init_param->temp_sense_periodic_measurement_enable;

	/* Control Out Setup */
	phy->pdata->ctrl_outs_ctrl.en_mask = init_param->ctrl_outs_enable_mask;
	phy->pdata->ctrl_outs_ctrl.index = init_param->ctrl_outs_index;

	/* External LNA Control */
	phy->pdata->elna_ctrl.settling_delay_ns = init_param->elna_settling_delay_ns;
	phy->pdata->elna_ctrl.gain_mdB = init_param->elna_gain_mdB;
	phy->pdata->elna_ctrl.bypass_loss_mdB = init_param->elna_bypass_loss_mdB;
	phy->pdata->elna_ctrl.elna_1_control_en =
		init_param->elna_rx1_gpo0_control_enable;
	phy->pdata->elna_ctrl.elna_2_control_en =
		init_param->elna_rx2_gpo1_control_enable;
	phy->pdata->elna_ctrl.elna_in_gaintable_all_index_en =
		init_param->elna_gaintable_all_index_enable;

	/* Digital Interface Control */
	phy->pdata->dig_interface_tune_skipmode =
		(init_param->digital_interface_tune_skip_mode);
	phy->pdata->dig_interface_tune_fir_disable =
		(init_param->digital_interface_tune_fir_disable);
	phy->pdata->port_ctrl.pp_conf[0] = (init_param->pp_tx_swap_enable << 7);
	phy->pdata->port_ctrl.pp_conf[0] |= (init_param->pp_rx_swap_enable << 6);
	phy->pdata->port_ctrl.pp_conf[0] |= (init_param->tx_channel_swap_enable << 5);
	phy->pdata->port_ctrl.pp_conf[0] |= (init_param->rx_channel_swap_enable << 4);
	phy->pdata->port_ctrl.pp_conf[0] |= (init_param->rx_frame_pulse_mode_enable <<
					     3);
	phy->pdata->port_ctrl.pp_conf[0] |= (init_param->two_t_two_r_timing_enable <<
					     2);
	phy->pdata->port_ctrl.pp_conf[0] |= (init_param->invert_data_bus_enable << 1);
	phy->pdata->port_ctrl.pp_conf[0] |= (init_param->invert_data_clk_enable << 0);
	phy->pdata->port_ctrl.pp_conf[1] = (init_param->fdd_alt_word_order_enable << 7);
	phy->pdata->port_ctrl.pp_conf[1] |= (init_param->invert_rx_frame_enable << 2);
	phy->pdata->port_ctrl.pp_conf[2] = (init_param->fdd_rx_rate_2tx_enable << 7);
	phy->pdata->port_ctrl.pp_conf[2] |= (init_param->swap_ports_enable << 6);
	phy->pdata->port_ctrl.pp_conf[2] |= (init_param->single_data_rate_enable << 5);
	phy->pdata->port_ctrl.pp_conf[2] |= (init_param->lvds_mode_enable << 4);
	phy->pdata->port_ctrl.pp_conf[2] |= (init_param->half_duplex_mode_enable << 3);
	phy->pdata->port_ctrl.pp_conf[2] |= (init_param->single_port_mode_enable << 2);
	phy->pdata->port_ctrl.pp_conf[2] |= (init_param->full_port_enable << 1);
	phy->pdata->port_ctrl.pp_conf[2] |= (init_param->full_duplex_swap_bits_enable <<
					     0);
	phy->pdata->port_ctrl.pp_conf[1] |= (init_param->delay_rx_data & 0x3);
	phy->pdata->port_ctrl.rx_clk_data_delay = DATA_CLK_DELAY(
				init_param->rx_data_clock_delay);
	phy->pdata->port_ctrl.rx_clk_data_delay |= RX_DATA_DELAY(
				init_param->rx_data_delay);
	phy->pdata->port_ctrl.tx_clk_data_delay = FB_CLK_DELAY(
				init_param->tx_fb_clock_delay);
	phy->pdata->port_ctrl.tx_clk_data_delay |= TX_DATA_DELAY(
				init_param->tx_data_delay);
	phy->pdata->port_ctrl.lvds_bias_ctrl = ((init_param->lvds_bias_mV - 75) / 75) &
					       0x7;
	phy->pdata->port_ctrl.lvds_bias_ctrl |=
		(init_param->lvds_rx_onchip_termination_enable << 5);
	phy->pdata->rx1rx2_phase_inversion_en = init_param->rx1rx2_phase_inversion_en;

	/* GPO Control */
	phy->pdata->gpo_ctrl.gpo0_inactive_state_high_en =
		init_param->gpo0_inactive_state_high_enable;
	phy->pdata->gpo_ctrl.gpo1_inactive_state_high_en =
		init_param->gpo1_inactive_state_high_enable;
	phy->pdata->gpo_ctrl.gpo2_inactive_state_high_en =
		init_param->gpo2_inactive_state_high_enable;
	phy->pdata->gpo_ctrl.gpo3_inactive_state_high_en =
		init_param->gpo3_inactive_state_high_enable;

	phy->pdata->gpo_ctrl.gpo0_slave_rx_en = init_param->gpo0_slave_rx_enable;
	phy->pdata->gpo_ctrl.gpo0_slave_tx_en = init_param->gpo0_slave_tx_enable;
	phy->pdata->gpo_ctrl.gpo1_slave_rx_en = init_param->gpo1_slave_rx_enable;
	phy->pdata->gpo_ctrl.gpo1_slave_tx_en = init_param->gpo1_slave_tx_enable;
	phy->pdata->gpo_ctrl.gpo2_slave_rx_en = init_param->gpo2_slave_rx_enable;
	phy->pdata->gpo_ctrl.gpo2_slave_tx_en = init_param->gpo2_slave_tx_enable;
	phy->pdata->gpo_ctrl.gpo3_slave_rx_en = init_param->gpo3_slave_rx_enable;
	phy->pdata->gpo_ctrl.gpo3_slave_tx_en = init_param->gpo3_slave_tx_enable;

	phy->pdata->gpo_ctrl.gpo0_rx_delay_us = init_param->gpo0_rx_delay_us;
	phy->pdata->gpo_ctrl.gpo0_tx_delay_us = init_param->gpo0_tx_delay_us;
	phy->pdata->gpo_ctrl.gpo1_rx_delay_us = init_param->gpo1_rx_delay_us;
	phy->pdata->gpo_ctrl.gpo1_tx_delay_us = init_param->gpo1_tx_delay_us;
	phy->pdata->gpo_ctrl.gpo2_rx_delay_us = init_param->gpo2_rx_delay_us;
	phy->pdata->gpo_ctrl.gpo2_tx_delay_us = init_param->gpo2_tx_delay_us;
	phy->pdata->gpo_ctrl.gpo3_rx_delay_us = init_param->gpo3_rx_delay_us;
	phy->pdata->gpo_ctrl.gpo3_tx_delay_us = init_param->gpo3_tx_delay_us;

	/* Tx Monitor Control */
	phy->pdata->txmon_ctrl.low_high_gain_threshold_mdB =
		init_param->low_high_gain_threshold_mdB;
	phy->pdata->txmon_ctrl.low_gain_dB = init_param->low_gain_dB;
	phy->pdata->txmon_ctrl.high_gain_dB = init_param->high_gain_dB;
	phy->pdata->txmon_ctrl.tx_mon_track_en = init_param->tx_mon_track_en;
	phy->pdata->txmon_ctrl.one_shot_mode_en = init_param->one_shot_mode_en;
	phy->pdata->txmon_ctrl.tx_mon_delay = init_param->tx_mon_delay;
	phy->pdata->txmon_ctrl.tx_mon_duration = init_param->tx_mon_duration;
	phy->pdata->txmon_ctrl.tx1_mon_front_end_gain =
		init_param->tx1_mon_front_end_gain;
	phy->pdata->txmon_ctrl.tx2_mon_front_end_gain =
		init_param->tx2_mon_front_end_gain;
	phy->pdata->txmon_ctrl.tx1_mon_lo_cm = init_param->tx1_mon_lo_cm;
	phy->pdata->txmon_ctrl.tx2_mon_lo_cm = init_param->tx2_mon_lo_cm;

	phy->pdata->debug_mode = true;
	phy->pdata->gpio_resetb = init_param->gpio_resetb;
	/* Optional: next three GPIOs are used for MCS synchronization */
	phy->pdata->gpio_sync = init_param->gpio_sync;
	phy->pdata->gpio_cal_sw1 = init_param->gpio_cal_sw1;
	phy->pdata->gpio_cal_sw2 = init_param->gpio_cal_sw2;

	phy->pdata->port_ctrl.digital_io_ctrl = 0;
	phy->pdata->port_ctrl.lvds_invert[0] = init_param->lvds_invert1_control;
	phy->pdata->port_ctrl.lvds_invert[1] = init_param->lvds_invert2_control;

	if (AD9364_DEVICE) {
		phy->pdata->rx2tx2 = false;
		phy->pdata->rx1tx1_mode_use_rx_num = 1;
		phy->pdata->rx1tx1_mode_use_tx_num = 1;
	}

#ifndef AXI_ADC_NOT_PRESENT
	phy->adc_conv->chip_info = &axiadc_chip_info_tbl[phy->pdata->rx2tx2 ?
						      ID_AD9361 : ID_AD9364];
#endif

	phy->rx_eq_2tx = false;

	phy->current_table = -1;
	phy->bypass_tx_fir = true;
	phy->bypass_rx_fir = true;
	phy->rate_governor = 1;
	phy->rfdc_track_en = true;
	phy->bbdc_track_en = true;
	phy->quad_track_en = true;

	phy->gt_info = ad9361_adi_gt_info;

	phy->bist_loopback_mode = 0;
	phy->bist_config = 0;
	phy->bist_prbs_mode = BIST_DISABLE;
	phy->bist_tone_mode = BIST_DISABLE;
	phy->bist_tone_freq_Hz = 0;
	phy->bist_tone_level_dB = 0;
	phy->bist_tone_mask = 0;

	ad9361_reset(phy);

	ret = ad9361_spi_read(phy->spi, REG_PRODUCT_ID);
	if ((ret & PRODUCT_ID_MASK) != PRODUCT_ID_9361) {
		printf("%s : Unsupported PRODUCT_ID 0x%X", __func__, (unsigned int)ret);
		ret = -ENODEV;
		goto out;
	}
	rev = ret & REV_MASK;

	phy->ad9361_rfpll_ext_recalc_rate = init_param->ad9361_rfpll_ext_recalc_rate;
	phy->ad9361_rfpll_ext_round_rate = init_param->ad9361_rfpll_ext_round_rate;
	phy->ad9361_rfpll_ext_set_rate = init_param->ad9361_rfpll_ext_set_rate;

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
