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
  uint8_t aux = light_bits & 0b00000001;
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

SIGNAL (PCINT2_vect)  {
  // Ignition and parking switch signals
  // Consider debounce

  // Disable inturrupts
  cli();
  uint8_t prevParkSW = state_bits & PARKSW_MASK;

  state_bits = (state_bits & !INPUT_MASK) | (PIND & INPUT_MASK);
  if (prevParkSW != (state_bits & PARKSW_MASK))  {
    // Code needs to go here
  }

  // Enable interrupts
	sei();
}

SIGNAL (TIMER1_COMPA_vect)  {
  // 100ms state updates
  state_bits = (state_bits & !INPUT_MASK) | (PIND & INPUT_MASK);
  state_bits = (state_bits & !LIGHT_MASK) | getLights(state_bits & LIGHT_MASK);

  // Turn off light home feature after 1 minute

  // Set output pins for headlights
  LIGHT_PORT = (LIGHT_PORT & !LIGHT_MASK) | (state_bits & LIGHT_MASK);
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
	EIMSK |= (1 << INT1) | (1 << INT0);

	// Set pin change interrupt for ignition
	PCICR |= (1 << PCIE2);
	PCMSK2 |= (1 << PCINT20) | (1 << PCINT21);

  // Set up timers for state and light home feature
	OCR1A  = 3125;						// 100ms equivalent

	TCCR1A = 0x00;						// Set prescaler to 256
	TCCR1B = 0x0C;						// Set mode CTC

	TIMSK1 = 0x02;						// Set inturrupt for OCR1A

  // Enable inturrupts
  sei();

  while (1)  {
    // do some stuff
  }

}