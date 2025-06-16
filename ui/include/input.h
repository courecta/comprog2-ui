#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// Mouse state
typedef struct {
    int x, y;
    bool left_button_down;
    bool right_button_down;
    bool left_button_pressed;
    bool right_button_pressed;
} MouseState;

// Input handler
typedef struct {
    MouseState mouse;
    bool quit_requested;
    bool keys[SDL_NUM_SCANCODES];
    bool keys_pressed[SDL_NUM_SCANCODES];
} InputHandler;

// Function declarations
void input_init(InputHandler* input);
void input_update(InputHandler* input);
bool input_handle_events(InputHandler* input, SDL_Event* event);

// Helper functions
bool input_key_down(InputHandler* input, SDL_Scancode key);
bool input_key_pressed(InputHandler* input, SDL_Scancode key);
bool input_mouse_in_rect(InputHandler* input, int x, int y, int w, int h);

#endif // INPUT_H
