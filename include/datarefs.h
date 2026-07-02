#pragma once
#include <string>
#include <vector>
#include "XPLMDataAccess.h"

namespace ostkp {

enum class DataRefType { Float, Double, Int, FloatArray, IntArray, ByteData };

struct DataRefItem {
    std::string name;
    DataRefType type;
    XPLMDataRef ref = nullptr;
    int array_count = 16;
    int send_every_frames = 1;
};

class DataRefs {
public:
    void initialize();
    std::string buildFrame(bool force_all = false);

private:
    std::vector<DataRefItem> items_;
    int frame_counter_ = 0;
};

}
