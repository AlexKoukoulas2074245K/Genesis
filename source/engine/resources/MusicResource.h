///------------------------------------------------------------------------------------------------
///  MusicResource.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef MusicResource_h
#define MusicResource_h

///------------------------------------------------------------------------------------------------

#include "IResource.h"

#include <SDL_mixer.h>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

class MusicResource final : public IResource
{
    friend class MusicLoader;

public:
    ~MusicResource();

    Mix_Music* GetSdlMusicHandle() const;

private:
    MusicResource(Mix_Music* const);

    Mix_Music* const mSdlMusicHandle;
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* TextureResource_h */
