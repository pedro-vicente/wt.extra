#ifndef WLEAFLET_HH
#define WLEAFLET_HH

#include <Wt/WCompositeWidget.h>
#include <vector>
#include <string>
#include "parser.hh"

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