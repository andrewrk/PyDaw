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
    int realMagic = makeId('F', 'L', 'h', 'd');
    int magic = read32LE();

    if (realMagic != magic) {
        m_errMsg = "Doesn't look like an .flp file.";
        return;
    }

    // header should be 6 bytes long
    const int header_len = read32LE();
    if (header_len != 6) {
        m_errMsg = "File format is too different from what we know"
            " (header should be 6 bytes long, but is ";
        m_errMsg += header_len;
        m_errMsg += ").";
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
    m_project.numChannels = read16LE();
    if (m_project.numChannels < 1 || m_project.numChannels > 1000) {
        m_errMsg = "invalid number of channels: ";
        m_errMsg += m_project.numChannels;
        m_errMsg += ".";
        return;
    }

    // ppq
    const int ppq = read16LE();
    if (ppq < 0) {
        m_errMsg = "invalid ppq: ";
        m_errMsg += ppq;
        m_errMsg += ".";
        return;
    }

    // search for FLdt chunk
    bool valid = false;
    while (true) {
        int id = read32LE();
        const int len = read32LE();
        if (m_file.eof()) {
            m_errMsg = "Unexpected end of file.";
            return;
        }
        // sanity check
        if (len < 0 || len >= 0x10000000) {
            m_errMsg = "Invalid chunk length: ";
            m_errMsg += len;
            m_errMsg += ".";
            return;
        }
        // check for FLdt
        if (id == makeId('F', 'L', 'd', 't')) {
            // TODO: read meta-information

            valid = true;
            break;
        }
        skip(len);
    }
    if (! valid) {
        m_errMsg = "Could not find FLdt chunk.";
        return;
    }
    
    // headers checked out ok. now read the events.
    for (int i=0; i<m_project.numChannels; ++i) {
        m_project.channels.push_back(FL_Channel());
    }

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

int Flp::makeId(char c1, char c2, char c3, char c4)
{
    return c1 | (c2 << 8) | (c3 << 16) | (c4 << 24);
}

void Flp::skip(int bytes)
{
    m_file.seekg(bytes, std::ios::cur);
}
