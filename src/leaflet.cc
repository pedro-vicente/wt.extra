#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WCheckBox.h>
#include <Wt/WBreak.h>
#include <vector>
#include <string>
#include "WLeaflet.hh"
#include "parser.hh"
#include "map.hh"

csv_parser* parser = nullptr;
std::string geojson_wards;

int load_dc311_simple();

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
  Wt::WLeaflet* map;
  Wt::WContainerWidget* map_container;
  Wt::WCheckBox* rodent_checkbox;
  void onCheckBoxChanged();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ApplicationMap
/////////////////////////////////////////////////////////////////////////////////////////////////////

ApplicationMap::ApplicationMap(const Wt::WEnvironment& env)
  : WApplication(env), map(nullptr), map_container(nullptr), rodent_checkbox(nullptr)
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
  sidebar->setWidth(150);
  std::unique_ptr<Wt::WVBoxLayout> layout_sidebar = std::make_unique<Wt::WVBoxLayout>();
  layout_sidebar->addWidget(std::make_unique<Wt::WText>("<h4>DC 311 Service Types</h4>"));
  for (size_t idx = 0; idx < services.size(); ++idx)
  {
    const std::string& service = services[idx];
    Wt::WCheckBox* check_box = layout_sidebar->addWidget(std::make_unique<Wt::WCheckBox>(service));

    if (service != "Rodent Inspection")
    {
      check_box->setDisabled(true);
      check_box->setChecked(false);
    }
    else
    {
      check_box->setDisabled(false);
      check_box->setChecked(true);
      rodent_checkbox = check_box;
    }
    check_box->changed().connect(this, &ApplicationMap::onCheckBoxChanged);
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
  map = container_map->addWidget(std::make_unique<Wt::WLeaflet>());
  map->resize(Wt::WLength::Auto, Wt::WLength::Auto);

  if (!geojson_wards.empty())
  {
    map->geojson = geojson_wards;
  }

  if (!parser->coordinates.empty())
  {
    map->coordinates = parser->coordinates;
  }

  layout->addWidget(std::move(container_map), 1);
  root()->setLayout(std::move(layout));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// onCheckBoxChanged
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ApplicationMap::onCheckBoxChanged()
{
  map_container->clear();
  std::unique_ptr<Wt::WLeaflet> m = std::make_unique<Wt::WLeaflet>();
  if (!geojson_wards.empty())
  {
    m->geojson = geojson_wards;
  }
  if (rodent_checkbox->isChecked())
  {
    if (!parser->coordinates.empty())
    {
      m->coordinates = parser->coordinates;
    }
  }
  map = map_container->addWidget(std::move(m));
  map->resize(Wt::WLength::Auto, Wt::WLength::Auto);
  triggerUpdate();
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
  if (load_dc311_simple() < 0)
  {
  }

  return Wt::WRun(argc, argv, &create_application);
}
