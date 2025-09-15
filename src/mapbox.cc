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
  void setup_map_with_data();
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
  setup_map_with_data();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ~ApplicationMap
/////////////////////////////////////////////////////////////////////////////////////////////////////

ApplicationMap::~ApplicationMap()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// setup_map_with_data
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ApplicationMap::setup_map_with_data()
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
  std::cout << "Loading CSV data ..." << std::endl;
  parser = new csv_parser("311_city_service_requests_2024_part1.csv", "311_city_service_requests_2024_part2.csv");

  auto start_time = std::chrono::high_resolution_clock::now();

  if (parser->load_file())
  {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "File loaded in: " << duration.count() << " ms" << std::endl;
    std::cout << "Combined rows: " << parser->data.size() << std::endl;
    std::cout << "Columns: " << parser->headers.size() << std::endl;

    std::cout << "Headers:" << std::endl;
    for (size_t idx = 0; idx < parser->headers.size(); ++idx)
    {
      std::cout << "  " << parser->headers[idx] << std::endl;
    }
  }

  int result = Wt::WRun(argc, argv, &create_application);

  delete parser;
  parser = nullptr;
  return result;
}