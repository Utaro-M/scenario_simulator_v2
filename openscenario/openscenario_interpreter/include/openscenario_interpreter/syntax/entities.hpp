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

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__ENTITIES_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__ENTITIES_HPP_

#include <openscenario_interpreter/syntax/entity_selection.hpp>
#include <openscenario_interpreter/syntax/scenario_object.hpp>

namespace openscenario_interpreter
{
inline namespace syntax
{
/* ==== Entities =============================================================
 *
 * <xsd:complexType name="Entities">
 *   <xsd:sequence>
 *     <xsd:element name="ScenarioObject" minOccurs="0" maxOccurs="unbounded" type="ScenarioObject"/>
 *     <xsd:element name="EntitySelection" minOccurs="0" maxOccurs="unbounded" type="EntitySelection"/>
 *   </xsd:sequence>
 * </xsd:complexType>
 *
 * ======================================================================== */
struct Entities
{
  template<typename Node>
  explicit Entities(const Node & node, Scope & scope)
  {
    callWithElements(
      node, "ScenarioObject", 0, unbounded, [&](auto && node)
      {
        scope.entities.emplace(
          readAttribute<String>("name", node, scope), make<ScenarioObject>(node, scope));
      });

    callWithElements(node, "EntitySelection", 0, unbounded, UNSUPPORTED());
  }
};
}
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__ENTITIES_HPP_