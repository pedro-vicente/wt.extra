#include "parser.hh"
#include <chrono>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  csv_parser parser("311_city_service_requests_2024_part1.csv", "311_city_service_requests_2024_part2.csv");

  std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

  if (parser.load_file())
  {
    std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "File loaded in: " << duration.count() << " ms" << std::endl;
    std::cout << "Rows: " << parser.data.size() << std::endl;
    std::cout << "Columns: " << parser.headers.size() << std::endl;

    std::cout << "Headers:" << std::endl;
    for (size_t idx = 0; idx < parser.headers.size(); ++idx)
    {
      std::cout << "  " << parser.headers[idx] << std::endl;
    }

    start_time = std::chrono::high_resolution_clock::now();

    if (parser.write_to_database("dc311.db") > 0)
    {
      end_time = std::chrono::high_resolution_clock::now();
      duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
      std::cout << "Database write completed in: " << duration.count() << " ms" << std::endl;
    }
  }

  return 0;
}