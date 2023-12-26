#include <pstl/glue_numeric_defs.h>

#include "daisy_seed.h"
#include "daisysp.h"
#include <string>

#define NUMBER_OF_VALUES 1024
#define TEST_MEMORY_SIZE (NUMBER_OF_VALUES * 16)

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
static size_t value_count = 0;
// 16bit adc
const float_t ADC_MAX = 0xFFFF;
const float_t VEE = 3.3;
const float_t R_1 = 1000;

void store_value(uint16_t value){
	int32_t *ram = (int32_t *)0xC0000000;
	unsigned char const *p = reinterpret_cast<unsigned char const *>(&value);
	memcpy((sizeof(uint16_t) * value_count) + ram, p, sizeof(uint16_t));
	value_count++;
}

void calculate_voltages_and_temp(){
	int32_t *ram = (int32_t *)0xC0000000;
	for (size_t i = 1; i < value_count; i++)
	{
		uint16_t result;
		unsigned char const *p = (unsigned char const *)((sizeof(uint16_t) * i) + ram);
		memcpy(&result, p, sizeof(uint16_t));
		// Calculate Voltage
		float_t v =  VEE * (result / ADC_MAX);
		// Calculate Resistance
		float_t t = ((VEE - v) * R_1) / v;
		hw.PrintLine("%7d;%2.6f;%4.6f", result, v, t);
	}
	value_count = 0;
}

int main(void)
{
	hw.Init();

	/** Configure and Initialize the DAC */
    DacHandle::Config dac_cfg;
    dac_cfg.bitdepth   = DacHandle::BitDepth::BITS_12;
    dac_cfg.buff_state = DacHandle::BufferState::ENABLED;
    dac_cfg.mode       = DacHandle::Mode::POLLING;
    dac_cfg.chn        = DacHandle::Channel::ONE;
    hw.dac.Init(dac_cfg);

	bool dac_is_high = false;

	// Wait for Serial connection
	hw.StartLog(true);
	hw.SetLed(true);
	AdcChannelConfig adcConfig;
	adcConfig.InitSingle(hw.GetPin(21));

	hw.adc.Init(&adcConfig, 1);
	hw.adc.Start();
	while(value_count < NUMBER_OF_VALUES) {	
		System::DelayUs(48);
		if(dac_is_high){
			hw.dac.WriteValue(DacHandle::Channel::ONE, 0b0); 
			dac_is_high = false;
		}
		else {
			// On rising Edge
			store_value(hw.adc.Get(0));
			// 4096 ~ 3.3V => 1284 ~ 1V
			hw.dac.WriteValue(DacHandle::Channel::ONE, 1284);
			dac_is_high = true;
		}
	}
	// 
	System::Delay(1000);
	// done turn of the LED
	hw.SetLed(false);
	calculate_voltages_and_temp();	
}
