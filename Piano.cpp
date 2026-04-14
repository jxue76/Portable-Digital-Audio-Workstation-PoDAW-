#include "Piano.hpp"
#include "Rhodey.h"

Piano::Piano()
    : Instrument("Piano", []() { return std::make_unique<stk::Rhodey>(); }, 21, 108, 1.0f) {
}
