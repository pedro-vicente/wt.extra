#ifndef MAP_HH
#define MAP_HH

#include <Wt/WCompositeWidget.h>
#include <Wt/WWebWidget.h>
#include <Wt/WApplication.h>
#include <string>
#include <vector>
#include <map>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// MapCoordinate (selected fields from database for map rendering)
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct MapCoordinate
{
  std::string LATITUDE;
  std::string LONGITUDE;
  std::string SERVICECODEDESCRIPTION;
  std::string STREETADDRESS;
  std::string ADDDATE;
  MapCoordinate(const std::string& latitude, const std::string& longitude, const std::string& service,
    const std::string& address, const std::string& date)
    : LATITUDE(latitude), LONGITUDE(longitude), SERVICECODEDESCRIPTION(service), STREETADDRESS(address), ADDDATE(date) {
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// utility functions
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string to_hex(int n);
std::string rgb_to_hex(int r, int g, int b);
std::string load_geojson(const std::string& name);
std::string escape_js_string(const std::string& input);

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
    std::vector<MapCoordinate> coordinates;
    std::map<std::string, std::string> colors;

  protected:
    Impl* impl;
    virtual void render(WFlags<RenderFlag> flags) override;
  };
}

#endif