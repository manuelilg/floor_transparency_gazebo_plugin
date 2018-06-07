#pragma once

#include <gazebo-7/gazebo/common/Plugin.hh>

namespace gazebo {

class FloorTransparencyPlugin : public VisualPlugin
{
public:
  FloorTransparencyPlugin();
  virtual void Load(rendering::VisualPtr _visual, sdf::ElementPtr _sdf);

};

}


