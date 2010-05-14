#include "Flp.h"

#include <iostream>

const int Flp::c_NumFLFxChannels = 64;

Flp::Flp(std::string filename) :
    m_good(false)
{
    m_file.open(filename.c_str(),std::fstream::in|std::fstream::binary);
    if (! m_file.good()) {
        m_errMsg = "Error opening the file.";
        return;
    }

    // check for the magic "FLhd" at the beginning
    int realMagic = 'F' | ('L' << 8) | ('h' << 16) | ('d' << 24);
    int magic = read32LE();

    if (realMagic != magic) {
        m_errMsg = "Doesn't look like an .flp file.";
        return;
    }

    // header should be 6 bytes long
    const int header_len = read32LE();
    if (header_len != 6) {
        m_errMsg = "File format is too different from what we know"
            " (header should be 6 bytes long).";
        return;
    }

    // some type thing
    const int type = read16LE();
    if (type != 0) {
        m_errMsg = "type ";
        m_errMsg += type;
        m_errMsg += " is not supported.";
        return;
    }

    // number of channels
    

    m_good = true;
}

Flp::~Flp()
{

}

int Flp::readByte()
{
    unsigned char c;
    m_file.read((char*)&c, 1);
    return static_cast<int>(c);
}

int Flp::read16LE()
{
    int value = readByte();
    value |= readByte() << 8;
    return value;
}

int Flp::read32LE()
{
    int value = readByte();
    value |= readByte() << 8;
    value |= readByte() << 16;
    value |= readByte() << 24;
    return value;
}

