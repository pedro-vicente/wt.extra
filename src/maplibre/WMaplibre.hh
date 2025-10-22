#ifndef WMAPLIBRE_HH
#define WMAPLIBRE_HH

#include <Wt/WCompositeWidget.h>
#include <vector>
#include <string>

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
    ~WMapLibre()
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