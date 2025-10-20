#ifndef WLEAFLET_HH
#define WLEAFLET_HH

#include <Wt/WCompositeWidget.h>
#include <vector>
#include <string>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// WLeaflet
/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Wt
{
  class WT_API WLeaflet : public WCompositeWidget
  {
    class Impl;
  public:
    WLeaflet();
    ~WLeaflet()
    {
      latitude.clear();
      longitude.clear();
      geojson.clear();
    }

    std::string geojson;
    std::vector<std::string> latitude;
    std::vector<std::string> longitude;

  protected:
    Impl* impl;
    virtual void render(WFlags<RenderFlag> flags) override;

  };
}

#endif