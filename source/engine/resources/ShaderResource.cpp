///------------------------------------------------------------------------------------------------
///  ShaderResource.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "ShaderResource.h"
#include "../rendering/opengl/Context.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

ShaderResource::ShaderResource
(
    const std::unordered_map<StringId, GLuint, StringIdHasher> uniformNamesToLocations,
    const GLuint programId
)
    : mShaderUniformNamesToLocations(uniformNamesToLocations) 
    , mProgramId(programId)
{
    
}

///------------------------------------------------------------------------------------------------

ShaderResource& ShaderResource::operator = (const ShaderResource& rhs)
{
    CopyConstruction(rhs);
    return *this;
}

///------------------------------------------------------------------------------------------------

ShaderResource::ShaderResource(const ShaderResource& rhs)
{
    CopyConstruction(rhs);
}

///------------------------------------------------------------------------------------------------

bool ShaderResource::SetMatrix4fv
(
    const StringId uniformName, 
    const glm::mat4& matrix, 
    const GLuint count /* 1 */,
    const bool transpose /* false */
) const
{
    if (mShaderUniformNamesToLocations.count(uniformName) > 0)
    {
        GL_CHECK(glUniformMatrix4fv(mShaderUniformNamesToLocations.at(uniformName), count, transpose, (GLfloat*)&matrix));
        return true;
    }    
    return false;
}

///------------------------------------------------------------------------------------------------

GLuint ShaderResource::GetProgramId() const
{
    return mProgramId;
}

///------------------------------------------------------------------------------------------------

const std::unordered_map<StringId, GLuint, StringIdHasher>& ShaderResource::GetUniformNamesToLocations() const
{
    return mShaderUniformNamesToLocations;
}

///------------------------------------------------------------------------------------------------

void ShaderResource::CopyConstruction(const ShaderResource& rhs)
{
    mProgramId = rhs.GetProgramId();
    for (const auto& uniformEntry: rhs.GetUniformNamesToLocations())
    {
        mShaderUniformNamesToLocations[uniformEntry.first] = uniformEntry.second;
    }
}

///------------------------------------------------------------------------------------------------

}

