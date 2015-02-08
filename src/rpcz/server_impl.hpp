// Copyright 2011 Google Inc. All Rights Reserved.
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
//
// Author: nadavs@google.com <Nadav Samet>
//         Jin Qing (http://blog.csdn.net/jq0123)

#ifndef RPCZ_SERVER_IMPL_H
#define RPCZ_SERVER_IMPL_H

#include <set>
#include <boost/noncopyable.hpp>

#include <rpcz/manager_ptr.hpp>
#include <rpcz/service_factory_map.hpp>
#include <rpcz/service_factory_ptr.hpp>

namespace rpcz {

// A server_impl object maps incoming RPC requests to a provided service interface.
// The service interface methods are executed inside a worker thread.
// Non-thread-safe.
class server_impl : boost::noncopyable {
 public:
  server_impl();
  ~server_impl();

  // Registers an rpc service factory with this server_impl.
  // All registrations must occur before bind() is called. TODO: allow after
  // The name parameter identifies the service for external clients. 

  void register_service_factory(service_factory_ptr factory, const std::string& name);

  void bind(const std::string& endpoint);

  // Must register service before bind. Registeration after bind will be ignored.

 private:
  manager_ptr manager_;
  typedef std::set<std::string> bind_endpoint_set;
  bind_endpoint_set endpoints_;
  bool binding_;  // To ignore registeration after bind.
  service_factory_map service_factory_map_;
  // TODO: use thread-safe service_factories to allow register after bind.
};

}  // namespace rpcz

#endif  // RPCZ_SERVER_IMPL_H
