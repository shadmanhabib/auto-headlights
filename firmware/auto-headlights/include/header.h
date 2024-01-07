/*
 * auto-headlights
 *
 * Shadman Habib
 * 
 * "header.h"
 * Header file containing the required definitions
 * 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

// 8 MHz clock
#define F_CPU				8000000

/*
 * uint8_t state_bits: store vehicle defined states
 * MSB bit 7: enable
 *         6: lock/unlock
 *         5: ignition
 *         4: park switch
 *         3: 
 *         2: lowbeam on
 *         1: parking on
 * LSB bit 0: aux parking on
 */

uint8_t state_bits = 0b00000000;

// State masks as defined above
#define ENABLE_MASK         0b10000000
#define PLOCK_MASK          0b01000000
#define IGNITION_MASK       0b00100000
#define PARKSW_MASK         0b00010000

// Outputs, pin definitions
#define HEADLIGHT_RELAY		PORTB2
#define PARKING_RELAY		PORTB1
#define AUX_PARK			PORTB0

#define LIGHT_PORT		    PORTB
#define LIGHT_MASK          0b00000111
#define APARK_MASK          0b00000001

// Inputs, pin definitions
#define IGNITION			PIND5			// PCINT21
#define PARKSW              PIND4			// PCINT20
#define LIGHT_SENSOR		PINE3
#define LOCK_SIGNAL			PIND2			// INT0
#define UNLOCK_SIGNAL		PIND3			// INT1

#define INPUT_MASK          0b00110000

#define TRUE				1
#define FALSE				0

// ADC Variables
uint8_t adcIndex = 0;
const uint8_t adcSize = 20;
uint16_t adcArray[adcSize] = {0};

// Software counter for time tracking
uint8_t sw_timer = 0;