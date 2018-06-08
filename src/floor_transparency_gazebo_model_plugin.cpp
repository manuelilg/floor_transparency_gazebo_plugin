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

  if(_sdf->HasElement("reference_model")) {
    referenceModelName = _sdf->GetElement("reference_model")->Get<std::string>();
  } else {
    gzerr << "No reference Model specified (add Element 'reference_model' to urdf/sdf)" << std::endl;
    return;
  }

  this->updateConnection = event::Events::ConnectWorldUpdateEnd(std::bind(&FloorTransparencyModelPlugin::onUpdateEnd, this));
  this->model = _model;

  this->node = transport::NodePtr(new transport::Node());
  this->node->Init(_model->GetWorld()->GetName());
  this->visPub = this->node->Advertise<msgs::Visual>("~/visual", 10);

  sdf::ElementPtr sdfModel = _sdf->GetParent();
  sdf::ElementPtr link = sdfModel->GetElement("link");

  while(link) {
    if (link->GetName() == "link") {
      std::string linkName = link->GetAttribute("name")->GetAsString();
      if (!visualNames2linkNamesMap.count(linkName)) {
        visualNames2linkNamesMap.emplace(linkName, std::vector<std::string>());
      }
//      gzdbg << "Model Link Attribut count: " << link->GetAttributeCount() << std::endl;
//      gzdbg << "Model Link name: " << link->GetAttribute("name")->GetAsString() << std::endl;

      sdf::ElementPtr visual = link->GetElement("visual");
      while(visual) {
        if (visual->GetName() == "visual") {
          visualNames2linkNamesMap.at(linkName).push_back(visual->GetAttribute("name")->GetAsString());
//          gzdbg << "Name of visual " << visual->GetAttribute("name")->GetAsString() << std::endl;
        }
        visual = visual->GetNextElement();
      }
    }

    link = link->GetNextElement();
  }

  linkVec = this->model->GetLinks();
  std::sort(linkVec.begin(), linkVec.end(), [](const physics::LinkPtr a, const physics::LinkPtr b) {return a->GetWorldPose().pos.z < b->GetWorldPose().pos.z;});
}

void FloorTransparencyModelPlugin::onUpdateEnd()
{
  static int counter = 0;

  if (counter > 5000) {

    double referenceZ = std::numeric_limits<double>::infinity();
    const physics::Model_V& models = model->GetWorld()->GetModels();
    physics::Model_V::const_iterator refModel = std::find_if(models.begin(), models.end(), [](physics::ModelPtr m) {return m->GetName() == "reference";});
    if (refModel != models.end()) {
      referenceZ = refModel->get()->GetWorldPose().pos.z;
    } else {
      gzdbg << "No Model found with name '" << referenceModelName << "'!" << std::endl;
    }

    physics::Link_V::iterator makeTransparent = std::find_if(linkVec.begin(), linkVec.end(), [=](physics::LinkPtr link) {return link->GetWorldPose().pos.z > referenceZ;});

    for (physics::Link_V::iterator visible = linkVec.begin(); visible != makeTransparent; ++visible) {
      physics::LinkPtr link = (*visible);

      for(std::string visualName : visualNames2linkNamesMap.at(link->GetName())) {
        msgs::Visual visualMsg = link->GetVisualMessage(link->GetScopedName() + "::" + visualName);
        visualMsg.set_transparency(0);
        this->visPub->Publish(visualMsg);
      }
    }


    for (; makeTransparent != linkVec.end(); ++makeTransparent) {
      physics::LinkPtr link = (*makeTransparent);

      for(std::string visualName : visualNames2linkNamesMap.at(link->GetName())) {
        msgs::Visual visualMsg = link->GetVisualMessage(link->GetScopedName() + "::" + visualName);
        visualMsg.set_transparency(0.5);
        this->visPub->Publish(visualMsg);
      }
    }

    counter = 0;
  } else {
    counter++;
  }

}

} // namespace gazebo

