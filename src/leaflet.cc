#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLeafletMap.h>
#include <Wt/WPushButton.h>
#include <Wt/WServer.h>
#include <Wt/WSpinBox.h>
#include <Wt/Json/Object.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include "parser.hh"

csv_parser* parser = nullptr;
std::string geojson_wards;

int load_dc311_simple();
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
  Wt::WLeafletMap* map;
  void addCirclesFromCSV();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ApplicationMap
/////////////////////////////////////////////////////////////////////////////////////////////////////

ApplicationMap::ApplicationMap(const Wt::WEnvironment& env)
  : WApplication(env),
  map(root()->addNew<Wt::WLeafletMap>())
{
  map->resize(1920, 1080);
  map->setZoomLevel(12);

  Wt::Json::Object options;
  options["maxZoom"] = 19;
  options["minZoom"] = 10;
  options["attribution"] = "&copy; <a href=\"https://www.openstreetmap.org/copyright\">OpenStreetMap</a> contributors &copy; <a href=\"https://carto.com/attributions\">CARTO</a>";
  map->addTileLayer("https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png", options);

  if (!geojson_wards.empty())
  {
    std::string js = "setTimeout(function() {";
    js += "L.geoJSON(" + geojson_wards + ", {";
    js += "style: {";
    js += "color: '#3388ff',";
    js += "weight: 2,";
    js += "opacity: 0.65,";
    js += "fillOpacity: 0.1";
    js += "}";
    js += "}).addTo(" + map->mapJsRef() + ");";
    js += "}, 100);";

    map->doJavaScript(js);
  }

  if (!parser->latitude.empty())
  {
    std::string js = "setTimeout(function() {";
    for (size_t idx = 0; idx < parser->latitude.size(); ++idx)
    {
      std::string lat = parser->latitude[idx];
      std::string lon = parser->longitude[idx];
      js += "L.circle([" + lat + ", " + lon + "], {";
      js += "stroke: false,";
      js += "fillColor: '#ff6b6b',";
      js += "fillOpacity: 0.5,";
      js += "radius: 50";
      js += "}).addTo(" + map->mapJsRef() + ");";
    }
    js += "}, 200);";
    map->doJavaScript(js);
  }

  map->panTo(Wt::WLeafletMap::Coordinate(38.85, -76.95));
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

  return Wt::WRun(argc, argv, &create_application);
}
