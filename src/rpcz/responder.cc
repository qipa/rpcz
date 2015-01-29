// Copyright 2011 Google Inc. All Rights Reserved.
// Copyright 2015 Jin Qing.
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

#include <rpcz/responder.hpp>

#include <zmq.hpp>  // for message_t

#include <rpcz/invalid_message_error.hpp>
#include <rpcz/logging.hpp>  // for CHECK()
#include <rpcz/responder_info.hpp>
#include <rpcz/router_connection.hpp>
#include <rpcz/rpcz.pb.h>  // for rpc_header
#include <rpcz/zmq_utils.hpp>  // for string_to_message()

namespace rpcz {

// TODO: Do not use router_connection pointer, because connection may be deleted.
responder::responder(router_connection& conn,
                     const std::string& event_id)
    : info_(new responder_info(&conn, event_id)) { // shared_ptr
};

responder::~responder() {
}

void responder::send(const google::protobuf::Message& response) const {
  assert(info_->router_conn);
  int msg_size = response.ByteSize();
  scoped_ptr<zmq::message_t> payload(new zmq::message_t(msg_size));
  if (!response.SerializeToArray(payload->data(), msg_size)) {
    throw invalid_message_error("Invalid response message");
  }
  rpc_header rpc_hdr;
  (void)rpc_hdr.mutable_resp_hdr();
  BOOST_ASSERT(rpc_hdr.has_resp_hdr());
  BOOST_ASSERT(!rpc_hdr.has_req_hdr());
  send(rpc_hdr, payload.release());
}

void responder::send(const std::string& response) const {
  assert(info_->router_conn);
  rpc_header rpc_hdr;
  (void)rpc_hdr.mutable_resp_hdr();
  send(rpc_hdr, string_to_message(response));
}

void responder::send_error(int error_code,
    const std::string& error_message/* = "" */) const {
  assert(info_->router_conn);
  rpc_header rpc_hdr;
  rpc_response_header* resp_hdr = rpc_hdr.mutable_resp_hdr();
  BOOST_ASSERT(resp_hdr);
  zmq::message_t* payload = new zmq::message_t();
  resp_hdr->set_error_code(error_code);
  if (!error_message.empty()) {
    resp_hdr->set_error_str(error_message);
  }
  send(rpc_hdr, payload);
}

// Sends rpc header and payload.
// Takes ownership of the provided payload message.
void responder::send(const rpc_header& rpc_hdr,
                     zmq::message_t* payload) const {
  size_t msg_size = rpc_hdr.ByteSize();
  zmq::message_t* zmq_hdr_msg = new zmq::message_t(msg_size);
  CHECK(rpc_hdr.SerializeToArray(zmq_hdr_msg->data(), msg_size));

  message_vector v;
  v.push_back(zmq_hdr_msg);
  v.push_back(payload);
  assert(info_->router_conn);
  info_->router_conn->reply(info_->event_id, &v);
}

}  // namespace rpcz