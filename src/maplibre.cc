#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WCheckBox.h>
#include <Wt/WBreak.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include <vector>
#include <string>
#include <map>
#include "WMaplibre.hh"
#include "service.hh"
#include "map.hh"

std::string geojson_wards;
std::string database_path = "dc311.db";

/////////////////////////////////////////////////////////////////////////////////////////////////////
// database
/////////////////////////////////////////////////////////////////////////////////////////////////////

int load_service_requests(
  const std::string& db_path,
  std::vector<Coordinate>& coordinates,
  const std::string& service_filter);

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
// load
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ApplicationMap::load()
{
  std::vector<Coordinate> all_coords;

  for (const auto& pair : service_checkboxes)
  {
    if (pair.second->isChecked())
    {
      std::vector<Coordinate> coords;
      if (load_service_requests(database_path, coords, pair.first) > 0)
      {
        all_coords.insert(all_coords.end(), coords.begin(), coords.end());
      }
    }
  }

  if (!all_coords.empty())
  {
    map->coordinates = all_coords;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// update
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ApplicationMap::update()
{
  map_container->clear();
  std::unique_ptr<Wt::WMapLibre> m = std::make_unique<Wt::WMapLibre>();

  if (!geojson_wards.empty())
  {
    m->geojson = geojson_wards;
  }

  std::vector<Coordinate> filtered_coords;

  for (const auto& pair : service_checkboxes)
  {
    if (pair.second->isChecked())
    {
      std::vector<Coordinate> coords;
      if (load_service_requests(database_path, coords, pair.first) > 0)
      {
        filtered_coords.insert(filtered_coords.end(), coords.begin(), coords.end());
      }
    }
  }

  if (!filtered_coords.empty())
  {
    m->coordinates = filtered_coords;
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
  geojson_wards = load_geojson("ward-2012.geojson");
  return Wt::WRun(argc, argv, &create_application);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// load_service_requests
/////////////////////////////////////////////////////////////////////////////////////////////////////

int load_service_requests(
  const std::string& db_path,
  std::vector<Coordinate>& coordinates,
  const std::string& service_filter)
{
  try
  {
    auto sqlite3 = std::make_unique<Wt::Dbo::backend::Sqlite3>(db_path);
    Wt::Dbo::Session session;
    session.setConnection(std::move(sqlite3));

    coordinates.clear();

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
      std::string escaped_filter = service_filter;
      size_t pos = 0;
      while ((pos = escaped_filter.find("'", pos)) != std::string::npos)
      {
        escaped_filter.replace(pos, 1, "''");
        pos += 2;
      }

      sql = "SELECT LATITUDE, LONGITUDE FROM service_requests "
        "WHERE SERVICECODEDESCRIPTION LIKE '%" + escaped_filter + "%' "
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
        coordinates.push_back(Coordinate(lat, lon));
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
