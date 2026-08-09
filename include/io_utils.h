#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>


void save_to_csv(const std::vector<float>& time, const std::vector<float>& data, const std::string& filename);

#endif // IO_UTILS_H