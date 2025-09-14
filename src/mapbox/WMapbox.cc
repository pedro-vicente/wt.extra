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
        "    center: [-76.90, 38.85],"
        "    style: '" + style + "',"
        "    zoom: 11"
        "  });"
        "  " + jsRef() + ".mapboxMap = map;"
        "}";

      app->doJavaScript(js);
    }


  } //WMapbox
}// namespace Wt
