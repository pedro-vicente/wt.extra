#ifndef WMAPBOX_HH
#define WMAPBOX_HH

#include <Wt/WCompositeWidget.h>
#include <vector>
#include <string>
#include <iostream>
#include "parser.hh"

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
      coordinates.clear();
      geojson.clear();
    }

    std::string geojson;
    std::vector<Coordinate> coordinates;

  protected:
    Impl* impl;
    virtual void render(WFlags<RenderFlag> flags) override;

  };
}

#endif