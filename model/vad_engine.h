#ifndef VAD_ENGINE_H
#define VAD_ENGINE_H

#include "model/vad_informer.h"
#include "torch/torch.h"
#include "torch/script.h"
#include "utils/logging.h"

namespace vad
{
    typedef struct VADEngineConfig_
    {

    }VADEngineConfig;

    class VADEngine{
        public:
            VADEngine() = delete;
            VADEngine(const VADEngine& other) = delete;
            VADEngine& operator=(const VADEngine&) = delete;
            int InitVADEngine(const VADEngineConfig & config);
            int Exit();
        private:

    };
}

#endif