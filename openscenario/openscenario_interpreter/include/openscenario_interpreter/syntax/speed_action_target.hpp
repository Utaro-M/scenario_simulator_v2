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

#ifndef OPENSCENARIO_INTERPRETER__SYNTAX__SPEED_ACTION_TARGET_HPP_
#define OPENSCENARIO_INTERPRETER__SYNTAX__SPEED_ACTION_TARGET_HPP_

#include <openscenario_interpreter/syntax/absolute_target_speed.hpp>
#include <openscenario_interpreter/syntax/relative_target_speed.hpp>
#include <utility>

namespace openscenario_interpreter
{
inline namespace syntax
{
/* ---- SpeedActionTarget ------------------------------------------------------
 *
 *  <xsd:complexType name="SpeedActionTarget">
 *    <xsd:choice>
 *      <xsd:element name="RelativeTargetSpeed" type="RelativeTargetSpeed"/>
 *      <xsd:element name="AbsoluteTargetSpeed" type="AbsoluteTargetSpeed"/>
 *    </xsd:choice>
 *  </xsd:complexType>
 *
 * -------------------------------------------------------------------------- */
struct SpeedActionTarget : public ComplexType
{
  template <typename Node, typename... Ts>
  explicit SpeedActionTarget(const Node & node, Ts &&... xs)
  // clang-format off
  : ComplexType(
      choice(node,
        std::make_pair("RelativeTargetSpeed", [&](auto && node) { return make<RelativeTargetSpeed>(std::forward<decltype(node)>(node), std::forward<decltype(xs)>(xs)...); }),
        std::make_pair("AbsoluteTargetSpeed", [&](auto && node) { return make<AbsoluteTargetSpeed>(std::forward<decltype(node)>(node), std::forward<decltype(xs)>(xs)...); })))
  // clang-format on
  {
  }

  auto getCalculateAbsoluteTargetSpeed() const -> std::function<double()>;

  auto getIsEnd() const -> std::function<bool(const EntityRef &)>;
};

DEFINE_LAZY_VISITOR(
  SpeedActionTarget,
  CASE(RelativeTargetSpeed),  //
  CASE(AbsoluteTargetSpeed),  //
);

DEFINE_LAZY_VISITOR(
  const SpeedActionTarget,
  CASE(RelativeTargetSpeed),  //
  CASE(AbsoluteTargetSpeed),  //
);
}  // namespace syntax
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__SYNTAX__SPEED_ACTION_TARGET_HPP_
