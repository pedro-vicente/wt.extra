#include "parser.hh"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>

extern csv_parser* parser;

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
    std::cout << "Rows: " << parser->coordinates.size() << std::endl;
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
