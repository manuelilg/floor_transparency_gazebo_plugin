#include "floor_transparency_gazebo_plugin/floor_transparency_gazebo_model_plugin.h"

//#include <gazebo-7/gazebo/common/Console.hh>
#include <gazebo-7/gazebo/physics/Model.hh>
#include <gazebo-7/gazebo/physics/World.hh>
#include <gazebo-7/gazebo/physics/Link.hh>
#include <gazebo-7/gazebo/physics/Entity.hh>
#include <gazebo-7/gazebo/physics/Base.hh>
#include <sdf/sdf.hh>
#include <sdf/Param.hh>
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

  sdf::ElementPtr sdfModel = _sdf->GetParent();
  gzdbg << "From SDF model name: " << sdfModel->GetAttribute("name")->GetAsString() << std::endl;

  gzdbg << "Model has Pose? " << sdfModel->HasElement("pose") << std::endl;
  gzdbg << "Model has Link? " << sdfModel->HasElement("link") << std::endl;
  sdf::ElementPtr link = sdfModel->GetElement("link");

  while(link) {
    if (link->GetName() == "link") {
      std::string linkName = link->GetAttribute("name")->GetAsString();
      if (!visualNames2linkNamesMap.count(linkName)) {
        visualNames2linkNamesMap.emplace(linkName, std::vector<std::string>());
      }

      gzdbg << "Model Link Attribut count: " << link->GetAttributeCount() << std::endl;
      gzdbg << "Model Link name: " << link->GetAttribute("name")->GetAsString() << std::endl;

      sdf::ElementPtr visual = link->GetElement("visual");
      while(visual) {
        if (visual->GetName() == "visual") {
          visualNames2linkNamesMap.at(linkName).push_back(visual->GetAttribute("name")->GetAsString());
          gzdbg << "Name of visual " << visual->GetAttribute("name")->GetAsString() << std::endl;
        }
        visual = visual->GetNextElement();
      }
    }

    link = link->GetNextElement();
  }

}

void FloorTransparencyModelPlugin::onUpdateEnd()
{
  static int counter = 0;

  if (counter > 5000) {
    gzdbg << "onUpdateEnd called" << std::endl;

    gzdbg << "Model name: " << this->model->GetName() << std::endl;
    gzdbg << "Model has " << this->model->GetLinks().size() << " joint(s)" << std::endl;

    gzdbg << "Link 0 has name " << this->model->GetLinks().at(0)->GetName() << std::endl;
//    uint32_t visualId;
//    if (this->model->GetLinks().at(0)->VisualId("visual", visualId)) {
//      gzdbg << "visualId: " << visualId << std::endl;
//    } else {
//      gzerr << "no visual id found" << std::endl;
//    }

    math::Pose linkPose = this->model->GetLinks().at(0)->GetWorldPose();

    physics::Link_V linkVec = this->model->GetLinks();

    std::sort(linkVec.begin(), linkVec.end(), [](const physics::LinkPtr a, const physics::LinkPtr b) {return a->GetWorldPose().pos.z < b->GetWorldPose().pos.z;});

//    std::for_each(linkVec.begin(), linkVec.end(), [](physics::LinkPtr link) {gzdbg << link->GetWorldPose().pos.z << std::endl;});

    physics::Link_V::iterator toModifyLinksIterator = std::find_if(linkVec.begin(), linkVec.end(), [](physics::LinkPtr link) {return link->GetWorldPose().pos.z > 1.0;});

    for (; toModifyLinksIterator != linkVec.end(); ++toModifyLinksIterator) {
      msgs::Visual visualMsg;

//      gzdbg << (*toModifyLinksIterator)->GetName() << std::endl;
      physics::LinkPtr link = (*toModifyLinksIterator);

      for(std::string visualName : visualNames2linkNamesMap.at(link->GetName())) {
//        std::string adr = link->GetScopedName() + "::" + visualName;
//        gzdbg << "GetVisualMessage with string: " << adr << std::endl;
        msgs::Visual visualMsg = link->GetVisualMessage(link->GetScopedName() + "::" + visualName);
        visualMsg.set_transparency(0.5);
        this->visPub->Publish(visualMsg);
      }



//      link->sdf;


//      visualMsg.set_name("test_box::link::visual");
//      visualMsg.set_parent_name(model->GetScopedName());
//      visualMsg.set_id(11);
//      visualMsg.set_transparency(0.5);
//      this->visPub->Publish(visualMsg);
    }


//    const std::vector<int>&(*func)(void) = [](){return std::vector{5,3,7,2};};
//    auto lambda = [](void) -> const std::vector<int>& {std::vector<int>{5,3,7,2};};

//    std::vector

//    auto lambda = [](void) {return std::vector();};

//    msgs::Visual msg;
//    msg.set_name("");
//    msg.transparency(0.1);

//    msgs::Visual msg = this->model->GetLinks().at(0)->GetVisualMessage("test_box::link::visual");
//    if (msg.has_id()) {
//      gzdbg << "Link 0 has visual ID " << msg.id() << std::endl;
//      gzdbg << "Link 0 has transpareny " << msg.transparency() << std::endl;
//      gzdbg << "Link 0 has geometry has box " << msg.geometry().has_box() << std::endl;
//      gzdbg << "Link 0 has geometry has sphere " << msg.geometry().has_sphere() << std::endl;
//      gzdbg << "Link 0 box size x " << msg.geometry().box().size().x() << std::endl;
//      gzdbg << "Link 0 sphere radius " << msg.geometry().sphere().radius() << std::endl;

//    } else {
//      gzdbg << "no msg found" << std::endl;
//    }

//    {
//      msgs::Visual visualMsg;

//      visualMsg.set_name("test_box::link::visual");
//      visualMsg.set_parent_name(model->GetScopedName());
//      visualMsg.set_id(11);
//      visualMsg.set_transparency(0.5);
//      this->visPub->Publish(visualMsg);
//    }
//    {
////      msgs::Visual visualMsg;

////      visualMsg.set_name("test_box::link::visual");
////      visualMsg.set_parent_name(model->GetScopedName());
////      visualMsg.set_id(11);
//////      visualMsg.mutable_geometry()->mutable_sphere()->set_radius(1);
////      visualMsg.clear_geometry();
////      visualMsg.geometry().mutable_
////      this->visPub->Publish(visualMsg);
//    }
    counter = 0;
  } else {
    counter++;
  }

}


}

