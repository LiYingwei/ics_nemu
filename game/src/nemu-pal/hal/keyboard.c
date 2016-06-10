#include <x86.h>
#include "hal.h"

#define NR_KEYS 18

enum {KEY_STATE_EMPTY, KEY_STATE_WAIT_RELEASE, KEY_STATE_RELEASE, KEY_STATE_PRESS};

/* Only the following keys are used in NEMU-PAL. */
static const int keycode_array[] = {
	K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ESCAPE,
	K_RETURN, K_SPACE, K_PAGEUP, K_PAGEDOWN, K_r,
	K_a, K_d, K_e, K_w, K_q,
	K_s, K_f, K_p
};

static int key_state[NR_KEYS]; // 0:release 1:press
static int key_event[NR_KEYS]; // 0:no event 1:press 2:release

static int get_key_index(int code) {
    int i;
    code = code & 0x7f;
    for(i = 0; i < NR_KEYS; i++) {
        if(keycode_array[i] == code)
            return i;
    }
    printf("unknown key_code = %d\n", code);
    return -1;
}

void
keyboard_event(void) {
	/* TODO: Fetch the scancode and update the key states. */
    int code = (unsigned) (unsigned char) in_byte(0x60);

    int index = get_key_index(code);

    if(code < 0x80) {
        key_event[index] = 1;
    } else {
        key_event[index] = 2;
    }
	//assert(0);
}

static inline int
get_keycode(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return keycode_array[index];
}

static inline int
query_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return key_state[index];
}

static inline void
release_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_WAIT_RELEASE;
}

static inline void
clear_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_EMPTY;
}

bool 
process_keys(void (*key_press_callback)(int), void (*key_release_callback)(int)) {
	cli();
	/* TODO: Traverse the key states. Find a key just pressed or released.
	 * If a pressed key is found, call ``key_press_callback'' with the keycode.
	 * If a released key is found, call ``key_release_callback'' with the keycode.
	 * If any such key is found, the function return true.
	 * If no such key is found, the function return false.
	 * Remember to enable interrupts before returning from the function.
	 */

    int index;
    for(index = 0; index < NR_KEYS; index++) {
        int state = key_state[index];
        int event = key_event[index];
        if(state == 0 && event == 1) {
            key_press_callback(keycode_array[index]);
            key_state[index] = 1;
            key_event[index] = 0;
            printf("gen key press: index = %d\n", index);
            index = NR_KEYS;
        } else if(state == 1 && event == 2) {
            key_release_callback(keycode_array[index]);
            key_state[index] = 0;
            key_event[index] = 0;
            printf("gen key release: index = %d\n", index);
            index = NR_KEYS;
        }
    }
	//assert(0);
	sti();
	return false;
}
