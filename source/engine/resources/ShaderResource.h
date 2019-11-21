///------------------------------------------------------------------------------------------------
///  ShaderResource.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef ShaderResource_h
#define ShaderResource_h

///------------------------------------------------------------------------------------------------

#include "IResource.h"
#include "../common/utils/MathUtils.h"
#include "../common/utils/StringUtils.h"

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

class ShaderResource final: public IResource
{
public:
    ShaderResource() = default;
    ShaderResource
    (
        const std::unordered_map<StringId, GLuint, StringIdHasher> uniformNamesToLocations,
        const GLuint programId
    );
    ShaderResource& operator = (const ShaderResource&);
    ShaderResource(const ShaderResource&);
    
    bool SetMatrix4fv(const StringId uniformName, const glm::mat4& matrix, const GLuint count = 1, const bool transpose = false) const;

    GLuint GetProgramId() const;    

protected:
    const std::unordered_map<StringId, GLuint, StringIdHasher>& GetUniformNamesToLocations() const;

private:
    void CopyConstruction(const ShaderResource&);
    
private:
    std::unordered_map<StringId, GLuint, StringIdHasher> mShaderUniformNamesToLocations;
    GLuint mProgramId;    
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* ShaderResource_h */
