///------------------------------------------------------------------------------------------------
///  AnimationTimerComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef AnimationTimerComponent_h
#define AnimationTimerComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/Timer.h"

#include <memory>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

class AnimationTimerComponent final : public ecs::IComponent
{
public:
    std::unique_ptr<Timer> mAnimationTimer = nullptr;

};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* AnimationTimerComponent_h */