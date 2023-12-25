#include <pstl/glue_numeric_defs.h>

#include "daisy_seed.h"
#include "daisysp.h"
#include <string>

#define NUMBER_OF_VALUES 1024
#define TEST_MEMORY_SIZE (NUMBER_OF_VALUES * 16)

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
static int value_count = 0;


void store_value(uint16_t value){
	int32_t *ram = (int32_t *)0xC0000000;
	unsigned char const *p = reinterpret_cast<unsigned char const *>(&value);
	memcpy((sizeof(uint16_t) * value_count) + ram, p, sizeof(uint16_t));
	value_count++;
}

void get_avg_from_ram(){
	int32_t *ram = (int32_t *)0xC0000000;
	uint64_t sum = 0.0;
	for (size_t i = 0; i < NUMBER_OF_VALUES; i++)
	{
		uint16_t result;
		unsigned char const *p = (unsigned char const *)((sizeof(uint16_t) * i) + ram);
		memcpy(&result, p, sizeof(uint16_t));
		sum += result;
		System::Delay(1);
	}
	uint16_t avg = sum / NUMBER_OF_VALUES;
	System::Delay(1);
}


void collect_temperature() {
		for (size_t i = 0; i < NUMBER_OF_VALUES; i++)
		{
			store_value(hw.adc.Get(0));
			System::Delay(1);
		}
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

    // hw.PrintLine("Verify CRT floating point format: %.3f.", 123.0f);
	hw.StartLog();
	hw.PrintLine("Welcom, lets start");
	Led led1;
	led1.Init(hw.GetPin(28), false);
	AdcChannelConfig adcConfig;
	adcConfig.InitSingle(hw.GetPin(21));

	hw.adc.Init(&adcConfig, 1);
	hw.adc.Start();
	hw.PrintLine("Adc Started, entering Loop");
	while(1) {	
		System::Delay(10);
		hw.PrintLine("ADC 1: %d", hw.adc.Get(0));
		if(dac_is_high){
			hw.dac.WriteValue(DacHandle::Channel::ONE, 0b0); 
			dac_is_high = false;
		}
		else {
			hw.dac.WriteValue(DacHandle::Channel::ONE, 0b111111111111);
			dac_is_high = true;
		}
	}
}
