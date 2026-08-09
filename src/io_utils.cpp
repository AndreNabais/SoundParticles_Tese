#include "io_utils.h"
#include <fstream>

void save_to_csv(const std::vector<float>& time, const std::vector<float>& data, const std::string& filename) {
    std::ofstream file(filename);
    for (size_t i = 0; i < time.size(); ++i)
        file << time[i] << "," << data[i] << "\n";
}