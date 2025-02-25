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

#include <openscenario_interpreter/reader/element.hpp>
#include <openscenario_interpreter/syntax/actors.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
Actors::Actors(const pugi::xml_node & node, Scope & scope)
: select_triggering_entities(
    readAttribute<Boolean>("selectTriggeringEntities", node, scope, Boolean()))
{
  callWithElements(
    node, "EntityRef", 0, unbounded, [&](auto && node) { scope.actors.emplace_back(node, scope); });
}
}  // namespace syntax
}  // namespace openscenario_interpreter
