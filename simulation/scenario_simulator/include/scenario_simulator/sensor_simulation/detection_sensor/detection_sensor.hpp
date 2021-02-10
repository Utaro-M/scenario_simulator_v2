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

#ifndef SCENARIO_SIMULATOR__SENSOR_SIMULATION__DETECTION_SENSOR__DETECTION_SENSOR_HPP_
#define SCENARIO_SIMULATOR__SENSOR_SIMULATION__DETECTION_SENSOR__DETECTION_SENSOR_HPP_

#include <simulation_api_schema.pb.h>

#include <rclcpp/rclcpp.hpp>
#include <autoware_perception_msgs/msg/dynamic_object_array.hpp>

namespace scenario_simulator
{
class DetectionSensor
{
public:
  DetectionSensor();

private:
};
}  // namespace scenario_simulator

#endif  // SCENARIO_SIMULATOR__SENSOR_SIMULATION__DETECTION_SENSOR__DETECTION_SENSOR_HPP_
