#pragma once
#include <string>
#include <vector>
#include "XPLMDataAccess.h"

namespace ostkp {

enum class DataRefType { Float, Double, FloatArray };

struct DataRefItem {
    std::string name;
    DataRefType type;
    XPLMDataRef ref = nullptr;
    int array_count = 16;
};

class DataRefs {
public:
    void initialize();
    std::string buildFrame() const;

private:
    std::vector<DataRefItem> items_;
};

}
