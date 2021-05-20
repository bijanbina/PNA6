#include "pna-siggen.h"

extern int8_t dac_buf[8*MAX_FFT_LENGTH];
extern unsigned char awg_data[MAX_FFT_LENGTH];

void send_dc_signal(int dds_sample_size, int amp, int channel_num)
{
	int s_size = get_sample_size();
	/*int amplitude_i_int = amplitude_i*dac_max/2;
	int amplitude_q_int = amplitude_q*dac_max/2;

	for (int i=0 ; i<dds_sample_size ; i++)
	{
		double inphase = amplitude_i_int;
		int16_t inphase_int = (int16_t)(inphase);
		double quad = amplitude_q_int;
		int16_t quad_int = (int16_t)(quad);
		dac_buf[i*s_size+channel_num*4] = (int8_t)(inphase_int%256);   // LSB
		dac_buf[i*s_size+channel_num*4+1] = (int8_t)(inphase_int/256);     // MSB
		dac_buf[i*s_size+channel_num*4+2] = (int8_t)(quad_int%256);   // LSB
		dac_buf[i*s_size+channel_num*4+3] = (int8_t)(quad_int/256);     // MSB
	}*/
	for (int i=0 ; i<dds_sample_size ; i++)
	{
		int16_t amp_short_int = (int16_t)(amp);
		dac_buf[i*s_size+channel_num*4+0] = (int8_t)(amp_short_int%256);   // LSB
		dac_buf[i*s_size+channel_num*4+1] = (int8_t)(amp_short_int/256);     // MSB
		dac_buf[i*s_size+channel_num*4+2] = (int8_t)(amp_short_int%256);   // LSB
		dac_buf[i*s_size+channel_num*4+3] = (int8_t)(amp_short_int/256);     // MSB
	}
	create_dds_buffer(dac_buf, dds_sample_size);
}

void send_sinc_signal(int dds_sample_size, int amp, double dds_freq, int channel_num)
{
	int s_size = get_sample_size();

	for (int i=0 ; i<dds_sample_size ; i++)
	{
		double x = i-dds_sample_size/2;
		double sinc;
		if ( x==0 )
		{
			sinc = amp;
		}
		else
		{
			sinc = sin(x/dds_freq)/(x/dds_freq)*amp;
		}
		int16_t sinc_int = (int16_t)(sinc);
		dac_buf[i*s_size+channel_num*4+0] = (int8_t)(sinc_int%256);   // LSB
		dac_buf[i*s_size+channel_num*4+1] = (int8_t)(sinc_int/256);     // MSB
		dac_buf[i*s_size+channel_num*4+2] = 0;     // MSB
		dac_buf[i*s_size+channel_num*4+3] = 0;     // MSB
		// pna_printf("DAC Buffer[%d]= %d ,\tx= %f\t sinc=%f, \t sinc_int=%d\r\n", i*s_size+channel_num*2+1, dac_buf[i*s_size+channel_num*2+1], x, sinc, sinc_int );
	}
	create_dds_buffer(dac_buf, dds_sample_size);
	for ( int i=dds_sample_size/2-5 ; i<dds_sample_size/2+5 ; i++)
	{
		pna_printf("DAC Buffer[%d]= %d ,%d , x= %d\r\n",
				i*s_size+channel_num*2+1,
				(uint8_t) dac_buf[i*s_size+channel_num*2+1],
				(uint8_t)dac_buf[i*s_size+channel_num*2],
				i-dds_sample_size/2);
	}
}

void send_pulse_signal(int dds_sample_size, int amp, int period_num, double duty_cycle, int sig_iq, int channel_num)
{
	int s_size = get_sample_size();
	int period_samples = dds_sample_size/period_num;
	int on_pulse_samples = duty_cycle*period_samples;
	double pulse, p_hilbert;

	for (int i=0; i<dds_sample_size; i++)
	{
		//// Real Part
		if(i%period_samples < on_pulse_samples &&
				(sig_iq == SIGNAL_INPHASE_ENABLED || sig_iq == SIGNAL_IQ_ENABLED))
		{
			pulse = amp;
		}
		else
		{
			pulse = 0;
		}

		//// Imaginary Part
		p_hilbert = 0;
		if(sig_iq == SIGNAL_QUAD_ENABLED || sig_iq == SIGNAL_IQ_ENABLED)
		{
			for(int j=-2; j<period_num+2; j++)
			{
				double num = i - j*period_samples;
				double denum = i -  j*period_samples - on_pulse_samples;
				if(num == 0) // log(0)
				{
					num = (i-1) - j*period_samples;
				}
				if(denum == 0) // log(x/0)
				{
					denum = i-1 -  j*period_samples - on_pulse_samples;
				}
				p_hilbert += amp*log(fabs(num/denum))/PI;
			}
		}
		else
		{
			p_hilbert = 0;
		}

		int16_t pulse_int = (int16_t)(pulse);
		int16_t p_hilbert_int = (int16_t)(p_hilbert);

		dac_buf[i*s_size+channel_num*4] = (int8_t)(pulse_int%256);   // LSB
		dac_buf[i*s_size+channel_num*4+1] = (int8_t)(pulse_int/256);     // MSB
		dac_buf[i*s_size+channel_num*4+2] = (int8_t)(p_hilbert_int%256);   // LSB
		dac_buf[i*s_size+channel_num*4+3] = (int8_t)(p_hilbert_int/256);     // MSB
	}
	create_dds_buffer(dac_buf, dds_sample_size);
}

void send_sin_signal(int dds_sample_size, int amp, int period_num, int sig_iq,  int channel_num)
{
	double sinous, cosinous, x;
	int s_size = get_sample_size();
	int period_sample_count = dds_sample_size/period_num;

	for (int i=0 ; i<period_num ; i++)
	{
		for(int j=0; j<period_sample_count; j++)
		{
			x = j*2*PI;
			x = x/period_sample_count;
			if(sig_iq == SIGNAL_QUAD_ENABLED || sig_iq == SIGNAL_IQ_ENABLED)
			{
				sinous = sin(x)*amp;// - dc_offset;
			}
			else
			{
				sinous = 0;
			}
			if(sig_iq == SIGNAL_INPHASE_ENABLED || sig_iq == SIGNAL_IQ_ENABLED)
			{
				cosinous = cos(x)*amp;// - dc_offset;
			}
			else
			{
				cosinous = 0;
			}
			int16_t sin_int = (int16_t)(sinous);
			int16_t cos_int = (int16_t)(cosinous);

			dac_buf[(j+i*period_sample_count)*s_size+channel_num*4] = (int8_t)(sin_int%256);   // LSB
			dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+1] = (int8_t)(sin_int/256);     // MSB
			dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+2] = (int8_t)(cos_int%256);   // LSB
			dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+3] = (int8_t)(cos_int/256);     // MSB
		}
	}
	create_dds_buffer(dac_buf, period_num*period_sample_count);
}

void send_triangle_signal(int dds_sample_size, int amp, int period_num, int channel_num)
{
	double triangle;
	int period_samples = dds_sample_size/period_num;
	int s_size = get_sample_size();

	for (int i=0; i<dds_sample_size; i++)
	{
		//// Real Part
		if(i%period_samples < period_samples/2)
		{
			triangle = (float)amp * 2.0 * (float) (i%(period_samples/2))/period_samples;
		}
		else
		{
			triangle = (float)amp * 2.0 * (float)(period_samples/2 - (i%(period_samples/2)))/period_samples;
		}
		//// Imaginary Part
		/*triangle_h = 0;
		if(sig_iq == SIGNAL_QUAD_ENABLED || sig_iq == SIGNAL_IQ_ENABLED)
		{
			for(int j=-2; j<period_num+2; j++)
			{
				double num = i - j*period_samples;
				double denum = i -  j*period_samples - on_pulse_samples;
				if(num == 0) // log(0)
				{
					num = (i-1) - j*period_samples;
				}
				if(denum == 0) // log(x/0)
				{
					denum = i-1 -  j*period_samples - on_pulse_samples;
				}
				p_hilbert += amplitude_int*log(fabs(num/denum))/PI;
			}
		}
		else
		{
			p_hilbert = 0;
		}*/

		int16_t triangle_int = (int16_t)(triangle);

		dac_buf[i*s_size+channel_num*4+0] = (int8_t)(triangle_int%256);   // LSB
		dac_buf[i*s_size+channel_num*4+1] = (int8_t)(triangle_int/256);     // MSB
		dac_buf[i*s_size+channel_num*4+2] = 0;   // LSB
		dac_buf[i*s_size+channel_num*4+3] = 0;     // MSB
	}
	create_dds_buffer(dac_buf, dds_sample_size);
}

void send_awg_signal(int dds_sample_size, double dig_amp, int channel_num)
{
	int s_size = get_sample_size();

	for(int i=0; i<dds_sample_size; i++) // 2 for [I/Q]
	{
		char number_str[5];
		memcpy(number_str, awg_data+2*i*AWG_NUM_WIDTH, AWG_NUM_WIDTH);
		number_str[4] = 0;
		int number_i = atoi(number_str) - 2048;
		double number_float_i = (double)number_i;
		number_float_i = number_float_i*dig_amp;
		number_i = (int)floor(number_float_i);
		int16_t number16_i = (int16_t)(number_i*16);

		memcpy(number_str, awg_data + (2*i+1)*AWG_NUM_WIDTH, AWG_NUM_WIDTH);
		number_str[4] = 0;
		int number_q = atoi(number_str) - 2048;
		double number_float_q = (double)number_q;
		number_float_q = number_float_q*dig_amp;
		number_q = (int)floor(number_float_q);
		int16_t number16_q = (int16_t)(number_q*16);
		dac_buf[i*s_size+channel_num*4] = (int8_t)(number16_i%256);   // LSB
		dac_buf[i*s_size+channel_num*4+1] = (int8_t)(number16_i/256);     // MSB
		dac_buf[i*s_size+channel_num*4+2] = (int8_t)(number16_q%256);   // LSB
		dac_buf[i*s_size+channel_num*4+3] = (int8_t)(number16_q/256);     // MSB
	}
	create_dds_buffer(dac_buf, dds_sample_size);
}
