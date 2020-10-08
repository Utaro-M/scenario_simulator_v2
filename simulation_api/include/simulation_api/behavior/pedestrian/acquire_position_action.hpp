// Copyright 2015-2020 Autoware Foundation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SIMULATION_API__BEHAVIOR__PEDESTRIAN__ACQUIRE_POSITION_ACTION_HPP_
#define SIMULATION_API__BEHAVIOR__PEDESTRIAN__ACQUIRE_POSITION_ACTION_HPP_

#include <simulation_api/entity/pedestrian_parameter.hpp>
#include <simulation_api/entity/entity_status.hpp>
#include <simulation_api/behavior/pedestrian/pedestrian_action_node.hpp>
#include <simulation_api/hdmap_utils/hdmap_utils.hpp>

#include <geometry_msgs/msg/point.hpp>
#include <behaviortree_cpp_v3/behavior_tree.h>
#include <behaviortree_cpp_v3/bt_factory.h>

#include <boost/optional.hpp>

#include <string>
#include <memory>
#include <vector>

namespace entity_behavior
{
namespace pedestrian
{
class AcquirePositionAction : public entity_behavior::PedestrianActionNode
{
public:
  AcquirePositionAction(const std::string & name, const BT::NodeConfiguration & config);
  BT::NodeStatus tick() override;
  void getBlackBoardValues();
  static BT::PortsList providedPorts()
  {
    BT::PortsList ports = {
      BT::InputPort<simulation_api::entity::EntityStatus>("target_status")
    };
    BT::PortsList parent_ports = entity_behavior::PedestrianActionNode::providedPorts();
    for (const auto & parent_port : parent_ports) {
      ports.emplace(parent_port.first, parent_port.second);
    }
    return ports;
  }

private:
  boost::optional<simulation_api::entity::EntityStatus> target_status_;
  boost::optional<std::vector<int>> route_;
};
}      // namespace pedestrian
}  // namespace entity_behavior

#endif  // SIMULATION_API__BEHAVIOR__PEDESTRIAN__ACQUIRE_POSITION_ACTION_HPP_
