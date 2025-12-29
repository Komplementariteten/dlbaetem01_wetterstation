#include "daisy_seed.h"
#include <string>
#include <array>
#include <math.h>

#define NUMBER_OF_VALUES 550
#define SIN_POINTS 512
#define TEST_MEMORY_SIZE (NUMBER_OF_VALUES * 16 * 3)

#define DAC_MAX 4095  // 12 bit

using namespace daisy;

DaisySeed hw;
static size_t value_count = 0;
// 16bit adc
const float_t ADC_MAX = 0xFFFF;
const float_t VEE = 3.3;
const float_t R_1 = 1000;

static std::array<uint16_t, NUMBER_OF_VALUES> SINE_WAVE_LOOKUP;

void calculate_sine_wave_lookup(){
	for (size_t i = 0; i < (SIN_POINTS - 1); i++)
	{
		// We only want the positive half wave
		float_t v = (DAC_MAX * sin((2.12 * i) / SIN_POINTS));
		SINE_WAVE_LOOKUP[i] = static_cast<uint16_t>(v);
		// hw.PrintLine("%3d;%7d:%7.5f", i, SINE_WAVE_LOOKUP[i], v);
	}
	SINE_WAVE_LOOKUP[SIN_POINTS - 1] = 0;	
}

void store_value(uint16_t u1, uint16_t u2, uint16_t u3){
	int32_t *ram = (int32_t *)0xC0000000;
	unsigned char const *p1 = reinterpret_cast<unsigned char const *>(&u1);
	memcpy((sizeof(uint16_t) * value_count) + ram, p1, sizeof(uint16_t));
	value_count++;
	unsigned char const *p2 = reinterpret_cast<unsigned char const *>(&u2);
	memcpy((sizeof(uint16_t) * value_count) + ram, p2, sizeof(uint16_t));
	value_count++;
	unsigned char const *p3 = reinterpret_cast<unsigned char const *>(&u3);
	memcpy((sizeof(uint16_t) * value_count) + ram, p3, sizeof(uint16_t));
	value_count++;
}

uint16_t read_value(size_t ) {

}

void calculate_voltages_and_temp(){
	int32_t *ram = (int32_t *)0xC0000000;
	for (size_t i = 0; i < value_count; i++)
	{
		auto v_type = i % 3;
		v_type = v_type == 0 ? 3 : v_type;
		uint16_t result;
		unsigned char const *p = (unsigned char const *)((sizeof(uint16_t) * i) + ram);
		memcpy(&result, p, sizeof(uint16_t));
		// Calculate Voltage
		uint16_t sin_value_indes = i / 3;
		float_t v =  VEE * (result / ADC_MAX);
		hw.PrintLine("%4d;%1d;%6d;%2.6f;%5d#", i, v_type, result, v, sin_value_indes < SIN_POINTS ? SINE_WAVE_LOOKUP[sin_value_indes]: 0);
	}
	value_count = 0;
}

int main(void)
{
	hw.Init();
	hw.SetLed(true);

	/** Configure and Initialize the DAC */
    DacHandle::Config dac_cfg;
    dac_cfg.bitdepth   = DacHandle::BitDepth::BITS_12;
    dac_cfg.buff_state = DacHandle::BufferState::ENABLED;
    dac_cfg.mode       = DacHandle::Mode::POLLING;
    dac_cfg.chn        = DacHandle::Channel::ONE;
    hw.dac.Init(dac_cfg);

	AdcChannelConfig adc_config[3];
	adc_config[0].InitSingle(seed::A6);
	adc_config[1].InitSingle(seed::A5);
	adc_config[2].InitSingle(seed::A4);
	hw.adc.Init(adc_config, 3);
	hw.adc.Start();

	Switch start_button;
	start_button.Init(seed::D26);


	bool do_measurement = false;
	calculate_sine_wave_lookup();

	hw.SetLed(false);

	// Don't wait for Serial connection
	hw.StartLog(false);

	while (1)
	{
		start_button.Debounce();
		do_measurement = start_button.Pressed();
		
		if(do_measurement) {
			hw.SetLed(true);
			for(int i = 0; i < NUMBER_OF_VALUES; i++){
				if(i < SIN_POINTS){
					hw.dac.WriteValue(DacHandle::Channel::ONE, SINE_WAVE_LOOKUP[i]);
				} else if (i == SIN_POINTS)
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
		}
		System::Delay(1);
	}
	
}
