
#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>

/* Key event types */
#define KEYPAD_NONE     0
#define KEYPAD_PRESS    1
#define KEYPAD_RELEASE  2
#define KEYPAD_QUIT    -1

typedef struct {
    int type;           /* KEYPAD_PRESS / RELEASE */
    uint16_t key;       /* Key code, without power flag */
    int power;          /* Power button modifier */
} keypad_event_t;

/* Initialize the keypad library */
void keypad_init(void);

/* Read one event without blocking */
int keypad_poll(keypad_event_t *event);

/* Wait until a key event or quit event occurs */
int keypad_wait(keypad_event_t *event);

/* Check whether a key is currently held */
int keypad_is_down(uint16_t key);

/* Get a readable name for a key */
const char *keypad_name(uint16_t key);

#endif
