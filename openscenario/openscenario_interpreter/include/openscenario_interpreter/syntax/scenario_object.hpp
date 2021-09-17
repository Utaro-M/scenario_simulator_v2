// Copyright 2015-2020 Tier IV, Inc. All rights reserved.
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

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__SCENARIO_OBJECT_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__SCENARIO_OBJECT_HPP_

#include <boost/lexical_cast.hpp>
#include <openscenario_interpreter/procedure.hpp>
#include <openscenario_interpreter/syntax/entity_object.hpp>
#include <openscenario_interpreter/syntax/object_controller.hpp>
#include <openscenario_interpreter/syntax/string.hpp>
#include <openscenario_interpreter/utility/overload.hpp>
#include <traffic_simulator/metrics/out_of_range_metric.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
/* ---- ScenarioObject ---------------------------------------------------------
 *
 *  <xsd:complexType name="ScenarioObject">
 *    <xsd:sequence>
 *      <xsd:group ref="EntityObject"/>
 *      <xsd:element name="ObjectController" minOccurs="0" type="ObjectController"/>
 *    </xsd:sequence>
 *    <xsd:attribute name="name" type="String" use="required"/>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct ScenarioObject
/* -----------------------------------------------------------------------------
 *
 *  The EntityObject (either instance of type Vehicle, Pedestrian or
 *  MiscObject).
 *
 *  NOTE: This framework expresses xsd:group as mixin.
 *
 * ------------------------------------------------------------------------- */
: public Scope,
  public EntityObject
{
  ObjectController object_controller;  // Controller of the EntityObject instance.

  bool is_added = false;  // NOTE: Is applied AddEntityAction?

  template <typename Node>
  explicit ScenarioObject(const Node & node, Scope & outer_scope)
  : Scope(outer_scope.makeChildScope(readAttribute<String>("name", node, outer_scope))),
    EntityObject(node, localScope()),
    object_controller(readElement<ObjectController>("ObjectController", node, localScope()))
  {
  }

  auto activateOutOfRangeMetric(const Vehicle & vehicle) const -> bool
  {
    const auto parameters = static_cast<openscenario_msgs::msg::VehicleParameters>(vehicle);

    metrics::OutOfRangeMetric::Config configuration;
    {
      configuration.target_entity = name;
      configuration.min_velocity = -parameters.performance.max_speed;
      configuration.max_velocity = +parameters.performance.max_speed;
      configuration.min_acceleration = -parameters.performance.max_deceleration;
      configuration.max_acceleration = +parameters.performance.max_acceleration;

      if (object_controller.is<Controller>()) {
        auto controller = object_controller.as<Controller>();
        auto max_jerk = controller["maxJerk"];
        auto min_jerk = controller["minJerk"];

        if (not max_jerk.name.empty()) {
          configuration.max_jerk = boost::lexical_cast<double>(max_jerk.value);
        }
        if (not min_jerk.name.empty()) {
          configuration.min_jerk = boost::lexical_cast<double>(min_jerk.value);
        }
      }

      if (object_controller.isEgo()) {
        configuration.jerk_topic =
          "/planning/scenario_planning/motion_velocity_optimizer/closest_jerk";
      }
    }

    connection.addMetric<metrics::OutOfRangeMetric>(name + "-out-of-range", configuration);

    return true;
  }

  auto activateSensors() -> bool
  {
    if (object_controller.isEgo()) {
      const auto architecture_type = getParameter<std::string>("architecture_type", "");
      if (architecture_type == "tier4/proposal") {
        return attachLidarSensor(traffic_simulator::helper::constructLidarConfiguration(
                 traffic_simulator::helper::LidarType::VLP16, name,
                 "/sensing/lidar/no_ground/pointcloud")) and
               attachDetectionSensor(
                 traffic_simulator::helper::constructDetectionSensorConfiguration(
                   name, "/perception/object_recognition/objects", 0.1));
      } else if (architecture_type == "awf/auto") {
        return attachLidarSensor(traffic_simulator::helper::constructLidarConfiguration(
          traffic_simulator::helper::LidarType::VLP16, name, "/perception/points_nonground"));
        // Autoware.Auto does not currently support object prediction
        // however it is work-in-progress for Cargo ODD
        // msgs are already implemented and autoware_auto_msgs::msg::PredictedObjects will probably be used here
        // topic name is yet unknown
      } else {
        throw SemanticError(
          "Unexpected architecture_type ", std::quoted(architecture_type), " specified");
      }
    } else {
      return true;
    }
  }
};

auto operator<<(std::ostream &, const ScenarioObject &) -> std::ostream &;
}  // namespace syntax
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__SCENARIO_OBJECT_HPP_
