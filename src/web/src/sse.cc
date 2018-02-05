/*
 *
 * Copyright (C) 2015-2016 Eaton
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
 * \file sse.cc
 * \author Nicolas DAVIET<nicolasdaviet@Eaton.com>
 * \brief Class and functions use by sse connection
 */


#include "sse.h"
#include "tokens.h"
#include "data.h"
#include "log.h"
#include "utils_json.h"
#include "str_defs.h"

//constructor

Sse::Sse()
{
}

//Clean object in destructor

Sse::~Sse()
{
  if (_clientMlm)
  {
    mlm_client_destroy(&_clientMlm);
  }
  if (_poller)
  {
    zpoller_destroy(&_poller);
  }
}

const char * Sse::connectMalamute()
{

  // connect to malamute
  _clientMlm = mlm_client_new();
  if (!_clientMlm)
  {
    log_critical("mlm_client_new() failed.");
    return "mlm_client_new() failed.";
  }

  std::string client_name = utils::generate_mlm_client_id("web.sse");
  log_debug("malamute client name = '%s'.", client_name.c_str());

  int rv = mlm_client_connect(_clientMlm, MLM_ENDPOINT, 1000, client_name.c_str());
  if (rv == -1)
  {
    log_critical("mlm_client_connect (endpoint = '%s', timeout = '%" PRIu32"', address = '%s') failed.",
                 MLM_ENDPOINT, 1000, client_name.c_str());
    return "mlm_client_connect() failed.";
  }

  _pipe = mlm_client_msgpipe(_clientMlm);
  if (!_pipe)
  {
    log_critical("mlm_client_msgpipe() failed.");
    return "mlm_client_msgpipe()";
  }

  _poller = zpoller_new(_pipe, NULL);
  if (!_poller)
  {
    log_critical("zpoller_new() failed.");
    return "zpoller_new() failed.";
  }

  return NULL;
}

int Sse::consumeStream(std::string stream, std::string pattern)
{
  return mlm_client_set_consumer(_clientMlm, stream.c_str(), pattern.c_str());
}

long int Sse::checkTokenValidity()
{
  long int tme;
  long int uid;
  long int gid;
  char * user_name;

  log_debug("Token : [%s]", _token.c_str());

  if (BiosProfile::Anonymous == tokens::get_instance()->verify_token(_token, &tme, &uid, &gid, &user_name))
  {
    log_info("sse : Token revoked or expired");
    return -1;
  }
  return tme;
}

bool Sse::isMessageComesFrom(const char * channel)
{
  return streq(mlm_client_command(_clientMlm), channel);
}

zmsg_t * Sse::getMessageFromMlm()
{
  return mlm_client_recv(_clientMlm);
}

const char * Sse::loadAssetFromDatacenter()
{

  std::map<std::string, int> elements;
  auto asset_element = persist::select_asset_element_web_byId(_connection, _datacenter_id);
  if (asset_element.status != 1)
  {
    return asset_element.msg.c_str();
  }
  log_debug("Asset element name = '%s'.", asset_element.item.name.c_str());
  elements.emplace(std::make_pair(asset_element.item.name, 5));

  try
  {
    int rv = persist::select_assets_by_container(_connection, _datacenter_id,
                                                 [&elements](const tntdb::Row & row) -> void
                                                 {
                                                   std::string name;
                                                   row[0].get(name);
                                                   elements.emplace(std::make_pair(name, 5));
                                                 });
    if (rv != 0)
    {
      return "persist::select_assets_by_container () failed.";
    }
  }
  catch (const tntdb::Error& e)
  {
    return e.what();
  }
  catch (const std::exception& e)
  {
    return e.what();
  }
  _assetsOfDatacenter = elements;
  log_debug("=== These elements are topologically under element id: '%" PRIu32"' ===", _datacenter_id);
  for (auto const& item : _assetsOfDatacenter)
  {
    log_debug("%s", item.first.c_str());
  }
  log_debug("=== end ===");

  return NULL;
}

std::string Sse::changeFtyProtoAlert2Json(fty_proto_t *alert)
{

  std::string json = "";
  if (_assetsOfDatacenter.find(fty_proto_name(alert)) == _assetsOfDatacenter.end())
  {
    log_debug("skipping due to element_src '%s' not being in the list", fty_proto_name(alert));
    return json;
  }

  std::string jsonPayload = getJsonAlert(_connection, alert);
  if (!jsonPayload.empty())
  {
    const char * rule_name = fty_proto_rule(alert);
    json += "data:{\"topic\":\"alarm/" + std::string(rule_name) + "\",\"payload\":";
    json += jsonPayload;
    json += "}\n\n";
  }

  return json;
}

std::string Sse::changeFtyProtoAsset2Json(fty_proto_t *asset)
{
  //return value
  std::string json = "";

  std::string nameElement = std::string(fty_proto_name(asset));
  //Check operation 
  //if delete send json
  if (streq(fty_proto_operation(asset), FTY_PROTO_ASSET_OP_DELETE))
  {
    log_debug("Sse get an delete message");
    if (_assetsOfDatacenter.find(nameElement) == _assetsOfDatacenter.end())
    {
      log_debug("skipping due to element_src '%s' not being in the list", fty_proto_name(asset));
      return json;
    }
    json += "data:{\"topic\":\"asset/" + nameElement + "\",\"payload\":{}}\n\n";

    //remove this asset from the assets list
    _assetsOfDatacenter.erase(nameElement);
  }
  else if (streq(fty_proto_operation(asset), FTY_PROTO_ASSET_OP_UPDATE)
          || streq(fty_proto_operation(asset), FTY_PROTO_ASSET_OP_CREATE))
  {
    log_debug("Sse get an update or create message");

    if (streq(fty_proto_operation(asset), FTY_PROTO_ASSET_OP_UPDATE))
    {
      //if update
      //Check if asset is in asset element
      if (_assetsOfDatacenter.find(nameElement) == _assetsOfDatacenter.end())
      {
        log_debug("skipping due to element_src '%s' is not an element of the datacenter",
                  nameElement.c_str());
        return json;
      }
    }
    else
    {
      //fty_proto_operation(asset) ==  FTY_PROTO_ASSET_OP_CREATE
      //Check if the parent is the filtered datacenter
      int i = 1;
      const char * parentName = fty_proto_aux_string(asset, ("parent_name." + std::to_string(i)).c_str(), "not found");
      bool found = streq(parentName, _datacenter.c_str());

      while (!found && !streq(parentName, "not found"))
      {
        i++;
        parentName = fty_proto_aux_string(asset, ("parent_name." + std::to_string(i)).c_str(), "not found");
        found = streq(parentName, _datacenter.c_str());
      }

      //if not the same datacenter, return
      if (!found)
      {
        log_debug("skipping due to element_src '%s' is not an element of the datacenter",
                  nameElement.c_str());
        return json;
      }

      //update the asset list
      _assetsOfDatacenter.emplace(std::make_pair(nameElement, 5));
    }

    //get id of this element
    int64_t elemId = persist::name_to_asset_id(nameElement);
    if (elemId == -1)
    {
      log_warning("Asset id not found");
      return json;
    }
    else if (elemId == -2)
    {
      log_warning("Error when get asset id");
    }
    log_debug("Sse-update get id Ok !!!");

    //All check Done and Ok 
    std::string jsonPayload = getJsonAsset(_clientMlm, elemId);

    if (!jsonPayload.empty())
    {
      json += "data:{\"topic\":\"asset/" + nameElement + "\",\"payload\":";
      json += jsonPayload;
      json += "}\n\n";

    }
  }
  return json;
}
