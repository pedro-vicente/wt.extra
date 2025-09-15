#include "WMapbox.hh"
#include "web/Configuration.h"

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
  // set_data
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  void WMapbox::set_data(const std::vector<std::string>& latitudes, const std::vector<std::string>& longitudes)
  {
    latitude = latitudes;
    longitude = longitudes;
    std::cout << "Set circle data: " << latitudes.size() << " points" << std::endl;
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
        "    zoom: 11"
        "  });"
        "  " + jsRef() + ".mapboxMap = map;"

        "  map.on('load', function() {"
        "    console.log('Map loaded, adding circles...');"
        "    ";

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // GeoJSON source for circles
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
          "        'circle-radius': 8,"
          "        'circle-color': ['get', 'color'],"
          "        'circle-opacity': 0.2,"
          "        'circle-stroke-width': 0,"
          "        'circle-stroke-color': ['get', 'color'],"
          "        'circle-stroke-opacity': 0.2"
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