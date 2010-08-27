#include "Flp.h"

#include <iostream>
#include <cassert>
#include <cstring>

#include "Utils.h"

const int Flp::c_NumFLFxChannels = 64;

Flp::Flp(std::string filename) :
    m_good(false),
    m_debug(false)
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
            " (header should be 6 bytes long, but is " +
            Utils::intToString(header_len) + ").";
        return;
    }

    // some type thing
    const int type = read16LE();
    if (type != 0) {
        m_errMsg = "type " + Utils::intToString(type) + " is not supported.";
        return;
    }

    // number of channels
    m_project.numChannels = read16LE();
    if (m_project.numChannels < 1 || m_project.numChannels > 1000) {
        m_errMsg = "invalid number of channels: " +
            Utils::intToString(m_project.numChannels) + ".";
        return;
    }

    // ppq
    const int ppq = read16LE();
    if (ppq < 0) {
        m_errMsg = "invalid ppq: " + Utils::intToString(ppq) + ".";
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
            m_errMsg = "Invalid chunk length: " + Utils::intToString(len) + ".";
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

    char * text = NULL;
    int text_len = 0;
    int cur_channel = -1;
    while (! m_file.eof()) {
        FLP_Events ev = static_cast<FLP_Events>(readByte());
        int data = readByte();

        // see FLP_Format for the juicy details of this disgusting file format.
        if (ev >= FLP_Word && ev < FLP_Text)
            data = data | (readByte() << 8);
        if (ev >= FLP_Int && ev < FLP_Text) {
            data = data | (readByte() << 16);
            data = data | (readByte() << 24);
        }
        if (ev >= FLP_Text) {
            text_len = data & 0x7F;
            unsigned char shift = 0;
            while (data & 0x80) {
                data = readByte();
                text_len = text_len | ((data & 0x7F) << (shift+=7));
            }
            delete[] text;
            text = new char[text_len+1];
            m_file.read(text, text_len);
            assert(m_file.good());
            text[text_len] = 0; // null byte of string
        }
        // TODO: name these variables better
        // puc = pointer to an unsigned char
        const unsigned char * puc = (const unsigned char*) text;
        // pi = pointer to an int
        const int * pi = (const int *) text;

        FL_Channel * cc = cur_channel >= 0 ?
            &m_project.channels[cur_channel] : NULL;

        switch (ev) {
        // BYTE EVENTS
        case FLP_Byte:
            if (m_debug)
                std::cerr << "undefined byte " << data << std::endl;
            break;
        case FLP_NoteOn:
            if (m_debug)
                std::cerr << "note on: " << data << std::endl;
            // data = pos   how to handle?
            break;

        case FLP_Vol:
            if (m_debug)
                std::cerr << "vol " << data << std::endl;
            break;

        case FLP_Pan:
            if (m_debug)
                std::cerr << "pan " << data << std::endl;
            break;

        case FLP_LoopActive:
            if (m_debug)
                std::cerr << "active loop: " << data << std::endl;
            break;

        case FLP_ShowInfo:
            if (m_debug)
                std::cerr << "show info: " << data << std::endl;
            break;

        case FLP_Shuffle:
            if (m_debug)
                std::cerr << "shuffle: " << data << std::endl;
            break;

        case FLP_MainVol:
            m_project.mainVolume = data;
            break;

        case FLP_PatLength:
            if (m_debug)
                std::cerr << "pattern length: " << data << std::endl;
            break;

        case FLP_BlockLength:
            if (m_debug)
                std::cerr << "block length: " << data << std::endl;
            break;

        case FLP_UseLoopPoints:
            cc->sampleUseLoopPoints = true;
            break;

        case FLP_LoopType:
            if (m_debug)
                std::cerr << "loop type: " << data << std::endl;
            break;

        case FLP_ChanType:
            if (m_debug)
                std::cerr << "channel type: " << data << std::endl;
            if (cc) {
                switch (data) {
                    case 0: cc->generatorName = "Sampler"; break;
                    case 1: cc->generatorName = "TS 404"; break;
                    case 2: cc->generatorName = "3x Osc"; break;
                    case 3: cc->generatorName = "Layer"; break;
                    default: break;
                }
            }
            break;

        case FLP_MixSliceNum:
            cc->fxChannel = data+1;
            break;

        case FLP_EffectChannelMuted:
            if( m_project.currentEffectChannel <= c_NumFLFxChannels ) {
                m_project.effectChannels[m_project.currentEffectChannel]
                    .isMuted = ( data & 0x08 ) > 0 ? false : true;
            }
            break;

            // WORD EVENTS
        case FLP_NewChan:
            cur_channel = data;
            break;

        case FLP_NewPat:
            m_project.currentPattern = data - 1;
            if( m_project.currentPattern > m_project.maxPatterns )
                m_project.maxPatterns = m_project.currentPattern;
            break;

        case FLP_Tempo:
            m_project.tempo = data;
            break;

        case FLP_CurrentPatNum:
            m_project.activeEditPattern = data;
            break;

        case FLP_FX:
            if (m_debug)
                std::cerr << "FX: " << data << std::endl;
            break;

        case FLP_Fade_Stereo:
            if( data & 0x02 )
            {
                cc->sampleReversed = true;
            }
            else if( data & 0x100 )
            {
                cc->sampleReverseStereo = true;
            }
            break;

        case FLP_CutOff:
            if (m_debug)
                std::cerr << "cutoff (sample): " << data << std::endl;
            break;

        case FLP_PreAmp:
            cc->sampleAmp = data;
            break;

        case FLP_Decay:
            if (m_debug)
                std::cerr << "decay (sample): " << data << std::endl;
            break;

        case FLP_Attack:
            if (m_debug)
                std::cerr << "attack (sample): " << data << std::endl;
            break;

        case FLP_MainPitch:
            m_project.mainPitch = data;
            break;

        case FLP_Resonance:
            if (m_debug)
                std::cerr << "resonance (sample): " << data << std::endl;
            break;

        case FLP_LoopBar:
            if (m_debug)
                std::cerr << "loop bar: " << data << std::endl;
            break;

        case FLP_StDel:
            if (m_debug)
                std::cerr << "stdel (delay?): " << data << std::endl;
            break;

        case FLP_FX3:
            if (m_debug)
                std::cerr << "FX 3: " << data << std::endl;
            break;

        case FLP_ShiftDelay:
            if (m_debug)
                std::cerr << "shift delay: " << data << std::endl;
            break;

        case FLP_Dot:
            cc->dots.push_back( ( data & 0xff ) +
                    ( m_project.currentPattern << 8 ) );
            break;

        case FLP_LayerChans:
            m_project.channels[data].layerParent = cur_channel;
            cc->generatorName = "Layer";

            // DWORD EVENTS
        case FLP_Color:
            // TODO: double check that this works
            cc->color.r = (data & 0xFF000000) >> 24;
            cc->color.g = (data & 0x00FF0000) >> 16;
            cc->color.b = (data & 0x0000FF00) >> 8;
            break;

        case FLP_PlayListItem:
            {
                FL_PlayListItem i;
                i.position = (data & 0xffff) * 192;
                i.length = 192;
                i.pattern = (data >> 16) - 1;
                m_project.playListItems.push_back(i);
                if (i.pattern > m_project.maxPatterns)
                    m_project.maxPatterns = i.pattern;
                break;
            }

        case FLP_FXSine:
            if (m_debug)
                std::cerr << "fx sine: " << data << std::endl;
            break;

        case FLP_CutCutBy:
            if (m_debug)
                std::cerr << "cut cut by: " << data << std::endl;
            break;

        case FLP_MiddleNote:
            cc->baseNote = data+9;
            break;

        case FLP_DelayReso:
            if (m_debug)
                std::cerr << "delay resonance: " << data << std::endl;
            break;

        case FLP_Reverb:
            if (m_debug)
                std::cerr << "reverb (sample): " << data << std::endl;
            break;

        case FLP_IntStretch:
            if (m_debug)
                std::cerr << "int stretch (sample): " << data << std::endl;
            break;

            // TEXT EVENTS
        case FLP_Text_ChanName:
            cc->name = text;
            break;

        case FLP_Text_PatName:
            m_project.patternNames[m_project.currentPattern] = text;
            break;

        case FLP_Text_CommentRTF:
            if (m_debug)
                std::cerr << "TODO: RTF text comment." << std::endl;
            /* TODO: support RTF comments
            {
                QByteArray ba( text, text_len );
                QBuffer buf( &ba );
                buf.open( QBuffer::ReadOnly );
                lineno = 0;
                attr_clear_all();
                op = html_init();
                hash_init();
                Word * word = word_read( &buf );
                QString out;
                word_print( word, out );
                word_free( word );
                op_free( op );

                p.projectNotes = out;
                outstring = "";
                break;
            }
            */

        case FLP_Text_Title:
            m_project.projectTitle = text;
            break;

        case FLP_Text_SampleFileName:
            cc->sampleFileName = text;
            cc->generatorName = "Sampler";
            m_sampleSet.insert(cc->sampleFileName);
            break;

        case FLP_Text_Version:
            {
                if (m_debug)
                    std::cerr << "FLP version: " << text << std::endl;
                m_project.versionString = text;
                // divide the version string into numbers
                std::vector<std::string> numbers;
                Utils::split(m_project.versionString, numbers, ".");
                m_project.version = (Utils::stringToInt(numbers[0]) << 8) +
                    (Utils::stringToInt(numbers[1]) << 4 ) +
                    (Utils::stringToInt(numbers[2]) << 0 );
                if( m_project.version >= 0x600 )
                    m_project.versionSpecificFactor = 100;
                break;
            }

        case FLP_Text_PluginName:
            {
                std::string pluginName = text;
                // we add all plugins to effects list and then
                // remove the ones that aren't effects later.
                m_effectPlugins.insert(pluginName);
                cc->generatorName = pluginName;
                if (m_debug)
                    std::cerr << "plugin: " << pluginName << std::endl;
                break;
            }

        case FLP_Text_EffectChanName:
            ++m_project.currentEffectChannel;
            if( m_project.currentEffectChannel <= c_NumFLFxChannels ) {
                m_project.effectChannels[m_project.currentEffectChannel]
                    .name = text;
            }
            break;

        case FLP_Text_Delay:
            if (m_debug)
                std::cerr << "delay data: " << text << std::endl;
            // pi[1] seems to be volume or similiar and
            // needs to be divided
            // by m_project.versionSpecificFactor
            break;

        case FLP_Text_TS404Params:
            if( cc && cc->pluginSettings == NULL ) {
                cc->pluginSettings = new char[text_len];
                std::memcpy( cc->pluginSettings, text, text_len );
                cc->pluginSettingsLength = text_len;
                cc->generatorName = "TS 404";
            }
            break;

        case FLP_Text_NewPlugin:
            // TODO: if it's an effect plugin make a new effect
            if (m_debug) {
                std::cerr << "new plugin: " << std::endl;
                dump_mem(text, text_len);
            }
            break;

        case FLP_Text_PluginParams:
            if( cc && cc->pluginSettings == NULL ) {
                cc->pluginSettings = new char[text_len];
                memcpy(cc->pluginSettings, text, text_len);
                cc->pluginSettingsLength = text_len;
            }
            break;

        case FLP_Text_ChanParams:
            cc->arpDir = pi[10];
            cc->arpRange = pi[11];
            cc->selectedArp = pi[12];
            if( cc->selectedArp < 8 ) {
                const int mappedArps[] = {0, 1, 5, 6, 2, 3, 4};
                cc->selectedArp = mappedArps[cc->selectedArp];
            }
            cc->arpTime = ( ( pi[13]+1 ) * m_project.tempo ) / ( 4*16 ) + 1;
            cc->arpGate = ( pi[14] * 100.0f ) / 48.0f;
            cc->arpEnabled = pi[10] > 0;
            break;

        case FLP_Text_EnvLfoParams:
            {
                const float scaling = 1.0 / 65536.0f;
                FL_Channel_Envelope e;

                switch (cc->envelopes.size()) {
                    case 1:
                        e.target = Volume;
                        break;
                    case 2:
                        e.target = Cut;
                        break;
                    case 3:
                        e.target = Resonance;
                        break;
                    default:
                        e.target = NumTargets;
                        break;
                }
                e.predelay = pi[2] * scaling;
                e.attack = pi[3] * scaling;
                e.hold = pi[4] * scaling;
                e.decay = pi[5] * scaling;
                e.sustain = 1-pi[6] / 128.0f;
                e.release = pi[7] * scaling;
                if (e.target == Volume)
                    e.amount = pi[1] ? 1 : 0;
                else
                    e.amount = pi[8] / 128.0f;
                cc->envelopes.push_back(e);
                break;
            }

        case FLP_Text_BasicChanParams:
            cc->volume = pi[1] / m_project.versionSpecificFactor;
            cc->panning = pi[0] / m_project.versionSpecificFactor;
            if (text_len > 12) {
                cc->filterType = puc[20];
                cc->filterCut = puc[12];
                cc->filterRes = puc[16];
                cc->filterEnabled = (puc[13] == 0);
                if( puc[20] >= 6 )
                    cc->filterCut *= 0.5f;
            }
            break;

        case FLP_Text_OldFilterParams:
            cc->filterType = puc[8];
            cc->filterCut = puc[0];
            cc->filterRes = puc[4];
            cc->filterEnabled = ( puc[1] == 0 );
            if( puc[8] >= 6 )
                cc->filterCut *= 0.5;
            break;

        case FLP_Text_AutomationData:
            {
                const int bpae = 12;
                const int imax = text_len / bpae;
                for (int i = 0; i < imax; ++i) {
                    FL_Automation a;
                    a.pos = pi[3*i+0] / (4*ppq / 192);
                    a.value = pi[3*i+2];
                    a.channel = pi[3*i+1] >> 16;
                    a.control = pi[3*i+1] & 0xffff;
                    if (a.channel >= 0 && a.channel < m_project.numChannels) {
                        m_project.channels[a.channel].automationData
                            .push_back(a);
                    }
                }
                break;
            }

        case FLP_Text_PatternNotes:
            {
                const int bpn = 20;
                const int imax = ( text_len + bpn - 1 ) / bpn;
                for (int i = 0; i < imax; ++i) {
                    int ch = *( puc + i*bpn + 6 );
                    int pan = *( puc + i*bpn + 16 );
                    int vol = *( puc + i*bpn + 17 );
                    int pos = *( (int *)( puc + i*bpn ) );
                    int key = *( puc + i*bpn + 12 );
                    int len = *( (int*)( puc + i*bpn + 8 ) );
                    pos /= (4*ppq) / 192;
                    len /= (4*ppq) / 192;
                    note n( len, pos, key, vol, pan);
                    if( ch < m_project.numChannels ) {
                        m_project.channels[ch].notes.push_back(
                            std::make_pair(m_project.currentPattern, n));
                    } else {
                        if (m_debug)
                            std::cerr << "Invalid ch: " << ch << std::endl;
                    }
                }
                break;
            }

        case FLP_Text_ChanGroupName:
            if (m_debug)
                std::cerr << "channel group name: " << text << std::endl;
            break;

        case 225:
            {
                enum FLP_EffectParams
                {
                    EffectParamVolume = 0x1fc0
                };

                const int bpi = 12;
                const int imax = text_len / bpi;
                for (int i = 0; i < imax; ++i) {
                    const int param = pi[i*3+1] & 0xffff;
                    const int ch = ( pi[i*3+1] >> 22 ) & 0x7f;
                    if( ch < 0 || ch > c_NumFLFxChannels )
                        continue;
                    const int val = pi[i*3+2];
                    if( param == EffectParamVolume ) {
                        m_project.effectChannels[ch].volume =
                            (val / m_project.versionSpecificFactor);
                    } else {
                        if (m_debug) {
                            std::cerr << "FX-ch: " << ch << "  param: "
                                << param << "  value: " << val << std::endl;
                        }
                    }
                }
                break;
            }

        case 233:    // playlist items
            {
                const int bpi = 28;
                const int imax = text_len / bpi;
                for (int i = 0; i < imax; ++i) {
                    const int pos = pi[i*bpi/sizeof(int)+0] /
                        ((4*ppq) / 192);
                    const int len = pi[i*bpi/sizeof(int)+2] /
                        ((4*ppq) / 192);
                    const int pat = pi[i*bpi/sizeof(int)+3] & 0xfff;
                    // whatever these magic numbers are for...
                    if( pat > 2146 && pat <= 2278 ) {
                        FL_PlayListItem i;
                        i.position = pos;
                        i.length = len;
                        i.pattern = 2278 - pat;
                        m_project.playListItems.push_back(i);
                    } else {
                        if (m_debug) {
                            std::cerr << "unknown playlist item: " << text
                                        << std::endl;
                        }
                    }
                }
                break;
            }
        default:
            if( ev >= FLP_Text ) {
                if (m_debug) {
                    std::cerr << "unhandled text (ev: " << ev << ", len: "
                        << text_len << "): " << text << std::endl;
                }
            } else {
                if (m_debug) {
                    std::cerr << "handling of FLP-event " << ev
                        << " not implemented yet (data=" << data << ")" 
                        << std::endl;
                }
            }
            break;
        }
    }
    // for each fruity wrapper, extract the plugin name.
    for (unsigned int i=0; i<m_project.channels.size(); ++i)
        tryFruityWrapper(&m_project.channels[i]);
    for (unsigned int i=0; i<m_project.effects.size(); ++i)
        tryFruityWrapper(&m_project.effects[i]);

    // create list of sample dependencies
    m_sampleStrings.clear();
    std::set<std::string>::iterator it;
    for (it=m_sampleSet.begin(); it != m_sampleSet.end(); ++it)
        m_sampleStrings.push_back(*it);

    // effects are the ones that aren't channels.
    m_channelPlugins.clear();
    m_effectStrings.clear();
    for (unsigned int i=0; i<m_project.channels.size(); ++i)
        m_channelPlugins.insert(m_project.channels[i].generatorName);
    for (it=m_effectPlugins.begin(); it != m_effectPlugins.end(); ++it) {
        if (m_channelPlugins.count(*it) == 0)
            m_effectStrings.push_back(*it);
    }
    

    m_good = true;
}

Flp::~Flp()
{

}

void Flp::tryFruityWrapper(FL_Plugin * plugin)
{
    if (Utils::toLower(plugin->generatorName).compare("fruity wrapper") != 0) 
        return;
    
    unsigned char * cursor = (unsigned char *) plugin->pluginSettings;
    plugin->generatorName = fruityWrapper(cursor, plugin->pluginSettingsLength);
}    

std::string Flp::fruityWrapper(unsigned char * buffer, int size)
{
    const int cidPluginName = 54;
    unsigned char * cursor = buffer;
    unsigned char * cursorEnd = cursor + size;
    int version = read32LEMem(&cursor);
    if (version <= 4) {
        // "old format"
        int extraBlockSize = read32LEMem(&cursor);
        int midiPort = read32LEMem(&cursor);
        int synthSaved = read32LEMem(&cursor);
        int pluginType = read32LEMem(&cursor);
        int pluginSpecificBlockSize = read32LEMem(&cursor);

        int pluginNameLen = readByteMem(&cursor);
        std::string pluginName((const char *)cursor, pluginNameLen);
        return pluginName;
    } else {
        // "new format"
        while (cursor < cursorEnd) {
            int chunkId = read32LEMem(&cursor);
            int64 chunkSize = read64LEMem(&cursor);
            if (chunkId == cidPluginName) {
                std::string pluginName((const char *)cursor,
                    (size_t)chunkSize);
                return pluginName;
            }
            skipMem(&cursor, chunkSize);
        }
    }
    return "";
}    

int Flp::readByte(std::fstream & in)
{
    unsigned char c;
    in.read((char*)&c, 1);
    return static_cast<int>(c);
}

int Flp::readByte()
{
    return readByte(m_file);
}

int Flp::readByteMem(unsigned char ** cursor)
{
    unsigned char c = (*cursor)[0];
    *cursor += 1;
    return static_cast<int>(c);
}

int Flp::read16LE(std::fstream & in)
{
    int value = readByte(in);
    value |= readByte(in) << 8;
    return value;
}

int Flp::read16LE()
{
    return read16LE(m_file);
}

int Flp::read16LEMem(unsigned char ** cursor)
{
    int value = readByteMem(cursor);
    value |= readByteMem(cursor) << 8;
    return value;
}

int Flp::read32LE(std::fstream & in)
{
    int value = readByte(in);
    value |= readByte(in) << 8;
    value |= readByte(in) << 16;
    value |= readByte(in) << 24;
    return value;
}

int Flp::read32LE()
{
    return read32LE(m_file);
}

int Flp::read32LEMem(unsigned char ** cursor)
{
    int value = readByteMem(cursor);
    value |= readByteMem(cursor) << 8;
    value |= readByteMem(cursor) << 16;
    value |= readByteMem(cursor) << 24;
    return value;
}

Flp::int64 Flp::read64LEMem(unsigned char ** cursor)
{
    int64 value = readByteMem(cursor);
    value |= ((int64)readByteMem(cursor)) << 8;
    value |= ((int64)readByteMem(cursor)) << 16;
    value |= ((int64)readByteMem(cursor)) << 24;

    value |= ((int64)readByteMem(cursor)) << 32;
    value |= ((int64)readByteMem(cursor)) << 40;
    value |= ((int64)readByteMem(cursor)) << 48;
    value |= ((int64)readByteMem(cursor)) << 56;
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

void Flp::skipMem(unsigned char ** cursor, int64 bytes)
{
    *cursor += bytes;
}

void Flp::dump_mem (const void * buffer, int n_bytes)
{
    unsigned char * cp = (unsigned char *) buffer;
    for (int k = 0; k < n_bytes; ++k) {
        //std::printf("%02x ", (unsigned int)cp[k]);
        std::cout << cp[k];
    }
    std::cout << "\n\n";
}

bool Flp::isValid(std::string filename)
{
    std::fstream in(filename.c_str(),std::fstream::in|std::fstream::binary);

    if (! in.good())
        return false;

    // check for the magic "FLhd" at the beginning
    int realMagic = makeId('F', 'L', 'h', 'd');
    int magic = read32LE(in);

    if (realMagic != magic)
        return false;

    // header should be 6 bytes long
    const int header_len = read32LE(in);
    if (header_len != 6)
        return false;

    // some type thing
    const int type = read16LE(in);
    if (type != 0) 
        return false;

    return true;
}
