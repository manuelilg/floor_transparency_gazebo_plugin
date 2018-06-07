#include "floor_transparency_gazebo_plugin/floor_transparency_gazebo_model_plugin.h"

//#include <gazebo-7/gazebo/common/Console.hh>
#include <gazebo-7/gazebo/physics/Model.hh>
#include <gazebo-7/gazebo/physics/World.hh>
#include <gazebo-7/gazebo/physics/Link.hh>
#include <functional>

namespace gazebo {

GZ_REGISTER_MODEL_PLUGIN(FloorTransparencyModelPlugin)

FloorTransparencyModelPlugin::FloorTransparencyModelPlugin()
{

}

void FloorTransparencyModelPlugin::Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
{
  gzdbg << "Load Plugin 'FloorTransparencyModelPlugin'" << std::endl;

  this->updateConnection = event::Events::ConnectWorldUpdateEnd(std::bind(&FloorTransparencyModelPlugin::onUpdateEnd, this));
  this->model = _model;

  this->node = transport::NodePtr(new transport::Node());
  this->node->Init(_model->GetWorld()->GetName());
  this->visPub = this->node->Advertise<msgs::Visual>("~/visual", 10);

}

void FloorTransparencyModelPlugin::onUpdateEnd()
{
  static int counter = 0;

  if (counter > 5000) {
    gzdbg << "onUpdateEnd called" << std::endl;

    gzdbg << "Model name: " << this->model->GetName() << std::endl;
    gzdbg << "Model has " << this->model->GetLinks().size() << " joint(s)" << std::endl;

    gzdbg << "Link 0 has name " << this->model->GetLinks().at(0)->GetName() << std::endl;
    uint32_t visualId;
    if (this->model->GetLinks().at(0)->VisualId("visual", visualId)) {
      gzdbg << "visualId: " << visualId << std::endl;
    } else {
      gzerr << "no visual id found" << std::endl;
    }

//    this->model->GetLinks().at(0)->




//    msgs::Visual msg;
//    msg.set_name("");
//    msg.transparency(0.1);

    msgs::Visual msg = this->model->GetLinks().at(0)->GetVisualMessage("test_box::link::visual");
    if (msg.has_id()) {
      gzdbg << "Link 0 has visual ID " << msg.id() << std::endl;
      gzdbg << "Link 0 has transpareny " << msg.transparency() << std::endl;
      gzdbg << "Link 0 has geometry has box " << msg.geometry().has_box() << std::endl;
      gzdbg << "Link 0 has geometry has sphere " << msg.geometry().has_sphere() << std::endl;
      gzdbg << "Link 0 box size x " << msg.geometry().box().size().x() << std::endl;
      gzdbg << "Link 0 sphere radius " << msg.geometry().sphere().radius() << std::endl;

    } else {
      gzdbg << "no msg found" << std::endl;
    }

    {
      msgs::Visual visualMsg;

      visualMsg.set_name("test_box::link::visual");
      visualMsg.set_parent_name(model->GetScopedName());
      visualMsg.set_id(11);
      visualMsg.set_transparency(0.5);
      this->visPub->Publish(visualMsg);
    }
    {
      msgs::Visual visualMsg;

      visualMsg.set_name("test_box::link::visual");
      visualMsg.set_parent_name(model->GetScopedName());
      visualMsg.set_id(11);
//      visualMsg.mutable_geometry()->mutable_sphere()->set_radius(1);
      visualMsg.clear_geometry();
      visualMsg.geometry().mutable_
      this->visPub->Publish(visualMsg);
    }
    counter = 0;
  } else {
    counter++;
  }

}


}

