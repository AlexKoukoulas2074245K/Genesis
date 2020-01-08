///------------------------------------------------------------------------------------------------
///  InputUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef InputUtils_h
#define InputUtils_h

///-----------------------------------------------------------------------------------------------

#include "../components/InputStateSingletonComponent.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace input
{

///-----------------------------------------------------------------------------------------------
/// Tests whether the state of the given input action is TAPPED.
/// @param[in] actionType the action type to check for its state.
/// @param[in] world the singular world of the ECS state.
/// @returns whether the state of the input action type is TAPPED.
inline bool IsActionTypeKeyTapped(const InputActionType actionType, const ecs::World& world)
{
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mCurrentInputState.at(actionType) == InputActionState::TAPPED;
}

///-----------------------------------------------------------------------------------------------
/// Tests whether the state of the given input action is PRESSED.
/// @param[in] actionType the action type to check for its state.
/// @param[in] world the singular world of the ECS state.
/// @returns whether the state of the input action type is PRESSED.
inline bool IsActionTypeKeyPressed(const InputActionType actionType, const ecs::World& world)
{
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mCurrentInputState.at(actionType) == InputActionState::PRESSED;
}

/// Tests whether the state of the given input action is RELEASED.
/// @param[in] actionType the action type to check for its state.
/// @param[in] world the singular world of the ECS state.
/// @returns whether the state of the input action type is RELEASED.
inline bool IsActionTypeKeyReleased(const InputActionType actionType, const ecs::World& world)
{
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mCurrentInputState.at(actionType) == InputActionState::RELEASED;
}

///-----------------------------------------------------------------------------------------------

/// Checks and returns the character from the currently tapped key.
/// @param[in] world the singular world of the ECS state.
/// @returns the tapped character, or 0 if no character is currently tapped.
inline char GetTypedCharacter(const ecs::World& world)
{
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();

    // Check whether modifier key is down
    const auto isShiftDown = IsActionTypeKeyPressed(InputActionType::SHIFT, world);

    // Find tapped key
    auto keyboardStateLength = 0;
    const auto* currentKeyboardState = SDL_GetKeyboardState(&keyboardStateLength);    
    for (auto i = 0U; i < inputStateComponent.mPreviousRawKeyboardState.size(); ++i)
    {
        if (currentKeyboardState[i] && !inputStateComponent.mPreviousRawKeyboardState[i])
        {
            if (isShiftDown && sKeyResultWithShiftModifier.count(static_cast<SDL_Scancode>(i)) != 0)
            {
                return static_cast<char>(sKeyResultWithShiftModifier.at(static_cast<SDL_Scancode>(i)));
            }
            else
            {
                return static_cast<char>(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i)));
            }            
        }
    }

    return 0;
}

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* InputUtils_h */
