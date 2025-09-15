#ifndef PARSER_HH
#define PARSER_HH

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// csv_parser
/////////////////////////////////////////////////////////////////////////////////////////////////////

class csv_parser
{
public:
  csv_parser(const std::string& file_path, char delimiter = ',');
  csv_parser(const std::string& file_path1, const std::string& file_path2, char delimiter = ',');

  int load_file();
  std::vector<std::string> parse_line(const std::string& line);
  std::string trim_whitespace(const std::string& str);
  int load_single_file(const std::string& file_path, int append_data = 0);
  

  std::string file_path1;
  std::string file_path2;
  char delimiter;
  int has_headers;
  int dual_file_mode;
  std::vector<std::string> headers;
  std::vector<std::vector<std::string>> data;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // DC311 simplified format
  // time,latitude,longitude,zip
  // 2016-10-29T15:09:22Z,38.91374182,-77.00176208,20002,
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  int load_simple_file();
  std::vector<std::string> latitude;
  std::vector<std::string> longitude;
 
};

#endif