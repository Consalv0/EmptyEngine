#pragma once

#define EE_MAX_GAMEPAD_COUNT 8

namespace EE
{
	enum EButtonState : char
	{
		ButtonState_Up = 0,
		ButtonState_Down = 1,
		ButtonState_Pressed = 2,
		ButtonState_Released = 4,
		ButtonState_Typed = 8,
	};

	struct ScancodeState
	{
		int state;
		uint64 framePressed;
		int repetitions;
	};

	enum EMouseButton
	{
		Mouse_Button0 = 1,
		Mouse_Button1,
		Mouse_Button2,
		Mouse_Button3,
		Mouse_Button4,
		Mouse_Button5,
        Mouse_Button_NUM
	};

	struct GamepadButtonState
	{
		int state;
		uint64_t framePressed;
	};

	// Taken from SDL Gamepad for easy compatibility
	enum EGamepadButton
	{
        Gamepad_Button_INVALID = -1,
        Gamepad_Button_A,
        Gamepad_Button_B,
        Gamepad_Button_X,
        Gamepad_Button_Y,
        Gamepad_Button_Back,
        Gamepad_Button_Guide,
        Gamepad_Button_Start,
        Gamepad_Button_LeftStick,
        Gamepad_Button_RightStick,
        Gamepad_Button_LeftShoulder,
        Gamepad_Button_RightShoulder,
        Gamepad_Button_DPadUp,
        Gamepad_Button_DPadDown,
        Gamepad_Button_DPadLeft,
        Gamepad_Button_DPadRtight,
        Gamepad_Button_Misc1,    /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */
        Gamepad_Button_Paddle1,  /* Xbox Elite paddle P1 (upper left, facing the back) */
        Gamepad_Button_Paddle2,  /* Xbox Elite paddle P3 (upper right, facing the back) */
        Gamepad_Button_Paddle3,  /* Xbox Elite paddle P2 (lower left, facing the back) */
        Gamepad_Button_Paddle4,  /* Xbox Elite paddle P4 (lower right, facing the back) */
        Gamepad_Button_Touchpad, /* PS4/PS5 touchpad button */
        Gamepad_Button_MAX
	};

	enum EGamepadAxis
	{
        Gamepad_Axis_INVALID = -1,
        Gamepad_Axis_LeftX,
        Gamepad_Axis_LeftY,
        Gamepad_Axis_RightX,
        Gamepad_Axis_RightY,
        Gamepad_Axis_LeftTrigger,
        Gamepad_Axis_RightTrigger,
        Gamepad_Axis_MAX
	};

	struct MouseButtonState
	{
		int state;
		uint64 framePressed;
		int Clicks;
	};

	/**
	 *  Taken from SDL Scancodes for easy compatibility
	 *  The values in this enumeration are based on the USB usage page standard:
	 *  https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
	 */
	enum EScancode
	{
        Scancode_Unknown = 0,

        /**
         *  \name Usage page 0x07
         *
         *  These values are from usage page 0x07 (USB keyboard page).
         */
         /* @{ */

        Scancode_A = 4,
        Scancode_B = 5,
        Scancode_C = 6,
        Scancode_D = 7,
        Scancode_E = 8,
        Scancode_F = 9,
        Scancode_G = 10,
        Scancode_H = 11,
        Scancode_I = 12,
        Scancode_J = 13,
        Scancode_K = 14,
        Scancode_L = 15,
        Scancode_M = 16,
        Scancode_N = 17,
        Scancode_O = 18,
        Scancode_P = 19,
        Scancode_Q = 20,
        Scancode_R = 21,
        Scancode_S = 22,
        Scancode_T = 23,
        Scancode_U = 24,
        Scancode_V = 25,
        Scancode_W = 26,
        Scancode_X = 27,
        Scancode_Y = 28,
        Scancode_Z = 29,

        Scancode_1 = 30,
        Scancode_2 = 31,
        Scancode_3 = 32,
        Scancode_4 = 33,
        Scancode_5 = 34,
        Scancode_6 = 35,
        Scancode_7 = 36,
        Scancode_8 = 37,
        Scancode_9 = 38,
        Scancode_0 = 39,

        Scancode_Return = 40,
        Scancode_Escape = 41,
        Scancode_Backspace = 42,
        Scancode_Tab = 43,
        Scancode_Space = 44,

        Scancode_Minus = 45,
        Scancode_Equal = 46,
        Scancode_LeftBracket = 47,
        Scancode_RightBracket = 48,
        Scancode_BackSlash = 49, /**< Located at the lower left of the return
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
        Scancode_NonuShash = 50, /**< ISO USB keyboards actually use this code
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
        Scancode_Semicolon = 51,
        Scancode_Apostrophe = 52,
        Scancode_Grave = 53, /**< Located in the top left corner (on both ANSI
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
        Scancode_Comma = 54,
        Scancode_Period = 55,
        Scancode_Slash = 56,

        Scancode_Capslocks = 57,

        Scancode_F1 = 58,
        Scancode_F2 = 59,
        Scancode_F3 = 60,
        Scancode_F4 = 61,
        Scancode_F5 = 62,
        Scancode_F6 = 63,
        Scancode_F7 = 64,
        Scancode_F8 = 65,
        Scancode_F9 = 66,
        Scancode_F10 = 67,
        Scancode_F11 = 68,
        Scancode_F12 = 69,

        Scancode_PrintScreen = 70,
        Scancode_ScrollLock = 71,
        Scancode_Pause = 72,
        Scancode_Insert = 73, /**< insert on PC, help on some Mac keyboards (but
                                       does send code 73, not 117) */
        Scancode_Home = 74,
        Scancode_PageUp = 75,
        Scancode_Delete = 76,
        Scancode_End = 77,
        Scancode_PageDown = 78,
        Scancode_Right = 79,
        Scancode_Left = 80,
        Scancode_Down = 81,
        Scancode_Up = 82,

        Scancode_NumLockClear = 83, /**< num lock on PC, clear on Mac keyboards
                                         */
        Scancode_KP_Divide = 84,
        Scancode_KP_Multiply = 85,
        Scancode_KP_Minus = 86,
        Scancode_KP_Plus = 87,
        Scancode_KP_Enter = 88,
        Scancode_KP_1 = 89,
        Scancode_KP_2 = 90,
        Scancode_KP_3 = 91,
        Scancode_KP_4 = 92,
        Scancode_KP_5 = 93,
        Scancode_KP_6 = 94,
        Scancode_KP_7 = 95,
        Scancode_KP_8 = 96,
        Scancode_KP_9 = 97,
        Scancode_KP_0 = 98,
        Scancode_KP_Period = 99,

        Scancode_NounsBackslash = 100, /**< This is the additional key that ISO
                                            *   keyboards have over ANSI ones,
                                            *   located between left shift and Y.
                                            *   Produces GRAVE ACCENT and TILDE in a
                                            *   US or UK Mac layout, REVERSE SOLIDUS
                                            *   (backslash) and VERTICAL LINE in a
                                            *   US or UK Windows layout, and
                                            *   LESS-THAN SIGN and GREATER-THAN SIGN
                                            *   in a Swiss German, German, or French
                                            *   layout. */
        Scancode_Application = 101, /**< windows contextual menu, compose */
        Scancode_Power = 102, /**< The USB document says this is a status flag,
                                   *   not a physical key - but some Mac keyboards
                                   *   do have a power key. */
        Scancode_KP_Equals = 103,
        Scancode_F13 = 104,
        Scancode_F14 = 105,
        Scancode_F15 = 106,
        Scancode_F16 = 107,
        Scancode_F17 = 108,
        Scancode_F18 = 109,
        Scancode_F19 = 110,
        Scancode_F20 = 111,
        Scancode_F21 = 112,
        Scancode_F22 = 113,
        Scancode_F23 = 114,
        Scancode_F24 = 115,
        Scancode_Execute = 116,
        Scancode_Help = 117,    /**< AL Integrated Help Center */
        Scancode_Menu = 118,    /**< Menu (show menu) */
        Scancode_Select = 119,
        Scancode_Stop = 120,    /**< AC Stop */
        Scancode_Again = 121,   /**< AC Redo/Repeat */
        Scancode_Undo = 122,    /**< AC Undo */
        Scancode_Cut = 123,     /**< AC Cut */
        Scancode_Copy = 124,    /**< AC Copy */
        Scancode_Paste = 125,   /**< AC Paste */
        Scancode_Find = 126,    /**< AC Find */
        Scancode_Mute = 127,
        Scancode_VolumeUp = 128,
        Scancode_VolumeDown = 129,

        Scancode_KP_Comma = 133,
        Scancode_KP_EqualSAS400 = 134,

        Scancode_International1 = 135, /**< used on Asian keyboards, see
                                                footnotes in USB doc */
        Scancode_International2 = 136,
        Scancode_International3 = 137, /**< Yen */
        Scancode_International4 = 138,
        Scancode_International5 = 139,
        Scancode_International6 = 140,
        Scancode_International7 = 141,
        Scancode_International8 = 142,
        Scancode_International9 = 143,
        Scancode_Lang1 = 144, /**< Hangul/English toggle */
        Scancode_Lang2 = 145, /**< Hanja conversion */
        Scancode_Lang3 = 146, /**< Katakana */
        Scancode_Lang4 = 147, /**< Hiragana */
        Scancode_Lang5 = 148, /**< Zenkaku/Hankaku */
        Scancode_Lang6 = 149, /**< reserved */
        Scancode_Lang7 = 150, /**< reserved */
        Scancode_Lang8 = 151, /**< reserved */
        Scancode_Lang9 = 152, /**< reserved */

        Scancode_AltErase = 153,    /**< Erase-Eaze */
        Scancode_SysReq = 154,
        Scancode_Cancel = 155,      /**< AC Cancel */
        Scancode_Clear = 156,
        Scancode_Prior = 157,
        Scancode_Return2 = 158,
        Scancode_Separator = 159,
        Scancode_Out = 160,
        Scancode_Oper = 161,
        Scancode_ClearAgain = 162,
        Scancode_CRSEL = 163,
        Scancode_EXSEL = 164,

        Scancode_KP_00 = 176,
        Scancode_KP_000 = 177,
        Scancode_ThousandsSeparator = 178,
        Scancode_DecimalSeparator = 179,
        Scancode_CurrencyUnit = 180,
        Scancode_CurrencySubunit = 181,
        Scancode_KP_LeftParen = 182,
        Scancode_KP_RightParen = 183,
        Scancode_KP_LeftBrace = 184,
        Scancode_KP_RightBrace = 185,
        Scancode_KP_Tab = 186,
        Scancode_KP_Backspace = 187,
        Scancode_KP_A = 188,
        Scancode_KP_B = 189,
        Scancode_KP_C = 190,
        Scancode_KP_D = 191,
        Scancode_KP_E = 192,
        Scancode_KP_F = 193,
        Scancode_KP_XOR = 194,
        Scancode_KP_Power = 195,
        Scancode_KP_Percent = 196,
        Scancode_KP_Less = 197,
        Scancode_KP_Greater = 198,
        Scancode_KP_Ampersand = 199,
        Scancode_KP_DBLAmpersand = 200,
        Scancode_KP_VerticalBar = 201,
        Scancode_KP_DBLVerticalBar = 202,
        Scancode_KP_Colon = 203,
        Scancode_KP_Hash = 204,
        Scancode_KP_Space = 205,
        Scancode_KP_At = 206,
        Scancode_KP_EXClam = 207,
        Scancode_KP_MEMStore = 208,
        Scancode_KP_MEMRecall = 209,
        Scancode_KP_MEMClear = 210,
        Scancode_KP_MEMAdd = 211,
        Scancode_KP_MEMSubtract = 212,
        Scancode_KP_MEMMultiply = 213,
        Scancode_KP_MEMDivide = 214,
        Scancode_KP_PlusMinus = 215,
        Scancode_KP_Clear = 216,
        Scancode_KP_ClearEntry = 217,
        Scancode_KP_Binary = 218,
        Scancode_KP_Octal = 219,
        Scancode_KP_Decimal = 220,
        Scancode_KP_Hexadecimal = 221,

        SDL_SCANCODE_LCTRL = 224,
        SDL_SCANCODE_LSHIFT = 225,
        SDL_SCANCODE_LALT = 226, /**< alt, option */
        SDL_SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
        SDL_SCANCODE_RCTRL = 228,
        SDL_SCANCODE_RSHIFT = 229,
        SDL_SCANCODE_RALT = 230, /**< alt gr, option */
        SDL_SCANCODE_RGUI = 231, /**< windows, command (apple), meta */

        SDL_SCANCODE_Mode = 257,    /**< I'm not sure if this is really not covered
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

        SDL_SCANCODE_AudioNext = 258,
        SDL_SCANCODE_AudioPrev = 259,
        SDL_SCANCODE_AudioStop = 260,
        SDL_SCANCODE_AudioPlay = 261,
        SDL_SCANCODE_AudioMute = 262,
        SDL_SCANCODE_MediaSelect = 263,
        SDL_SCANCODE_WWW = 264,             /**< AL Internet Browser */
        SDL_SCANCODE_Mail = 265,
        SDL_SCANCODE_Calculator = 266,      /**< AL Calculator */
        SDL_SCANCODE_Computer = 267,
        SDL_SCANCODE_AC_Search = 268,       /**< AC Search */
        SDL_SCANCODE_AC_Home = 269,         /**< AC Home */
        SDL_SCANCODE_AC_Back = 270,         /**< AC Back */
        SDL_SCANCODE_AC_Forward = 271,      /**< AC Forward */
        SDL_SCANCODE_AC_Stop = 272,         /**< AC Stop */
        SDL_SCANCODE_AC_Refresh = 273,      /**< AC Refresh */
        SDL_SCANCODE_AC_Bookmarks = 274,    /**< AC Bookmarks */

        /* @} *//* Usage page 0x0C */

        /**
         *  \name Walther keys
         *
         *  These are values that Christian Walther added (for mac keyboard?).
         */
         /* @{ */

        Scancode_BrightnessDown = 275,
        Scancode_BrightnessUp = 276,
        Scancode_DisplaySwitch = 277, /**< display mirroring/dual display
                                               switch, video mode switch */
        Scancode_KBDIllumToggle = 278,
        Scancode_KBDIllumDown = 279,
        Scancode_KBDIllumUp = 280,
        Scancode_Eject = 281,
        Scancode_Sleep = 282,           /**< SC System Sleep */

        Scancode_APP1 = 283,
        Scancode_APP2 = 284,

        /* @} *//* Walther keys */

        /**
         *  \name Usage page 0x0C (additional media keys)
         *
         *  These values are mapped from usage page 0x0C (USB consumer page).
         */
         /* @{ */

        Scancode_AusioRewind = 285,
        Scancode_AudioFastForward = 286,

        /* @} *//* Usage page 0x0C (additional media keys) */

        /**
         *  \name Mobile keys
         *
         *  These are values that are often used on mobile phones.
         */
         /* @{ */

        Scancode_SoftLeft = 287, /**< Usually situated below the display on phones and
                                          used as a multi-function feature key for selecting
                                          a software defined function shown on the bottom left
                                          of the display. */
        Scancode_SoftRight = 288, /**< Usually situated below the display on phones and
                                           used as a multi-function feature key for selecting
                                           a software defined function shown on the bottom right
                                           of the display. */
        Scancode_Call = 289, /**< Used for accepting phone calls. */
        Scancode_EendCall = 290, /**< Used for rejecting phone calls. */

        /* @} *//* Mobile keys */

        /* Add any other keys here. */

        Scancode_NUM = 512 /**< not a key, just marks the number of scancodes
                                     for array bounds */
	};

}