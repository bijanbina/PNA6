#include "pna-help.h"

int check_argument(char *token, char *function, char *argument)
{
	if(token==NULL)
	{
		print_help(function, argument);
		return 0;
	}
	char input[1000];
	strcpy(input, token);

	if(!strcmp(function, COMMAND_TXBW) || !strcmp(function, COMMAND_RXBW))
	{
		long long bandwidth = get_frequency(input);
		if(bandwidth>=BW_MIN && bandwidth<=BW_MAX)
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else if(!strcmp(function, COMMAND_VGA))
	{
		if(!strcmp(argument, "value"))
		{
			char *sz = NULL;
			long long vga_gain = -strtoll(input, &sz, 10);
			if(vga_gain>=VGA_MIN && vga_gain<=VGA_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, COMMAND_LNA))
	{
		if(!strcmp(argument, "value"))
		{
			char *sz = NULL;
			long long lna_gain = strtoll(input, &sz, 10);
			if(lna_gain>=LNA_MIN && lna_gain<=LNA_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, COMMAND_AGC))
	{
		if(!strcmp(argument, "value"))
		{
			if(!strcmp(input, "man") || !strcmp(input, "hyb") || !strcmp(input, "fas") || !strcmp(input, "slo") ||
				!strcmp(input, "manual") || !strcmp(input, "hybrid") || !strcmp(input, "fast-attack") ||
				!strcmp(input, "slow-attack"))
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, COMMAND_TXRATE) || !strcmp(function, COMMAND_RXRATE))
	{
		long long rate = get_frequency(input);
		if(rate>=RATE_MIN && rate<=RATE_MAX)
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else if(!strcmp(function, COMMAND_TXSS) || !strcmp(function, COMMAND_RXSS))
	{
		long long size = atoi(input);
		if(size==16 || size==32 || size==64 || size==128 || size==256 ||
			size==512 || size==1024 || size==2048 || size==4096 || size==8192)
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else if(!strcmp(function, COMMAND_CALIB))
	{
		if(!strcmp(input, "auto") || !strcmp(input, "manual") || !strcmp(input, "manual_tx_quad") ||
			!strcmp(input, "tx_quad") || !strcmp(input, "rf_dc_offs") || !strcmp(input, "rssi_gain_step"))
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else if(!strcmp(function, COMMAND_TXFREQ) || !strcmp(function, COMMAND_RXFREQ)
			|| !strcmp(function, COMMAND_TXFREQ_CAL))
	{
		long long freq = get_frequency(input);
		if(freq>=FRQ_MIN && freq<=FRQ_MAX)
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else if(!strcmp(function, COMMAND_RFOUT) || !strcmp(function, COMMAND_WINENABLE) ||
			    !strcmp(function, COMMAND_RXFIR) || !strcmp(function, COMMAND_TXFIR) ||
				!strcmp(function, COMMAND_BBDC) || !strcmp(function, COMMAND_RFDC) ||
				!strcmp(function, COMMAND_QUAD) || !strcmp(function, COMMAND_NLF))
	{
		int en = atoi(input);
		if(en==0 || en==1)
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else if(!strcmp(function, COMMAND_SIGPOW))
	{
		if(!strcmp(argument, "value"))
		{
			double pow = atof(input);
			if(pow>=SIGPOW_MIN || pow<=SIGPOW_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, COMMAND_VNAFREQ))
	{
		long long freq = get_frequency(input);
		if(freq>=DIFFRQ_MIN && freq<=DIFFRQ_MAX)
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else if(!strcmp(function, COMMAND_VNAPOW))
	{
		int pow = atoi(input);
		if(pow>=VNAPOW_MIN || pow<=VNAPOW_MAX)
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else if(!strcmp(function, COMMAND_SVNA))
	{
		if(!strcmp(argument, "freq"))
		{
			long long freq = get_frequency(input);
			if(freq>=FRQ_MIN && freq<=FRQ_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "lna gain"))
		{
			char *sz = NULL;
			long long lna_gain = strtoll(input, &sz, 10);
			if(lna_gain>=LNA_MIN && lna_gain<=LNA_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "vga gain"))
		{
			char *sz = NULL;
			long long vga_gain = -strtoll(input, &sz, 10);
			if(vga_gain>=VGA_MIN && vga_gain<=VGA_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 0;
		}
	}
	else if(!strcmp(function, COMMAND_SCAL))
	{
		if(!strcmp(argument, "freq"))
		{
			long long freq = get_frequency(input);
			if(freq>=FRQ_MIN && freq<=FRQ_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "point"))
		{
			int point = atoi(input);
			if(point>=CALPNT_MIN && point<=CALPNT_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "lna gain"))
		{
			char *sz = NULL;
			long long lna_gain = strtoll(input, &sz, 10);
			if(lna_gain>=LNA_MIN && lna_gain<=LNA_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "power"))
		{
			double power = atof(input);
			if(power>=SIGPOW_MIN && power<=SIGPOW_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, COMMAND_FILLPRO))
	{
		if(!strcmp(argument, "freq"))
		{
			long long freq = get_frequency(input);
			if(freq>=FRQ_MIN && freq<=FRQ_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "span"))
		{
			long long span = get_frequency(input);
			if(span>=SWSPAN_MIN && span<=SWSPAN_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 0;
		}
	}
	else if(!strcmp(function, COMMAND_REGISTER))
	{
		if(!strcmp(argument, "address"))
		{
			int addr = atoi(input);
			if(addr >= 0)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "value"))
		{
			if(strlen(input)>=3 && input[0]=='0' && input[1]=='x')
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 0;
		}
	}
	else if(!strcmp(function, COMMAND_FIR_COEF))
	{
		int len = strlen(input);
		if(!strcmp(input+len-4, ".ftr") && len>4)
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else if(!strcmp(function, WAVE_AWG))
	{
		if(!strcmp(argument, "len"))
		{
			int len = atoi(token);
			if(len>=AWGLEN_MIN && len<=AWGLEN_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "offset"))
		{
			double offset = atof(token);
			if(offset>=POWOFFSET_MIN && offset<=POWOFFSET_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 0;
		}
	}
	else if(!strcmp(function, WAVE_SINDC))
	{
		if(!strcmp(argument, "dig_amp"))
		{
			double dig_amp = atof(token);
			if(dig_amp>=0 && dig_amp<=1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "offset"))
		{
			double offset = atof(token);
			if(offset>=POWOFFSET_MIN && offset<=POWOFFSET_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 0;
		}
	}
	else if(!strcmp(function, WAVE_PULSEIQ))
	{
		if(!strcmp(argument, "dig_amp"))
		{
			double dig_amp = atof(token);
			if(dig_amp>=0 && dig_amp<=1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "duty_cycle"))
		{
			double duty_cycle = atof(token);
			if(duty_cycle>=0 && duty_cycle<=1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "sig_iq"))
		{
			int iq = atoi(token);
			if(iq==SIGNAL_INPHASE_ENABLED || iq==SIGNAL_QUAD_ENABLED ||
				iq==SIGNAL_IQ_ENABLED)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "period"))
		{
			int pn = atoi(token);
			if(pn>=PERIOD_MIN && pn<=PERIOD_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "offset"))
		{
			double offset = atof(token);
			if(offset>=POWOFFSET_MIN && offset<=POWOFFSET_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 0;
		}
	}
	else if(!strcmp(function, WAVE_SINIQ))
	{
		if(!strcmp(argument, "dig_amp"))
		{
			double dig_amp = atof(token);
			if(dig_amp>=0 && dig_amp<=1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "sig_iq"))
		{
			int iq = atoi(token);
			if(iq==SIGNAL_INPHASE_ENABLED || iq==SIGNAL_QUAD_ENABLED ||
				iq==SIGNAL_IQ_ENABLED)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "period"))
		{
			int pn = atoi(token);
			if(pn>=PERIOD_MIN && pn<=PERIOD_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "offset"))
		{
			double offset = atof(token);
			if(offset>=POWOFFSET_MIN && offset<=POWOFFSET_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 0;
		}
	}
	else if(!strcmp(function, WAVE_SIN))
	{
		if(!strcmp(argument, "dig_amp"))
		{
			double dig_amp = atof(token);
			if(dig_amp>=0 && dig_amp<=1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "period"))
		{
			int pn = atoi(token);
			if(pn>=PERIOD_MIN && pn<=PERIOD_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 0;
		}
	}
	else if(!strcmp(function, WAVE_PULSE))
	{
		if(!strcmp(argument, "dig_amp"))
		{
			double dig_amp = atof(token);
			if(dig_amp>=0 && dig_amp<=1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "duty_cycle"))
		{
			double duty_cycle = atof(token);
			if(duty_cycle>=0 && duty_cycle<=1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "period"))
		{
			int pn = atoi(token);
			if(pn>=PERIOD_MIN && pn<=PERIOD_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 0;
		}
	}
	else if(!strcmp(function, WAVE_DC))
	{
		if(!strcmp(argument, "vga_gain"))
		{
			char *sz = NULL;
			long long vga_gain = -strtoll(input, &sz, 10);
			if(vga_gain>=VGA_MIN && vga_gain<=VGA_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, WAVE_SINC))
	{
		if(!strcmp(argument, "dds_freq"))
		{
			int dds_freq = atoi(input);
			if(dds_freq>0)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, WAVE_TRIANGLE))
	{
		if(!strcmp(argument, "period"))
		{
			int period = atoi(input);
			if(period>=PERIOD_MIN && period<=PERIOD_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, COMMAND_EMIO))
	{
		if(!strcmp(argument, "base"))
		{
			int base = atoi(input);
			if(base>=0)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "nch"))
		{
			int ch = atoi(input);
			if(ch>0)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "value"))
		{
			int val = atoi(input);
			if(val>=0)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, COMMAND_DBG))
	{
		return 1;
	}
	else if(!strcmp(function, PLOT_ADCCAL))
	{
		if(!strcmp(argument, "len"))
		{
			int len = atoi(input);
			if(len>=SAMPLESIZE_MIN && len<=SAMPLESIZE_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, PLOT_ADCFFT))
	{
		int ch = atoi(input);
		if(ch==1 || ch==2)
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else if(!strcmp(function, PLOT_FFT4))
	{
		if(!strcmp(argument, "len"))
		{
			int len = atoi(input);
			if(len>0)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "rw"))
		{
			int rw = atoi(input);
			if(rw==0 || rw==1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, PLOT_FFTSPAN))
	{
		if(!strcmp(argument, "span"))
		{
			int span = atof(input);
			if(span>=0 && span<=1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "zip"))
		{
			int zip = atoi(input);
			if(zip==0 || zip==1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, PLOT_SWEEP))
	{
		if(!strcmp(argument, "span"))
		{
			long long span = get_frequency(input);
			if(span>=SWSPAN_MIN && span<=SWSPAN_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, PLOT_SWEEP2))
	{
		if(!strcmp(argument, "zip"))
		{
			int zip = atoi(input);
			if(zip==0 || zip==1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, PLOT_ADC))
	{
		int ch = atoi(input);
		if(ch==1 || ch==2)
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else if(!strcmp(function, PLOT_ADCIQ))
	{
		if(!strcmp(argument, "zip"))
		{
			int zip = atoi(input);
			if(zip==0 || zip==1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, PLOT_ADCTRIG))
	{
		if(!strcmp(argument, "zip"))
		{
			int zip = atoi(input);
			if(zip==0 || zip==1)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "mode"))
		{
			int mode = atoi(input);
			if(mode==TRIG_POS_I || mode==TRIG_POS_Q || mode==TRIG_NEG_I || mode==TRIG_NEG_Q)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "level"))
		{
			int lvl = atoi(input);
			if(lvl>=TRIG_LVL_MIN || lvl<=TRIG_LVL_MAX)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else if(!strcmp(argument, "channel"))
		{
			int ch = atoi(input);
			if(ch==1 || ch==2)
			{
				return 1;
			}
			else
			{
				print_help(function, argument);
				return 0;
			}
		}
		else
		{
			print_help(function, "NO ARG");
			return 1;
		}
	}
	else if(!strcmp(function, COMMAND_DACMAX))
	{
		int dac_max = atoi(input);
		if(dac_max>=0 && dac_max<=SAMPLESIZE_MAX)
		{
			return 1;
		}
		else
		{
			print_help(function, argument);
			return 0;
		}
	}
	else
	{
		print_help(function, argument);
		return 0;
	}
}

void print_help(char *function, char *argument)
{
	char arg_buf[200];
	char err_massage[200];

	sprintf(err_massage, "Error: Value for argument \'%s\' is not valid.", argument);

	if(!strcmp(function, COMMAND_TXBW) || !strcmp(function, COMMAND_RXBW))
	{
		sprintf(arg_buf, "[bandwidth <%d:%d>]", (int)BW_MIN, (int)BW_MAX);
	}
	else if(!strcmp(function, COMMAND_VGA))
	{
		sprintf(arg_buf, "[channel <1,2>] [vga gain <%d:%d>]", VGA_MIN, VGA_MAX);
	}
	else if(!strcmp(function, COMMAND_LNA))
	{
		sprintf(arg_buf, "[channel <1,2>] [lna gain <%d:%d>]", LNA_MIN, LNA_MAX);
	}
	else if(!strcmp(function, COMMAND_AGC))
	{
		sprintf(arg_buf, "[channel <1,2>] [agc <hyp,man,fas,slo>]");
	}
	else if(!strcmp(function, COMMAND_TXRATE) || !strcmp(function, COMMAND_RXRATE))
	{
		sprintf(arg_buf, "[rate <%d:%d>]", RATE_MIN, RATE_MAX);
	}
	else if(!strcmp(function, COMMAND_TXSS) || !strcmp(function, COMMAND_RXSS))
	{
		sprintf(arg_buf, "[sample size 2^n, n in range <4:13>]");
	}
	else if(!strcmp(function, COMMAND_CALIB))
	{
		sprintf(arg_buf, "[auto manual manual_tx_quad tx_quad rf_dc_offs rssi_gain_step]");
	}
	else if(!strcmp(function, COMMAND_TXFREQ) || !strcmp(function, COMMAND_RXFREQ)
			|| !strcmp(function, COMMAND_TXFREQ_CAL))
	{
		sprintf(arg_buf, "[LO frequency <%lld:%lld>]", (long long)FRQ_MIN, (long long)FRQ_MAX);
	}
	else if(!strcmp(function, COMMAND_RFOUT) || !strcmp(function, COMMAND_WINENABLE) ||
				!strcmp(function, COMMAND_RXFIR) || !strcmp(function, COMMAND_TXFIR) ||
				!strcmp(function, COMMAND_BBDC) || !strcmp(function, COMMAND_RFDC) ||
				!strcmp(function, COMMAND_QUAD) || !strcmp(function, COMMAND_NLF))
	{
		sprintf(arg_buf, "[enable <0,1>]");
	}
	else if(!strcmp(function, COMMAND_SIGPOW))
	{
		sprintf(arg_buf, "[channel <1,2>][power <%d:%d>]", SIGPOW_MIN, SIGPOW_MAX);
	}
	else if(!strcmp(function, COMMAND_VNAFREQ))
	{
		sprintf(arg_buf, "[frequency difference <%lld:%lld>]",
				(long long)DIFFRQ_MIN, (long long)DIFFRQ_MAX);
	}
	else if(!strcmp(function, COMMAND_VNAPOW))
	{
		sprintf(arg_buf, "[digital power <%d:%d>]", VNAPOW_MIN, VNAPOW_MAX);
	}
	else if(!strcmp(function, COMMAND_SVNA))
	{
		sprintf(arg_buf, "[freq <%lld:%lld>][lna1 <%d:%d>]"
				"[lna2 <%d:%d>][vga1 <%d:%d>][vga2 <%d:%d>]",
				(long long)FRQ_MIN, (long long)FRQ_MAX,
				LNA_MIN, LNA_MAX, LNA_MIN, LNA_MAX,
				VGA_MIN, VGA_MAX, VGA_MIN, VGA_MAX);
	}
	else if(!strcmp(function, COMMAND_SCAL))
	{
		sprintf(arg_buf, "[freq <%lld:%lld>][point <%d:%d>][port-rx <1,2>]"
				"[port-tx <1,2>][lna <%d:%d>][power <%d:%d>]",
				(long long)FRQ_MIN, (long long)FRQ_MAX, CALPNT_MIN, CALPNT_MAX,
				LNA_MIN, LNA_MAX, SIGPOW_MIN, SIGPOW_MAX);
	}
	else if(!strcmp(function, COMMAND_FILLPRO))
	{
		sprintf(arg_buf, "[freq <%lld:%lld>][span <%lld:%lld>]",
						(long long)FRQ_MIN, (long long)FRQ_MAX,
						(long long)SWSPAN_MIN, (long long)SWSPAN_MAX);
	}
	else if(!strcmp(function, COMMAND_REGISTER))
	{
		sprintf(arg_buf, "[address <hex>][value 0x<hex>]\r\n    example: reg a0 0x12");
	}
	else if(!strcmp(function, COMMAND_FIR_COEF))
	{
		sprintf(arg_buf, "[file_name]");
	}
	else if(!strcmp(function, WAVE_AWG))
	{
		sprintf(arg_buf, "[length <%d:%d>][offset <%d:%d>][channel <1,2>]",
				AWGLEN_MIN, AWGLEN_MAX, POWOFFSET_MIN, POWOFFSET_MAX);
	}
	else if(!strcmp(function, WAVE_SINDC))
	{
		sprintf(arg_buf, "[offset <%d:%d>][channel <1,2>][dig_amp <0:1>]",
						POWOFFSET_MIN, POWOFFSET_MAX);
	}
	else if(!strcmp(function, WAVE_PULSEIQ))
	{
		sprintf(arg_buf, "[I/Q <%d,%d,%d>][period <%d:%d>][duty_cycle <0:1>][offset <%d:%d>]"
				     "[channel <1,2>][dig_amp <0:1>]", SIGNAL_INPHASE_ENABLED,
					 SIGNAL_QUAD_ENABLED, SIGNAL_IQ_ENABLED, PERIOD_MIN,
					 PERIOD_MAX, POWOFFSET_MIN, POWOFFSET_MAX);
	}
	else if(!strcmp(function, WAVE_SINIQ))
	{
		sprintf(arg_buf, "[I/Q <%d,%d,%d>][period <%d:%d>][offset <%d:%d>][channel <1,2>]"
					 "[dig_amp <0:1>]", SIGNAL_INPHASE_ENABLED, SIGNAL_QUAD_ENABLED,
					 SIGNAL_IQ_ENABLED, PERIOD_MIN, PERIOD_MAX,
					 POWOFFSET_MIN, POWOFFSET_MAX);
	}
	else if(!strcmp(function, WAVE_SIN))
	{
		sprintf(arg_buf, "[period <%d:%d>][dig_amp <0:1>][channel <1,2>]",
				            PERIOD_MIN, PERIOD_MAX);
	}
	else if(!strcmp(function, WAVE_DC))
	{
		sprintf(arg_buf, "[vga gain <%d:%d>][channel <1,2>]", VGA_MIN, VGA_MAX);
	}
	else if(!strcmp(function, WAVE_PULSE))
	{
		sprintf(arg_buf, "[period <%d:%d>][duty_cycle <0:1>][dig_amp <0:1>]"
						     "[channel <1,2>]", PERIOD_MIN, PERIOD_MAX);
	}
	else if(!strcmp(function, WAVE_SINC))
	{
		sprintf(arg_buf, "[dds freq <0:?>][channel <1,2>]");
	}
	else if(!strcmp(function, WAVE_TRIANGLE))
	{
		sprintf(arg_buf, "[period <%d:%d>][channel <1,2>]", PERIOD_MIN, PERIOD_MAX);
	}
	else if(!strcmp(function, COMMAND_EMIO))
	{
		sprintf(arg_buf, "[base <0:?>][nchannel <1:?>][value <0:?>]");
	}
	else if(!strcmp(function, COMMAND_DBG))
	{
		sprintf(arg_buf, "[attribute][value]");
	}
	else if(!strcmp(function, PLOT_ADCCAL))
	{
		sprintf(arg_buf, "[length <%d:%d>][channel <1,2>]", SAMPLESIZE_MIN, SAMPLESIZE_MAX);
	}
	else if(!strcmp(function, PLOT_ADCFFT))
	{
		sprintf(arg_buf, "[channel <1,2>]");
	}
	else if(!strcmp(function, PLOT_FFT4))
	{
		sprintf(arg_buf, "[R/W <0,1>][len <1:8192]");
	}
	else if(!strcmp(function, PLOT_FFTSPAN))
	{
		sprintf(arg_buf, "[span ratio <0:1>][channel <1,2>][compression <0,1>]");
	}
	else if(!strcmp(function, PLOT_SWEEP))
	{
		sprintf(arg_buf, "[channel <1,2>][span <%lld:%lld>]",
				(long long)SWSPAN_MIN, (long long)SWSPAN_MAX);
	}
	else if(!strcmp(function, PLOT_SWEEP2))
	{
		sprintf(arg_buf, "[channel <1,2>][compression <0,1>]");
	}
	else if(!strcmp(function, PLOT_ADC))
	{
		sprintf(arg_buf, "[channel <1,2>]");
	}
	else if(!strcmp(function, PLOT_ADCIQ))
	{
		sprintf(arg_buf, "[channel <1,2>][compression <0,1>]");
	}
	else if(!strcmp(function, PLOT_ADCTRIG))
	{
		sprintf(arg_buf, "[mode <%d,%d,%d,%d>][level <%d:%d>][channel <1,2>][compression <0,1>]",
				TRIG_POS_I, TRIG_NEG_I, TRIG_POS_Q, TRIG_NEG_Q, TRIG_LVL_MIN,
				TRIG_LVL_MAX);
	}
	else if(!strcmp(function, COMMAND_DACMAX))
	{
		sprintf(arg_buf, "[DAC max <0:%d>]", SAMPLESIZE_MAX);
	}
	else
	{
		sprintf(err_massage, "Error: function %s not found.", function);
		sprintf(arg_buf, "cannot use this function!");
	}

	pna_printf("---------------------------------------------------------------\r\n"
							"%s\r\nUsage:\r\n    %s: %s\r\n"
							"---------------------------------------------------------------\r\n" ,
							err_massage, function, arg_buf);
}

void print_error(char *function, int error_code)
{
	char arg_buf[200];
	char err_massage[200];
	char usage[200];
	char cmd_name[20];

	if(error_code == ERROR_ARG)
	{
		strcpy(err_massage, "arguments are not enough.");
	}
	else if(error_code == ERROR_CH)
	{
		strcpy(err_massage, "Channel number should be 1 or 2.");
	}
	else
	{
		strcpy(err_massage, "Error massage not found");
	}

	if(!strcmp(function, "sweep"))
	{
		strcpy(arg_buf, "[port#][compression enable]");
		strcpy(usage, "Capture spectrum of signal while sweeping.");
		strcpy(cmd_name, "sweep");
	}
	else if(!strcmp(function, "fft"))
	{
		strcpy(arg_buf, "[span ratio][port#][compression enable]");
		strcpy(usage, "Capture spectrum of signal.");
		strcpy(cmd_name, "fft_span");
	}
	else if(!strcmp(function, "fft4"))
	{
		strcpy(arg_buf, "[r:0/w:1][bytes]");
		strcpy(usage, "FFT module read/write test.");
		strcpy(cmd_name, "fft4");
	}
	else if(!strcmp(function, "adc"))
	{
		strcpy(arg_buf, "[port#][compression enable]");
		strcpy(usage, "Capture receiver signal from port.");
		strcpy(cmd_name, "adc_iq");
	}
	else if(!strcmp(function, "trigged adc"))
	{
		strcpy(arg_buf, "[mode][level][port#][compression enable]");
		strcpy(usage, "Capture receiver signal from port and trig by level and mode.");
		strcpy(cmd_name, "adc_trig");
	}
	else
	{
		strcpy(usage, "");
		strcpy(arg_buf, "");
		strcpy(cmd_name, "");
		strcpy(err_massage, "function not found");
	}

	pna_printf("---------------------------------------------------------------\r\n"
						"%s: %s\r\n%s\r\nUsage:\r\n    %s %s\r\n"
						"---------------------------------------------------------------\r\n" ,
						function, err_massage, usage, cmd_name, arg_buf);
}
