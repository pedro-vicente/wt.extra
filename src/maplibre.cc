#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WCheckBox.h>
#include <Wt/WBreak.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "WMaplibre.hh"
#include "parser.hh"

csv_parser* parser = nullptr;
std::string geojson_wards;
std::string database_path = "dc311.db";

int load_geojson();

// DC 311 Service types based on SERVICECODEDESCRIPTION
std::vector<std::string> services = {
    "Abandoned Vehicle",
    "Bulk Collection",
    "Illegal Dumping",
    "Pothole",
    "Streetlight Repair",
    "Tree Inspection",
    "Parking Enforcement",
    "Graffiti Removal",
    "Trash Collection",
    "Rodent Inspection"
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ApplicationMap
/////////////////////////////////////////////////////////////////////////////////////////////////////

class ApplicationMap : public Wt::WApplication
{
public:
  ApplicationMap(const Wt::WEnvironment& env);
  virtual ~ApplicationMap();

private:
  Wt::WMapLibre* map;
  Wt::WContainerWidget* map_container;
  std::map<std::string, Wt::WCheckBox*> service_checkboxes;
  
  void onCheckBoxChanged(const std::string& service_type);
  void load();
  void update();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ApplicationMap
/////////////////////////////////////////////////////////////////////////////////////////////////////

ApplicationMap::ApplicationMap(const Wt::WEnvironment& env)
  : WApplication(env), map(nullptr), map_container(nullptr)
{
  setTitle("DC 311 Service Requests Map");

  std::unique_ptr<Wt::WHBoxLayout> layout = std::make_unique<Wt::WHBoxLayout>();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // menu sidebar
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::unique_ptr<Wt::WContainerWidget> sidebar = std::make_unique<Wt::WContainerWidget>();
  sidebar->setPadding(10);
  sidebar->setWidth(200);
  std::unique_ptr<Wt::WVBoxLayout> layout_sidebar = std::make_unique<Wt::WVBoxLayout>();
  layout_sidebar->addWidget(std::make_unique<Wt::WText>("<h4>DC 311 Service Types</h4>"));
  
  for (const std::string& service : services)
  {
    Wt::WCheckBox* check_box = layout_sidebar->addWidget(std::make_unique<Wt::WCheckBox>(service));

    if (service == "Rodent Inspection")
    {
      check_box->setChecked(true);
    }
    else
    {
      check_box->setChecked(false);
    }
    
    service_checkboxes[service] = check_box;
    
    check_box->changed().connect([this, service]() {
      onCheckBoxChanged(service);
    });
    
    layout_sidebar->addWidget(std::make_unique<Wt::WBreak>());
  }
  
  layout_sidebar->addStretch(1);
  sidebar->setLayout(std::move(layout_sidebar));
  layout->addWidget(std::move(sidebar), 0);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // map
  // stretch factor 1 (fills remaining space).
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::unique_ptr<Wt::WContainerWidget> container_map = std::make_unique<Wt::WContainerWidget>();
  map_container = container_map.get();
  map = container_map->addWidget(std::make_unique<Wt::WMapLibre>());
  map->resize(Wt::WLength::Auto, Wt::WLength::Auto);

  if (!geojson_wards.empty())
  {
    map->geojson = geojson_wards;
  }

  load();

  layout->addWidget(std::move(container_map), 1);
  root()->setLayout(std::move(layout));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// loadInitialData
// Load initial data filtered by checked services from database
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ApplicationMap::load()
{
  std::vector<std::string> all_lat, all_lon;
  
  for (const auto& pair : service_checkboxes)
  {
    if (pair.second->isChecked())
    {
      std::vector<std::string> lat, lon;
      if (load_service_requests(database_path, lat, lon, pair.first) > 0)
      {
        all_lat.insert(all_lat.end(), lat.begin(), lat.end());
        all_lon.insert(all_lon.end(), lon.begin(), lon.end());
      }
    }
  }
  
  if (!all_lat.empty())
  {
    map->latitude = all_lat;
    map->longitude = all_lon;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// updateMapWithFilters
// Recreate map with current filter selections from database
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ApplicationMap::update()
{
  map_container->clear();
  std::unique_ptr<Wt::WMapLibre> m = std::make_unique<Wt::WMapLibre>();
  
  if (!geojson_wards.empty())
  {
    m->geojson = geojson_wards;
  }
  
  std::vector<std::string> filtered_lat, filtered_lon;
  for (const auto& pair : service_checkboxes)
  {
    if (pair.second->isChecked())
    {
      std::vector<std::string> lat, lon;
      if (load_service_requests(database_path, lat, lon, pair.first) > 0)
      {
        filtered_lat.insert(filtered_lat.end(), lat.begin(), lat.end());
        filtered_lon.insert(filtered_lon.end(), lon.begin(), lon.end());
      }
    }
  }
  
  if (!filtered_lat.empty())
  {
    m->latitude = filtered_lat;
    m->longitude = filtered_lon;
  }
 
  map = map_container->addWidget(std::move(m));
  map->resize(Wt::WLength::Auto, Wt::WLength::Auto);
  triggerUpdate();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// onCheckBoxChanged
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ApplicationMap::onCheckBoxChanged(const std::string& service_type)
{
  update();
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
  if (load_geojson() < 0)
  {
  }
  return Wt::WRun(argc, argv, &create_application);
}
