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
    LEFT_ARROW,
    RIGHT_ARROW,
    UP_ARROW,
    DOWN_ARROW,
    A_BUTTON,
    B_BUTTON,
    START_BUTTON,
    SELECT_BUTTON,
    DEBUG_CAMERA_UP,
    DEBUG_CAMERA_DOWN,
    DEBUG_CAMERA_FORWARD,
    DEBUG_CAMERA_BACKWARD,
    DEBUG_SPEED_UP
};

///-----------------------------------------------------------------------------------------------

enum class InputActionState
{
    RELEASED, PRESSED, TAPPED
};

///-----------------------------------------------------------------------------------------------

class InputStateSingletonComponent final: public ecs::IComponent
{
public:
    std::unordered_map<InputActionType, InputActionState> mCurrentInputState =
    {
        { InputActionType::LEFT_ARROW,            InputActionState::RELEASED },
        { InputActionType::RIGHT_ARROW,           InputActionState::RELEASED },
        { InputActionType::UP_ARROW,              InputActionState::RELEASED },
        { InputActionType::DOWN_ARROW,            InputActionState::RELEASED },
        { InputActionType::A_BUTTON,              InputActionState::RELEASED },
        { InputActionType::B_BUTTON,              InputActionState::RELEASED },
        { InputActionType::START_BUTTON,          InputActionState::RELEASED },
        { InputActionType::SELECT_BUTTON,         InputActionState::RELEASED },
        { InputActionType::DEBUG_CAMERA_UP,       InputActionState::RELEASED },
        { InputActionType::DEBUG_CAMERA_DOWN,     InputActionState::RELEASED },
        { InputActionType::DEBUG_CAMERA_FORWARD,  InputActionState::RELEASED },
        { InputActionType::DEBUG_CAMERA_BACKWARD, InputActionState::RELEASED },
    };

    std::unordered_map<SDL_Scancode, InputActionType> mKeybindings =
    {
        { SDL_SCANCODE_LEFT,      InputActionType::LEFT_ARROW },
        { SDL_SCANCODE_RIGHT,     InputActionType::RIGHT_ARROW },
        { SDL_SCANCODE_UP,        InputActionType::UP_ARROW },
        { SDL_SCANCODE_DOWN,      InputActionType::DOWN_ARROW },
        { SDL_SCANCODE_Z,         InputActionType::A_BUTTON },
        { SDL_SCANCODE_X,         InputActionType::B_BUTTON },
        { SDL_SCANCODE_RETURN,    InputActionType::START_BUTTON },
        { SDL_SCANCODE_BACKSPACE, InputActionType::SELECT_BUTTON },
        { SDL_SCANCODE_Q,         InputActionType::DEBUG_CAMERA_UP },
        { SDL_SCANCODE_E,         InputActionType::DEBUG_CAMERA_DOWN },
        { SDL_SCANCODE_W,         InputActionType::DEBUG_CAMERA_FORWARD },
        { SDL_SCANCODE_S,         InputActionType::DEBUG_CAMERA_BACKWARD },
        { SDL_SCANCODE_SPACE,     InputActionType::DEBUG_SPEED_UP },
    };

    // Needed for tapped input checks 
    std::vector<unsigned char> mPreviousRawKeyboardState;
    
    bool mHasBeenConsumed = false;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* InputStateSingletonComponent_h */
