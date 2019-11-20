///------------------------------------------------------------------------------------------------
///  DataFileResource.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef DataFileResource_h
#define DataFileResource_h

///------------------------------------------------------------------------------------------------

#include "IResource.h"

#include <string> 

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

class DataFileResource final: public IResource
{
    friend class DataFileLoader;

public:
    const std::string& GetContents() const;
    
private:
    DataFileResource(const std::string& contents);
    
private:
    const std::string mContents;

};

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* DataFileResource_h */
