#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

namespace Utils
{
    const double RadHalf = 0.70710678118654757;

    // return the lesser of the two args
    template <class T>
    T min(T x, T y) { return (x > y) ? y : x; }

    // return the greater of two args
    template <class T>
    T max(T x, T y) { return (x > y) ? x : y; }

    // distance formula
    double distance(double x1, double y1, double x2, double y2);

    // distance squared
    double distance2(double x1, double y1, double x2, double y2);

    // returns true if a double is pretty much zero
    bool isZero(double d);

    // type conversion
    int stringToInt(std::string value);
    std::string intToString(int value);
    bool stringToBool(std::string value);
    std::string boolToString(bool value);

    // memory parsing
    std::string readString(const char ** cursor);
    int readInt(const char ** cursor);
    double readDouble(const char ** cursor);

    template <class T>
    T* readStruct(const char** cursor) {
        const char* value = *cursor;
        *cursor += sizeof(T);
        return (T*)value;
    }

    // string parsing
    bool isSpace(char c);
    std::string trim(std::string s);
    std::string ltrim(std::string s);
    std::string rtrim(std::string s);
    std::string toLower(std::string s);
    std::string toUpper(std::string s);
    // tokenize a string
    void split(const std::string& str, std::vector<std::string>& tokens,
        const std::string& delimiters = " ");
}

#endif
