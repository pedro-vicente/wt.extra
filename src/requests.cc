#include "parser.hh"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>

extern csv_parser* parser;
extern std::string geojson_wards;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//to_hex
//convert int to hex string, apply zero padding
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string to_hex(int n)
{
  std::stringstream ss;
  ss << std::hex << n;
  std::string str(ss.str());
  return str.size() == 1 ? "0" + str : str;
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

/////////////////////////////////////////////////////////////////////////////////////////////////////
// load_geojson
/////////////////////////////////////////////////////////////////////////////////////////////////////

int load_geojson()
{
  std::ifstream file("ward-2012.geojson");
  if (!file.is_open())
  {
    return -1;
  }

  auto start_time = std::chrono::high_resolution_clock::now();

  std::string line;
  geojson_wards.clear();
  while (std::getline(file, line))
  {
    geojson_wards += line;
  }
  file.close();

  if (!geojson_wards.empty())
  {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    return 1;
  }

  return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// load_dc311_full
/////////////////////////////////////////////////////////////////////////////////////////////////////

int load_dc311_full()
{
  parser = new csv_parser("311_city_service_requests_2024_part1.csv", "311_city_service_requests_2024_part2.csv");

  auto start_time = std::chrono::high_resolution_clock::now();

  if (parser->load_file())
  {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "File loaded in: " << duration.count() << " ms" << std::endl;
    std::cout << "Rows: " << parser->data.size() << std::endl;
    std::cout << "Columns: " << parser->headers.size() << std::endl;

    std::cout << "Headers:" << std::endl;
    for (size_t idx = 0; idx < parser->headers.size(); ++idx)
    {
      std::cout << "  " << parser->headers[idx] << std::endl;
    }
    return 1;
  }

  return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// load_dc311_simple
/////////////////////////////////////////////////////////////////////////////////////////////////////

int load_dc311_simple()
{
  parser = new csv_parser("dc_311-2016.csv.s0311.csv");

  auto start_time = std::chrono::high_resolution_clock::now();

  if (parser->load_simple_file())
  {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "File loaded in: " << duration.count() << " ms" << std::endl;
    std::cout << "Rows: " << parser->latitude.size() << std::endl;
    std::cout << "Columns: " << parser->headers.size() << std::endl;

    std::cout << "Headers:" << std::endl;
    for (size_t idx = 0; idx < parser->headers.size(); ++idx)
    {
      std::cout << "  " << parser->headers[idx] << std::endl;
    }
    return 1;
  }

  return -1;
}
