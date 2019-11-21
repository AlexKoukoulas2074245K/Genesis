///------------------------------------------------------------------------------------------------
///  SfxLoader.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef SfxLoader_h
#define SfxLoader_h

///------------------------------------------------------------------------------------------------

#include "IResourceLoader.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace res
{

///------------------------------------------------------------------------------------------------

class SfxLoader final: public IResourceLoader
{
    friend class ResourceLoadingService;

public:
    void VInitialize() override;
    std::unique_ptr<IResource> VCreateAndLoadResource(const std::string& path) const override;

private:
    SfxLoader() = default;

};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* SfxLoader_h */
