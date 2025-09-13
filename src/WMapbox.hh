#ifndef WMAPBOX_HH
#define WMAPBOX_HH

#include <Wt/WBrush.h>
#include <Wt/WCompositeWidget.h>
#include <Wt/WJavaScript.h>
#include <Wt/WPen.h>
#include <Wt/Json/Object.h>

namespace Wt 
{

  class WT_API WMapbox : public WCompositeWidget
  {
    class Impl;
  public:
    WMapbox();

  protected:
    Impl* impl;
    virtual void render(WFlags<RenderFlag> flags) override;
    
    
  };

}

#endif