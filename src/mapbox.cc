#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include "WMapbox.hh"
#include "parser.hh"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

csv_parser* parser = nullptr;
std::string geojson;

int load_dc311_simple();
int load_dc311_full();
int load_geojson();

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ApplicationMap
/////////////////////////////////////////////////////////////////////////////////////////////////////

class ApplicationMap : public Wt::WApplication
{
public:
  ApplicationMap(const Wt::WEnvironment& env);
  virtual ~ApplicationMap();

private:
  Wt::WMapbox* map;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ApplicationMap
/////////////////////////////////////////////////////////////////////////////////////////////////////

ApplicationMap::ApplicationMap(const Wt::WEnvironment& env)
  : WApplication(env), map(nullptr)
{
  setTitle("DC 311 Service Requests Map");

  map = root()->addWidget(std::make_unique<Wt::WMapbox>());
  map->resize(1920, 1080);
 
  if (!geojson.empty()) 
  {
    map->geojson = geojson;
  }

  if (parser && !parser->latitude.empty())
  {
    map->latitude = parser->latitude;
    map->longitude = parser->longitude;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ~ApplicationMap
/////////////////////////////////////////////////////////////////////////////////////////////////////

ApplicationMap::~ApplicationMap()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// create_application
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<Wt::WApplication> create_application(const Wt::WEnvironment& env)
{
  return std::make_unique<ApplicationMap>(env);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  std::cout << "Loading data files..." << std::endl;

  if (load_geojson() < 0)
  {  
  }

  if (load_dc311_simple() < 0)
  { 
  }

  int result = Wt::WRun(argc, argv, &create_application);

  delete parser;
  parser = nullptr;
  return result;
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
  geojson.clear();
  while (std::getline(file, line))
  {
    geojson += line;
  }
  file.close();

  if (!geojson.empty())
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


