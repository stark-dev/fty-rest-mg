/*
 *
 * Copyright (C) 2014 - 2020 Eaton
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

#include "shared/configure_inform.h"

#include <malamute.h>
#include <stdexcept>

#include <fty_proto.h>
#include <fty_common_db_dbpath.h>
#include <asset/conversion/proto.h>
#include <fty_common_db_asset_insert.h>
#include <fty_common.h>
#include <fty_common_mlm_utils.h>

static zhash_t*
s_map2zhash (const std::map<std::string, std::string>& m)
{
    zhash_t *ret = zhash_new ();
    zhash_autofree (ret);
    for (const auto& it : m) {
        zhash_insert (ret, ::strdup (it.first.c_str()), ::strdup (it.second.c_str()));
    }
    return ret;
}

void
    send_configure (
        const std::vector <std::pair<db_a_elmnt_t,persist::asset_operation>> &rows,
        const std::string &agent_name)
{
    mlm_client_t *client = mlm_client_new();

    if ( client == NULL ) {
        throw std::runtime_error(" mlm_client_new () failed.");
    }
    int r = mlm_client_connect (client, MLM_ENDPOINT, 1000, agent_name.c_str ());
    if ( r == -1 ) {
        mlm_client_destroy (&client);
        throw std::runtime_error(" mlm_client_connect () failed.");
    }

    r = mlm_client_set_producer (client, FTY_PROTO_STREAM_ASSETS);
    if ( r == -1 ) {
        mlm_client_destroy (&client);
        throw std::runtime_error(" mlm_client_set_producer () failed.");
    }
    tntdb::Connection conn = tntdb::connect (DBConn::url);
    for ( const  auto &oneRow : rows ) {

        std::string s_priority = std::to_string (oneRow.first.priority);
        std::string s_parent = std::to_string (oneRow.first.parent_id);
        std::string s_asset_name = oneRow.first.name;
        std::string s_asset_type = persist::typeid_to_type (oneRow.first.type_id);

        std::string subject;
        subject = persist::typeid_to_type (oneRow.first.type_id);
        subject.append (".");
        subject.append (persist::subtypeid_to_subtype (oneRow.first.subtype_id));
        subject.append ("@");
        subject.append (oneRow.first.name);

        zhash_t *aux = zhash_new ();
        zhash_autofree (aux);
        zhash_insert (aux, "priority", (void*) s_priority.c_str ());
        zhash_insert (aux, "type", (void*) s_asset_type.c_str());
        zhash_insert (aux, "subtype", (void*) persist::subtypeid_to_subtype (oneRow.first.subtype_id).c_str());
        zhash_insert (aux, "parent", (void*) s_parent.c_str ());
        zhash_insert (aux, "status", (void*) oneRow.first.status.c_str());

        // this is a bit hack, but we now that our topology ends with datacenter (hopefully)
        std::string dc_name;

        std::function<void(const tntdb::Row&)> cb = \
            [aux, &dc_name](const tntdb::Row &row) {
            for (const auto& name: {"parent_name1", "parent_name2", "parent_name3", "parent_name4",
                        "parent_name5", "parent_name6", "parent_name7", "parent_name8",
                        "parent_name9", "parent_name10"}) {
                std::string foo;
                row [name].get (foo);
                std::string hash_name = name;
                //                11 == strlen ("parent_name")
                hash_name.insert (11, 1, '.');
                if (!foo.empty ()) {
                    zhash_insert (aux, hash_name.c_str (), (void*) foo.c_str ());
                    dc_name = foo;
                }
            }
        };
        int r = DBAssets::select_asset_element_super_parent (conn, oneRow.first.id, cb);
        if (r == -1) {
            zhash_destroy (&aux);
            mlm_client_destroy (&client);
            throw std::runtime_error ("persist::select_asset_element_super_parent () failed.");
        }

        zhash_t *ext = s_map2zhash (oneRow.first.ext);

        zmsg_t *msg = fty_proto_encode_asset (
                aux,
                oneRow.first.name.c_str(),
                operation2str (oneRow.second).c_str (),
                ext);

        r = mlm_client_send (client, subject.c_str (), &msg);
        if ( r != 0 ) {
            mlm_client_destroy (&client);
            throw std::runtime_error("mlm_client_send () failed.");
        }

        zhash_destroy (&aux);
        zhash_destroy (&ext);

        // ask fty-asset to republish so we would get UUID
        if (streq (operation2str (oneRow.second).c_str (), FTY_PROTO_ASSET_OP_CREATE) ||
            streq (operation2str (oneRow.second).c_str (), FTY_PROTO_ASSET_OP_UPDATE)) {
            zmsg_t *republish = zmsg_new ();
            zmsg_addstr (republish, s_asset_name.c_str ());
            mlm_client_sendto (client, "asset-agent", "REPUBLISH", NULL, 5000, &republish);
        }

        //data for uptime
        if (oneRow.first.subtype_id == persist::asset_subtype::UPS) {
            zhash_t *aux = zhash_new ();

            if (!DBUptime::get_dc_upses (dc_name.c_str(), aux))
                log_error ("Cannot read upses for dc with id = %s", dc_name.c_str ());

            zhash_update (aux, "type", (void*) "datacenter");
            zmsg_t *msg = fty_proto_encode_asset (
                    aux,
                    dc_name.c_str (),
                    "inventory",
                    NULL);
            std::string subject = "datacenter.unknown@";
            subject.append (dc_name);
            r = mlm_client_send (client, subject.c_str (), &msg);
            zhash_destroy (&aux);
            if ( r != 0 ) {
                mlm_client_destroy (&client);
                throw std::runtime_error("mlm_client_send () failed.");
            }
        }
    }

    zclock_sleep (500); // ensure that everything was send before we destroy the client
    mlm_client_destroy (&client);
}

void
    send_configure (
        db_a_elmnt_t row,
        persist::asset_operation action_type,
        const std::string &agent_name)
{
    send_configure(std::vector<std::pair<db_a_elmnt_t,persist::asset_operation>>{std::make_pair(row, action_type)}, agent_name);
}

void
    send_configure (
        const std::pair<db_a_elmnt_t, persist::asset_operation> &row,
        const std::string &agent_name)
{
    send_configure(std::vector<std::pair<db_a_elmnt_t,persist::asset_operation>>{row}, agent_name);
}

void
    send_configure (
        const std::vector<fty::Asset>& assets,
        const std::string &operation,
        const std::string &agent_name)
{
    mlm_client_t *client = mlm_client_new();

    if ( client == NULL ) {
        throw std::runtime_error(" mlm_client_new () failed.");
    }
    int r = mlm_client_connect (client, MLM_ENDPOINT, 1000, agent_name.c_str ());
    if ( r == -1 ) {
        mlm_client_destroy (&client);
        throw std::runtime_error(" mlm_client_connect () failed.");
    }

    r = mlm_client_set_producer (client, FTY_PROTO_STREAM_ASSETS);
    if ( r == -1 ) {
        mlm_client_destroy (&client);
        throw std::runtime_error(" mlm_client_set_producer () failed.");
    }
    tntdb::Connection conn = tntdb::connect (DBConn::url);
    for (const fty::Asset& asset : assets) {

        std::string subject = asset.getAssetType();
        subject.append (".");
        subject.append (asset.getAssetSubtype());
        subject.append ("@");
        subject.append (asset.getInternalName());

        fty_proto_t *proto = fty::conversion::toFtyProto(asset, operation);

        zhash_t *aux;
        zhash_autofree(aux);
        
        zhash_t *ext;
        zhash_autofree(ext);

        aux = fty_proto_get_aux(proto);
        ext = fty_proto_get_ext(proto);

        zmsg_t *msg = fty_proto_encode_asset (
                aux,
                asset.getInternalName().c_str(),
                operation.c_str(),
                ext);
        
        r = mlm_client_send (client, subject.c_str (), &msg);
        if ( r != 0 ) {
            mlm_client_destroy (&client);
            throw std::runtime_error("mlm_client_send () failed.");
        }

        zhash_destroy (&aux);
        zhash_destroy (&ext);

        // ask fty-asset to republish so we would get UUID
        if (operation == FTY_PROTO_ASSET_OP_CREATE ||
            operation == FTY_PROTO_ASSET_OP_UPDATE) {
            zmsg_t *republish = zmsg_new ();
            zmsg_addstr (republish, asset.getInternalName().c_str ());
            mlm_client_sendto (client, "asset-agent", "REPUBLISH", NULL, 5000, &republish);
        }
    }

    zclock_sleep (500); // ensure that everything was send before we destroy the client
    mlm_client_destroy (&client);
}
