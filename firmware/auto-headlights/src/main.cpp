/*
 * auto-headlights
 * 
 * Shadman Habib
 * 
 * "main.cpp"
 * Main source file for auto headlight controls on 4GEN Liberty/Legacy vehicles 
 *
 */

#include "header.h"

uint8_t getLights(uint8_t light_bits)  {
  uint8_t aux = light_bits & APARK_MASK;
  light_bits = light_bits >> 1;

  ADCSRA |= (1 << ADSC);
  while ((ADCSRA & 0x40) > 0)  {
    // Wait for ADC conversion to complete
  }

  adcArray[adcIndex] = ADC;
  adcIndex = (adcIndex + 1) % adcSize;

  uint16_t adcSum = 0;
  for (int i = 0; i < adcSize; i++)  {
    adcSum = adcSum + adcArray[i];
  }

  uint16_t adcVal = adcSum / adcSize;

  switch (light_bits)  {
  case 0b00:
    if (adcVal > 650)  {
			light_bits = 0b00;
		} else if (adcVal > 350)  {
			light_bits = 0b01;
		} else  {
			light_bits = 0b11;
		}
    break;
  case 0b01:
    if (adcVal > 850)  {
			light_bits = 0b00;
		} else if (adcVal > 350)  {
			light_bits = 0b01;
		} else  {
			light_bits = 0b11;
		}
    break;
  case 0b11:
    if (adcVal > 850)  {
			light_bits = 0b00;
		} else if (adcVal > 500)  {
			light_bits = 0b01;
		} else  {
			light_bits = 0b11;
		}
    break;
  default:
    light_bits = 0b00;
    break;
  }

  light_bits = (light_bits << 1) | aux;

  return light_bits;
}

SIGNAL (INT0_vect)  {
  // Inturrupt from lock signal
  cli();

  if ((state_bits & IGNITION_MASK) == 0)  {
    if ((state_bits & LOCK_MASK) == 0)  {
      state_bits |= LOCK_MASK;
    } else  {
      state_bits &= !APARK_MASK;
      state_bits &= !LIGHTHOME_MASK;
    }

    // Set output pins for headlights
    LIGHT_PORT = (LIGHT_PORT & !LIGHT_MASK) | (state_bits & APARK_MASK);
  }

  sei();
}

SIGNAL (TIMER1_COMPA_vect)  {
  // 100ms state updates
  uint8_t prevState_bits = state_bits;

  state_bits = (state_bits & !INPUT_MASK) | (PIND & INPUT_MASK);
  state_bits = (state_bits & !LIGHT_MASK) | getLights(state_bits & LIGHT_MASK);

  uint8_t changed_states = state_bits ^ prevState_bits;

  // Turn off light home feature after 1 minute
  if ((state_bits & LIGHTHOME_MASK) > 0)  {
    sw_timer++;

    if (sw_timer >= 250)  {
      state_bits &= !APARK_MASK;
      state_bits &= !LIGHTHOME_MASK;
      sw_timer = 0;
    }
  }

  // Control functions on ignition change
  if ((changed_states & IGNITION_MASK) > 0)  {
    if ((state_bits & IGNITION_MASK) > 0)  {
      state_bits &= !APARK_MASK;                    // Turn off aux output if ignition turned on
      state_bits &= !ENABLE_MASK;
      sw_timer = 0;
    } else if ((state_bits & LIGHT_MASK) > 0)  {
      state_bits |= APARK_MASK;                     // Turn on aux output if ignition turned off and it's dark
      state_bits |= LIGHTHOME_MASK;
    }
  }

  // Re-enable auto headlights ater ignition
  if ((state_bits & ENABLE_MASK) == 0)  {
    sw_timer++;

    if (sw_timer >= 50)  {
      state_bits |= ENABLE_MASK;
      sw_timer = 0;
    }
  }

  // Control function of parking switch and aux output
  if ((state_bits & PARKSW_MASK) > 0)  {
    state_bits |= APARK_MASK;                       // Turn on parking lights with aux output if parking switch is on
  } else if ((prevState_bits & PARKSW_MASK) > 0)  {
    state_bits &= !APARK_MASK;                      // Turn off aux output if parking switch changes to off
  }

  // Set output pins for headlights
  if ((state_bits & ENABLE_MASK) == 0 || (state_bits & IGNITION_MASK) == 0)  {
    LIGHT_PORT = (LIGHT_PORT & !LIGHT_MASK) | (state_bits & APARK_MASK);
  } else  {
    LIGHT_PORT = (LIGHT_PORT & !LIGHT_MASK) | (state_bits & LIGHT_MASK);
  }
}

int main(void)  {
  // Set output pins
  DDRB |= (1 << HEADLIGHT_RELAY) | (1 << PARKING_RELAY) | (1 << AUX_PARK);

	// Set input pins and enable pull-ups
	DDRD &= ~(1 << PARKSW) & ~(1 << IGNITION) & ~(1 << LOCK_SIGNAL) & ~(1 << UNLOCK_SIGNAL);
	PORTD |= (1 << PARKSW) | (1 << IGNITION) | (1 << LOCK_SIGNAL) | (1 << UNLOCK_SIGNAL);

	// Set ADC pin as input
	DDRE &= ~(1 << LIGHT_SENSOR);

  // Set up ADC for light sensor with interrupt
	ADMUX |= (1 << REFS0) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);
	ADCSRA |= (1 << ADEN)  | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	// Set lock and unlock interrupts
	EICRA |= (1 << ISC11) | (1 << ISC10);
	EIMSK |= (1 << INT0);

  // Set up timers for state and light home feature
	OCR1A  = 3125;						// 100ms equivalent

	TCCR1A = 0x00;						// Set prescaler to 256
	TCCR1B = 0x0C;						// Set mode CTC

	TIMSK1 = 0x02;						// Set inturrupt for OCR1A

  // Enable inturrupts
  sei();

  state_bits |= ENABLE_MASK;

  while (1)  {
    // Controller to run indefinitely
    // Sleep when not working
    SMCR |= (1 << SE);
  }

}