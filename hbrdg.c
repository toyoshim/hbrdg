#include <avr/io.h>

#define USE_INTERNAL_PULLUP

int main(int argc, char** argv) {
  // Set GPIO pins for DIP switches as inputs with pull-up.
  // PB[0]  : Button 1 Auto Fire (0: ON, 1: OFF)
  // PB[1]  : Button 2 Auto Fire (0: ON, 1: OFF)
  // PB[2]  : Button 3 Auto Fire (0: ON, 1: OFF)
  // PB[6]  : Button Rotate (0: 1234 -> 2341, 1: Bypass)
  // PB[7]  : 2x2 Buttons Mode (0: 1234 -> 1212, 1: Bypass)
  // PD[5]  : Auto Fire Sync (0: TIMER, 1: CSYNC)
  // PD[7:6]: Auto Fire Speed (b00: 30/s, b01: 20/s, b10: 15/s, b11: 10/s)
  PORTB |= (_BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB6) | _BV(PB7));
  DDRB &= ~(_BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB6) | _BV(PB7));
  PORTD |= (_BV(PD5) | _BV(PD6) | _BV(PD7));
  DDRD &= ~(_BV(PD5) | _BV(PD6) | _BV(PD7));

  // Set GPIO pins for button 1-4 inputs as inputs with (without) pull-up.
#if defined(USE_INTERNAL_PULLUP)
  PORTD |= (_BV(PD0) | _BV(PD1) | _BV(PD2) | _BV(PD3));
#else
  PORTD &= ~(_BV(PD0) | _BV(PD1) | _BV(PD2) | _BV(PD3));
#endif
  DDRD &= ~(_BV(PD0) | _BV(PD1) | _BV(PD2) | _BV(PD3));

  // Set GPIO pins for button 1-4 outputs as inputs without pull-up to be Hi-Z.
  PORTC &= ~(_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3));
  DDRC &= ~(_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3));

  // Set a GPIO pin for CSYNC input as an input without pull-up.
  PORTD &= ~_BV(PD4);
  DDRD &= ~_BV(PD4);

  // Set Timer for Auto Fire Control
  TCCR0B = (TCCR0B & ~0x7) | 0x05;  // CLK/1024

  for (;;) {
    static int rapid_mask = 0;
    static int sync_mode = 0;

    // Read original buttons.
    uint8_t bin = PIND & 0x0f;

    // Button Rotate
    if (!(PINB & _BV(PB6)))
      bin = ((bin >> 1) | (bin << 3)) & 0x0f;

    // Check Sync mode.
    int new_sync_mode = PIND & _BV(PD5);
    if (sync_mode != new_sync_mode) {
      int clkpr = new_sync_mode ? 0 : 3;
      sync_mode = new_sync_mode;
      CLKPR = _BV(CLKPCE);
      CLKPR = clkpr;
    }

    // Generate Auto Fire timing.
    if (!sync_mode) {
      // With TIMER.
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
          break;
      }
      if (TCNT0 > threshold) {
        TCNT0 = 0;
        rapid_mask = ~rapid_mask;
      }
    } else {
      // With CSYNC.
      static uint8_t csync_count = 0;
      static uint8_t csync_value = 0;
      static uint8_t csync_lpf_count = 0;
      static uint8_t csync_lpf_value = 0;
      if (PIND & _BV(PD4))
        csync_lpf_value++;
      csync_lpf_count++;
      if (csync_lpf_count == 8) {
        uint8_t csync_new_value = (csync_lpf_value < 4) ? 0 : 1;
        csync_lpf_value = 0;
        csync_lpf_count = 0;
        if (csync_value == 1 && csync_new_value == 0) {
          csync_count += 2;
          uint8_t threshold;
          switch (PIND & 0xc0) {
            case 0x00:
              threshold = 2;  // 30/s
              break;
            case 0x40:
              threshold = 3;  // 20/s
              break;
            case 0x80:
              threshold = 4;  // 15/s
              break;
            default:
              threshold = 6;  // 10/s
              break;
          }
          if (csync_count >= threshold) {
            csync_count -= threshold;
            rapid_mask = ~rapid_mask;
          }
        }
        csync_value = csync_new_value;
      }
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

    // 2x2 Buttons Mode
    if (!(PINB & _BV(PB7)))
      bin = ((bin >> 2) | 0x0c) & bin;

    // Out low for low, Hi-Z for high.
    uint8_t ddr = (~bin & 0x0f);
    DDRC = (DDRC & ~(_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3))) | ddr;
  }
  return 0;
}
