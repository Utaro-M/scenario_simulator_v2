// Copyright 2015-2020 TierIV.inc. All rights reserved.
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

#ifndef SCENARIO_SIMULATOR__EXCEPTION_HPP_
#define SCENARIO_SIMULATOR__EXCEPTION_HPP_

namespace scenario_simulator
{
class SimulationRuntimeError : public std::runtime_error
{
public:
  explicit SimulationRuntimeError(XmlRpc::XmlRpcValue value)
  : runtime_error(value["message"]) {}
  explicit SimulationRuntimeError(const char * message)
  : runtime_error(message) {}

private:
};
}  // namespace scenario_simulator

#endif  // SCENARIO_SIMULATOR__EXCEPTION_HPP_