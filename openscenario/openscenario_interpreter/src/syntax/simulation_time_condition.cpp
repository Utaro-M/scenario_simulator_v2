// Copyright 2015-2021 Tier IV, Inc. All rights reserved.
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

#include <iomanip>
#include <openscenario_interpreter/procedure.hpp>
#include <openscenario_interpreter/reader/attribute.hpp>
#include <openscenario_interpreter/syntax/simulation_time_condition.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
SimulationTimeCondition::SimulationTimeCondition(const pugi::xml_node & node, Scope & scope)
: value(readAttribute<Double>("value", node, scope)),
  compare(readAttribute<Rule>("rule", node, scope))
{
}

auto SimulationTimeCondition::description() const -> String
{
  std::stringstream description;

  description << "Is the simulation time (= " << std::fixed << std::setprecision(6) << result
              << ") is " << compare << " " << value << "?";

  return description.str();
}

auto SimulationTimeCondition::evaluate() -> Object
{
  return asBoolean(compare(result = getCurrentTime(), value));
}
}  // namespace syntax
}  // namespace openscenario_interpreter
