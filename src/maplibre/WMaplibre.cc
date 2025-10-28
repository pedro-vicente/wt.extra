#include "WMaplibre.hh"
#include "web/Configuration.h"
#include "map.hh"

#include <sstream>

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
  // WMapLibre::Impl
  /////////////////////////////////////////////////////////////////////////////////////////////////////

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

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // WMapLibre
  /////////////////////////////////////////////////////////////////////////////////////////////////////

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
      // Create map with Canvas renderer
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

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // geoJSON with Wards
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      js << "map.on('load', function() {\n";

      js << "var ward_color = [";
      for (size_t idx = 0; idx < ward_color.size(); ++idx)
      {
        js << "'" << ward_color[idx] << "'";
        if (idx < ward_color.size() - 1) js << ",";
      }
      js << "];\n";

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // add ward GeoJSON source and layer
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
      // incidents as circles with GeoJSON FeatureCollection
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      if (!coordinates.empty())
      {
        js << "var circles = [];\n";

        for (size_t idx = 0; idx < coordinates.size(); ++idx)
        {
          std::string lat = coordinates[idx].latitude;
          std::string lon = coordinates[idx].longitude;

          if (!lat.empty() && !lon.empty())
          {
            js << "circles.push({"
              << "'type': 'Feature',"
              << "'geometry': {"
              << "'type': 'Point',"
              << "'coordinates': [" << lon << ", " << lat << "]"
              << "},"
              << "'properties': {}"
              << "});\n";
          }
        }

        js << "map.addSource('incidents', {\n"
          << "  'type': 'geojson',\n"
          << "  'data': {\n"
          << "    'type': 'FeatureCollection',\n"
          << "    'features': circles\n"
          << "  }\n"
          << "});\n"

          << "map.addLayer({\n"
          << "  'id': 'incidents-circles',\n"
          << "  'type': 'circle',\n"
          << "  'source': 'incidents',\n"
          << "  'paint': {\n"
          << "    'circle-radius': [\n"
          << "      'interpolate', ['linear'], ['zoom'],\n"
          << "      10, 2,\n"
          << "      12, 4,\n"
          << "      14, 6,\n"
          << "      16, 8\n"
          << "    ],\n"
          << "    'circle-color': '#ff0000',\n"
          << "    'circle-opacity': 0.4\n"
          << "  }\n"
          << "});\n";
      }

      //close map.on('load')
      js << "});\n";

#ifdef _WIN32
      if (coordinates.empty())
      {
        OutputDebugStringA(js.str().c_str());
      }
#endif

      WApplication* app = WApplication::instance();
      app->doJavaScript(js.str());
    }


  } //WMapLibre
}// namespace Wt
