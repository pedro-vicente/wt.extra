#ifndef WMAPBOX_HH
#define WMAPBOX_HH

#include <Wt/WCompositeWidget.h>
#include <vector>
#include <string>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// WMapbox
/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Wt
{
  class WT_API WMapbox : public WCompositeWidget
  {
    class Impl;
  public:
    WMapbox();
    ~WMapbox()
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