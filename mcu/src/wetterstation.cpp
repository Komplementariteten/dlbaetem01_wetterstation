#include <array>
#include <math.h>
#include "daisy_seed.h"
#define NUMBER_OF_VALUES 2048
#define SIN_POINTS 50
#define TEST_MEMORY_SIZE (NUMBER_OF_VALUES * 16 * 3)

#define DAC_OUT 1595 // 12 bit (4095) as 3V

using namespace daisy;

DaisySeed hw;
static size_t value_count = 0;
// 16bit adc
const float_t ADC_MAX = 0xFFFF;
const float_t VEE = 3.3;
const float_t R_1 = 1000;
const float_t PI_F = 3.14159265359;
const int32_t R22 = 499000;
const int32_t R4 = 49900;
const float_t numerator_c = ((R4 + (2 * R22)) * log(2));
const float_t c = 3245.9;
const float_t a3 = -10457;
const float_t a2 = 10732;
const float_t a1 = -3465.6;
const int32_t zero_threshold = 2;

// static std::array<uint16_t, NUMBER_OF_VALUES> SINE_WAVE_LOOKUP;

// static std::array<uint16_t, NUMBER_OF_VALUES> ADC1_VALUES;
// static std::array<uint16_t, NUMBER_OF_VALUES> ADC2_VALUES;
// static std::array<uint16_t, NUMBER_OF_VALUES> ADC3_VALUES;

static float_t calculate_c(const float_t freq) {
	return numerator_c / freq;
}

const float_t calculate_rh(const float_t c, const float_t freq) {
	const float_t x = 1 / (2 * PI_F * freq * c);
	return (a1 * pow(x, 3) + (a2 * pow(x, 2)) + (a3 * x) + c);
}

void store_value(uint16_t u1, uint16_t u2, uint16_t u3){
	int32_t *ram = (int32_t *)0xC0000000;
	memcpy((sizeof(uint16_t) * value_count) + ram, &u1, sizeof(uint16_t));
	value_count++;
	memcpy((sizeof(uint16_t) * value_count) + ram, &u2, sizeof(uint16_t));
	value_count++;
	memcpy((sizeof(uint16_t) * value_count) + ram, &u3, sizeof(uint16_t));
	value_count++;
}

int main(void)
{
	hw.Init();
	hw.SetLed(true);

	/** Configure and Initialize the DAC */
	DacHandle::Config dac_cfg;
	dac_cfg.bitdepth = DacHandle::BitDepth::BITS_12;
	dac_cfg.buff_state = DacHandle::BufferState::ENABLED;
	dac_cfg.mode = DacHandle::Mode::POLLING;
	dac_cfg.chn = DacHandle::Channel::ONE;
	hw.dac.Init(dac_cfg);

	AdcChannelConfig adc_config[3];
	adc_config[0].InitSingle(seed::A6);
	adc_config[1].InitSingle(seed::A5);
	adc_config[2].InitSingle(seed::A4);
	hw.adc.Init(adc_config, 3);
	hw.adc.Start();

	hw.SetLed(false);

	// Don't wait for Serial connection
	hw.StartLog(false);

	// Calculate delay time to get output Frequency
	uint32_t tick1 = System::GetTick();
	System::DelayUs(1);
	uint32_t tick2 = System::GetTick();
	uint32_t delay_time = (tick2 - tick1) * (500000 / 6650);
	auto c = calculate_c(delay_time);
	calculate_rh(c, delay_time);

	bool out_high = true;

	hw.SetLed(true);

	uint32_t current_ticks = 0;

	while(1)
	{
		uint32_t delta_ticks = System::GetTick() - current_ticks;
		// Handle Output
		if(out_high) {
			hw.dac.WriteValue(DacHandle::Channel::ONE, DAC_OUT);
			
		} else {
			hw.dac.WriteValue(DacHandle::Channel::ONE, 0);
		}

		if(delta_ticks > delay_time) {
			out_high = !out_high;
			current_ticks = System::GetTick();
		}

		/* if (do_measurement)
		{
			hw.SetLed(true);
			for (int i = 0; i < NUMBER_OF_VALUES; i++)
			{
				if (i < SIN_POINTS)
				{
					hw.dac.WriteValue(DacHandle::Channel::ONE, SINE_WAVE_LOOKUP[i]);
				}
				else if (i == SIN_POINTS)
				{
					hw.dac.WriteValue(DacHandle::Channel::ONE, 0);
				}
				store_value(hw.adc.Get(0), hw.adc.Get(1), hw.adc.Get(2));
				System::DelayUs(10);
			}

			calculate_voltages_and_temp();
			hw.PrintLine("DONE");
			System::Delay(40);
			hw.SetLed(false);
			// Wait 5 seconds
		} */
	}
}
