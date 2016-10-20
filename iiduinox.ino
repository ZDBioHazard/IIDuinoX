#include "MultiReport/NKROKeyboard.h"
#include "TimerOne.h"

// ---------------------------------------------------------------------------

#define KEY_ANIMATION_RATE 75
#define KEYS_SCAN_RATE 250
#define KEYS_MASK 0xFF

#define TT_THRESHOLD 24
#define TT_TIMEOUT 300
#define TT_DEBOUNCE 4
#define PIN_TT_PRIMARY 2
#define PIN_TT_SECONDARY 3
#define KEY_TT_UP 'q'
#define KEY_TT_DOWN 'a'

// ---------------------------------------------------------------------------

// XXX - WARNING
// DO NOT SET ANY PINS CONNECTED TO KEYS TO OUTPUT HIGH!
// THE CIRCUIT WILL SHORT IF A KEY IS PRESSED WHILE SET TO OUTPUT HIGH!
// To light up a button, toggle between input (off), and output low (on).
// Of course the buttons can't be read while set to output.
// XXX - WARNING
const uint8_t keys_pins[] = { 10,  16,  14,  15,   7,   8,   9,   5,   6};
const uint8_t keys_keys[] = {'z', 's', 'x', 'd', 'c', 'f', 'v', 'w', 'e'};

volatile int8_t tt_pos = 0;
volatile int8_t tt_flipflop = 0;
volatile int8_t tt_debounce = 0;

// ---------------------------------------------------------------------------

void read_buttons( void ) {
    static volatile uint8_t keys_bounce[sizeof(keys_pins)] = { 0xFF };
    static volatile uint8_t keys_pressed[sizeof(keys_pins)] = { false };

    for ( uint8_t i = 0 ; i < sizeof(keys_pins) ; i++ ) {
        keys_bounce[i] = (keys_bounce[i] << 1) | digitalRead(keys_pins[i]);
        if ( (keys_bounce[i] & KEYS_MASK) == (0x00 & KEYS_MASK) ) {
            if ( !keys_pressed[i] ) {
                NKROKeyboard.press(keys_keys[i]);
                keys_pressed[i] = true;
            }
        } else if ( (keys_bounce[i] & KEYS_MASK) == (0xFF & KEYS_MASK) ) {
            if ( keys_pressed[i] ) {
                NKROKeyboard.release(keys_keys[i]);
                keys_pressed[i] = false;
            }
        }
    }
}

// ---------------------------------------------------------------------------

void read_turntable( void ) {
    // FIXME - If you spin the turntable at just the right speed, it will
    //         trigger again just as it comes off cooldown. Also, if you
    //         need to spin the turntable again before the cooldown is up,
    //         you must spin it the opposite direction. This issue is very
    //         apparent in some songs and should be fixed. Possibly using
    //         some kind of velocity detection algorithm?
    static volatile uint8_t tt_pressed = 0;
    static volatile uint16_t tt_ticks = 0;

    if ( tt_debounce > 0 ) {
        tt_debounce--;
    }

    if ( tt_pos > TT_THRESHOLD || tt_pos < -TT_THRESHOLD ) {
        uint8_t tt_new_key = ( tt_pos > 0 ) ? KEY_TT_UP : KEY_TT_DOWN;

        if ( tt_pressed != tt_new_key ) {
            NKROKeyboard.press(tt_new_key);
            if ( tt_pressed ) {
                NKROKeyboard.release(tt_pressed);
            }
            tt_pressed = tt_new_key;
        }
        tt_ticks = 0;
        tt_pos = 0;
    }

    // Release the turntable key on timeout.
    if ( tt_pressed ) {
        if ( tt_ticks >= TT_TIMEOUT ) {
            NKROKeyboard.release(tt_pressed);
            tt_pressed = 0;
            tt_ticks = 0;
        } else {
            tt_ticks++;
        }
    }
}

// ---------------------------------------------------------------------------

void startup_animation( void ) {
    for ( uint8_t i = 0 ; i <= sizeof(keys_pins) ; i++ ) {
        if ( i > 0 ) {
            pinMode(keys_pins[i - 1], INPUT_PULLUP);
        }
        if ( i < sizeof(keys_pins) ) {
            pinMode(keys_pins[i], OUTPUT);
            digitalWrite(keys_pins[i], LOW);
        }
        delay(KEY_ANIMATION_RATE);
    }
}

// ---------------------------------------------------------------------------

void tt_interrupt_one( void ) {
    if ( tt_debounce != 0 || tt_flipflop == KEY_TT_UP ) {
        return;
    }

    if ( digitalRead(PIN_TT_PRIMARY) == digitalRead(PIN_TT_SECONDARY) ) {
        tt_pos++;
    } else {
        tt_pos--;
    }

    tt_flipflop = KEY_TT_UP;
    tt_debounce = TT_DEBOUNCE;
}

void tt_interrupt_two( void ) {
    if ( tt_debounce != 0 || tt_flipflop == KEY_TT_DOWN ) {
        return;
    }

    if ( digitalRead(PIN_TT_PRIMARY) == digitalRead(PIN_TT_SECONDARY) ) {
        tt_pos--;
    } else {
        tt_pos++;
    }

    tt_flipflop = KEY_TT_DOWN;
    tt_debounce = TT_DEBOUNCE;
}

// ---------------------------------------------------------------------------

void timer_interrupt( void ) {
    noInterrupts(); // I don't mind if some encoder blips are missed.

    read_buttons();
    read_turntable();

    interrupts();
}

// ---------------------------------------------------------------------------

void setup( void ) {
    pinMode(PIN_TT_PRIMARY, INPUT_PULLUP);
    pinMode(PIN_TT_SECONDARY, INPUT_PULLUP);

    startup_animation();

    for ( uint8_t i = 0 ; i < sizeof(keys_pins) ; i++ )
        pinMode(keys_pins[i], INPUT_PULLUP);

    NKROKeyboard.begin();

    Timer1.initialize(KEYS_SCAN_RATE);
    Timer1.attachInterrupt(timer_interrupt, KEYS_SCAN_RATE);

    attachInterrupt(digitalPinToInterrupt(PIN_TT_PRIMARY),
                    tt_interrupt_one, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_TT_SECONDARY),
                    tt_interrupt_two, CHANGE);
}

// ---------------------------------------------------------------------------

void loop( void ) { }
