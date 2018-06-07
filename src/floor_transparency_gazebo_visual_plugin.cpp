#include "floor_transparency_gazebo_plugin/floor_transparency_gazebo_visual_plugin.h"

#include <gazebo-7/gazebo/rendering/Visual.hh>

namespace gazebo {

GZ_REGISTER_VISUAL_PLUGIN(FloorTransparencyPlugin)

FloorTransparencyPlugin::FloorTransparencyPlugin() {

}

void FloorTransparencyPlugin::Load(rendering::VisualPtr _visual, sdf::ElementPtr _sdf)
{
  gzdbg << "Load FloorTransparencyPlugin" << std::endl;

  _visual->SetTransparency(0.5);
}

} // namespace gazebo

