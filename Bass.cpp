#include "Bass.hpp"
#include <stk/Bass.h>

Bass::Bass() : Instrument("Bass", std::make_shared<stk::Bass>(), 36, 81, 1.0f) {
}