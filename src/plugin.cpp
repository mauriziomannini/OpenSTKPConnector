#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "logger.h"
#include "tcp_server.h"
#include "datarefs.h"

#include <cstring>
#include <memory>

namespace {
std::unique_ptr<ostkp::TcpServer> g_server;
ostkp::DataRefs g_datarefs;

float flightLoopCallback(float, float, int, void*) {
    if (g_server && g_server->isRunning()) {
        g_server->broadcast(g_datarefs.buildFrame());
    }
    return 0.05f;
}
}

PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc) {
    std::strcpy(outName, "OpenSTKPConnector");
    std::strcpy(outSig, "com.openstkp.connector");
    std::strcpy(outDesc, "Open Apple Silicon STKPConnector-compatible bridge for SimToolkitPro.");

    ostkp::log("starting");
    g_datarefs.initialize();
    g_server = std::make_unique<ostkp::TcpServer>(51303);
    g_server->start();

    XPLMRegisterFlightLoopCallback(flightLoopCallback, 1.0f, nullptr);
    return 1;
}

PLUGIN_API void XPluginStop(void) {
    ostkp::log("stopping");
    XPLMUnregisterFlightLoopCallback(flightLoopCallback, nullptr);
    if (g_server) {
        g_server->stop();
        g_server.reset();
    }
}

PLUGIN_API void XPluginDisable(void) { ostkp::log("disabled"); }
PLUGIN_API int XPluginEnable(void) { ostkp::log("enabled"); return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, int, void*) {}
