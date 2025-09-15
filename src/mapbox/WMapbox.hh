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
    }

    void set_data(const std::vector<std::string>& latitudes, const std::vector<std::string>& longitudes);

  protected:
    Impl* impl;
    virtual void render(WFlags<RenderFlag> flags) override;

  private:
    std::vector<std::string> latitude;
    std::vector<std::string> longitude;
  };
}

#endif