/*
 *
 * Copyright (C) 2018 Eaton
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

/*!
 * \file sse.h
 * \author Nicolas DAVIET <nicolasdaviet@Eaton.com>
 * \brief Header file for sse (Server Side Events) Class
 *
 * How it works
 * ============
 * This files contains class and functions use for the sse connection.
 */

#ifndef SRC_WEB_INCLUDE_SSE_H
#define SRC_WEB_INCLUDE_SSE_H

#include <fty_proto.h>

#include <exception>
#include <string>
#include <map>
#include <functional>
#include <malamute.h>
#include <sys/types.h>
#include <unistd.h>
#include <tntdb/connection.h>
#include <tntdb/error.h>

class Sse
{
private:
  std::string _token;
  std::string _json;
  std::string _datacenter;
  tntdb::Connection _connection;
  std::map<std::string, int> _assetsOfDatacenter;
  uint32_t _datacenter_id;
  mlm_client_t *_clientMlm = NULL;
  zsock_t *_pipe = NULL;
  zpoller_t *_poller = NULL;
  
public:
  //Constructor/destructor
  Sse();
  ~Sse();

  //getter/setter
  zsock_t * getPipe()
  {
    return _pipe;
  };

  zpoller_t *getPoller()
  {
    return _poller;
  };

  void setToken(std::string value)
  {
    _token = value;
  };

  void setDatacenter(std::string value)
  {
    _datacenter = value;
  };

  void setConnection(tntdb::Connection value)
  {
    _connection = value;
  };

  void setDatacenterId(uint32_t value)
  {
    _datacenter_id = value;
  };

  //Connect to malamute and initialize pipe and the poller
  //return Null if connection to malamute is ok, else an error message
  const char * connectMalamute();

  //Add consumer stream to malamute client
  int consumeStream(std::string stream, std::string pattern);

  //Check if the token is still valid
  //return the time in second before expiration or -1 if token isn't valid
  long int checkTokenValidity();

  //Check if the message comes from a channel in parameter
  bool isMessageComesFrom(const char * channel);

  // get the message from malamate
  zmsg_t * getMessageFromMlm();

  //Search all asset included in the datacenter
  //Return null or an error message if error
  const char * loadAssetFromDatacenter();

  //Convert an fty_proto_alert message to json
  //Return an empty string if error
  std::string changeFtyProtoAlert2Json(fty_proto_t *alert);

  //Convert an fty_proto_asset message to json
  //Return an empty string if error
  std::string changeFtyProtoAsset2Json(fty_proto_t *asset);
  
};

#endif // SRC_WEB_INCLUDE_SSE_H

