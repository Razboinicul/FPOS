
#include "keypad.h"

#include <string.h>

#include "syscode.h"

/*
 * fpdoom key codes use 15 bits for the key
 * and bit 15 as the power-button modifier.
 */
#define KEYPAD_KEY_MASK 0x7fff
#define KEYPAD_STATE_COUNT 32768

static uint8_t key_state[KEYPAD_STATE_COUNT];

void keypad_init(void)
{
    memset(key_state, 0, sizeof(key_state));
}

/*
 * Returns:
 *   KEYPAD_PRESS    on key down
 *   KEYPAD_RELEASE  on key up
 *   KEYPAD_NONE     if no event is available
 *   KEYPAD_QUIT     when the system requests exit
 */
int keypad_poll(keypad_event_t *event)
{
    int key;
    int type;
    int power;
    int result;

    type = sys_event(&key);

    if (type == EVENT_END)
        return KEYPAD_NONE;

    if (type == EVENT_QUIT)
        return KEYPAD_QUIT;

    if (type != EVENT_KEYDOWN && type != EVENT_KEYUP)
        return KEYPAD_NONE;

    power = ((unsigned)key >> 15) & 1;
    key &= KEYPAD_KEY_MASK;

    result = (type == EVENT_KEYDOWN)
           ? KEYPAD_PRESS
           : KEYPAD_RELEASE;

    if (key < KEYPAD_STATE_COUNT) {
        key_state[key] = (result == KEYPAD_PRESS);
    }

    if (event) {
        event->type = result;
        event->key = (uint16_t)key;
        event->power = power;
    }

    return result;
}

/*
 * Blocking wait. Yields briefly when no event is ready.
 */
int keypad_wait(keypad_event_t *event)
{
    int result;

    for (;;) {
        result = keypad_poll(event);

        if (result != KEYPAD_NONE)
            return result;

        sys_wait_ms(10);
    }
}

/* Check the latest known state of a key */
int keypad_is_down(uint16_t key)
{
    if (key >= KEYPAD_STATE_COUNT)
        return 0;

    return key_state[key] != 0;
}

/* Convert a key code to its symbolic name */
const char *keypad_name(uint16_t key)
{
    switch (key) {
#define X(val, name) case val: return #name;
        KEYPAD_ENUM(X)
#undef X
    default:
        return "UNKNOWN";
    }
}
