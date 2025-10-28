#ifndef MAP_HH
#define MAP_HH

#include <string>

std::string to_hex(int n);
std::string rgb_to_hex(int r, int g, int b);
std::string load_geojson(const std::string& name);

#endif