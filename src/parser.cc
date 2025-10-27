#include <fstream>
#include <sstream>
#include <iostream>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include "parser.hh"
#include "service.hh" 

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

/////////////////////////////////////////////////////////////////////////////////////////////////////
// load_simple_file
// load simple time,latitude,longitude,zip format
/////////////////////////////////////////////////////////////////////////////////////////////////////

int csv_parser::load_simple_file()
{
  std::ifstream file(file_path1);
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
      // store headers: time,latitude,longitude,zip
      headers = row;
      first_line = 0;
    }
    else
    {
      //parse simple format: time,latitude,longitude,zip
      std::string time_str = row[0];
      std::string lat = row[1];
      std::string lon = row[2];

      try
      {
        double lat_ = std::stod(lat);
        double lon_ = std::stod(lon);
        latitude.push_back(lat);
        longitude.push_back(lon);
      }
      catch (const std::exception& e)
      {
      }

      if (first_line) first_line = 0;
    }
  }

  file.close();
  return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// write_to_database
/////////////////////////////////////////////////////////////////////////////////////////////////////

int csv_parser::write_to_database(const std::string& db_path)
{
  try
  {
    std::unique_ptr<Wt::Dbo::backend::Sqlite3> sqlite3 = std::make_unique<Wt::Dbo::backend::Sqlite3>(db_path);

    Wt::Dbo::Session session;
    session.setConnection(std::move(sqlite3));
    session.mapClass<ServiceRequest>("service_requests");
    session.createTables();
    Wt::Dbo::Transaction transaction(session);

    for (std::vector<std::vector<std::string>>::const_iterator it = data.begin(); it != data.end(); ++it)
    {
      const std::vector<std::string>& row = *it;
      std::unique_ptr<ServiceRequest> record = std::make_unique<ServiceRequest>();
      record->X = row[0];
      record->Y = row[1];
      record->SERVICEREQUESTID = row[2];
      record->STREETADDRESS = row[3];
      record->CITY = row[4];
      record->STATE = row[5];
      record->ZIPCODE = row[6];
      record->WARD = row[7];
      record->SERVICECODE = row[8];
      record->SERVICECODEDESCRIPTION = row[9];
      record->SERVICETYPECODEDESCRIPTION = row[10];
      record->ORGANIZATIONACRONYM = row[11];
      record->SERVICECALLCOUNT = row[12];
      record->ADDDATE = row[13];
      record->RESOLUTIONDATE = row[14];
      record->SERVICEDUEDATE = row[15];
      record->SERVICEORDERDATE = row[16];
      record->STATUS_CODE = row[17];
      record->SERVICEORDERSTATUS = row[18];
      record->INSPECTIONFLAG = row[19];
      record->INSPECTIONDATE = row[20];
      record->INSPECTORNAME = row[21];
      record->PRIORITY = row[22];
      record->DETAILS = row[23];
      record->XCOORD = row[24];
      record->YCOORD = row[25];
      record->LATITUDE = row[26];
      record->LONGITUDE = row[27];
      record->MARADDRESSREPOSITORYID = row[28];
      record->GIS_ID = row[29];
      record->GLOBALID = row[30];
      record->CREATED = row[31];
      record->EDITED = row[32];
      record->GDB_FROM_DATE = row[33];
      record->GDB_TO_DATE = row[34];
      record->GDB_ARCHIVE_OID = row[35];
      record->SE_ANNO_CAD_DATA = row[36];
      record->OBJECTID = row[37];

      session.add(std::move(record));
    }

    transaction.commit();
    return 1;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
