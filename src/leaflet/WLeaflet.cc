#include "WLeaflet.hh"
#include "web/Configuration.h"
#include <sstream>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//to_hex
//convert int to hex string, apply zero padding
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string to_hex(int n)
{
  std::stringstream ss;
  ss << std::hex << n;
  std::string str(ss.str());
  return str.size() == 1 ? "0" + str : str;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//rgb_to_hex
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string rgb_to_hex(int r, int g, int b)
{
  std::string str("#");
  str += to_hex(r);
  str += to_hex(g);
  str += to_hex(b);
  return str;
}

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

namespace Wt
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // WLeaflet::Impl
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  class WLeaflet::Impl : public WWebWidget
  {
  public:
    Impl();
    virtual DomElementType domElementType() const override;
  };

  WLeaflet::Impl::Impl()
  {
    setInline(false);
  }

  DomElementType WLeaflet::Impl::domElementType() const
  {
    return DomElementType::DIV;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // WLeaflet
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  WLeaflet::WLeaflet()
  {
    setImplementation(std::unique_ptr<Impl>(impl = new Impl()));
    WApplication* app = WApplication::instance();
    this->addCssRule("body", "margin: 0; padding: 0;");
    this->addCssRule("#" + id(), "position: absolute; top: 0; bottom: 0; width: 100%;");
    app->useStyleSheet("https://unpkg.com/leaflet@1.9.4/dist/leaflet.css");
    const std::string library = "https://unpkg.com/leaflet@1.9.4/dist/leaflet.js";
    app->require(library, "leaflet");
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // render
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::render(WFlags<RenderFlag> flags)
  {
    WCompositeWidget::render(flags);

    if (flags.test(RenderFlag::Full))
    {
      std::stringstream js;
      js << "var map = L.map(" << jsRef() << ", {"
        << "center: [38.85, -76.95],"
        << "zoom: 12"
        << "});"
        << "L.tileLayer('https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png', {"
        << "maxZoom: 19,"
        << "minZoom: 10,"
        << "attribution: '&copy OpenStreetMap contributors'"
        << "}).addTo(map);";

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // geoJSON with Wards
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      js << "var ward_color = [";
      for (size_t idx = 0; idx < ward_color.size(); ++idx)
      {
        js << "'" << ward_color[idx] << "'";
        if (idx < ward_color.size() - 1) js << ",";
      }
      js << "];";

      js << "L.geoJSON(" << geojson << ", {"
        << "style: function (feature) {"
        << "  var ward_num = feature.properties.WARD || 1;"
        << "    return {"
        << "      color: ward_color[ward_num - 1],"
        << "      stroke: false"
        << "     };"
        << "  }"
        << "}).addTo(map);";

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // incidents as circles
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      for (size_t idx = 0; idx < latitude.size() && idx < longitude.size(); ++idx)
      {
        std::string lat = latitude[idx];
        std::string lon = longitude[idx];

        if (!lat.empty() && !lon.empty())
        {
          js << "L.circle([" << lat << ", " << lon << "], {"
            << "stroke: false,"
            << "color: '#ff0000',"
            << "radius: 100"
            << "}).addTo(map);";
        }
      }


      WApplication* app = WApplication::instance();
      app->doJavaScript(js.str());
    }


  } //WLeaflet
}// namespace Wt
