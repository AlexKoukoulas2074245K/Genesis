///------------------------------------------------------------------------------------------------
///  IResourceLoader.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef IResourceLoader_h
#define IResourceLoader_h

///------------------------------------------------------------------------------------------------

#include <memory> 
#include <string> 

///------------------------------------------------------------------------------------------------

namespace genesis
{ 

///------------------------------------------------------------------------------------------------

class IResource;

///------------------------------------------------------------------------------------------------

class IResourceLoader
{
public:
    virtual ~IResourceLoader() = default;
    IResourceLoader(const IResourceLoader&) = delete;
    const IResourceLoader& operator = (const IResourceLoader&) = delete;
    
    virtual void VInitialize() = 0;    
    virtual std::unique_ptr<IResource> VCreateAndLoadResource(const std::string& path) const = 0;

protected:
    IResourceLoader() = default;
};

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* IResourceLoader.h */
