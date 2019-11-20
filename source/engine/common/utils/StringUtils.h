///------------------------------------------------------------------------------------------------
///  StringUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef StringUtils_h
#define StringUtils_h

///-----------------------------------------------------------------------------------------------

#include "TypeTraits.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

///-----------------------------------------------------------------------------------------------
/// Checks whether the given string (s) starts with a given pattern (pattern)
inline bool StringStartsWith(const std::string& s, const std::string& pattern)
{
    if (s.size() < pattern.size()) return false;
    
    for (auto i = 0U; i < pattern.size(); ++i)
    {
        if (s[i] != pattern[i]) return false;
    }
    
    return true;
}

///-----------------------------------------------------------------------------------------------
/// Checks whether the given string (s) ends with a given pattern (pattern)
inline bool StringEndsWith(const std::string& s, const std::string& pattern)
{
    if (s.size() < pattern.size()) return false;
    
    for (auto i = 0U; i < pattern.size(); ++i)
    {
        if (s[s.size() - pattern.size() + i] != pattern[i]) return false;
    }
    
    return true;
}

///-----------------------------------------------------------------------------------------------
/// Returns a copy of the given string (s) in uppercase
inline std::string StringToUpper(const std::string& s)
{
    auto stringCopy = s;
    std::transform(stringCopy.begin(), stringCopy.end(), stringCopy.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(static_cast<int>(c))); });

    return stringCopy;
}

///-----------------------------------------------------------------------------------------------
/// Returns a copy of the given string (s) in lowercase
inline std::string StringToLower(const std::string& s)
{
    auto stringCopy = s;
    std::transform(stringCopy.begin(), stringCopy.end(), stringCopy.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(static_cast<int>(c))); });
    
    return stringCopy;
}

///-----------------------------------------------------------------------------------------------
/// Splits the given string (s) based on a delimiter character (delim)
inline std::vector<std::string> StringSplit(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    
    return elems;
}

///-----------------------------------------------------------------------------------------------
/// Returns the formatted time string HH:MM from the given number of seconds
inline std::string GetHoursMinutesStringFromSeconds(const int seconds)
{
    const auto minutes = seconds / 60;
    const auto hours   = minutes / 60;

    const auto hoursString = std::to_string(hours);
    auto minutesString = std::to_string(minutes % 60);

    if (minutesString.size() == 1)
    {
        minutesString = "0" + minutesString;
    }

    return hoursString + ":" + minutesString;
}

///-----------------------------------------------------------------------------------------------
/// Replace all occurences of pattern with the replacement string in the original string
inline void StringReplaceAllOccurences(std::string& s, const std::string& pattern, const std::string& replacement)
{
    s = regex_replace(s, std::regex(pattern), replacement);
}

///-----------------------------------------------------------------------------------------------
/// Provides a unique identifier for a string, aimed at optimizing string comparisons
class StringId final
{
public:
    StringId()
        : mString("")
        , mStringId(0)
    {
    }
    
    StringId(const std::string& str)
        : mString(str)
        , mStringId(GetStringHash(str))
    {
    }
    
    operator size_t () { return mStringId; }
    bool operator == (const StringId& other) const { return mStringId == other.GetStringId(); }
    bool operator != (const StringId& other) const { return mStringId != other.GetStringId(); }

    const std::string& GetString() const { return mString; }
    size_t GetStringId() const { return mStringId; }
    
private:
    std::string mString;
    size_t      mStringId;
};

///-----------------------------------------------------------------------------------------------
/// Custom less operator for StringIds to be used indirectly by stl containers
inline bool operator < (const StringId& lhs, const StringId& rhs)
{
    return lhs.GetStringId() < rhs.GetStringId();
}

///-----------------------------------------------------------------------------------------------
/// Custom StringId hasher to be used in stl containers
struct StringIdHasher
{
    std::size_t operator()(const StringId& key) const
    {
        return key.GetStringId();
    }
};

///-----------------------------------------------------------------------------------------------

#endif /* StringUtils_h */
