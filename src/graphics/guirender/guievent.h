#ifndef GUIEVENT_H
#define GUIEVENT_H


#include <SDL.h>
#include "../../common/either.h"

namespace gfx {

namespace gui {

// Mouse events
enum class MouseButton { Left, Middle, Right };

struct MouseMoveEvent
{
    int32_t x;
    int32_t y;
};

struct MouseDragEvent
{
    float x_rel;
    float y_rel;
};

struct MouseButtonDownEvent
{
    MouseButton button;
    int32_t x;
    int32_t y;
};

struct MouseButtonUpEvent
{
    MouseButton button;
    int32_t x;
    int32_t y;
};

struct MouseWheelScrollEvent
{
    int32_t y_rel;
};

struct MouseEnterEvent { };

struct MouseLeaveEvent { };

// Key events
using KeyCode = SDL_Keycode;

struct KeyDownEvent
{
    KeyCode key_code;
};

struct KeyUpEvent
{
    KeyCode key_code;
};

struct KeyPressEvent
{
    KeyCode key_code;
};

struct ResizedEvent
{
    float w_abs;
    float h_abs;
};

struct FocusGainedEvent {};

struct FocusLostEvent {};


using GUIEvent = stdext::either<
    MouseMoveEvent,
    MouseButtonDownEvent,
    MouseButtonUpEvent,
    MouseEnterEvent,
    MouseLeaveEvent,
    MouseDragEvent,
    MouseWheelScrollEvent,
    KeyDownEvent,
    KeyUpEvent,
    KeyPressEvent,
    ResizedEvent,
    FocusGainedEvent,
    FocusLostEvent
>;

}

}

#endif // GUIEVENT_H
