/*
 * effects_switch.c
 *
 * Created: 10/23/2019 11:09:46 PM
 * Author : leaf
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

// IO ports
#define OUT_RELAYS PORTA
#define IN_BUTTONS PINB
#define LED_PATCH PORTD
#define LED_BANK PORTE

// timing constants
#define PRE_MUTE_DELAY 15
#define POST_MUTE_DELAY 15
#define DEBOUNCE_DELAY 500
#define SHORT_PRESS_DELAY 200
#define LONG_PRESS_DELAY 300

void changePatch(uint8_t newPatch)
{
	// mute output, switch relays, unmute output
	PORTC = 0b00000001;
	_delay_ms(PRE_MUTE_DELAY);
	OUT_RELAYS = newPatch;
	_delay_ms(POST_MUTE_DELAY);
	PORTC = 0b00000000;
}

void savePatch(uint8_t save_bank, uint8_t save_patch, uint8_t save_loops)
{
	// save specified patch to EEPROM
	eeprom_update_byte((uint8_t*)((save_bank*8)+save_patch), save_loops);
	
	// blink appropriate patch LED
	for (uint8_t i = 0; i < 3; i++)
	{
		LED_PATCH = 0b00000000;
		_delay_ms(150);
		LED_PATCH = (1<<save_patch);
		_delay_ms(150);
	}
}

int main(void)
{
	// configure IO
	DDRA = 0b11111111;	// relay outputs
	DDRB = 0b00000000;	// button inputs
	DDRC = 0b00000001;	// mute output
	DDRD = 0b11111111;	// patch LED outputs
	DDRE = 0b00000111;	// bank LED outputs
	
	// initialize outputs to off
    PORTA = 0b00000000;
	PORTC = 0b00000000;
	PORTD = 0b00000000;
	PORTE = 0b00000000;
	
	// initialize patch variables
	uint8_t current_bank = 0;
	uint8_t current_patch = 0;
	uint8_t patch[3][8];
	
	// load patches from EEPROM
	for (uint8_t i_bank = 0; i_bank <= 3; i_bank++)
	{
		for (uint8_t i_patch = 0; i_patch <= 7; i_patch++)
		{
			patch[i_bank][i_patch] = eeprom_read_byte((uint8_t*)((i_bank*8)+i_patch));
		}
	}
	
	// initialize to first patch
	LED_PATCH = 0b00000001;
	LED_BANK = 0b00000001;
	uint8_t loaded_patch = patch[current_bank][current_patch];
	changePatch(loaded_patch);
	
	// main loop
    while (1) 
    {
		switch (IN_BUTTONS) {
			case 0b11111111:
				break;
			case 0b11111110:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					// select patch 0
					current_patch = 0;
					LED_PATCH = (1<<current_patch);
					loaded_patch = patch[current_bank][current_patch];
					changePatch(loaded_patch);
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b11111110)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11111110)
					{
						// toggle loop 0
						loaded_patch ^= (1 << 0);
						changePatch(loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				else if (IN_BUTTONS == 0b11111100)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11111100)
					{
						// save current patch
						patch[current_bank][current_patch] = loaded_patch;
						savePatch(current_bank, current_patch, loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				break;
			case 0b11111101:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					// select patch 1
					current_patch = 1;
					LED_PATCH = (1<<current_patch);
					loaded_patch = patch[current_bank][current_patch];
					changePatch(loaded_patch);
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b11111101)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11111101)
					{
						// toggle loop 1
						loaded_patch ^= (1 << 1);
						changePatch(loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				else if (IN_BUTTONS == 0b11111100)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11111100)
					{
						// save current patch
						patch[current_bank][current_patch] = loaded_patch;
						savePatch(current_bank, current_patch, loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				break;
			case 0b11111100:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b11111100)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11111100)
					{
						// save current patch
						patch[current_bank][current_patch] = loaded_patch;
						savePatch(current_bank, current_patch, loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
				}
				break;
			case 0b11111011:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					// select patch 2
					current_patch = 2;
					LED_PATCH = (1<<current_patch);
					loaded_patch = patch[current_bank][current_patch];
					changePatch(loaded_patch);
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b11111011)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11111011)
					{
						// toggle loop 2
						loaded_patch ^= (1 << 2);
						changePatch(loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				else if (IN_BUTTONS == 0b11110011)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11110011)
					{
						// bank down
						if (current_bank > 0)
						{
							current_bank--;
							LED_BANK = (1<<current_bank);
						}
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				break;
			case 0b11110111:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					// select patch 3
					current_patch = 3;
					LED_PATCH = (1<<current_patch);
					loaded_patch = patch[current_bank][current_patch];
					changePatch(loaded_patch);
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b11110111)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11110111)
					{
						// toggle loop 3
						loaded_patch ^= (1 << 3);
						changePatch(loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				else if (IN_BUTTONS == 0b11110011)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11110011)
					{
						// bank down
						if (current_bank > 0)
						{
							current_bank--;
							LED_BANK = (1<<current_bank);
						}
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				break;
			case 0b11110011:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b11110011)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11110011)
					{
						// bank down
						if (current_bank > 0)
						{
							current_bank--;
							LED_BANK = (1<<current_bank);
						}
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				break;
			case 0b11101111:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					// select patch 4
					current_patch = 4;
					LED_PATCH = (1<<current_patch);
					loaded_patch = patch[current_bank][current_patch];
					changePatch(loaded_patch);
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b11101111)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11101111)
					{
						// toggle loop 4
						loaded_patch ^= (1 << 4);
						changePatch(loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				else if (IN_BUTTONS == 0b11001111)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11001111)
					{
						// bank up
						if (current_bank < 3)
						{
							current_bank++;
							if (current_bank == 3)
							{
								LED_BANK = 0b00000111;
							}
							else
							{
								LED_BANK = (1<<current_bank);
							}
						}
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				break;
			case 0b11011111:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					// select patch 5
					current_patch = 5;
					LED_PATCH = (1<<current_patch);
					loaded_patch = patch[current_bank][current_patch];
					changePatch(loaded_patch);
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b11011111)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if  (IN_BUTTONS == 0b11011111)
					{
						// toggle loop 5
						loaded_patch ^= (1 << 5);
						changePatch(loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				else if (IN_BUTTONS == 0b11001111)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11001111)
					{
						// bank up
						if (current_bank < 3)
						{
							current_bank++;
							if (current_bank == 3)
							{
								LED_BANK = 0b00000111;
							}
							else
							{
								LED_BANK = (1<<current_bank);
							}
						}
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				break;
			case 0b11001111:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b11001111)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b11001111)
					{
						// bank up
						if (current_bank < 3)
						{
							current_bank++;
							if (current_bank == 3)
							{
								LED_BANK = 0b00000111;
							}
							else
							{
								LED_BANK = (1<<current_bank);
							}
						}
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				break;
			case 0b10111111:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					// select patch 6
					current_patch = 6;
					LED_PATCH = (1<<current_patch);
					loaded_patch = patch[current_bank][current_patch];
					changePatch(loaded_patch);
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b10111111)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b10111111)
					{
						// toggle loop 6
						loaded_patch ^= (1 << 6);
						changePatch(loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				else if (IN_BUTTONS == 0b00111111)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b00111111)
					{
						// reset current patch
						loaded_patch = 0b00000000;
						changePatch(loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				break;
			case 0b01111111:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					// select patch 7
					current_patch = 7;
					LED_PATCH = (1<<current_patch);
					loaded_patch = patch[current_bank][current_patch];
					changePatch(loaded_patch);
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b01111111)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b01111111)
					{
						// toggle loop 7
						loaded_patch ^= (1 << 7);
						changePatch(loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				else if (IN_BUTTONS == 0b00111111)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b00111111)
					{
						// reset current patch
						loaded_patch = 0b00000000;
						changePatch(loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				break;
			case 0b00111111:
				_delay_ms(SHORT_PRESS_DELAY);
				if (IN_BUTTONS == 0b11111111)
				{
					_delay_ms(DEBOUNCE_DELAY);
					break;
				}
				else if (IN_BUTTONS == 0b00111111)
				{
					_delay_ms(LONG_PRESS_DELAY);
					if (IN_BUTTONS == 0b00111111)
					{
						//  reset current patch
						loaded_patch = 0b00000000;
						changePatch(loaded_patch);
						_delay_ms(DEBOUNCE_DELAY);
						break;
					}
					break;
				}
				break;
		}
	}
}