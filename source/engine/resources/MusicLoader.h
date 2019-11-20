///------------------------------------------------------------------------------------------------
///  MusicLoader.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef MusicLoader_h
#define MusicLoader_h

///------------------------------------------------------------------------------------------------

#include "IResourceLoader.h"

#include <memory>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

class MusicLoader final: public IResourceLoader
{
    friend class ResourceLoadingService;

public:
    void VInitialize() override;
    std::unique_ptr<IResource> VCreateAndLoadResource(const std::string& path) const override;

private:
    MusicLoader() = default;    

};

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* MusicLoader_h */
