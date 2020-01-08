///------------------------------------------------------------------------------------------------
///  InputStateSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef InputStateSingletonComponent_h
#define InputStateSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

#include <SDL_keyboard.h>
#include <unordered_map>
#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace input
{

///-----------------------------------------------------------------------------------------------

static constexpr int DEFAULT_KEY_COUNT = 512;

///-----------------------------------------------------------------------------------------------

enum class InputActionType
{    
    CAMERA_MOVE_UP,
    CAMERA_MOVE_DOWN,
    CAMERA_MOVE_LEFT,
    CAMERA_MOVE_RIGHT,
    CAMERA_MOVE_FORWARD,
    CAMERA_MOVE_BACKWARD,   
    CAMERA_LOOK_UP,
    CAMERA_LOOK_DOWN,
    CAMERA_LOOK_LEFT,
    CAMERA_LOOK_RIGHT,
    CAMERA_ZOOM_IN,
    CAMERA_ZOOM_OUT,
    CONSOLE_TOGGLE,
    SHIFT
};

///-----------------------------------------------------------------------------------------------

enum class InputActionState
{
    RELEASED, PRESSED, TAPPED
};

///-----------------------------------------------------------------------------------------------


static std::unordered_map<SDL_Scancode, SDL_Keycode> sKeyResultWithShiftModifier =
{
    { SDL_SCANCODE_1,            SDLK_EXCLAIM },
    { SDL_SCANCODE_2,            SDLK_AT },
    { SDL_SCANCODE_3,            SDLK_HASH },
    { SDL_SCANCODE_4,            SDLK_DOLLAR },
    { SDL_SCANCODE_5,            SDLK_PERCENT },
    { SDL_SCANCODE_6,            SDLK_CARET },
    { SDL_SCANCODE_7,            SDLK_AMPERSAND },
    { SDL_SCANCODE_8,            SDLK_ASTERISK },
    { SDL_SCANCODE_9,            SDLK_LEFTPAREN },
    { SDL_SCANCODE_0,            SDLK_RIGHTPAREN },
    { SDL_SCANCODE_MINUS,        SDLK_UNDERSCORE },
    { SDL_SCANCODE_EQUALS,       SDLK_PLUS },
    { SDL_SCANCODE_SEMICOLON,    SDLK_COLON },
    { SDL_SCANCODE_COMMA,        SDLK_LESS },
    { SDL_SCANCODE_PERIOD,       SDLK_GREATER },
    { SDL_SCANCODE_SLASH,        SDLK_QUESTION },
    { SDL_SCANCODE_LEFTBRACKET,  static_cast<SDL_Keycode>('{') },
    { SDL_SCANCODE_RIGHTBRACKET, static_cast<SDL_Keycode>('}') }
};

///-----------------------------------------------------------------------------------------------

class InputStateSingletonComponent final: public ecs::IComponent
{
public:
    std::unordered_map<InputActionType, InputActionState> mCurrentInputState =
    {        
        { InputActionType::CAMERA_MOVE_UP,       InputActionState::RELEASED },
        { InputActionType::CAMERA_MOVE_DOWN,     InputActionState::RELEASED },
        { InputActionType::CAMERA_MOVE_LEFT,     InputActionState::RELEASED },
        { InputActionType::CAMERA_MOVE_RIGHT,    InputActionState::RELEASED },
        { InputActionType::CAMERA_MOVE_FORWARD,  InputActionState::RELEASED },
        { InputActionType::CAMERA_MOVE_BACKWARD, InputActionState::RELEASED },
        { InputActionType::CAMERA_LOOK_UP,       InputActionState::RELEASED },
        { InputActionType::CAMERA_LOOK_DOWN,     InputActionState::RELEASED },
        { InputActionType::CAMERA_LOOK_LEFT,     InputActionState::RELEASED },
        { InputActionType::CAMERA_LOOK_RIGHT,    InputActionState::RELEASED },
        { InputActionType::CAMERA_ZOOM_IN,       InputActionState::RELEASED },
        { InputActionType::CAMERA_ZOOM_OUT,      InputActionState::RELEASED },
        { InputActionType::CONSOLE_TOGGLE,       InputActionState::RELEASED },
        { InputActionType::SHIFT,                InputActionState::RELEASED }
    };

    std::unordered_map<SDL_Scancode, InputActionType> mKeybindings =
    {        
        { SDL_SCANCODE_Q,      InputActionType::CAMERA_MOVE_UP },
        { SDL_SCANCODE_E,      InputActionType::CAMERA_MOVE_DOWN },
        { SDL_SCANCODE_W,      InputActionType::CAMERA_MOVE_FORWARD },
        { SDL_SCANCODE_S,      InputActionType::CAMERA_MOVE_BACKWARD },
        { SDL_SCANCODE_A,      InputActionType::CAMERA_MOVE_LEFT },
        { SDL_SCANCODE_D,      InputActionType::CAMERA_MOVE_RIGHT },
        { SDL_SCANCODE_UP,     InputActionType::CAMERA_LOOK_UP },
        { SDL_SCANCODE_DOWN,   InputActionType::CAMERA_LOOK_DOWN },
        { SDL_SCANCODE_LEFT,   InputActionType::CAMERA_LOOK_LEFT },
        { SDL_SCANCODE_RIGHT,  InputActionType::CAMERA_LOOK_RIGHT },
        { SDL_SCANCODE_Z,      InputActionType::CAMERA_ZOOM_IN },
        { SDL_SCANCODE_X,      InputActionType::CAMERA_ZOOM_OUT },
        { SDL_SCANCODE_GRAVE,  InputActionType::CONSOLE_TOGGLE },
        { SDL_SCANCODE_LSHIFT, InputActionType::SHIFT }
    };

    // Needed for tapped input checks 
    std::vector<unsigned char> mPreviousRawKeyboardState;        
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* InputStateSingletonComponent_h */
