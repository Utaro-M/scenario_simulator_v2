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

#include <xmlrpc_interface/conversions.hpp>

#include <simulation_api_schema.pb.h>
#include <test.pb.h>

int main()
{
  std::cout << __FILE__ << "," << __LINE__ << std::endl;
  test::String string;
  // simulation_api_schema::Result ret;
  std::cout << __FILE__ << "," << __LINE__ << std::endl;
  // result.set_description("test");
  /*
  result.set_description("test");
  result.set_success(true);
  res.set_allocated_result(&result);
  XmlRpc::XmlRpcValue xml;
  xmlrpc_interfae::fromProto(res, xml);
  */
  return 0;
}