#pragma once

#include <gazebo-7/gazebo/common/Plugin.hh>
#include <gazebo-7/gazebo/gazebo.hh>

namespace gazebo {

class FloorTransparencyModelPlugin : public ModelPlugin
{
public:
  FloorTransparencyModelPlugin();
  virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf);
  void onUpdateEnd();

private:
  event::ConnectionPtr updateConnection;
  physics::ModelPtr model;
  transport::NodePtr node;
  transport::PublisherPtr visPub;
  std::map<std::string, std::vector<std::string>> visualNames2linkNamesMap;
  physics::Link_V linkVec;
  std::string referenceModelName;
};

}

