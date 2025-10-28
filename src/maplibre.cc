#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WCompositeWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WCheckBox.h>
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
// Coordinate
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct Coordinate
{
  std::string latitude;
  std::string longitude;
  std::string service;
  Coordinate(const std::string& lat, const std::string& lon, const std::string& service)
    : latitude(lat), longitude(lon), service(service) {
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// globals
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string geojson_wards;
std::map<std::string, std::string> colors;
int load_service_requests(std::vector<Coordinate>& coordinates, const std::string& service);

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

std::vector<std::string> ward_color =
{ rgb_to_hex(128, 128, 0), //olive
  rgb_to_hex(255, 255, 0), //yellow 
  rgb_to_hex(0, 128, 0), //green
  rgb_to_hex(0, 255, 0), //lime
  rgb_to_hex(0, 128, 128), //teal
  rgb_to_hex(0, 255, 255), //aqua
  rgb_to_hex(0, 0, 255), //blue
  rgb_to_hex(128, 0, 128) //purple
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// WMapLibre
/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Wt
{
  class WT_API WMapLibre : public WCompositeWidget
  {
    class Impl;
  public:
    WMapLibre();
    ~WMapLibre();

    std::string geojson;
    std::vector<Coordinate> coordinates;

  protected:
    Impl* impl;
    virtual void render(WFlags<RenderFlag> flags) override;
  };
}

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
      std::vector<Coordinate> coords;
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
      std::vector<Coordinate> coords;
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
  colors["Abandoned Vehicle"] = "#8B4513";      // Brown 
  colors["Bulk Collection"] = "#00FF00";        // Green
  colors["Illegal Dumping"] = "#0000FF";        // Blue
  colors["Pothole"] = "#FFFF00";                // Yellow
  colors["Streetlight Repair"] = "#FF00FF";     // Magenta
  colors["Tree Inspection"] = "#00FFFF";        // Cyan
  colors["Parking Enforcement"] = "#FFA500";    // Orange
  colors["Graffiti Removal"] = "#800080";       // Purple
  colors["Trash Collection"] = "#FFC0CB";       // Pink
  colors["Rodent Inspection"] = "#FF0000";      // Red 

  geojson_wards = load_geojson("ward-2012.geojson");
  return Wt::WRun(argc, argv, &create_application);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// load_service_requests
/////////////////////////////////////////////////////////////////////////////////////////////////////

int load_service_requests(std::vector<Coordinate>& coordinates, const std::string& service)
{
  try
  {
    std::unique_ptr<Wt::Dbo::backend::Sqlite3> sqlite3 = std::make_unique<Wt::Dbo::backend::Sqlite3>("dc311.db");
    Wt::Dbo::Session session;
    session.setConnection(std::move(sqlite3));

    Wt::Dbo::Transaction transaction(session);
    std::string sql = "SELECT LATITUDE, LONGITUDE FROM service_requests "
      "WHERE SERVICECODEDESCRIPTION LIKE '%" + service + "%' "
      "AND LATITUDE IS NOT NULL AND LONGITUDE IS NOT NULL "
      "AND LATITUDE != '' AND LONGITUDE != ''";

    Wt::Dbo::collection<std::tuple<std::string, std::string>> results = session.query<std::tuple<std::string, std::string>>(sql).resultList();

    typedef Wt::Dbo::collection<std::tuple<std::string, std::string>>::const_iterator ResultIterator;
    for (ResultIterator row_it = results.begin(); row_it != results.end(); ++row_it)
    {
      const std::tuple<std::string, std::string>& row = *row_it;
      std::string lat = std::get<0>(row);
      std::string lon = std::get<1>(row);
      try
      {
        double lat_d = std::stod(lat);
        double lon_d = std::stod(lon);
        coordinates.push_back(Coordinate(lat, lon, service));
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

/////////////////////////////////////////////////////////////////////////////////////////////////////
// WMapLibre
/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Wt
{
  class WMapLibre::Impl : public WWebWidget
  {
  public:
    Impl();
    virtual DomElementType domElementType() const override;
  };

  WMapLibre::Impl::Impl()
  {
    setInline(false);
  }

  DomElementType WMapLibre::Impl::domElementType() const
  {
    return DomElementType::DIV;
  }

  WMapLibre::WMapLibre()
  {
    setImplementation(std::unique_ptr<Impl>(impl = new Impl()));
    WApplication* app = WApplication::instance();
    this->addCssRule("body", "margin: 0; padding: 0;");
    this->addCssRule("#" + id(), "position: absolute; top: 0; bottom: 0; width: 100%;");
    app->useStyleSheet("https://unpkg.com/maplibre-gl@4.7.1/dist/maplibre-gl.css");
    const std::string library = "https://unpkg.com/maplibre-gl@4.7.1/dist/maplibre-gl.js";
    app->require(library, "maplibre");
  }

  WMapLibre::~WMapLibre()
  {
    coordinates.clear();
    geojson.clear();
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // render
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  void WMapLibre::render(WFlags<RenderFlag> flags)
  {
    WCompositeWidget::render(flags);

    if (flags.test(RenderFlag::Full))
    {
      std::stringstream js;

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // create map
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      js << "const map = new maplibregl.Map({\n"
        << "  container: " << jsRef() << ",\n"
        << "  style: 'https://basemaps.cartocdn.com/gl/positron-gl-style/style.json',\n"
        << "  center: [-77.0369, 38.9072],\n"
        << "  zoom: 12\n"
        << "});\n"

        << "map.addControl(new maplibregl.NavigationControl());\n";

#ifdef _WIN32
      OutputDebugStringA(js.str().c_str());
#endif

      js << "map.on('load', function() {\n";

      js << "var ward_color = [";
      for (size_t idx = 0; idx < ward_color.size(); ++idx)
      {
        js << "'" << ward_color[idx] << "'";
        if (idx < ward_color.size() - 1) js << ",";
      }
      js << "];\n";

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // add ward layer
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      js << "map.addSource('wards', {\n"
        << "  'type': 'geojson',\n"
        << "  'data': " << geojson << "\n"
        << "});\n"

        << "map.addLayer({\n"
        << "  'id': 'wards-fill',\n"
        << "  'type': 'fill',\n"
        << "  'source': 'wards',\n"
        << "  'paint': {\n"
        << "    'fill-color': ['get', ['to-string', ['get', 'WARD']], ['literal', {\n";

      for (size_t idx = 0; idx < ward_color.size(); ++idx)
      {
        js << "      '" << (idx + 1) << "': '" << ward_color[idx] << "'";
        if (idx < ward_color.size() - 1) js << ",\n";
      }

      js << "\n    }]],\n"
        << "    'fill-opacity': 0.2\n"
        << "  }\n"
        << "});\n";

#ifdef _WIN32
      OutputDebugStringA(js.str().c_str());
#endif

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // create separate layer for each service type
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      if (!coordinates.empty())
      {
        std::map<std::string, std::vector<Coordinate>> by_service;

        for (size_t idx = 0; idx < coordinates.size(); ++idx)
        {
          std::string service = coordinates[idx].service;
          by_service[service].push_back(coordinates[idx]);
        }

        std::map<std::string, std::vector<Coordinate>>::iterator service_it;
        for (service_it = by_service.begin(); service_it != by_service.end(); ++service_it)
        {
          const std::string& service = service_it->first;
          const std::vector<Coordinate>& coords = service_it->second;

          std::string color = "#ff0000";
          std::map<std::string, std::string>::iterator color_it = colors.find(service);
          if (color_it != colors.end())
          {
            color = color_it->second;
          }

          std::string service_ = service;
          for (size_t i = 0; i < service_.length(); ++i)
          {
            if (service_[i] == ' ') service_[i] = '_';
          }

          std::string source_id = "incidents-" + service_;
          std::string layer_id = "incidents-layer-" + service_;

          js << "var features_" << service_ << " = [];\n";

          for (size_t idx = 0; idx < coords.size(); ++idx)
          {
            std::string lat = coords[idx].latitude;
            std::string lon = coords[idx].longitude;

            if (!lat.empty() && !lon.empty())
            {
              js << "features_" << service_ << ".push({"
                << "'type': 'Feature',"
                << "'geometry': {"
                << "'type': 'Point',"
                << "'coordinates': [" << lon << ", " << lat << "]"
                << "},"
                << "'properties': {"
                << "'service': '" << service << "'"
                << "}"
                << "});\n";
            }
          }

          js << "map.addSource('" << source_id << "', {\n"
            << "  'type': 'geojson',\n"
            << "  'data': {\n"
            << "    'type': 'FeatureCollection',\n"
            << "    'features': features_" << service_ << "\n"
            << "  }\n"
            << "});\n";

          js << "map.addLayer({\n"
            << "  'id': '" << layer_id << "',\n"
            << "  'type': 'circle',\n"
            << "  'source': '" << source_id << "',\n"
            << "  'paint': {\n"
            << "    'circle-radius': [\n"
            << "      'interpolate', ['linear'], ['zoom'],\n"
            << "      10, 3,\n"
            << "      12, 5,\n"
            << "      14, 7,\n"
            << "      16, 9\n"
            << "    ],\n"
            << "    'circle-color': '" << color << "',\n"
            << "    'circle-opacity': 0.2\n"
            << "  }\n"
            << "});\n";

        }
      }

      //close map.on('load')
      js << "});\n";

      WApplication* app = WApplication::instance();
      app->doJavaScript(js.str());
    }
  }

}// namespace Wt
