#ifndef ImageComponent_h_seen
#define ImageComponent_h_seen

#include "Component.h"
#include <string>
class Image;
class ImageComponent: public Component
{
  public:
    ImageComponent(const std::string & filename);
    virtual void paint(const nds::Rectangle & clip);
    virtual ~ImageComponent();

  private:
    Image * m_image;

};
#endif
