///------------------------------------------------------------------------------------------------
///  ShaderLoader.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef ShaderLoader_h
#define ShaderLoader_h

///------------------------------------------------------------------------------------------------

#include "IResourceLoader.h"
#include "../common/utils/StringUtils.h"

#include <memory>
#include <string>
#include <unordered_map>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace res
{

///------------------------------------------------------------------------------------------------

using GLuint = unsigned int;

///------------------------------------------------------------------------------------------------

class ShaderLoader final : public IResourceLoader
{
    friend class ResourceLoadingService;

public:
    void VInitialize() override;
    std::unique_ptr<IResource> VCreateAndLoadResource(const std::string& path) const override;

private:
    static const std::string VERTEX_SHADER_FILE_EXTENSION;
    static const std::string FRAGMENT_SHADER_FILE_EXTENSION;
    
    ShaderLoader() = default;
    
    std::string ReadFileContents(const std::string& filePath) const;
    std::unordered_map<StringId, GLuint, StringIdHasher> GetUniformNamesToLocationsMap
    (
        const GLuint programId,
        const std::string& vertexShaderFileContents,
        const std::string& fragmentShaderFileContents
    ) const;
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* ShaderLoader_h */
