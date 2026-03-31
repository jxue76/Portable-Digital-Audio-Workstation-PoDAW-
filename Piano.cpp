#include "Piano.hpp"
#include <stk/Rhodey.h>

Piano::Piano() : Instrument("Piano", std::make_shared<stk::Rhodey>(), 21, 108, 1.0f) {
}
