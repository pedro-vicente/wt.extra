#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WCompositeWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WCheckBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WBreak.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include "map.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// globals
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string geojson_wards;
int load_service_requests(std::vector<MapCoordinate>& coordinates, const std::string& service);

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

  for (size_t idx = 0; idx < services.size(); ++idx)
  {
    const std::string& service = services[idx];
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

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // selection controls
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  layout_sidebar->addWidget(std::make_unique<Wt::WBreak>());
  layout_sidebar->addWidget(std::make_unique<Wt::WText>("<h4>Selection</h4>"));
  layout_sidebar->addWidget(std::make_unique<Wt::WText>(
    "<p style='font-size:11px;'>Hold <strong>Shift</strong> and drag on map to select area</p>"));

  Wt::WPushButton* clear_btn = layout_sidebar->addWidget(
    std::make_unique<Wt::WPushButton>("Clear Selection")
  );
  clear_btn->clicked().connect([=]() {
    WApplication::instance()->doJavaScript("if (window.clearSelection) window.clearSelection();");
    });

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
  map->coordinates.clear();
  map->coordinates.reserve(5000);

  std::map<std::string, Wt::WCheckBox*>::iterator it;
  for (it = service_checkboxes.begin(); it != service_checkboxes.end(); ++it)
  {
    if (it->second->isChecked())
    {
      std::vector<MapCoordinate> coords;
      if (load_service_requests(coords, it->first) > 0)
      {
        map->coordinates.insert(map->coordinates.end(), coords.begin(), coords.end());
      }
    }
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

  m->coordinates.reserve(5000);

  std::map<std::string, Wt::WCheckBox*>::iterator it;
  for (it = service_checkboxes.begin(); it != service_checkboxes.end(); ++it)
  {
    if (it->second->isChecked())
    {
      std::vector<MapCoordinate> coords;
      if (load_service_requests(coords, it->first) > 0)
      {
        m->coordinates.insert(m->coordinates.end(), coords.begin(), coords.end());
      }
    }
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

int load_service_requests(std::vector<MapCoordinate>& coordinates, const std::string& service)
{
  try
  {
    std::unique_ptr<Wt::Dbo::backend::Sqlite3> sqlite3 = std::make_unique<Wt::Dbo::backend::Sqlite3>("dc311.db");
    Wt::Dbo::Session session;
    session.setConnection(std::move(sqlite3));

    Wt::Dbo::Transaction transaction(session);
    std::string sql = "SELECT LATITUDE, LONGITUDE, STREETADDRESS, ADDDATE FROM service_requests "
      "WHERE SERVICECODEDESCRIPTION LIKE '%" + service + "%' "
      "AND LATITUDE IS NOT NULL AND LONGITUDE IS NOT NULL "
      "AND LATITUDE != '' AND LONGITUDE != ''";

    Wt::Dbo::collection<std::tuple<std::string, std::string, std::string, std::string>> results = session.query<std::tuple<std::string, std::string, std::string, std::string>>(sql).resultList();

    typedef Wt::Dbo::collection<std::tuple<std::string, std::string, std::string, std::string>>::const_iterator ResultIterator;
    for (ResultIterator row_it = results.begin(); row_it != results.end(); ++row_it)
    {
      const std::tuple<std::string, std::string, std::string, std::string>& row = *row_it;
      std::string lat = std::get<0>(row);
      std::string lon = std::get<1>(row);
      std::string address = std::get<2>(row);
      std::string date = std::get<3>(row);
      try
      {
        double lat_d = std::stod(lat);
        double lon_d = std::stod(lon);
        coordinates.push_back(MapCoordinate(lat, lon, service, address, date));
      }
      catch (...)
      {
      }
    }

    transaction.commit();
    std::cout << service << ": " << coordinates.size() << " coordinates." << std::endl;
    return 1;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
