#include <avr/io.h>

int main(int argc, char** argv) {
  // Set GPIO pins for DIP switches as inputs with pull-up.
  // PB[0]  : Button 1 Auto Fire (0: ON, 1: OFF)
  // PB[1]  : Button 2 Auto Fire (0: ON, 1: OFF)
  // PB[2]  : Button 3 Auto Fire (0: ON, 1: OFF)
  // PB[6]  : Button Rotate (0: 1234 -> 2341, 1: Bypass)
  // PD[7:6]: Auto Fire Speed (b00: 30/s, b01: 20/s, b10: 15/s, b11: 10/s)
  PORTB |= (_BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB6));
  DDRB &= ~(_BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB6));
  PORTD |= (_BV(PD6) | _BV(PD7));
  DDRD &= ~(_BV(PD6) | _BV(PD7));

  // Set GPIO pins for button 1-4 inputs as inputs without pull-up.
  PORTD &= ~(_BV(PD0) | _BV(PD1) | _BV(PD2) | _BV(PD3));
  DDRD &= ~(_BV(PD0) | _BV(PD1) | _BV(PD2) | _BV(PD3));

  // Set GPIO pins for button 1-4 outputs as inputs without pull-up to be Hi-Z.
  PORTC &= ~(_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3));
  DDRC &= ~(_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3));

  // Set Timer for Auto Fire Control
  TCCR0B = (TCCR0B & ~0x7) | 0x05;  // CLK/1024

  for (;;) {
    static int rapid_mask = 0;

    // Read original buttons.
    uint8_t bin = PIND & 0x0f;

    // Button Rotate
    if (!(PINB & _BV(PB6)))
      bin = ((bin >> 1) | (bin << 3)) & 0x0f;

    // Generate Auto Fire timing.
    uint8_t threshold;
    switch (PIND & 0xc0) {
      case 0x00:
        threshold = 15;  // 30/s
        break;
      case 0x40:
        threshold = 23;  // 20/s
        break;
      case 0x80:
        threshold = 31;  // 15/s
        break;
      default:
        threshold = 47;  // 10/s
    }
    if (TCNT0 > threshold) {
      TCNT0 = 0;
      rapid_mask = ~rapid_mask;
    }
    // Adopt Auto Fire on exact cycles for enabled buttons.
    if (rapid_mask) {
      if (!(PINB & 0x01))
        bin |= 0x01;
      if (!(PINB & 0x02))
        bin |= 0x02;
      if (!(PINB & 0x04))
        bin |= 0x04;
    }

    // Out low for low, Hi-Z for high.
    uint8_t ddr = (~bin & 0x0f);
    DDRC = (DDRC & ~(_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3))) | ddr;
  }
  return 0;
}
