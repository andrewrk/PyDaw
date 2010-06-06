#ifndef FLP_H
#define FLP_H

#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <map>
#include <set>

class Flp
{
public:
    Flp(std::string filename);
    ~Flp();

    // returns whether a file is an FL Studio project.
    static bool isValid(std::string filename);

    // true if loaded ok
    bool good() const { return m_good; }
    // if not good, this tells why
    std::string errorMessage() const { return m_errMsg; }

    // whether or not to print unrecognized events to stderr
    void setDebug(bool value) { m_debug = value;}
    bool debug() const {return m_debug;}

    // data about this project
    int tempo() const { return m_project.tempo; }
    std::string title() const { return m_project.projectTitle; }
    std::string versionString() const { return m_project.versionString; }

    // dependencies
    // channels
    int channelCount() const { return m_project.channels.size(); }
    std::string channelName(int i) const { return m_project.channels[i].name; }
    std::string channelPluginName(int i) const { return m_project.channels[i].generatorName; }
    // effects
    int effectCount() const { return m_effectStrings.size(); }
    std::string effectPluginName(int i) const { return m_effectStrings[i]; }
    // samples
    int sampleCount() const { return m_sampleStrings.size(); }
    std::string sampleFileName(int i) const { return m_sampleStrings[i]; }

private:
    typedef long long int int64;
    typedef int int32;
    typedef short int int16;
    static const int c_NumFLFxChannels;

    struct Rgb
    {
        Rgb(int _r, int _g, int _b) :
            r(_r),
            g(_g),
            b(_b)
        {
        }
        int r,g,b;
    };

    struct FL_Plugin
    {
        FL_Plugin(std::string _name):
            name(_name),
            pluginSettings( NULL ),
            pluginSettingsLength( 0 )
        {
        }

        ~FL_Plugin()
        {
            delete[] pluginSettings;
        }

        std::string name; // the nickname given by the user
        std::string generatorName; // the actual name of the plugin.
        char * pluginSettings;
        int pluginSettingsLength;
    };

    enum FLP_Events
    {
        // BYTE EVENTS
        FLP_Byte        = 0,
        FLP_Enabled        = 0,
        FLP_NoteOn        = 1,    //+pos (byte)
        FLP_Vol            = 2,
        FLP_Pan            = 3,
        FLP_MIDIChan        = 4,
        FLP_MIDINote        = 5,
        FLP_MIDIPatch        = 6,
        FLP_MIDIBank        = 7,
        FLP_LoopActive        = 9,
        FLP_ShowInfo        = 10,
        FLP_Shuffle        = 11,
        FLP_MainVol        = 12,
        FLP_Stretch        = 13,    // old byte version
        FLP_Pitchable        = 14,
        FLP_Zipped        = 15,
        FLP_Delay_Flags        = 16,
        FLP_PatLength        = 17,
        FLP_BlockLength        = 18,
        FLP_UseLoopPoints    = 19,
        FLP_LoopType        = 20,
        FLP_ChanType        = 21,
        FLP_MixSliceNum        = 22,
        FLP_EffectChannelMuted    = 27,

        // WORD EVENTS
        FLP_Word        = 64,
        FLP_NewChan        = FLP_Word,
        FLP_NewPat        = FLP_Word + 1,        //+PatNum (word)
        FLP_Tempo        = FLP_Word + 2,
        FLP_CurrentPatNum    = FLP_Word + 3,
        FLP_PatData        = FLP_Word + 4,
        FLP_FX            = FLP_Word + 5,
        FLP_Fade_Stereo        = FLP_Word + 6,
        FLP_CutOff        = FLP_Word + 7,
        FLP_DotVol        = FLP_Word + 8,
        FLP_DotPan        = FLP_Word + 9,
        FLP_PreAmp        = FLP_Word + 10,
        FLP_Decay        = FLP_Word + 11,
        FLP_Attack        = FLP_Word + 12,
        FLP_DotNote        = FLP_Word + 13,
        FLP_DotPitch        = FLP_Word + 14,
        FLP_DotMix        = FLP_Word + 15,
        FLP_MainPitch        = FLP_Word + 16,
        FLP_RandChan        = FLP_Word + 17,
        FLP_MixChan        = FLP_Word + 18,
        FLP_Resonance        = FLP_Word + 19,
        FLP_LoopBar        = FLP_Word + 20,
        FLP_StDel        = FLP_Word + 21,
        FLP_FX3            = FLP_Word + 22,
        FLP_DotReso        = FLP_Word + 23,
        FLP_DotCutOff        = FLP_Word + 24,
        FLP_ShiftDelay        = FLP_Word + 25,
        FLP_LoopEndBar        = FLP_Word + 26,
        FLP_Dot            = FLP_Word + 27,
        FLP_DotShift        = FLP_Word + 28,
        FLP_LayerChans        = FLP_Word + 30,

        // DWORD EVENTS
        FLP_Int            = 128,
        FLP_Color        = FLP_Int,
        FLP_PlayListItem    = FLP_Int + 1,    //+Pos (word) +PatNum (word)
        FLP_Echo        = FLP_Int + 2,
        FLP_FXSine        = FLP_Int + 3,
        FLP_CutCutBy        = FLP_Int + 4,
        FLP_WindowH        = FLP_Int + 5,
        FLP_MiddleNote        = FLP_Int + 7,
        FLP_Reserved        = FLP_Int + 8,    // may contain an invalid
                            // version info
        FLP_MainResoCutOff    = FLP_Int + 9,
        FLP_DelayReso        = FLP_Int + 10,
        FLP_Reverb        = FLP_Int + 11,
        FLP_IntStretch        = FLP_Int + 12,
        FLP_SSNote        = FLP_Int + 13,
        FLP_FineTune        = FLP_Int + 14,

        // TEXT EVENTS
        FLP_Undef        = 192,        //+Size (var length)
        FLP_Text        = FLP_Undef,    //+Size (var length)+Text
                            //    (Null Term. String)
        FLP_Text_ChanName    = FLP_Text,    // name for the current channel
        FLP_Text_PatName    = FLP_Text + 1,    // name for the current pattern
        FLP_Text_Title        = FLP_Text + 2,    // title of the loop
        FLP_Text_Comment    = FLP_Text + 3,    // old comments in text format.
                            // Not used anymore
        FLP_Text_SampleFileName    = FLP_Text + 4,    // filename for the sample in
                            // the current channel, stored
                            // as relative path
        FLP_Text_URL        = FLP_Text + 5,
        FLP_Text_CommentRTF    = FLP_Text + 6,    // new comments in Rich Text
                            // format
        FLP_Text_Version    = FLP_Text + 7,
        FLP_Text_PluginName    = FLP_Text + 9,    // plugin file name
                            // (without path)

        FLP_Text_EffectChanName    = FLP_Text + 12,
        FLP_Text_MIDICtrls    = FLP_Text + 16,
        FLP_Text_Delay        = FLP_Text + 17,
        FLP_Text_TS404Params    = FLP_Text + 18,
        FLP_Text_DelayLine    = FLP_Text + 19,
        FLP_Text_NewPlugin    = FLP_Text + 20,
        FLP_Text_PluginParams    = FLP_Text + 21,
        FLP_Text_ChanParams    = FLP_Text + 23,// block of various channel
                            // params (can grow)
        FLP_Text_EnvLfoParams    = FLP_Text + 26,
        FLP_Text_BasicChanParams= FLP_Text + 27,
        FLP_Text_OldFilterParams= FLP_Text + 28,
        FLP_Text_AutomationData    = FLP_Text + 31,
        FLP_Text_PatternNotes    = FLP_Text + 32,
        FLP_Text_ChanGroupName    = FLP_Text + 39,
        FLP_Text_PlayListItems    = FLP_Text + 41,

        FLP_CmdCount

    };

    struct FL_Automation
    {
        FL_Automation() :
            pos( 0 ),
            value( 0 ),
            channel( 0 ),
            control( 0 )
        {
        }

        enum Controls
        {
            ControlVolume            = 0,
            ControlPanning            = 1,
            ControlFilterCut        = 2,
            ControlFilterRes        = 3,
            ControlPitch            = 4,
            ControlFilterType        = 5,
            ControlFXChannel        = 8,

            ControlVolPredelay        = 4354,
            ControlVolAttack,
            ControlVolHold,
            ControlVolDecay,
            ControlVolSustain,
            ControlVolRelease,
            ControlVolLfoPredelay        = ControlVolPredelay+7,
            ControlVolLfoAttack,
            ControlVolLfoAmount,
            ControlVolLfoSpeed,
            ControlVolAttackTension        = ControlVolPredelay+12,
            ControlVolDecayTension,
            ControlVolReleaseTension,
            ControlCutPredelay        = 4610,
            ControlCutAttack,
            ControlCutHold,
            ControlCutDecay,
            ControlCutSustain,
            ControlCutRelease,
            ControlCutAmount,
            ControlCutLfoPredelay        = ControlCutPredelay+7,
            ControlCutLfoAttack,
            ControlCutLfoAmount,
            ControlCutLfoSpeed,
            ControlCutAttackTension        = ControlCutPredelay+12,
            ControlCutDecayTension,
            ControlCutReleaseTension,

            ControlResPredelay        = 4866,
            ControlResAttack,
            ControlResHold,
            ControlResDecay,
            ControlResSustain,
            ControlResRelease,
            ControlResAmount,
            ControlResLfoPredelay        = ControlResPredelay+7,
            ControlResLfoAttack,
            ControlResLfoAmount,
            ControlResLfoSpeed,
            ControlResAttackTension        = ControlResPredelay+12,
            ControlResDecayTension,
            ControlResReleaseTension
        };

        int pos;
        int value;
        int channel;
        int control;
    };

    struct note
    {
        note(int _len, int _pos, int _key, int _vol, int _pan) :
            key(_key),
            volume(_vol),
            panning(_pan),
            length(_len),
            position(_pos)
        {
        }
        int key;
        int volume;
        int panning;
        int length;
        int position;
        int detuning;
    };

    enum Targets
    {
        Volume,
        Cut,
        Resonance,
        NumTargets
    };

    struct FL_Channel_Envelope
    {
        Targets target;
        float predelay;
        float attack;
        float hold;
        float decay;
        float sustain;
        float release;
        float amount;
    };

    enum FilterTypes
    {
        LowPass,
        HiPass,
        BandPass_CSG,
        BandPass_CZPG,
        Notch,
        AllPass,
        Moog,
        DoubleLowPass,
        Lowpass_RC12,
        Bandpass_RC12,
        Highpass_RC12,
        Lowpass_RC24,
        Bandpass_RC24,
        Highpass_RC24,
        Formantfilter,
        NumFilters
    };

    enum ArpDirections
    {
        ArpDirUp,
        ArpDirDown,
        ArpDirUpAndDown,
        ArpDirRandom,
        NumArpDirections
    };

    struct FL_Channel : public FL_Plugin
    {
        std::vector<FL_Automation> automationData;

        int volume;
        int panning;
        int baseNote;
        int fxChannel;
        int layerParent;

        typedef std::vector<std::pair<int, note> > noteVector;
        noteVector notes;

        std::vector<int> dots;

        std::string sampleFileName;
        int sampleAmp;
        bool sampleReversed;
        bool sampleReverseStereo;
        bool sampleUseLoopPoints;

        std::vector<FL_Channel_Envelope> envelopes;

        int filterType;
        float filterCut;
        float filterRes;
        bool filterEnabled;

        int arpDir;
        int arpRange;
        int selectedArp;
        float arpTime;
        float arpGate;
        bool arpEnabled;

        Rgb color;


        FL_Channel(std::string name = "") :
            FL_Plugin(name),
            automationData(),
            volume( 100 ),
            panning( 0 ),
            baseNote( 57 ),
            fxChannel( 0 ),
            layerParent( -1 ),
            notes(),
            dots(),
            sampleFileName(),
            sampleAmp( 100 ),
            sampleReversed( false ),
            sampleReverseStereo( false ),
            sampleUseLoopPoints( false ),
            envelopes(),
            filterType( LowPass ),
            filterCut( 10000 ),
            filterRes( 0.1 ),
            filterEnabled( false ),
            arpDir( ArpDirUp ),
            arpRange( 0 ),
            selectedArp( 0 ),
            arpTime( 100 ),
            arpGate( 100 ),
            arpEnabled( false ),
            color( Rgb( 64, 128, 255 ) )
        {
        }

    };


    struct FL_Effect : public FL_Plugin
    {
        FL_Effect(std::string name) :
            FL_Plugin(name),
            fxChannel( 0 ),
            fxPos( 0 )
        {
        }

        int fxChannel;
        int fxPos;

    };

    struct FL_PlayListItem
    {
        FL_PlayListItem() :
            position( 0 ),
            length( 1 ),
            pattern( 0 )
        {
        }
        int position;
        int length;
        int pattern;
    };


    struct FL_EffectChannel
    {
        FL_EffectChannel() :
            name(),
            volume( 300 ),
            isMuted( false )
        {
        }

        std::string name;
        int volume;
        bool isMuted;
    };

    struct FL_Project
    {
        int mainVolume;
        int mainPitch;
        int tempo; // in beats per minute
        int numChannels;

        std::vector<FL_Channel> channels;
        std::vector<FL_Effect> effects;
        std::vector<FL_PlayListItem> playListItems;

        std::map<int, std::string> patternNames;
        int maxPatterns;
        int currentPattern;
        int activeEditPattern;

        FL_EffectChannel effectChannels[64+1];
        int currentEffectChannel;

        std::string projectNotes;
        std::string projectTitle;

        std::string versionString;
        int version;
        int versionSpecificFactor;

        FL_Project() :
            mainVolume( 300 ),
            mainPitch( 0 ),
            tempo( 140 ),
            numChannels( 0 ),
            channels(),
            effects(),
            playListItems(),
            patternNames(),
            maxPatterns( 0 ),
            currentPattern( 0 ),
            activeEditPattern( 0 ),
            effectChannels(),
            currentEffectChannel( -1 ),
            projectNotes(),
            projectTitle(),
            versionString(),
            version( 0x100 ),
            versionSpecificFactor( 1 )
        {
        }

    };

    bool m_good;
    // why not good
    std::string m_errMsg;
    // file handle
    std::fstream m_file;

    bool m_debug;
    
    FL_Project m_project;

    std::vector<std::string> m_effectStrings;
    std::vector<std::string> m_sampleStrings;

    std::set<std::string> m_sampleSet;
    std::set<std::string> m_effectPlugins;
    std::set<std::string> m_channelPlugins;

    // read an 8bit signed integer and move the read position
    int readByte();
    static int readByte(std::fstream &);
    int readByteMem(unsigned char ** cursor);
    // read a 16bit little endian signed integer and move the read position
    int read16LE();
    static int read16LE(std::fstream &);
    int read16LEMem(unsigned char ** cursor);
    // read a 32bit little endian signed integer and move the read position
    int read32LE();
    static int read32LE(std::fstream &);
    int read32LEMem(unsigned char ** cursor);
    // read a 64bit little endian signed integer and move the read position
    int64 read64LEMem(unsigned char ** cursor);
    // skip the read pointer forward 
    void skip(int bytes);
    void skipMem(unsigned char ** cursor, int64 bytes);
    // create an integer out of 4 characters
    static int makeId(char,char,char,char);

    static void dump_mem (const void * buffer, int n_bytes);
    void tryFruityWrapper(FL_Plugin * plugin);
    std::string fruityWrapper(unsigned char * buffer, int size);
};

#endif
