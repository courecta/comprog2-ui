#include "input.h"
#include <string.h>

void input_init(InputHandler* input) {
    if (!input) return;
    
    memset(input, 0, sizeof(InputHandler));
}

void input_update(InputHandler* input) {
    if (!input) return;
    
    // Reset pressed states (these are one-frame only)
    memset(input->keys_pressed, 0, sizeof(input->keys_pressed));
    input->mouse.left_button_pressed = false;
    input->mouse.right_button_pressed = false;
}

bool input_handle_events(InputHandler* input, SDL_Event* event) {
    if (!input || !event) return false;
    
    bool handled = false;
    
    switch (event->type) {
        case SDL_QUIT:
            input->quit_requested = true;
            handled = true;
            break;
            
        case SDL_KEYDOWN:
            if (event->key.repeat == 0) {  // Only handle first press, not repeats
                input->keys[event->key.keysym.scancode] = true;
                input->keys_pressed[event->key.keysym.scancode] = true;
                handled = true;
            }
            break;
            
        case SDL_KEYUP:
            input->keys[event->key.keysym.scancode] = false;
            handled = true;
            break;
            
        case SDL_MOUSEBUTTONDOWN:
            switch (event->button.button) {
                case SDL_BUTTON_LEFT:
                    input->mouse.left_button_down = true;
                    input->mouse.left_button_pressed = true;
                    handled = true;
                    break;
                case SDL_BUTTON_RIGHT:
                    input->mouse.right_button_down = true;
                    input->mouse.right_button_pressed = true;
                    handled = true;
                    break;
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            switch (event->button.button) {
                case SDL_BUTTON_LEFT:
                    input->mouse.left_button_down = false;
                    handled = true;
                    break;
                case SDL_BUTTON_RIGHT:
                    input->mouse.right_button_down = false;
                    handled = true;
                    break;
            }
            break;
            
        case SDL_MOUSEMOTION:
            input->mouse.x = event->motion.x;
            input->mouse.y = event->motion.y;
            handled = true;
            break;
    }
    
    return handled;
}

bool input_key_down(InputHandler* input, SDL_Scancode key) {
    if (!input || key >= SDL_NUM_SCANCODES) return false;
    return input->keys[key];
}

bool input_key_pressed(InputHandler* input, SDL_Scancode key) {
    if (!input || key >= SDL_NUM_SCANCODES) return false;
    return input->keys_pressed[key];
}

bool input_mouse_in_rect(InputHandler* input, int x, int y, int w, int h) {
    if (!input) return false;
    
    return (input->mouse.x >= x && input->mouse.x < x + w &&
            input->mouse.y >= y && input->mouse.y < y + h);
}
