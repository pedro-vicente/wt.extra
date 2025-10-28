#ifndef WMAPLIBRE_HH
#define WMAPLIBRE_HH

#include <Wt/WCompositeWidget.h>
#include <vector>
#include <string>
#include "parser.hh"

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