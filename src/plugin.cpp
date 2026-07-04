#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "logger.h"
#include "tcp_server.h"
#include "datarefs.h"

#include <cstring>
#include <memory>
#include <string>

namespace {
constexpr const char* kPluginVersion = "v0.6-dev";

std::unique_ptr<ostkp::TcpServer> g_server;
ostkp::DataRefs g_datarefs;

float flightLoopCallback(float, float, int, void*) {
    if (g_server && g_server->isRunning() && g_server->hasClients()) {
        g_server->pollClients();
        const bool force_full_frame = g_server->consumeInitialSnapshotRequestCount() > 0;
        g_server->broadcast(g_datarefs.buildFrame(force_full_frame));
    }
    return 0.05f;
}
}

PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc) {
    std::strcpy(outName, "OpenSTKPConnector");
    std::strcpy(outSig, "com.openstkp.connector");
    std::strcpy(outDesc, "Open Apple Silicon STKPConnector-compatible bridge for SimToolkitPro.");

    ostkp::log(std::string("starting ") + kPluginVersion);
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
