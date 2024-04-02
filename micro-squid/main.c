#include "util.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "us100.h"
#include "digit_display.h"
#include "onboard_lcd.h"

#define GREEN_LED PA0
#define RED_LED PA1
#define BUZZ_PIN PC0
#define DETECT_DIST 450

// How much difference in distance to accept
// before registering it as movement.
#define MOVE_THRESHOLD 0

// 0: Red light, 1: Green light
volatile uint8_t state = 1;

ISR(TIMER1_OVF_vect) {
	PORTE ^= 1;
	TCNT1 = 65536 - 23438;
	
	state = (state == 0 ? 1 : 0);
}

void int_timer_init(void) {
	DDRE = 1;
	TCCR1B |= BIT(CS12) | BIT(CS10);
	TCNT1 = 65536 - 23438;
	TIMSK |= BIT(TOIE1);
	sei();
}

void leds_init(void) {
  // Set LED pins as output
  DDRA |= (1 << GREEN_LED) | (1 << RED_LED);
}

void buzzer_init(void) {
	DDRC |= BIT(BUZZ_PIN);
}

void play_buzzer() {
  PORTC |= BIT(BUZZ_PIN);
  _delay_ms(200);
  PORTC &= ~BIT(BUZZ_PIN);
}

void switch_led() {
  if (state == 0) {
    // Red light state
    PORTA |= (1 << RED_LED);
    PORTA &= ~(1 << GREEN_LED);
  } else {
    // Green light state
    PORTA |= (1 << GREEN_LED);
    PORTA &= ~(1 << RED_LED);

  }
}

int main() {
  us100_init();
  leds_init();
  buzzer_init();
  onboard_lcd_init();
  digit_display_init();
  int_timer_init();

  uint16_t prev_dist = 0;
  uint16_t curr_dist = 0;

  while (1) {
    switch_led(state);

    curr_dist = us100_get_dist_cm();

    if (curr_dist > DETECT_DIST) continue;

    if (state == 0 && abs(curr_dist - prev_dist) > MOVE_THRESHOLD) {
      // Player moved during red light, eliminate
      play_buzzer();
      break;
    }

    // Update previous distance
    prev_dist = curr_dist;

  }

  onboard_lcd_set_str("yoU deth'd (x_X)");

  PORTA &= ~BIT(GREEN_LED);
  PORTA &= ~BIT(RED_LED);

  while (1) {
    // Player eliminated
    PORTA &= ~BIT(GREEN_LED);
    PORTA |= BIT(RED_LED);
    _delay_ms(300);

    PORTA &= ~BIT(RED_LED);
    PORTA |= BIT(GREEN_LED);
    _delay_ms(300);
  }

  return 0;
}
