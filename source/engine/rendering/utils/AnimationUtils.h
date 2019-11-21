///------------------------------------------------------------------------------------------------
///  AnimationUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef AnimationUtils_h
#define AnimationUtils_h

///-----------------------------------------------------------------------------------------------

#include "../../rendering/components/RenderableComponent.h"
#include "../../rendering/components/AnimationTimerComponent.h"
#include "../../common/utils/StringUtils.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

inline void ChangeAnimationIfCurrentPlayingIsDifferent
(
    const StringId animationNameId,
    RenderableComponent& renderableComponent
)
{
    if (renderableComponent.mActiveAnimationNameId != animationNameId)
    {
        renderableComponent.mActiveAnimationNameId = animationNameId;
        renderableComponent.mActiveMeshIndex       = 0;
    }
}

///-----------------------------------------------------------------------------------------------

inline void PauseAndResetCurrentlyPlayingAnimation
(
    AnimationTimerComponent& animationTimerComponent,
    RenderableComponent& renderableComponent
)
{
    renderableComponent.mActiveMeshIndex = 0;
    animationTimerComponent.mAnimationTimer->Pause();
}

///-----------------------------------------------------------------------------------------------

inline void ResumeCurrentlyPlayingAnimation(AnimationTimerComponent& animationTimerComponent)
{
    animationTimerComponent.mAnimationTimer->Resume();
}

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* AnimationUtils_h */
