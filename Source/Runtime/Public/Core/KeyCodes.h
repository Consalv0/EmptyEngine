#pragma once

#define EE_MAX_GAMEPAD_COUNT 8

namespace EE
{
    enum EButtonStateFlags : char
    {
        ButtonState_Unknown =   0,
        ButtonState_Up =        1 << 0,
        ButtonState_Down =      1 << 1,
        ButtonState_Typed =     1 << 2
    };
    ENUM_FLAGS_OPERATORS( EButtonStateFlags );

    struct KeyboardKeyState
    {
        EButtonStateFlags state = ButtonState_Up;
        uint64 frameDown = UINT64_MAX;
        uint32 repetitions = 0;
    };

    enum EMouseButton
    {
        MouseButton_INVALID = 0,
        MouseButton_Left = 1,
        MouseButton_Middle,
        MouseButton_Right,
        MouseButton_4,
        MouseButton_5,
        MouseButton_6,
        MouseButton_7,
        MouseButton_8,
        MouseButton_9,
        MouseButton_10,
        MouseButton_11,
        MouseButton_12,
        MouseButton_13,
        MouseButton_14,
        MouseButton_15,
        MouseButton_16,
        MouseButton_NUM
    };

    struct GamepadButtonState
    {
        EButtonStateFlags state = ButtonState_Up;
        uint64 frameDown = UINT64_MAX;
    };

    // Taken from SDL Gamepad for easy compatibility
    enum EGamepadButton
    {
        GamepadButton_INVALID = -1,
        GamepadButton_A,
        GamepadButton_B,
        GamepadButton_X,
        GamepadButton_Y,
        GamepadButton_Back,
        GamepadButton_Guide,
        GamepadButton_Start,
        GamepadButton_LeftStick,
        GamepadButton_RightStick,
        GamepadButton_LeftShoulder,
        GamepadButton_RightShoulder,
        GamepadButton_DPadUp,
        GamepadButton_DPadDown,
        GamepadButton_DPadLeft,
        GamepadButton_DPadRtight,
        GamepadButton_Misc1,    /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */
        GamepadButton_Paddle1,  /* Xbox Elite paddle P1 (upper left, facing the back) */
        GamepadButton_Paddle2,  /* Xbox Elite paddle P3 (upper right, facing the back) */
        GamepadButton_Paddle3,  /* Xbox Elite paddle P2 (lower left, facing the back) */
        GamepadButton_Paddle4,  /* Xbox Elite paddle P4 (lower right, facing the back) */
        GamepadButton_Touchpad, /* PS4/PS5 touchpad button */
        GamepadButton_NUM
    };

    enum EGamepadAxis
    {
        GamepadAxis_INVALID = -1,
        GamepadAxis_LeftX,
        GamepadAxis_LeftY,
        GamepadAxis_RightX,
        GamepadAxis_RightY,
        GamepadAxis_LeftTrigger,
        GamepadAxis_RightTrigger,
        GamepadAxis_NUM
    };

    struct MouseButtonState
    {
        EButtonStateFlags state = ButtonState_Up;
        uint64 frameDown = UINT64_MAX;
        uint32 clicks = 0;
    };

    /**
     *  Taken from SDL Scancodes for easy compatibility
     *  The values in this enumeration are based on the USB usage page standard:
     *  https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
     */
    enum EKeyboardKey
    {
        KeyboardKey_Unknown = 0,

        /**
         *  \name Usage page 0x07
         *
         *  These values are from usage page 0x07 (USB keyboard page).
         */
         /* @{ */

        KeyboardKey_A = 4,
        KeyboardKey_B = 5,
        KeyboardKey_C = 6,
        KeyboardKey_D = 7,
        KeyboardKey_E = 8,
        KeyboardKey_F = 9,
        KeyboardKey_G = 10,
        KeyboardKey_H = 11,
        KeyboardKey_I = 12,
        KeyboardKey_J = 13,
        KeyboardKey_K = 14,
        KeyboardKey_L = 15,
        KeyboardKey_M = 16,
        KeyboardKey_N = 17,
        KeyboardKey_O = 18,
        KeyboardKey_P = 19,
        KeyboardKey_Q = 20,
        KeyboardKey_R = 21,
        KeyboardKey_S = 22,
        KeyboardKey_T = 23,
        KeyboardKey_U = 24,
        KeyboardKey_V = 25,
        KeyboardKey_W = 26,
        KeyboardKey_X = 27,
        KeyboardKey_Y = 28,
        KeyboardKey_Z = 29,

        KeyboardKey_1 = 30,
        KeyboardKey_2 = 31,
        KeyboardKey_3 = 32,
        KeyboardKey_4 = 33,
        KeyboardKey_5 = 34,
        KeyboardKey_6 = 35,
        KeyboardKey_7 = 36,
        KeyboardKey_8 = 37,
        KeyboardKey_9 = 38,
        KeyboardKey_0 = 39,

        KeyboardKey_Return = 40,
        KeyboardKey_Escape = 41,
        KeyboardKey_Backspace = 42,
        KeyboardKey_Tab = 43,
        KeyboardKey_Space = 44,

        KeyboardKey_Minus = 45,
        KeyboardKey_Equal = 46,
        KeyboardKey_LeftBracket = 47,
        KeyboardKey_RightBracket = 48,
        KeyboardKey_BackSlash = 49, /**< Located at the lower left of the return
                                      *   key on ISO keyboards and at the right end
                                      *   of the QWERTY row on ANSI keyboards.
                                      *   Produces REVERSE SOLIDUS (backslash) and
                                      *   VERTICAL LINE in a US layout, REVERSE
                                      *   SOLIDUS and VERTICAL LINE in a UK Mac
                                      *   layout, NUMBER SIGN and TILDE in a UK
                                      *   Windows layout, DOLLAR SIGN and POUND SIGN
                                      *   in a Swiss German layout, NUMBER SIGN and
                                      *   APOSTROPHE in a German layout, GRAVE
                                      *   ACCENT and POUND SIGN in a French Mac
                                      *   layout, and ASTERISK and MICRO SIGN in a
                                      *   French Windows layout.
                                      */
        KeyboardKey_NonuShash = 50, /**< ISO USB keyboards actually use this code
                                      *   instead of 49 for the same key, but all
                                      *   OSes I've seen treat the two codes
                                      *   identically. So, as an implementor, unless
                                      *   your keyboard generates both of those
                                      *   codes and your OS treats them differently,
                                      *   you should generate SDL_SCANCODE_BACKSLASH
                                      *   instead of this code. As a user, you
                                      *   should not rely on this code because SDL
                                      *   will never generate it with most (all?)
                                      *   keyboards.
                                      */
        KeyboardKey_Semicolon = 51,
        KeyboardKey_Apostrophe = 52,
        KeyboardKey_Grave = 53, /**< Located in the top left corner (on both ANSI
                                  *   and ISO keyboards). Produces GRAVE ACCENT and
                                  *   TILDE in a US Windows layout and in US and UK
                                  *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                                  *   and NOT SIGN in a UK Windows layout, SECTION
                                  *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                                  *   layouts on ISO keyboards, SECTION SIGN and
                                  *   DEGREE SIGN in a Swiss German layout (Mac:
                                  *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                                  *   DEGREE SIGN in a German layout (Mac: only on
                                  *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                                  *   French Windows layout, COMMERCIAL AT and
                                  *   NUMBER SIGN in a French Mac layout on ISO
                                  *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                                  *   SIGN in a Swiss German, German, or French Mac
                                  *   layout on ANSI keyboards.
                                  */
        KeyboardKey_Comma = 54,
        KeyboardKey_Period = 55,
        KeyboardKey_Slash = 56,

        KeyboardKey_Capslocks = 57,

        KeyboardKey_F1 = 58,
        KeyboardKey_F2 = 59,
        KeyboardKey_F3 = 60,
        KeyboardKey_F4 = 61,
        KeyboardKey_F5 = 62,
        KeyboardKey_F6 = 63,
        KeyboardKey_F7 = 64,
        KeyboardKey_F8 = 65,
        KeyboardKey_F9 = 66,
        KeyboardKey_F10 = 67,
        KeyboardKey_F11 = 68,
        KeyboardKey_F12 = 69,

        KeyboardKey_PrintScreen = 70,
        KeyboardKey_ScrollLock = 71,
        KeyboardKey_Pause = 72,
        KeyboardKey_Insert = 73, /**< insert on PC, help on some Mac keyboards (but
                                       does send code 73, not 117) */
        KeyboardKey_Home = 74,
        KeyboardKey_PageUp = 75,
        KeyboardKey_Delete = 76,
        KeyboardKey_End = 77,
        KeyboardKey_PageDown = 78,
        KeyboardKey_Right = 79,
        KeyboardKey_Left = 80,
        KeyboardKey_Down = 81,
        KeyboardKey_Up = 82,

        KeyboardKey_NumLockClear = 83, /**< num lock on PC, clear on Mac keyboards
                                         */
        KeyboardKey_KP_Divide = 84,
        KeyboardKey_KP_Multiply = 85,
        KeyboardKey_KP_Minus = 86,
        KeyboardKey_KP_Plus = 87,
        KeyboardKey_KP_Enter = 88,
        KeyboardKey_KP_1 = 89,
        KeyboardKey_KP_2 = 90,
        KeyboardKey_KP_3 = 91,
        KeyboardKey_KP_4 = 92,
        KeyboardKey_KP_5 = 93,
        KeyboardKey_KP_6 = 94,
        KeyboardKey_KP_7 = 95,
        KeyboardKey_KP_8 = 96,
        KeyboardKey_KP_9 = 97,
        KeyboardKey_KP_0 = 98,
        KeyboardKey_KP_Period = 99,

        KeyboardKey_NounsBackslash = 100, /**< This is the additional key that ISO
                                            *   keyboards have over ANSI ones,
                                            *   located between left shift and Y.
                                            *   Produces GRAVE ACCENT and TILDE in a
                                            *   US or UK Mac layout, REVERSE SOLIDUS
                                            *   (backslash) and VERTICAL LINE in a
                                            *   US or UK Windows layout, and
                                            *   LESS-THAN SIGN and GREATER-THAN SIGN
                                            *   in a Swiss German, German, or French
                                            *   layout. */
        KeyboardKey_Application = 101, /**< windows contextual menu, compose */
        KeyboardKey_Power = 102, /**< The USB document says this is a status flag,
                                   *   not a physical key - but some Mac keyboards
                                   *   do have a power key. */
        KeyboardKey_KP_Equals = 103,
        KeyboardKey_F13 = 104,
        KeyboardKey_F14 = 105,
        KeyboardKey_F15 = 106,
        KeyboardKey_F16 = 107,
        KeyboardKey_F17 = 108,
        KeyboardKey_F18 = 109,
        KeyboardKey_F19 = 110,
        KeyboardKey_F20 = 111,
        KeyboardKey_F21 = 112,
        KeyboardKey_F22 = 113,
        KeyboardKey_F23 = 114,
        KeyboardKey_F24 = 115,
        KeyboardKey_Execute = 116,
        KeyboardKey_Help = 117,    /**< AL Integrated Help Center */
        KeyboardKey_Menu = 118,    /**< Menu (show menu) */
        KeyboardKey_Select = 119,
        KeyboardKey_Stop = 120,    /**< AC Stop */
        KeyboardKey_Again = 121,   /**< AC Redo/Repeat */
        KeyboardKey_Undo = 122,    /**< AC Undo */
        KeyboardKey_Cut = 123,     /**< AC Cut */
        KeyboardKey_Copy = 124,    /**< AC Copy */
        KeyboardKey_Paste = 125,   /**< AC Paste */
        KeyboardKey_Find = 126,    /**< AC Find */
        KeyboardKey_Mute = 127,
        KeyboardKey_VolumeUp = 128,
        KeyboardKey_VolumeDown = 129,

        KeyboardKey_KP_Comma = 133,
        KeyboardKey_KP_EqualSAS400 = 134,

        KeyboardKey_International1 = 135, /**< used on Asian keyboards, see
                                                footnotes in USB doc */
        KeyboardKey_International2 = 136,
        KeyboardKey_International3 = 137, /**< Yen */
        KeyboardKey_International4 = 138,
        KeyboardKey_International5 = 139,
        KeyboardKey_International6 = 140,
        KeyboardKey_International7 = 141,
        KeyboardKey_International8 = 142,
        KeyboardKey_International9 = 143,
        KeyboardKey_Lang1 = 144, /**< Hangul/English toggle */
        KeyboardKey_Lang2 = 145, /**< Hanja conversion */
        KeyboardKey_Lang3 = 146, /**< Katakana */
        KeyboardKey_Lang4 = 147, /**< Hiragana */
        KeyboardKey_Lang5 = 148, /**< Zenkaku/Hankaku */
        KeyboardKey_Lang6 = 149, /**< reserved */
        KeyboardKey_Lang7 = 150, /**< reserved */
        KeyboardKey_Lang8 = 151, /**< reserved */
        KeyboardKey_Lang9 = 152, /**< reserved */

        KeyboardKey_AltErase = 153,    /**< Erase-Eaze */
        KeyboardKey_SysReq = 154,
        KeyboardKey_Cancel = 155,      /**< AC Cancel */
        KeyboardKey_Clear = 156,
        KeyboardKey_Prior = 157,
        KeyboardKey_Return2 = 158,
        KeyboardKey_Separator = 159,
        KeyboardKey_Out = 160,
        KeyboardKey_Oper = 161,
        KeyboardKey_ClearAgain = 162,
        KeyboardKey_CRSEL = 163,
        KeyboardKey_EXSEL = 164,

        KeyboardKey_KP_00 = 176,
        KeyboardKey_KP_000 = 177,
        KeyboardKey_ThousandsSeparator = 178,
        KeyboardKey_DecimalSeparator = 179,
        KeyboardKey_CurrencyUnit = 180,
        KeyboardKey_CurrencySubunit = 181,
        KeyboardKey_KP_LeftParen = 182,
        KeyboardKey_KP_RightParen = 183,
        KeyboardKey_KP_LeftBrace = 184,
        KeyboardKey_KP_RightBrace = 185,
        KeyboardKey_KP_Tab = 186,
        KeyboardKey_KP_Backspace = 187,
        KeyboardKey_KP_A = 188,
        KeyboardKey_KP_B = 189,
        KeyboardKey_KP_C = 190,
        KeyboardKey_KP_D = 191,
        KeyboardKey_KP_E = 192,
        KeyboardKey_KP_F = 193,
        KeyboardKey_KP_XOR = 194,
        KeyboardKey_KP_Power = 195,
        KeyboardKey_KP_Percent = 196,
        KeyboardKey_KP_Less = 197,
        KeyboardKey_KP_Greater = 198,
        KeyboardKey_KP_Ampersand = 199,
        KeyboardKey_KP_DBLAmpersand = 200,
        KeyboardKey_KP_VerticalBar = 201,
        KeyboardKey_KP_DBLVerticalBar = 202,
        KeyboardKey_KP_Colon = 203,
        KeyboardKey_KP_Hash = 204,
        KeyboardKey_KP_Space = 205,
        KeyboardKey_KP_At = 206,
        KeyboardKey_KP_EXClam = 207,
        KeyboardKey_KP_MEMStore = 208,
        KeyboardKey_KP_MEMRecall = 209,
        KeyboardKey_KP_MEMClear = 210,
        KeyboardKey_KP_MEMAdd = 211,
        KeyboardKey_KP_MEMSubtract = 212,
        KeyboardKey_KP_MEMMultiply = 213,
        KeyboardKey_KP_MEMDivide = 214,
        KeyboardKey_KP_PlusMinus = 215,
        KeyboardKey_KP_Clear = 216,
        KeyboardKey_KP_ClearEntry = 217,
        KeyboardKey_KP_Binary = 218,
        KeyboardKey_KP_Octal = 219,
        KeyboardKey_KP_Decimal = 220,
        KeyboardKey_KP_Hexadecimal = 221,

        KeyboardKey_LCtrl = 224,
        KeyboardKey_LShift = 225,
        KeyboardKey_LAlt = 226, /**< alt, option */
        KeyboardKey_LGui = 227, /**< windows, command (apple), meta */
        KeyboardKey_RCtrl = 228,
        KeyboardKey_RShift = 229,
        KeyboardKey_RAlt = 230, /**< alt gr, option */
        KeyboardKey_RGui = 231, /**< windows, command (apple), meta */

        KeyboardKey_Mode = 257,    /**< I'm not sure if this is really not covered
                                     *   by any of the above, but since there's a
                                     *   special SDL_KMOD_MODE for it I'm adding it here
                                     */

                                     /* @} *//* Usage page 0x07 */

                                     /**
                                      *  \name Usage page 0x0C
                                      *
                                      *  These values are mapped from usage page 0x0C (USB consumer page).
                                      *  See https://usb.org/sites/default/files/hut1_2.pdf
                                      *
                                      *  There are way more keys in the spec than we can represent in the
                                      *  current scancode range, so pick the ones that commonly come up in
                                      *  real world usage.
                                      */
                                      /* @{ */

        KeyboardKey_AudioNext = 258,
        KeyboardKey_AudioPrev = 259,
        KeyboardKey_AudioStop = 260,
        KeyboardKey_AudioPlay = 261,
        KeyboardKey_AudioMute = 262,
        KeyboardKey_MediaSelect = 263,
        KeyboardKey_WWW = 264,             /**< AL Internet Browser */
        KeyboardKey_Mail = 265,
        KeyboardKey_Calculator = 266,      /**< AL Calculator */
        KeyboardKey_Computer = 267,
        KeyboardKey_AC_Search = 268,       /**< AC Search */
        KeyboardKey_AC_Home = 269,         /**< AC Home */
        KeyboardKey_AC_Back = 270,         /**< AC Back */
        KeyboardKey_AC_Forward = 271,      /**< AC Forward */
        KeyboardKey_AC_Stop = 272,         /**< AC Stop */
        KeyboardKey_AC_Refresh = 273,      /**< AC Refresh */
        KeyboardKey_AC_Bookmarks = 274,    /**< AC Bookmarks */

        /* @} *//* Usage page 0x0C */

        /**
         *  \name Walther keys
         *
         *  These are values that Christian Walther added (for mac keyboard?).
         */
         /* @{ */

        KeyboardKey_BrightnessDown = 275,
        KeyboardKey_BrightnessUp = 276,
        KeyboardKey_DisplaySwitch = 277, /**< display mirroring/dual display
                                               switch, video mode switch */
        KeyboardKey_KBDIllumToggle = 278,
        KeyboardKey_KBDIllumDown = 279,
        KeyboardKey_KBDIllumUp = 280,
        KeyboardKey_Eject = 281,
        KeyboardKey_Sleep = 282,           /**< SC System Sleep */

        KeyboardKey_APP1 = 283,
        KeyboardKey_APP2 = 284,

        /* @} *//* Walther keys */

        /**
         *  \name Usage page 0x0C (additional media keys)
         *
         *  These values are mapped from usage page 0x0C (USB consumer page).
         */
         /* @{ */

        KeyboardKey_AusioRewind = 285,
        KeyboardKey_AudioFastForward = 286,

        /* @} *//* Usage page 0x0C (additional media keys) */

        /**
         *  \name Mobile keys
         *
         *  These are values that are often used on mobile phones.
         */
         /* @{ */

        KeyboardKey_SoftLeft = 287, /**< Usually situated below the display on phones and
                                          used as a multi-function feature key for selecting
                                          a software defined function shown on the bottom left
                                          of the display. */
        KeyboardKey_SoftRight = 288, /**< Usually situated below the display on phones and
                                           used as a multi-function feature key for selecting
                                           a software defined function shown on the bottom right
                                           of the display. */
        KeyboardKey_Call = 289, /**< Used for accepting phone calls. */
        KeyboardKey_EndCall = 290, /**< Used for rejecting phone calls. */

        /* @} *//* Mobile keys */

        /* Add any other keys here. */

        KeyboardKey_Reserved = 400,    /**< 400-500 reserved for dynamic keycodes */

        KeyboardKey_NUM = 512 /**< not a key, just marks the number of scancodes
                                     for array bounds */
	};

}