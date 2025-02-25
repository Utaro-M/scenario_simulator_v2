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

#ifndef OPENSCENARIO_INTERPRETER__EXPRESSION_HPP_
#define OPENSCENARIO_INTERPRETER__EXPRESSION_HPP_

#include <openscenario_interpreter/pointer.hpp>

namespace openscenario_interpreter
{
struct Expression  // NOTE: Member functions are lexicographically sorted.
{
  virtual bool accomplished() { return false; }

  virtual auto description() const -> std::string { return ""; }

  virtual auto evaluate(const Pointer<Expression> &) -> Pointer<Expression>
  {
    throw SemanticError("No viable evaluation for class ", type().name());
  }

  virtual auto currentState() const -> const Pointer<Expression> &
  {
    throw SemanticError("Class ", type().name(), " is not a StoryboardElementType");
  }

  virtual auto type() const noexcept -> const std::type_info & { return typeid(Expression); }

  virtual auto write(std::ostream & os) const -> std::ostream &
  {
    return IfHasStreamOutputOperator<Expression>::invoke(os, *this);
  }
};
}  // namespace openscenario_interpreter

#endif  // OPENSCENARIO_INTERPRETER__EXPRESSION_HPP_
