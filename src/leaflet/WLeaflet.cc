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
      js << "const map = L.map(" << jsRef() << ").setView([38.85, -76.95], 12);"
        << "L.tileLayer('https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png', {"
        << "maxZoom: 19,"
        << "minZoom: 10,"
        << "attribution: '© OpenStreetMap contributors'"
        << "}).addTo(map);";

      WApplication* app = WApplication::instance();
      app->doJavaScript(js.str());
    }


  } //WLeaflet
}// namespace Wt
