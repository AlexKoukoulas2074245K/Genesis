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

inline bool IsActionTypeKeyTapped(const VirtualActionType actionType, const ecs::World& world)
{
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mCurrentInputState.at(actionType) == VirtualActionInputState::TAPPED;
}

///-----------------------------------------------------------------------------------------------

inline bool IsActionTypeKeyPressed(const VirtualActionType actionType, const ecs::World& world)
{
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mCurrentInputState.at(actionType) == VirtualActionInputState::PRESSED;
}

///-----------------------------------------------------------------------------------------------

inline bool IsActionTypeKeyReleased(const VirtualActionType actionType, const ecs::World& world)
{
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mCurrentInputState.at(actionType) == VirtualActionInputState::RELEASED;
}

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* InputUtils_h */
