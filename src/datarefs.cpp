#include "datarefs.h"
#include "logger.h"
#include <sstream>
#include <iomanip>

namespace ostkp {

void DataRefs::initialize() {
    items_ = {
        {"sim/flightmodel/position/groundspeed", DataRefType::Float},
        {"sim/flightmodel/position/indicated_airspeed", DataRefType::Float},
        {"sim/flightmodel/position/true_airspeed", DataRefType::Float},
        {"sim/flightmodel/position/latitude", DataRefType::Double},
        {"sim/flightmodel/position/longitude", DataRefType::Double},
        {"sim/flightmodel/position/y_agl", DataRefType::Float},
        {"sim/flightmodel/position/elevation", DataRefType::Double},
        {"sim/flightmodel/position/mag_psi", DataRefType::Float},
        {"sim/flightmodel/position/magnetic_variation", DataRefType::Float},
        {"sim/flightmodel/position/theta", DataRefType::Float},
        {"sim/flightmodel/position/phi", DataRefType::Float},
        {"sim/flightmodel/position/psi", DataRefType::Float},
        {"sim/flightmodel/position/local_vy", DataRefType::Float},
        {"sim/flightmodel/forces/g_nrml", DataRefType::Float},
        {"sim/flightmodel/position/local_vx", DataRefType::Float},
        {"sim/flightmodel/position/local_vz", DataRefType::Float},
        {"sim/time/local_time_sec", DataRefType::Float},
        {"sim/time/zulu_time_sec", DataRefType::Float},
        {"sim/flightmodel/weight/m_fuel_total", DataRefType::Float},
        {"sim/flightmodel/failures/onground_all", DataRefType::Int},
        {"sim/flightmodel/failures/onground_any", DataRefType::Int},
        {"sim/flightmodel2/engines/N2_percent", DataRefType::FloatArray, nullptr, 16},
        {"sim/flightmodel2/gear/on_ground", DataRefType::IntArray, nullptr, 10},
    };

    for (auto& item : items_) {
        item.ref = XPLMFindDataRef(item.name.c_str());
        if (!item.ref) log("DataRef not found: " + item.name);
    }
    log("DataRefs initialized: " + std::to_string(items_.size()));
}

std::string DataRefs::buildFrame() const {
    std::ostringstream out;
    out << std::setprecision(10);

    for (const auto& item : items_) {
        if (!item.ref) continue;
        switch (item.type) {
            case DataRefType::Float:
                out << "uf " << item.name << " " << XPLMGetDataf(item.ref) << "\n";
                break;
            case DataRefType::Double:
                out << "ud " << item.name << " " << XPLMGetDatad(item.ref) << "\n";
                break;
            case DataRefType::Int:
                out << "ui " << item.name << " " << XPLMGetDatai(item.ref) << "\n";
                break;
            case DataRefType::FloatArray: {
                std::vector<float> values(static_cast<size_t>(item.array_count), 0.0f);
                XPLMGetDatavf(item.ref, values.data(), 0, item.array_count);
                out << "ufa " << item.name << " [";
                for (int i = 0; i < item.array_count; ++i) {
                    if (i > 0) out << ",";
                    out << values[static_cast<size_t>(i)];
                }
                out << "]\n";
                break;
            }
            case DataRefType::IntArray: {
                std::vector<int> values(static_cast<size_t>(item.array_count), 0);
                XPLMGetDatavi(item.ref, values.data(), 0, item.array_count);
                out << "uia " << item.name << " [";
                for (int i = 0; i < item.array_count; ++i) {
                    if (i > 0) out << ",";
                    out << values[static_cast<size_t>(i)];
                }
                out << "]\n";
                break;
            }
        }
    }
    return out.str();
}

}
