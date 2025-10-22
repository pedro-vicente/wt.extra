#include "WMapbox.hh"
#include "web/Configuration.h"

std::string to_hex(int n);
std::string rgb_to_hex(int r, int g, int b);

namespace Wt
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // WMapbox::Impl
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  class WMapbox::Impl : public WWebWidget
  {
  public:
    Impl();
    virtual DomElementType domElementType() const override;
  };

  WMapbox::Impl::Impl()
  {
    setInline(false);
  }

  DomElementType WMapbox::Impl::domElementType() const
  {
    return DomElementType::DIV;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // WMapbox
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  WMapbox::WMapbox()
  {
    setImplementation(std::unique_ptr<Impl>(impl = new Impl()));
    WApplication* app = WApplication::instance();
    this->addCssRule("body", "margin: 0; padding: 0;");
    this->addCssRule("#" + id(), "position: absolute; top: 0; bottom: 0; width: 100%;");
    app->useStyleSheet("https://api.mapbox.com/mapbox-gl-js/v3.15.0/mapbox-gl.css");
    const std::string library = "https://api.mapbox.com/mapbox-gl-js/v3.15.0/mapbox-gl.js";
    app->require(library, "mapbox");
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // render
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  void WMapbox::render(WFlags<RenderFlag> flags)
  {
    WCompositeWidget::render(flags);

    std::string style = "mapbox://styles/mapbox/light-v11";

    if (flags.test(RenderFlag::Full))
    {
      WApplication* app = WApplication::instance();
      std::string js =
        "{"
        "  mapboxgl.accessToken = 'pk.eyJ1IjoicGVkcm92aWNlbnRlIiwiYSI6ImNtZmlsZTAzdzAwNmgya3BwaDluYzE2a2cifQ.d7DMAInp3KDvRRsAAohqlA';"
        "  const map = new mapboxgl.Map({"
        "    container: " + jsRef() + ","
        "    center: [-76.90, 38.85]," //longitude, latitude
        "    style: '" + style + "',"
        "    zoom: 11,"
        "    maxZoom: 16,"
        "    minZoom:9"
        "  });"
        "  " + jsRef() + ".mapboxMap = map;"

        "  map.on('load', function() {"
        "    console.log('Map loaded, adding data...');"
        "    ";

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // GeoJSON polygons with different ward colors
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      if (!geojson.empty())
      {
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

        js +=
          "    map.addSource('polygons', {"
          "      'type': 'geojson',"
          "      'data': " + geojson +
          "    });"
          "    "

          /////////////////////////////////////////////////////////////////////////////////////////////////////
          // add polygon fill layer 
          /////////////////////////////////////////////////////////////////////////////////////////////////////

          "    map.addLayer({"
          "      'id': 'polygon-fill',"
          "      'type': 'fill',"
          "      'source': 'polygons',"
          "      'paint': {"
          "        'fill-color': ["
          "          'match',"
          "          ['get', 'WARD'],"
          "          1, '" + ward_color[0] + "',"
          "          2, '" + ward_color[1] + "',"
          "          3, '" + ward_color[2] + "',"
          "          4, '" + ward_color[3] + "',"
          "          5, '" + ward_color[4] + "',"
          "          6, '" + ward_color[5] + "',"
          "          7, '" + ward_color[6] + "',"
          "          8, '" + ward_color[7] + "',"
          "          '#888888'" //match expression requires a default fallback value at the end
          "        ],"
          "        'fill-opacity': 0.2"
          "      }"
          "    });"
          "    "
          ;
      }

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // GeoJSON source for circles (on top of polygons)
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      if (!latitude.empty() && !longitude.empty())
      {
        js +=
          "    map.addSource('circle-points', {"
          "      'type': 'geojson',"
          "      'data': {"
          "        'type': 'FeatureCollection',"
          "        'features': [";

        bool first_feature = true;

        for (size_t idx = 0; idx < latitude.size() && idx < longitude.size(); ++idx)
        {
          std::string lat = latitude[idx];
          std::string lon = longitude[idx];

          if (!lat.empty() && !lon.empty())
          {
            if (!first_feature) js += ",";
            first_feature = false;

            std::string color = "#ff0000";
            js +=
              "{"
              "  'type': 'Feature',"
              "  'geometry': {"
              "    'type': 'Point',"
              "    'coordinates': [" + lon + ", " + lat + "]"
              "  },"
              "  'properties': {"
              "    'color': '" + color + "',"
              "    'index': " + std::to_string(idx) + ","
              "    'lat': '" + lat + "',"
              "    'lng': '" + lon + "'"
              "  }"
              "}";
          }
        }

        js +=
          "        ]"
          "      }"
          "    });"
          "    "
          //add circle layer
          "    map.addLayer({"
          "      'id': 'circles',"
          "      'type': 'circle',"
          "      'source': 'circle-points',"
          "      'paint': {"
          "        'circle-radius': 4, "
          "        'circle-color': ['get', 'color'],"
          "        'circle-opacity': 0.6,"
          "        'circle-stroke-width': 1,"
          "        'circle-stroke-color': '#FFFFFF',"
          "        'circle-stroke-opacity': 1.0"
          "      }"
          "    });"
          "    "
          "    console.log('Added " + std::to_string(latitude.size()) + " circles to map');";
      }

      js +=
        "  });"
        "}";

      app->doJavaScript(js);
    }


  } //WMapbox
}// namespace Wt
