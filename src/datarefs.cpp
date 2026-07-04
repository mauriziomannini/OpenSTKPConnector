#include "datarefs.h"
#include "logger.h"
#include <sstream>
#include <iomanip>

namespace ostkp {

namespace {

constexpr int kDefaultScalarCount = 16;
constexpr int kGearArrayCount = 10;
constexpr int kOverrideArrayCount = 20;
constexpr int kVeryFrequentFrames = 10;
constexpr int kFrequentFrames = 25;
constexpr int kRareFrames = 1000;

std::string base64Encode(const unsigned char* data, size_t size) {
    static constexpr char kAlphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encoded;
    encoded.reserve(((size + 2) / 3) * 4);

    for (size_t i = 0; i < size; i += 3) {
        const unsigned int octet_a = data[i];
        const unsigned int octet_b = (i + 1 < size) ? data[i + 1] : 0;
        const unsigned int octet_c = (i + 2 < size) ? data[i + 2] : 0;
        const unsigned int triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        encoded.push_back(kAlphabet[(triple >> 18) & 0x3F]);
        encoded.push_back(kAlphabet[(triple >> 12) & 0x3F]);
        encoded.push_back((i + 1 < size) ? kAlphabet[(triple >> 6) & 0x3F] : '=');
        encoded.push_back((i + 2 < size) ? kAlphabet[triple & 0x3F] : '=');
    }

    return encoded;
}

}

void DataRefs::initialize() {
    items_ = {
        // Simulator state.
        {"sim/time/paused", DataRefType::Int, nullptr, kDefaultScalarCount, kRareFrames},
        {"sim/time/sim_speed", DataRefType::Int, nullptr, kDefaultScalarCount, kRareFrames},

        // Aircraft position, speed, and attitude.
        {"sim/flightmodel/position/groundspeed", DataRefType::Float},
        {"sim/flightmodel/position/indicated_airspeed", DataRefType::Float, nullptr, kDefaultScalarCount, kVeryFrequentFrames},
        {"sim/flightmodel/position/true_airspeed", DataRefType::Float},
        {"sim/flightmodel/position/latitude", DataRefType::Double},
        {"sim/flightmodel/position/longitude", DataRefType::Double},
        {"sim/flightmodel/position/y_agl", DataRefType::Float},
        {"sim/flightmodel/position/elevation", DataRefType::Double},
        {"sim/flightmodel/position/mag_psi", DataRefType::Float},
        {"sim/flightmodel/position/magnetic_variation", DataRefType::Float, nullptr, kDefaultScalarCount, kVeryFrequentFrames},
        {"sim/flightmodel/position/theta", DataRefType::Float},
        {"sim/flightmodel/position/phi", DataRefType::Float},
        {"sim/flightmodel/position/psi", DataRefType::Float},
        {"sim/flightmodel/position/local_vy", DataRefType::Float},
        {"sim/flightmodel/forces/g_nrml", DataRefType::Float},

        // Aircraft metadata and configuration.
        {"sim/aircraft/view/acf_descrip", DataRefType::ByteData, nullptr, 0, kRareFrames},
        {"sim/aircraft/parts/acf_gear_deploy", DataRefType::FloatArray, nullptr, kGearArrayCount, kRareFrames},
        {"sim/flightmodel/controls/flaprqst", DataRefType::Float, nullptr, kDefaultScalarCount, kFrequentFrames},
        {"sim/aircraft/controls/acf_flap_detents", DataRefType::Int, nullptr, kDefaultScalarCount, kRareFrames},
        {"sim/operation/override/override_planepath", DataRefType::IntArray, nullptr, kOverrideArrayCount, kRareFrames},

        // Local velocity and clock values used by STKP tracking.
        {"sim/flightmodel/position/local_vx", DataRefType::Float},
        {"sim/flightmodel/position/local_vz", DataRefType::Float},
        {"sim/time/local_time_sec", DataRefType::Float},
        {"sim/time/zulu_time_sec", DataRefType::Float},
        {"sim/time/is_in_replay", DataRefType::Int, nullptr, kDefaultScalarCount, kRareFrames},
        {"sim/cockpit/radios/com1_freq_hz", DataRefType::Int, nullptr, kDefaultScalarCount, kRareFrames},
        {"sim/flightmodel/weight/m_fuel_total", DataRefType::Float, nullptr, kDefaultScalarCount, kFrequentFrames},

        // Landing and ground-contact state. These stay unthrottled for touchdown detection.
        {"sim/flightmodel/failures/onground_all", DataRefType::Int},
        {"sim/flightmodel/failures/onground_any", DataRefType::Int},
        {"sim/flightmodel2/engines/N2_percent", DataRefType::FloatArray, nullptr, kDefaultScalarCount},
        {"sim/flightmodel2/gear/on_ground", DataRefType::IntArray, nullptr, kGearArrayCount},

        // Static aircraft identity.
        {"sim/aircraft/engine/acf_num_engines", DataRefType::Int, nullptr, kDefaultScalarCount, kRareFrames},
        {"sim/aircraft/view/acf_ICAO", DataRefType::ByteData, nullptr, 0, kRareFrames},
    };

    int found_count = 0;
    int missing_count = 0;

    for (auto& item : items_) {
        item.ref = XPLMFindDataRef(item.name.c_str());
        if (item.ref) {
            ++found_count;
        } else {
            ++missing_count;
            log("DataRef not found: " + item.name);
        }
    }
    log("DataRefs initialized: " + std::to_string(items_.size()) +
        "; found: " + std::to_string(found_count) +
        "; missing: " + std::to_string(missing_count));
}

std::string DataRefs::buildFrame(bool force_all) {
    std::ostringstream out;
    out << std::setprecision(10);
    ++frame_counter_;

    for (const auto& item : items_) {
        if (!item.ref) continue;
        if (!force_all && item.send_every_frames > 1 && frame_counter_ % item.send_every_frames != 0) continue;
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
            case DataRefType::ByteData: {
                int byte_count = item.array_count;
                if (byte_count <= 0) byte_count = XPLMGetDatab(item.ref, nullptr, 0, 0);
                if (byte_count <= 0) {
                    out << "ub " << item.name << " \n";
                    break;
                }

                std::vector<char> values(static_cast<size_t>(byte_count), 0);
                const int bytes_read = XPLMGetDatab(item.ref, values.data(), 0, byte_count);
                if (bytes_read < byte_count && bytes_read > 0) values.resize(static_cast<size_t>(bytes_read));

                const auto* bytes = reinterpret_cast<const unsigned char*>(values.data());
                out << "ub " << item.name << " " << base64Encode(bytes, values.size()) << "\n";
                break;
            }
        }
    }
    return out.str();
}

}
