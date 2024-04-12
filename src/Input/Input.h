#pragma once

#include "Defines.h"
#include "Core/Memory.h"
#include "Core/Layer.h"

namespace yoyo
{
    enum class MouseButton : uint8_t
    {

    };

    enum class KeyCode : uint16_t
    {
    Key_Uknown = 0,

    Key_Return = '\r',
    Key_Escape = '\x1b',
    Key_Backspace = '\b',
    Key_Tab = '\t',
    Key_Space = ' ',
    Key_Exclaim = '!',
    Key_Quotedbl = '"',
    Key_Hash = '#',
    Key_Percent = '%',
    Key_Dollar = '$',
    Key_Ampersand = '&',
    Key_Quote = '\'',
    Key_Leftparen = '(',
    Key_Rightparen = ')',
    Key_Asterisk = '*',
    Key_Plus = '+',
    Key_Comma = ',',
    Key_Minus = '-',
    Key_Period = '.',
    Key_Slash = '/',
    Key_0 = '0',
    Key_1 = '1',
    Key_2 = '2',
    Key_3 = '3',
    Key_4 = '4',
    Key_5 = '5',
    Key_6 = '6',
    Key_7 = '7',
    Key_8 = '8',
    Key_9 = '9',
    Key_Colon = ':',
    Key_Semicolon = ';',
    Key_Less = '<',
    Key_Equals = '=',
    Key_Greater = '>',
    Key_Question = '?',
    Key_At = '@',

    /*
       skip uppercase letters
    */

    Key_leftbracket = '[',
    Key_backslash = '\\',
    Key_rightbracket = ']',
    Key_caret = '^',
    Key_underscore = '_',
    Key_backquote = '`',
    Key_a = 'a',
    Key_b = 'b',
    Key_c = 'c',
    Key_d = 'd',
    Key_e = 'e',
    Key_f = 'f',
    Key_g = 'g',
    Key_h = 'h',
    Key_i = 'i',
    Key_j = 'j',
    Key_k = 'k',
    Key_l = 'l',
    Key_m = 'm',
    Key_n = 'n',
    Key_o = 'o',
    Key_p = 'p',
    Key_q = 'q',
    Key_r = 'r',
    Key_s = 's',
    Key_t = 't',
    Key_u = 'u',
    Key_v = 'v',
    Key_w = 'w',
    Key_x = 'x',
    Key_y = 'y',
    Key_z = 'z'
    };

    class YAPI Input
    {
    public:
        // Returns true key is currently pressed 
        static bool GetKey(KeyCode key);

        // Returns true during the frame the user starts pressing down the key
        static bool GetKeyDown(KeyCode key);

        // Returns true key is currently pressed 
        static bool GetMouseButton(int index);

        // Returns true during the frame the user starts pressing down the key
        static bool GetMouseButtonDown(KeyCode key);
    private:
        friend class InputLayer;

        static bool LastKeys[322];
        static bool Keys[322];
        static bool LastMouseButtons[4];
        static bool MouseButtons[4];
    };

    class Event;
    class YAPI InputLayer : public Layer
    {
    public:
        InputLayer() {};
        virtual ~InputLayer() {};

        virtual void OnAttach() override;
        virtual void OnDetatch() override;

        virtual void OnEnable() override;
        virtual void OnDisable() override;

        bool OnEvent(Ref<Event> event); 

        LayerType(InputLayer)
    };
}