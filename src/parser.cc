#include <fstream>
#include <sstream>
#include <iostream>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Sqlite3.h>
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

    // remove UTF-8 BOM (EF BB BF) from first line if present
    if (first_line && line.length() >= 3)
    {
      if ((unsigned char)line[0] == 0xEF &&
        (unsigned char)line[1] == 0xBB &&
        (unsigned char)line[2] == 0xBF)
      {
        line = line.substr(3);
      }
    }

    std::vector<std::string> row = parse_line(line);

    if (first_line && has_headers)
    {
      if (!append_data)
      {
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

    // remove UTF-8 BOM (EF BB BF) from first line if present
    if (first_line && line.length() >= 3)
    {
      if ((unsigned char)line[0] == 0xEF &&
        (unsigned char)line[1] == 0xBB &&
        (unsigned char)line[2] == 0xBF)
      {
        line = line.substr(3);
      }
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
    auto sqlite3 = std::make_unique<Wt::Dbo::backend::Sqlite3>(db_path);
    Wt::Dbo::Session session;
    session.setConnection(std::move(sqlite3));
    
    Wt::Dbo::Transaction transaction(session);

    session.execute(
      "CREATE TABLE IF NOT EXISTS service_requests ("
      "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "  X TEXT,"
      "  Y TEXT,"
      "  SERVICEREQUESTID TEXT,"
      "  STREETADDRESS TEXT,"
      "  CITY TEXT,"
      "  STATE TEXT,"
      "  ZIPCODE TEXT,"
      "  WARD TEXT,"
      "  SERVICECODE TEXT,"
      "  SERVICECODEDESCRIPTION TEXT,"
      "  SERVICETYPECODEDESCRIPTION TEXT,"
      "  ORGANIZATIONACRONYM TEXT,"
      "  SERVICECALLCOUNT TEXT,"
      "  ADDDATE TEXT,"
      "  RESOLUTIONDATE TEXT,"
      "  SERVICEDUEDATE TEXT,"
      "  SERVICEORDERDATE TEXT,"
      "  STATUS_CODE TEXT,"
      "  SERVICEORDERSTATUS TEXT,"
      "  INSPECTIONFLAG TEXT,"
      "  INSPECTIONDATE TEXT,"
      "  INSPECTORNAME TEXT,"
      "  PRIORITY TEXT,"
      "  DETAILS TEXT,"
      "  XCOORD TEXT,"
      "  YCOORD TEXT,"
      "  LATITUDE TEXT,"
      "  LONGITUDE TEXT,"
      "  MARADDRESSREPOSITORYID TEXT,"
      "  GIS_ID TEXT,"
      "  GLOBALID TEXT,"
      "  CREATED TEXT,"
      "  EDITED TEXT,"
      "  GDB_FROM_DATE TEXT,"
      "  GDB_TO_DATE TEXT,"
      "  GDB_ARCHIVE_OID TEXT,"
      "  SE_ANNO_CAD_DATA TEXT,"
      "  OBJECTID TEXT"
      ")");

    session.execute("CREATE INDEX IF NOT EXISTS idx_service_type ON service_requests(SERVICECODEDESCRIPTION)");
    session.execute("CREATE INDEX IF NOT EXISTS idx_ward ON service_requests(WARD)");
    session.execute("CREATE INDEX IF NOT EXISTS idx_lat_lon ON service_requests(LATITUDE, LONGITUDE)");

    int count = 0;
    for (const auto& row : data)
    {
      std::vector<std::string> val;
      for (const auto& value : row)
      {
        std::string escaped = value;
        size_t pos = 0;
        while ((pos = escaped.find("'", pos)) != std::string::npos)
        {
          escaped.replace(pos, 1, "''");
          pos += 2;
        }
        val.push_back(escaped);
      }

      std::string sql =
        "INSERT INTO service_requests ("
        "  X, Y, SERVICEREQUESTID, STREETADDRESS, CITY, STATE, ZIPCODE, WARD,"
        "  SERVICECODE, SERVICECODEDESCRIPTION, SERVICETYPECODEDESCRIPTION,"
        "  ORGANIZATIONACRONYM, SERVICECALLCOUNT, ADDDATE, RESOLUTIONDATE,"
        "  SERVICEDUEDATE, SERVICEORDERDATE, STATUS_CODE, SERVICEORDERSTATUS,"
        "  INSPECTIONFLAG, INSPECTIONDATE, INSPECTORNAME, PRIORITY, DETAILS,"
        "  XCOORD, YCOORD, LATITUDE, LONGITUDE, MARADDRESSREPOSITORYID,"
        "  GIS_ID, GLOBALID, CREATED, EDITED, GDB_FROM_DATE, GDB_TO_DATE,"
        "  GDB_ARCHIVE_OID, SE_ANNO_CAD_DATA, OBJECTID"
        ") VALUES ("
        "  '" + val[0] + "', '" + val[1] + "', '" + val[2] + "', "
        "  '" + val[3] + "', '" + val[4] + "', '" + val[5] + "', "
        "  '" + val[6] + "', '" + val[7] + "', '" + val[8] + "', "
        "  '" + val[9] + "', '" + val[10] + "', '" + val[11] + "', "
        "  '" + val[12] + "', '" + val[13] + "', '" + val[14] + "', "
        "  '" + val[15] + "', '" + val[16] + "', '" + val[17] + "', "
        "  '" + val[18] + "', '" + val[19] + "', '" + val[20] + "', "
        "  '" + val[21] + "', '" + val[22] + "', '" + val[23] + "', "
        "  '" + val[24] + "', '" + val[25] + "', '" + val[26] + "', "
        "  '" + val[27] + "', '" + val[28] + "', '" + val[29] + "', "
        "  '" + val[30] + "', '" + val[31] + "', '" + val[32] + "', "
        "  '" + val[33] + "', '" + val[34] + "', '" + val[35] + "', "
        "  '" + val[36] + "', '" + val[37] + "'"
        ")";

      session.execute(sql);

      count++;
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

/////////////////////////////////////////////////////////////////////////////////////////////////////
// load_service_requests
/////////////////////////////////////////////////////////////////////////////////////////////////////

int load_service_requests(
  const std::string& db_path,
  std::vector<std::string>& latitude,
  std::vector<std::string>& longitude,
  const std::string& service_filter)
{
  try
  {
    auto sqlite3 = std::make_unique<Wt::Dbo::backend::Sqlite3>(db_path);
    Wt::Dbo::Session session;
    session.setConnection(std::move(sqlite3));

    latitude.clear();
    longitude.clear();

    Wt::Dbo::Transaction transaction(session);

    std::string sql;

    if (service_filter.empty())
    {
      sql = "SELECT LATITUDE, LONGITUDE FROM service_requests "
        "WHERE LATITUDE IS NOT NULL AND LONGITUDE IS NOT NULL "
        "AND LATITUDE != '' AND LONGITUDE != ''";
    }
    else
    {
      std::string filter = service_filter;
      size_t pos = 0;
      while ((pos = filter.find("'", pos)) != std::string::npos)
      {
        filter.replace(pos, 1, "''");
        pos += 2;
      }

      sql = "SELECT LATITUDE, LONGITUDE FROM service_requests "
        "WHERE SERVICECODEDESCRIPTION LIKE '%" + filter + "%' "
        "AND LATITUDE IS NOT NULL AND LONGITUDE IS NOT NULL "
        "AND LATITUDE != '' AND LONGITUDE != ''";
    }

    auto results = session.query<std::tuple<std::string, std::string>>(sql).resultList();

    for (const auto& row : results)
    {
      std::string lat = std::get<0>(row);
      std::string lon = std::get<1>(row);

      try
      {
        double lat_d = std::stod(lat);
        double lon_d = std::stod(lon);
        latitude.push_back(lat);
        longitude.push_back(lon);
      }
      catch (...)
      {

      }
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

int load_service_requests(
  const std::string& db_path,
  std::vector<std::string>& latitude,
  std::vector<std::string>& longitude)
{
  return load_service_requests(db_path, latitude, longitude, "");
}
