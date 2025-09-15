#include <string>
#include <fstream>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// csv_splitter
/////////////////////////////////////////////////////////////////////////////////////////////////////

class csv_splitter
{
public:
  csv_splitter(const std::string& input_file, const std::string& output_file1, const std::string& output_file2);
  int split(double percentage_for_file1);

  std::string input_file;
  std::string output_file1;
  std::string output_file2;

  size_t file1_rows;
  size_t file2_rows;
  size_t total_rows;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// csv_splitter
/////////////////////////////////////////////////////////////////////////////////////////////////////

csv_splitter::csv_splitter(const std::string& input_file, const std::string& output_file1, const std::string& output_file2)
  : input_file(input_file),
  output_file1(output_file1),
  output_file2(output_file2),
  file1_rows(0),
  file2_rows(0),
  total_rows(0)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// split
/////////////////////////////////////////////////////////////////////////////////////////////////////

int csv_splitter::split(double percentage_for_file1)
{
  file1_rows = 0;
  file2_rows = 0;
  total_rows = 0;

  std::ifstream count_file(input_file);
  if (!count_file.is_open())
  {
    return -1;
  }

  std::string line;
  size_t total_data_rows = 0;
  std::string header_line;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // read header, count rows 
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (std::getline(count_file, header_line))
  {
    while (std::getline(count_file, line))
    {
      if (!line.empty())
      {
        total_data_rows++;
      }
    }
  }
  count_file.close();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // calculate split point
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t split_point = static_cast<size_t>(total_data_rows * percentage_for_file1);

  std::ifstream input(input_file);
  std::ofstream output1(output_file1);
  std::ofstream output2(output_file2);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // write headers to both files
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (std::getline(input, line))
  {
    output1 << line << std::endl;
    output2 << line << std::endl;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // split data rows
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t current_row = 0;
  while (std::getline(input, line))
  {
    if (line.empty())
    {
      continue;
    }

    if (current_row < split_point)
    {
      output1 << line << std::endl;
      file1_rows++;
    }
    else
    {
      output2 << line << std::endl;
      file2_rows++;
    }

    current_row++;
    total_rows++;
  }

  return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  std::string input_file = "311_city_service_requests_2024.csv";
  std::string output_file1 = "311_city_service_requests_2024_part1.csv";
  std::string output_file2 = "311_city_service_requests_2024_part2.csv";
  double percentage_for_file1 = 0.5;
  csv_splitter splitter(input_file, output_file1, output_file2);
  if (splitter.split(percentage_for_file1) < 0)
  {
    return -1;
  }
  std::cout << output_file1 << " : " << splitter.file1_rows << " rows" << std::endl;
  std::cout << output_file2 << " : " << splitter.file2_rows << " rows" << std::endl;
  return 0;
}
