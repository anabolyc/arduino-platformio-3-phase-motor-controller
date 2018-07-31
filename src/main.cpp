#include <avr/io.h>
#include <util/delay.h>

#include "main.h"
#include "Waveforms.h"

// sample for the 1Hz speed
#define ONE_HZ_SAMLE_LEN_US 1000000 / WF_SAMPLES_MAX

#define WF_INDEX 0
#define WF_FREQ_HZ_MAX_SPIN_UP 10
#define WF_FREQ_HZ_MAX 150
#define WF_SAMPLE_LEN_US ONE_HZ_SAMLE_LEN_US / WF_FREQ_HZ_MAX

void setup(void)
{
    // Pin mode - output
    OUT_PIN_DIR_1 = OUT_PIN_DIR_1 | OUT_PIN_1 | OUT_PIN_2;
    OUT_PIN_DIR_2 = OUT_PIN_DIR_2 | OUT_PIN_3 | OUT_PIN_4;

    // Enable the ADC and prescale = 128 (16MHz/128 = 125kHz)
    ADCSRA = _BV(ADEN) | 7;
    // Select AVCC as reference
    ADMUX = _BV(REFS0);

    // Setup FAST PWM on Timer0 (non inverting, freq = clk/64)
    // OC0A(D6) <- OCR0A, OCOB(D5) <- OCR0B
    TCCR0A = 0B10 << COM0A0 | 0B10 << COM0B0 | 0B11 << WGM00;
    TCCR0B = 0B0 << WGM02 | PWM_CLK << CS00;

    // Setup FAST PWM on Timer1 (non inverting, freq = clk/64)
    // OC1A(B1) <- OCR1A, OC1B(B2) <- OCR1B
    TCCR1A = 0B10 << COM1A0 | 0B10 << COM1B0 | 0B01 << WGM10;
    TCCR1B = 0B01 << WGM12 | PWM_CLK << CS10;

    spin_up();
}

uint8_t i = 0;

void set_waveform(uint8_t index)
{
    OCR0B = waveformsTable[WF_INDEX][index] >> 4;
    OCR0A = waveformsTable[WF_INDEX][(index + WF_SAMPLES_MAX_3) % WF_SAMPLES_MAX] >> 4;
    OCR1B = waveformsTable[WF_INDEX][(index + WF_SAMPLES_MAX_3 + WF_SAMPLES_MAX_3) % WF_SAMPLES_MAX] >> 4;
}

void loop()
{
    set_waveform(i++);

    if (i == WF_SAMPLES_MAX)
        i = 0;

    // wait
    //uint16_t ratio = freq_ratio();
    //for (uint16_t i = 0; i < ratio; i++) {
    _delay_us(WF_SAMPLE_LEN_US);
    //}
}

void spin_up()
{
    for (uint16_t r = WF_FREQ_HZ_MAX / WF_FREQ_HZ_MAX_SPIN_UP; r >= 1; r--)
    {
        uint16_t revs = (WF_FREQ_HZ_MAX - r);
        uint16_t rounds = revs * WF_SAMPLES_MAX / r;
        for (uint16_t j = 0; j < rounds; j++)
        {
            set_waveform(i++);

            if (i == WF_SAMPLES_MAX)
                i = 0;

            for (uint8_t x = 0; x <= r; x++)
                _delay_us(WF_SAMPLE_LEN_US);
        }
    }
}

uint16_t adc_read(uint8_t pin)
{
    /* adcx is the analog pin we want to use.  ADMUX's first few bits are
	 * the binary representations of the numbers of the pins so we can
	 * just 'OR' the pin's number with ADMUX to select that pin.
	 * We first zero the four bits by setting ADMUX equal to its higher
	 * four bits. */
    ADMUX &= 0xf0;
    ADMUX |= pin;

    /* This starts the conversion. */
    ADCSRA |= _BV(ADSC);

    /* This is an idle loop that just wait around until the conversion
	 * is finished.  It constantly checks ADCSRA's ADSC bit, which we just
	 * set above, to see if it is still set.  This bit is automatically
	 * reset (zeroed) when the conversion is ready so if we do this in
	 * a loop the loop will just go until the conversion is ready. */
    while ((ADCSRA & _BV(ADSC)))
        ;

    /* Finally, we return the converted value to the calling function. */
    return ADC;
}

uint16_t freq_ratio()
{
    // ADC 0 -> (INF) -> slow freq
    // ADC 1024 -> (1) -> freq = WF_FREQ_HZ_MAX
    return 1024 / adc_read(IN_PIN);
}
