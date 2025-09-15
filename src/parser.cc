#include "parser.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// csv_parser
/////////////////////////////////////////////////////////////////////////////////////////////////////

csv_parser::csv_parser(const std::string& file_path, char delimiter)
  : file_path1(file_path), delimiter(delimiter), has_headers(1), dual_file_mode(0)
{
}

csv_parser::csv_parser(const std::string& file_path1, const std::string& file_path2, char delimiter)
  : file_path1(file_path1), file_path2(file_path2), delimiter(delimiter), has_headers(1), dual_file_mode(1)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// load_file
/////////////////////////////////////////////////////////////////////////////////////////////////////

int csv_parser::load_file()
{
  data.clear();
  headers.clear();

  if (!load_single_file(file_path1, false))
  {
    return -1;
  }

  // load second file if in dual mode
  if (dual_file_mode && !file_path2.empty())
  {
    if (!load_single_file(file_path2, 1))
    {
      return -1;
    }
  }

  return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// load_single_file
/////////////////////////////////////////////////////////////////////////////////////////////////////

int csv_parser::load_single_file(const std::string& file_path, int append_data)
{
  std::ifstream file(file_path);
  if (!file.is_open())
  {
    return -1;
  }

  std::string line;
  int first_line = 1;

  while (std::getline(file, line))
  {
    if (line.empty())
    {
      continue;
    }

    std::vector<std::string> row = parse_line(line);

    if (first_line && has_headers)
    {
      if (!append_data)
      {
        // first file, store headers
        headers = row;
      }
      else
      {
      }
      first_line = 0;
    }
    else
    {
      data.push_back(row);
      if (first_line)
      {
        first_line = 0;
      }
    }
  }

  file.close();
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// parse_line
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> csv_parser::parse_line(const std::string& line)
{
  std::vector<std::string> result;
  std::string cell;
  int in_quotes = false;

  for (size_t i = 0; i < line.length(); ++i)
  {
    char c = line[i];

    if (c == '"')
    {
      in_quotes = !in_quotes;
    }
    else if (c == delimiter && !in_quotes)
    {
      result.push_back(trim_whitespace(cell));
      cell.clear();
    }
    else
    {
      cell += c;
    }
  }

  result.push_back(trim_whitespace(cell));
  return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// trim_whitespace
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string csv_parser::trim_whitespace(const std::string& str)
{
  size_t start = str.find_first_not_of(" \t\r\n");
  if (start == std::string::npos)
  {
    return "";
  }

  size_t end = str.find_last_not_of(" \t\r\n");
  return str.substr(start, end - start + 1);
}
