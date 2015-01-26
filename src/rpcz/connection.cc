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

#include <rpcz/connection.hpp>

#include <zmq.hpp>

#include <rpcz/internal_commands.hpp>
#include <rpcz/manager.hpp>
#include <rpcz/rpc_context.hpp>  // for rpc_context
#include <rpcz/zmq_utils.hpp>

namespace rpcz {

connection::connection()
    : manager_(manager::get()),
      connection_id_(0) {
}

connection::connection(uint64 connection_id)
    : manager_(manager::get()),
      connection_id_(connection_id) {
}

void connection::send_request(
    message_vector& request,
    rpc_context* ctx) {
  BOOST_ASSERT(ctx);
  zmq::socket_t& socket = manager_->get_frontend_socket();
  send_empty_message(&socket, ZMQ_SNDMORE);
  send_char(&socket, kRequest, ZMQ_SNDMORE);
  send_uint64(&socket, connection_id_, ZMQ_SNDMORE);
  send_pointer(&socket, ctx, ZMQ_SNDMORE);
  write_vector_to_socket(&socket, request);
}

}  // namespace rpcz
