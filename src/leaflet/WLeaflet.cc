#include <Wt/WWebWidget.h>
#include <Wt/WApplication.h>
#include "WLeaflet.hh"
#include <sstream>
#include "map.hh"

#ifdef _WIN32
#include <windows.h>
#endif

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

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // Create map with Canvas renderer
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      js << "var map = L.map(" << jsRef() << ", {\n"
        << "  center: [38.90, -77.00],\n"
        << "  zoom: 13,\n"
        << "  preferCanvas: true,\n"
        << "  renderer: L.canvas({ padding: 0.5 })\n"
        << "});\n"

        << "L.tileLayer('https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png', {\n"
        << "  maxZoom: 19,\n"
        << "  minZoom: 10,\n"
        << "  attribution: '&copy OpenStreetMap contributors'\n"
        << "}).addTo(map);";

#ifdef _WIN32
      OutputDebugStringA(js.str().c_str());
#endif

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
        << "      fillColor: ward_color[ward_num - 1],"
        << "      fillOpacity: 0.2,"
        << "      stroke: false,"
        << "      weight: 0"
        << "     };"
        << "  },"
        << "  renderer: L.canvas()"
        << "}).addTo(map);";

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // incidents as circles with GeoJSON FeatureCollection
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      if (!coordinates.empty())
      {
        js << "var circles = [];";

        for (size_t idx = 0; idx < coordinates.size(); ++idx)
        {
          std::string lat = coordinates[idx].latitude;
          std::string lon = coordinates[idx].longitude;

          if (!lat.empty() && !lon.empty())
          {
            js << "circles.push({"
              << "  'type': 'Feature',"
              << "  'geometry': {"
              << "    'type': 'Point',"
              << "    'coordinates': [" << lon << ", " << lat << "]"
              << "  },"
              << "  'properties': {}"
              << "});";
          }
        }

        js << "var data = {"
          << "  'type': 'FeatureCollection',"
          << "  'features': circles"
          << "};"

          << "var style = {"
          << "  radius: 40,"
          << "  color: '#ff0000',"
          << "  fillOpacity: 0.4,"
          << "  stroke: false"
          << "};"

          << "var options = {"
          << "  pointToLayer: function(feature, latlng) {"
          << "    return L.circle(latlng, style);"
          << "  },"
          << "  renderer: L.canvas()"
          << "};"

          << "L.geoJSON(data, options).addTo(map);";
      }

      WApplication* app = WApplication::instance();
      app->doJavaScript(js.str());
    }


  } //WLeaflet
}// namespace Wt
