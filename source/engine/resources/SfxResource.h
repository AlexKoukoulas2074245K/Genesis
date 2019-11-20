///------------------------------------------------------------------------------------------------
///  SfxResource.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef SfxResource_h
#define SfxResource_h

///------------------------------------------------------------------------------------------------

#include "IResource.h"

#include <SDL_mixer.h>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

class SfxResource final : public IResource
{
    friend class SfxLoader;    

public:
    ~SfxResource();

    Mix_Chunk* GetSdlSfxHandle() const;

private:
    SfxResource(Mix_Chunk*);

    Mix_Chunk* const mSdlSfxHandle;
};

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* SfxResource_h */
