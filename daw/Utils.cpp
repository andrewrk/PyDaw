#include "Utils.h"
#include <cmath>
#include <fstream>


double Utils::distance(double x1, double y1, double x2, double y2)
{
    return std::sqrt(Utils::distance2(x1, y1, x2, y2));
}

double Utils::distance2(double x1, double y1, double x2, double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return dx * dx + dy * dy;
}

bool Utils::isZero(double d)
{
    return std::fabs(d) < 0.000001;
}

int Utils::stringToInt(std::string value)
{
    std::stringstream ss;
    ss << value;
    int out;
    ss >> out;
    return out;
}

std::string Utils::intToString(int value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

bool Utils::stringToBool(std::string value)
{
    std::string lower = toLower(value);
    return  lower.compare("true") == 0 ||
            lower.compare("on") == 0 ||
            lower.compare("1") == 0 ||
            lower.compare("yes") == 0 ||
            lower.compare("y") == 0 ||
            lower.compare("t") == 0;
}

std::string Utils::boolToString(bool value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

std::string Utils::toLower(std::string s)
{
    for(unsigned int i=0; i<s.size(); ++i) {
        s[i] = std::tolower(s[i]);
    }
    return s;
}

std::string Utils::toUpper(std::string s)
{
    for(unsigned int i=0; i<s.size(); ++i) {
        s[i] = std::toupper(s[i]);
    }
    return s;
}

std::string Utils::readString(const char ** cursor)
{
    int size = readInt(cursor);
    std::string value(*cursor, size);
    *cursor += size;
    return value;
}

int Utils::readInt(const char ** cursor)
{
    int value = *(int*)*cursor;
    *cursor += sizeof(int);
    return value;
}

double Utils::readDouble(const char ** cursor)
{
    double value = *(double*)*cursor;
    *cursor += sizeof(double);
    return value;
}

bool Utils::isSpace(char c)
{
    return std::isspace(c) || c == '\r';
}

std::string Utils::trim(std::string s)
{
    return ltrim(rtrim(s));
}

std::string Utils::ltrim(std::string s)
{
    if (s.size() == 1 && isSpace(s[0]))
        return "";

    unsigned int goodStart = 0;
    for (unsigned int i = goodStart; i < s.size(); i++) {
        if (isSpace(s[i]))
            continue;
        goodStart = i;
        break;
    }
    if (goodStart == 0)
        return s;
    return s.substr(goodStart);
}

std::string Utils::rtrim(std::string s)
{
    if (s.size() == 1 && isSpace(s[0]))
        return "";

    unsigned int goodEnd = s.size();
    for (unsigned int i = goodEnd; i > 0; i--) {
        if (isSpace(s[i - 1]))
            continue;
        goodEnd = i;
        break;
    }
    if (goodEnd == s.size())
        return s;
    return s.substr(0, goodEnd);
}

void Utils::split(const std::string& str, std::vector<std::string>& tokens,
    const std::string& delimiters)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}
