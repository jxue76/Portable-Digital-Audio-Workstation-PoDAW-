#include "Bass.hpp"
#include "StifKarp.h"

Bass::Bass()
    : Instrument("Bass", []() { return std::make_unique<stk::StifKarp>(); }, 36, 81, 1.0f) {
}