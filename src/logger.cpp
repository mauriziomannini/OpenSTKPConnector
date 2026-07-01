#include "logger.h"
#include "XPLMUtilities.h"

namespace ostkp {
void log(const std::string& message) {
    XPLMDebugString(("[OpenSTKPConnector] " + message + "\n").c_str());
}
}
