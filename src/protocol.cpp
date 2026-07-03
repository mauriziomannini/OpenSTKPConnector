#include "protocol.h"

namespace ostkp {

namespace {
constexpr const char* kProtocolSubscriptions[] = {
    "sim/time/paused",
    "sim/time/sim_speed",
    "sim/flightmodel/position/groundspeed",
    "sim/flightmodel/position/indicated_airspeed",
    "sim/flightmodel/position/true_airspeed",
    "sim/flightmodel/position/latitude",
    "sim/flightmodel/position/longitude",
    "sim/flightmodel/position/y_agl",
    "sim/flightmodel/position/elevation",
    "sim/flightmodel/failures/onground_any",
    "sim/flightmodel/failures/onground_all",
    "sim/flightmodel2/gear/on_ground",
    "sim/flightmodel/position/mag_psi",
    "sim/flightmodel/position/theta",
    "sim/flightmodel/position/phi",
    "sim/flightmodel/position/psi",
    "sim/flightmodel/position/local_vy",
    "sim/flightmodel/forces/g_nrml",
    "sim/aircraft/view/acf_descrip",
    "sim/aircraft/parts/acf_gear_deploy",
    "sim/flightmodel/controls/flaprqst",
    "sim/aircraft/controls/acf_flap_detents",
    "sim/operation/override/override_planepath",
    "sim/flightmodel/position/local_vx",
    "sim/flightmodel/position/local_vz",
    "sim/aircraft/parts/acf_gear_deploy",
    "sim/time/local_time_sec",
    "sim/time/zulu_time_sec",
    "sim/time/is_in_replay",
    "sim/flightmodel/position/magnetic_variation",
    "sim/cockpit/radios/com1_freq_hz",
    "sim/flightmodel/weight/m_fuel_total",
    "sim/flightmodel2/engines/N2_percent",
    "sim/aircraft/engine/acf_num_engines",
    "sim/aircraft/view/acf_ICAO",
};
}

std::string buildProtocolGreeting() {
    std::string greeting =
        "STKPCONNECT 1\n"
        "STKPCONNECT-VERSION 2020\n"
        "\n";

    for (const char* dataref : kProtocolSubscriptions) {
        greeting += "sub ";
        greeting += dataref;
        greeting += "\n";
    }

    return greeting;
}

}
