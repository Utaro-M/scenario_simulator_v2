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

#include <quaternion_operation/quaternion_operation.h>

#include <awapi_accessor/accessor.hpp>
#include <memory>
#include <openscenario_msgs/msg/waypoints_array.hpp>
#include <string>
#include <traffic_simulator/entity/ego_entity.hpp>
#include <unordered_map>
#include <vector>

namespace traffic_simulator
{
namespace entity
{
std::unordered_map<std::string, std::shared_ptr<autoware_api::Accessor>> EgoEntity::autowares{};

EgoEntity::EgoEntity(
  const std::string & name, const boost::filesystem::path & lanelet2_map_osm,
  const double step_time, const openscenario_msgs::msg::VehicleParameters & parameters)
: VehicleEntity(name, parameters),
  vehicle_model_ptr_(std::make_shared<SimModelTimeDelaySteer>(
    parameters.performance.max_speed,          // vel_lim,
    parameters.axles.front_axle.max_steering,  // steer_lim,
    parameters.performance.max_acceleration,   // accel_rate,
    5.0,                                       // steer_rate_lim,
    parameters.axles.front_axle.position_x - parameters.axles.rear_axle.position_x,
    step_time,  // dt,
    0.25,       // vel_time_delay,
    0.5,        // vel_time_constant,
    0.3,        // steer_time_delay,
    0.3,        // steer_time_constant,
    0.0         // deadzone_delta_steer
    ))
{
  auto launch_autoware = [&]() {
    auto get_parameter = [](const std::string & name, const auto & alternate) {
      rclcpp::Node node{"get_parameter", "simulation"};

      auto value = alternate;

      using value_type = typename std::decay<decltype(value)>::type;

      node.declare_parameter<value_type>(name, value);
      node.get_parameter<value_type>(name, value);

      return value;
    };

    /* ---- NOTE -----------------------------------------------------------
       *
       *  The actual values of these parameters are set by
       *  scenario_test_runner.launch.py as parameters of
       *  openscenario_interpreter_node.
       *
       * ------------------------------------------------------------------ */
    const auto autoware_launch_package = get_parameter("autoware_launch_package", std::string(""));
    const auto autoware_launch_file = get_parameter("autoware_launch_file", std::string(""));

    auto child = [&]() {
      const std::vector<std::string> argv{
        "python3",
        "/opt/ros/foxy/bin/ros2",  // NOTE: The command 'ros2' is a Python script.
        "launch",
        autoware_launch_package,
        autoware_launch_file,
        std::string("map_path:=") += lanelet2_map_osm.parent_path().string(),
        std::string("lanelet2_map_file:=") += lanelet2_map_osm.filename().string(),
        "vehicle_model:=ymc_golfcart_proto2",
        "sensor_model:=aip_x1",
        "rviz_config:=" + ament_index_cpp::get_package_share_directory("scenario_test_runner") +
          "/planning_simulator_v2.rviz",
        "scenario_simulation:=true"};

#ifdef TRAFFIC_SIMULATOR_ISOLATE_STANDARD_OUTPUT_FROM_AUTOWARE
      const std::string name = "/tmp/scenario_test_runner/autoware-output.txt";
      const auto fd = ::open(name.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
      ::dup2(fd, STDOUT_FILENO);
      ::dup2(fd, STDERR_FILENO);
      ::close(fd);
#endif

      if (execute(argv) < 0) {
        std::cout << std::system_error(errno, std::system_category()).what() << std::endl;
        std::exit(EXIT_FAILURE);
      }
    };

    if ((autoware_process_id = fork()) < 0) {
      throw std::system_error(errno, std::system_category());
    } else if (autoware_process_id == 0) {
      return child();
    }
  };

  if (autowares.find(name) == std::end(autowares)) {
    auto my_name = name;
    std::replace(std::begin(my_name), std::end(my_name), ' ', '_');
    autowares.emplace(
      name, std::make_shared<autoware_api::Accessor>(
              "awapi_accessor",
              "simulation/" + my_name,  // NOTE: Specified in scenario_test_runner.launch.py
              rclcpp::NodeOptions().use_global_arguments(false)));

    launch_autoware();
  }

  /* ---- NOTE ---------------------------------------------------------------
   *
   *  The simulator needs to run in a fixed-cycle loop, but the communication
   *  part with Autoware needs to run at a higher frequency (e.g. the
   *  transform in map -> base_link needs to be updated at a higher frequency
   *  even if the value does not change). We also need to keep collecting the
   *  latest values of topics from Autoware, independently of the simulator.
   *
   *  For this reason, autoware_api::Accessor, which is responsible for
   *  communication with Autoware, should run in an independent thread. This
   *  is probably an EXTREMELY DIRTY HACK.
   *
   *  Ideally, the constructor caller of traffic_simulator::API should
   *  provide a std::shared_ptr to autoware_api::Accessor and spin that node
   *  with MultiThreadedExecutor.
   *
   *  If you have a nice idea to solve this, and are interested in improving
   *  the quality of the Tier IV simulator, please contact @yamacir-kit.
   *
   * ---------------------------------------------------------------------- */
  if (autowares.at(name).use_count() < 2) {
    accessor_status = std::make_shared<std::promise<void>>();
    accessor_spinner = std::make_shared<std::thread>(
      [](const auto node, auto status)  // NOTE: This copy increments use_count to 2 from 1.
      {
        while (rclcpp::ok() &&
               status.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
          rclcpp::spin_some(node);
        }
      },
      autowares.at(name), std::move(accessor_status->get_future()));
  }
}

void EgoEntity::requestAssignRoute(
  const std::vector<openscenario_msgs::msg::LaneletPose> & waypoints)
{
  assert(1 < waypoints.size());

  const auto destination = (*hdmap_utils_ptr_).toMapPose(waypoints.back());

  std::vector<geometry_msgs::msg::PoseStamped> constraints{};

  for (auto iter = std::cbegin(waypoints); std::next(iter) != std::cend(waypoints); ++iter) {
    constraints.push_back((*hdmap_utils_ptr_).toMapPose(*iter));
  }

  return requestAcquirePosition(destination, constraints);
}

openscenario_msgs::msg::WaypointsArray EgoEntity::getWaypoints() const
{
  openscenario_msgs::msg::WaypointsArray waypoints{};

  for (const auto & point : std::atomic_load(&autowares.at(name))->getTrajectory().points) {
    waypoints.waypoints.emplace_back(point.pose.position);
  }

  return waypoints;
}

bool EgoEntity::setStatus(const openscenario_msgs::msg::EntityStatus & status)
{
  // NOTE Currently, setStatus always succeeds.
  const bool success = VehicleEntity::setStatus(status);

  const auto current = getStatus();

  if (autoware_initialized) {
    updateAutoware(current.pose);
  }

  if (!initial_pose_) {
    initial_pose_ = current.pose;
  }

  return success;
}

void EgoEntity::onUpdate(double current_time, double step_time)
{
  Eigen::VectorXd input(2);
  {
    input << std::atomic_load(&autowares.at(name))->getVehicleCommand().control.velocity,
      std::atomic_load(&autowares.at(name))->getVehicleCommand().control.steering_angle;
  }

  (*vehicle_model_ptr_).setInput(input);
  (*vehicle_model_ptr_).update(step_time);

  setStatus(getEntityStatus(current_time + step_time, step_time));

  if (previous_linear_velocity_) {
    linear_jerk_ = (vehicle_model_ptr_->getVx() - previous_linear_velocity_.get()) / step_time;
  } else {
    linear_jerk_ = 0;
  }

  previous_linear_velocity_ = vehicle_model_ptr_->getVx();
  previous_angular_velocity_ = vehicle_model_ptr_->getWz();
}

const openscenario_msgs::msg::EntityStatus EgoEntity::getEntityStatus(
  const double time, const double step_time) const
{
  geometry_msgs::msg::Vector3 rpy;
  {
    rpy.x = 0;
    rpy.y = 0;
    rpy.z = vehicle_model_ptr_->getYaw();
  }

  geometry_msgs::msg::Pose pose;
  {
    pose.position.x = vehicle_model_ptr_->getX();
    pose.position.y = vehicle_model_ptr_->getY();
    pose.position.z = 0.0;
    pose.orientation = quaternion_operation::convertEulerAngleToQuaternion(rpy);
  }

  geometry_msgs::msg::Twist twist;
  {
    twist.linear.x = vehicle_model_ptr_->getVx();
    twist.angular.z = vehicle_model_ptr_->getWz();
  }

  geometry_msgs::msg::Accel accel;
  {
    if (previous_angular_velocity_ && previous_linear_velocity_) {
      accel.linear.x = (twist.linear.x - previous_linear_velocity_.get()) / step_time;
      accel.angular.z = (twist.angular.z - previous_angular_velocity_.get()) / step_time;
    }
  }

  Eigen::VectorXd v(3);
  {
    v(0) = pose.position.x;
    v(1) = pose.position.y;
    v(2) = pose.position.z;

    v = quaternion_operation::getRotationMatrix((*initial_pose_).orientation) * v;
  }

  openscenario_msgs::msg::EntityStatus status;
  {
    status.time = time;
    status.type.type = openscenario_msgs::msg::EntityType::EGO;
    status.bounding_box = getBoundingBox();
    status.action_status.twist = twist;
    status.action_status.accel = accel;
    status.pose.position.x = v(0) + initial_pose_.get().position.x;
    status.pose.position.y = v(1) + initial_pose_.get().position.y;
    status.pose.position.z = v(2) + initial_pose_.get().position.z;
    const auto closest_lanelet_id = hdmap_utils_ptr_->getClosetLanletId(status.pose);
    if (!closest_lanelet_id) {
      throw SimulationRuntimeError("failed to closest lane.");
    }
    traffic_simulator::math::CatmullRomSpline spline(
      hdmap_utils_ptr_->getCenterPoints(closest_lanelet_id.get()));
    const auto s_value = spline.getSValue(status.pose.position);
    if (s_value) {
      status.pose.position.z = spline.getPoint(s_value.get()).z;
    }

    status.pose.orientation = initial_pose_.get().orientation * pose.orientation;

    const auto lanelet_pose = hdmap_utils_ptr_->toLaneletPose(status.pose);

    status.lanelet_pose_valid = static_cast<bool>(lanelet_pose);

    if (lanelet_pose) {
      status.lanelet_pose = lanelet_pose.get();
    }
  }

  return status;
}
}  // namespace entity
}  // namespace traffic_simulator
