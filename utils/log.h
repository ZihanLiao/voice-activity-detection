// Copyright (c) 2021 Mobvoi Inc (Binbin Zhang)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef UTILS_LOG_H_
#define UTILS_LOG_H_
#include <iostream>
#include <cstdlib>       // for ssize_t.
#include <cstdint>       // for ?int*_t.

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
// Because openfst is a dynamic library compiled with gflags/glog, we must use
// the gflags/glog from openfst to avoid them linked both statically and
// dynamically into the executable.
// #include "fst/log.h"

class LogMessage
{
public:
    explicit LogMessage(std::string type) : fatal_(type == "FATAL")
    {
        std::cerr << type << ": ";
    }
    ~LogMessage()
    {
        std::cerr << std::endl;
        if (fatal_)
            exit(1);
    }
    std::ostream &stream() { return std::cerr; }

private:
    bool fatal_;
};

#define LOG(type) LogMessage(#type).stream()
#define VLOG(level)             \
    if ((level) <= FST_FLAGS_v) \
    LOG(INFO)

// Checks.
inline void FstCheck(bool x, std::string expr, std::string file, int line)
{
    if (!x)
    {
        LOG(FATAL) << "Check failed: \"" << expr << "\" file: " << file << " line: " << line;
    }
}
#define CHECK(x) FstCheck(static_cast<bool>(x), #x, __FILE__, __LINE__)
#define CHECK_GT(x, y) CHECK((x) > (y))
#define CHECK_GE(x, y) CHECK((x) >= (y))
#define CHECK_EQ(x, y) CHECK((x) == (y))
#endif // UTILS_LOG_H_
