#include "map.hh"
#include <iomanip> 
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <chrono>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//to_hex
//convert int to hex string, apply zero padding
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string to_hex(int n)
{
  std::stringstream ss;
  ss << std::hex << std::setw(2) << std::setfill('0') << n; // Use std::setw and std::setfill for zero padding
  return ss.str();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//rgb_to_hex
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string rgb_to_hex(int r, int g, int b)
{
  std::string str("#");
  str += to_hex(r);
  str += to_hex(g);
  str += to_hex(b);
  return str;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// load_geojson
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string load_geojson(const std::string& name)
{
  std::ifstream file(name);
  if (!file.is_open())
  {
    return "";
  }
  auto start_time = std::chrono::high_resolution_clock::now();
  std::string str;
  std::string line;
  while (std::getline(file, line))
  {
    str += line;
  }
  file.close();
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  return str;
}

