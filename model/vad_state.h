#ifndef VAD_STATE_H
#define VAD_STATE_H

namespace vad
{
enum class VADIteratorState
{
    HumanVoiceEnded,
    HumanVoiceFound,
    StillInSpeech,
    BeforeSpeech
};

enum class VADInformerState
{
    FoundNothing = -1,
    FoundStart = 1,
    FoundPair = 2,
    FoundStartOnly = 3
};
}
#endif