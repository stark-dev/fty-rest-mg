/*
 *
 * Copyright (C) 2015 - 2020 Eaton
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
 * \file test-topology-power-from.cc
 * \author Alena Chernikava <AlenaChernikava@Eaton.com>
 * \author Karol Hrdina <KarolHrdina@Eaton.com>
 * \author Michal Hrusecky <MichalHrusecky@Eaton.com>
 * \brief Not yet documented file
 */
#include <catch.hpp>

#include <iostream>
#include <czmq.h>

#include <fty_common.h>

#include "assettopology.h"
#include "common_msg.h"
#include "assetcrud.h"

#include "cleanup.h"

TEST_CASE("Power topology from #1","[db][topology][power][from][power_topology.sql][n1]")
{
    log_info ("=============== POWER FROM #1 ==================");
    _scoped_asset_msg_t* getmsg = asset_msg_new (ASSET_MSG_GET_POWER_FROM);
    assert ( getmsg );
    asset_msg_set_element_id (getmsg, 5019);
//    asset_msg_print (getmsg);

    _scoped_zmsg_t* retTopology = get_return_power_topology_from (url.c_str(), getmsg);
    assert ( retTopology );
    REQUIRE ( is_common_msg (retTopology) );
    _scoped_common_msg_t* cretTopology = common_msg_decode (&retTopology);
    assert ( cretTopology );
//    common_msg_print (cretTopology);
    REQUIRE ( common_msg_errtype (cretTopology) == DB_ERR );
    REQUIRE ( common_msg_errorno (cretTopology) == DB_ERROR_NOTFOUND );

    asset_msg_destroy (&getmsg);
    common_msg_destroy (&cretTopology);
}

TEST_CASE("Power topology from #2","[db][topology][power][from][power_topology.sql][n2]")
{
    log_info ("=============== POWER FROM #2 ==================");
    _scoped_asset_msg_t* getmsg = asset_msg_new (ASSET_MSG_GET_POWER_FROM);
    assert ( getmsg );
    asset_msg_set_element_id (getmsg, 5020);
//    asset_msg_print (getmsg);
    // the expected devices

    std::set<std::tuple<int,std::string,std::string>> sdevices;
    sdevices.insert (std::make_tuple(5020, "UPSFROM2", "ups")); // id,  device_name, device_type_name

    _scoped_zmsg_t* retTopology = get_return_power_topology_from (url.c_str(), getmsg);
    assert ( retTopology );
    REQUIRE ( is_asset_msg (retTopology) );
    _scoped_asset_msg_t* cretTopology = asset_msg_decode (&retTopology);
    assert ( cretTopology );

    // check the devices, should be one
    zframe_t* frame = asset_msg_devices (cretTopology);
#if CZMQ_VERSION_MAJOR == 3
    byte* buffer1 = zframe_data (frame);
    assert ( buffer1 );

    _scoped_zmsg_t *zmsg = zmsg_decode (buffer1, zframe_size (frame));
#else
    _scoped_zmsg_t *zmsg = zmsg_decode (frame);
#endif
    assert ( zmsg );
    assert ( zmsg_is (zmsg) );

    _scoped_zmsg_t* pop = zmsg_popmsg (zmsg);
    // the first device
    REQUIRE ( pop != NULL );

    _scoped_asset_msg_t* item = asset_msg_decode (&pop); // pop is freed
    assert ( item );
//    asset_msg_print (item);

    REQUIRE ( asset_msg_element_id (item) == 5020);
    REQUIRE ( !strcmp(asset_msg_name (item), "UPSFROM2") );
    REQUIRE ( !strcmp(asset_msg_type_name (item), "ups") );
    asset_msg_destroy (&item);

    pop = zmsg_popmsg (zmsg);
    // there is no more devices
    REQUIRE ( pop == NULL );

    // check powers, should be empty
    _scoped_zlist_t* powers = asset_msg_get_powers (cretTopology);
    REQUIRE ( powers );
    REQUIRE ( zlist_size (powers) == 0 );

    zlist_destroy (&powers);

    asset_msg_destroy (&getmsg);
    asset_msg_destroy (&cretTopology);
    zmsg_destroy (&zmsg);
    // TODO need to do smth with buffer ??
}

TEST_CASE("Power topology from #3","[db][topology][power][from][power_topology.sql][n3]")
{
    log_info ("=============== POWER FROM #3 ==================");
    _scoped_asset_msg_t* getmsg = asset_msg_new (ASSET_MSG_GET_POWER_FROM);
    assert ( getmsg );
    asset_msg_set_element_id (getmsg, 5021);
//    asset_msg_print (getmsg);

    _scoped_zmsg_t* retTopology = get_return_power_topology_from (url.c_str(), getmsg);
    assert ( retTopology );
    REQUIRE ( is_asset_msg (retTopology) );
    _scoped_asset_msg_t* cretTopology = asset_msg_decode (&retTopology);
    assert ( cretTopology );

    // check the devices, should be one
    zframe_t* frame = asset_msg_devices (cretTopology);
#if CZMQ_VERSION_MAJOR == 3
    byte* buffer2 = zframe_data (frame);
    assert ( buffer2 );

    _scoped_zmsg_t *zmsg = zmsg_decode ( buffer2, zframe_size (frame));
#else
    _scoped_zmsg_t *zmsg = zmsg_decode (frame);
#endif
    assert ( zmsg );
    assert ( zmsg_is (zmsg) );

    _scoped_zmsg_t* pop = zmsg_popmsg (zmsg);
    // the first device
    REQUIRE ( pop != NULL );

    _scoped_asset_msg_t* item = asset_msg_decode (&pop); // pop is freed
    assert ( item );
//    asset_msg_print (item);

    REQUIRE ( asset_msg_element_id (item) == 5021);
    REQUIRE ( !strcmp(asset_msg_name (item), "UPSFROM3") );
    REQUIRE ( !strcmp(asset_msg_type_name (item), "ups") );
    asset_msg_destroy (&item);

    pop = zmsg_popmsg (zmsg);
    // there is no more devices
    REQUIRE ( pop == NULL );

    // check powers, should be empty
    _scoped_zlist_t* powers = asset_msg_get_powers (cretTopology);
    REQUIRE ( powers );
    REQUIRE ( zlist_size (powers) == 0 );

    zlist_destroy (&powers);

    asset_msg_destroy (&getmsg);
    asset_msg_destroy (&cretTopology);
    zmsg_destroy (&zmsg);
    // TODO need to do smth with buffer ??
}

TEST_CASE("Power topology from #4","[db][topology][power][from][power_topology.sql][n4]")
{
    log_info ("=============== POWER FROM #4 ==================");
    _scoped_asset_msg_t* getmsg = asset_msg_new (ASSET_MSG_GET_POWER_FROM);
    assert ( getmsg );
    asset_msg_set_element_id (getmsg, 5024);
//    asset_msg_print (getmsg);

    // the expected devices
    std::set<std::tuple<int,std::string,std::string>> sdevices;
    sdevices.insert (std::make_tuple(5024, "UPSFROM4", "ups")); // id,  device_name, device_type_name
    sdevices.insert (std::make_tuple(5027, "SINK5", "sink"));

    _scoped_zmsg_t* retTopology = get_return_power_topology_from (url.c_str(), getmsg);
    assert ( retTopology );
    REQUIRE ( is_asset_msg (retTopology) );
    _scoped_asset_msg_t* cretTopology = asset_msg_decode (&retTopology);
    assert ( cretTopology );
//    asset_msg_print (cretTopology);
//    print_frame_devices (asset_msg_devices (cretTopology));

    // check powers, should be one link
    _scoped_zlist_t* powers = asset_msg_get_powers (cretTopology);
    REQUIRE ( powers );

    char first1[17]  = "ABCD:5024:2:5027";//src_socket:src_id:dst_socket:dst_id
    char* first  = (char*) zlist_first  (powers);
    REQUIRE ( first  != NULL );
    REQUIRE ( zlist_next (powers)   == NULL );
    REQUIRE ( strstr(first, first1) == first );

    zlist_destroy (&powers);

    // check the devices, should be two
    zframe_t* frame = asset_msg_devices (cretTopology);
#if CZMQ_VERSION_MAJOR == 3
    byte* buffer3 = zframe_data (frame);
    assert ( buffer3 );

    _scoped_zmsg_t *zmsg = zmsg_decode ( buffer3, zframe_size (frame));
#else
    _scoped_zmsg_t *zmsg = zmsg_decode (frame);
#endif
    assert ( zmsg );
    assert ( zmsg_is (zmsg) );

    _scoped_zmsg_t* pop = NULL;
    int n = sdevices.size();
    for (int i = 1 ; i <= n ; i ++ )
    {
        pop = zmsg_popmsg (zmsg);
        REQUIRE ( pop != NULL );

        _scoped_asset_msg_t* item = asset_msg_decode (&pop); // pop is freed
        assert ( item );
//    asset_msg_print (item);
        auto it = sdevices.find ( std::make_tuple ( asset_msg_element_id (item),
                                                    asset_msg_name (item),
                                                    asset_msg_type_name (item) ));
        REQUIRE ( it != sdevices.end() );
        sdevices.erase (it);
        asset_msg_destroy (&item);
    }

    // there is no more devices
    pop = zmsg_popmsg (zmsg);
    REQUIRE ( pop == NULL );
    REQUIRE ( sdevices.empty() );

    asset_msg_destroy (&getmsg);
    asset_msg_destroy (&cretTopology);
    zmsg_destroy (&zmsg);
}

TEST_CASE("Power topology from #5","[db][topology][power][from][power_topology.sql][n5]")
{
    log_info ("=============== POWER FROM #5 ==================");
    _scoped_asset_msg_t* getmsg = asset_msg_new (ASSET_MSG_GET_POWER_FROM);
    assert ( getmsg );
    asset_msg_set_element_id (getmsg, 5028);
//    asset_msg_print (getmsg);

    // the expected devices
    std::set<std::tuple<int,std::string,std::string>> sdevices;
    sdevices.insert (std::make_tuple(5028, "UPSFROM5", "ups")); // id,  device_name, device_type_name
    sdevices.insert (std::make_tuple(5031, "SINK8", "sink"));
    sdevices.insert (std::make_tuple(5032, "SINK9", "sink"));
    sdevices.insert (std::make_tuple(5033, "SINK10", "sink"));

    //the expected links
    std::set<std::string> spowers;
    spowers.insert (std::string(SRCOUT_DESTIN_IS_NULL) + ":5028:3:5031");
    spowers.insert (std::string(SRCOUT_DESTIN_IS_NULL) + ":5028:" + std::string(SRCOUT_DESTIN_IS_NULL) + ":5032");
    spowers.insert ("4:5028:" + std::string(SRCOUT_DESTIN_IS_NULL) + ":5033");

    _scoped_zmsg_t* retTopology = get_return_power_topology_from (url.c_str(), getmsg);
    assert ( retTopology );
    REQUIRE ( is_asset_msg (retTopology) );
    _scoped_asset_msg_t* cretTopology = asset_msg_decode (&retTopology);
    assert ( cretTopology );
//    asset_msg_print (cretTopology);
//    print_frame_devices (asset_msg_devices (cretTopology));

    // check powers, should be three links
    _scoped_zlist_t* powers = asset_msg_get_powers (cretTopology);
    REQUIRE ( powers );

    char* a = NULL;
    int n = zlist_size(powers);
    for ( int i = 1; i <= n; i++ )
    {
        if ( i == 1 )
            a = (char*)zlist_first (powers);
        else
            a = (char*)zlist_next (powers);
        REQUIRE ( a != NULL );
        auto it = spowers.find (std::string(a));
        REQUIRE ( it != spowers.end() );
        spowers.erase(it);
    }
    REQUIRE ( zlist_next (powers) == NULL );
    REQUIRE ( spowers.empty() );
    zlist_destroy (&powers);

    // check the devices, should be four devices
    zframe_t* frame = asset_msg_devices (cretTopology);
#if CZMQ_VERSION_MAJOR == 3
    byte* buffer4 = zframe_data (frame);
    assert ( buffer4 );

    _scoped_zmsg_t *zmsg = zmsg_decode ( buffer4, zframe_size (frame));
#else
    _scoped_zmsg_t *zmsg = zmsg_decode (frame);
#endif
    assert ( zmsg );
    assert ( zmsg_is (zmsg) );

    _scoped_zmsg_t* pop = NULL;
    n = sdevices.size();
    for (int i = 1 ; i <= n ; i ++ )
    {
        pop = zmsg_popmsg (zmsg);
        REQUIRE ( pop != NULL );

        _scoped_asset_msg_t* item = asset_msg_decode (&pop); // pop is freed
        assert ( item );
//    asset_msg_print (item);
        auto it = sdevices.find ( std::make_tuple ( asset_msg_element_id (item),
                                                    asset_msg_name (item),
                                                    asset_msg_type_name (item) ));
        REQUIRE ( it != sdevices.end() );
        sdevices.erase (it);
        asset_msg_destroy (&item);
    }

    // there is no more devices
    pop = zmsg_popmsg (zmsg);
    REQUIRE ( pop == NULL );
    REQUIRE ( sdevices.empty() );


    asset_msg_destroy (&getmsg);
    asset_msg_destroy (&cretTopology);
    zmsg_destroy (&zmsg);
}

TEST_CASE("Power topology from #6","[db][topology][power][from][power_topology.sql][n6]")
{
    log_info ("=============== POWER FROM #6 ==================");
    _scoped_asset_msg_t* getmsg = asset_msg_new (ASSET_MSG_GET_POWER_FROM);
    assert ( getmsg );
    asset_msg_set_element_id (getmsg, 5034);
//    asset_msg_print (getmsg);

    // the expected devices
    std::set<std::tuple<int,std::string,std::string>> sdevices;
    sdevices.insert (std::make_tuple(5034, "UPSFROM6", "ups")); // id,  device_name, device_type_name
    sdevices.insert (std::make_tuple(5035, "SINK11", "sink"));
    sdevices.insert (std::make_tuple(5036, "SINK12", "sink"));
    sdevices.insert (std::make_tuple(5037, "SINK13", "sink"));

    //the expected links
    std::set<std::string> spowers;
    spowers.insert (std::string(SRCOUT_DESTIN_IS_NULL) + ":5034:" + std::string(SRCOUT_DESTIN_IS_NULL) + ":5035");
    spowers.insert (std::string(SRCOUT_DESTIN_IS_NULL) + ":5034:" + std::string(SRCOUT_DESTIN_IS_NULL) + ":5036");
    spowers.insert (std::string(SRCOUT_DESTIN_IS_NULL) + ":5034:" + std::string(SRCOUT_DESTIN_IS_NULL) + ":5037");

    _scoped_zmsg_t* retTopology = get_return_power_topology_from (url.c_str(), getmsg);
    assert ( retTopology );
    REQUIRE ( is_asset_msg (retTopology) );
    _scoped_asset_msg_t* cretTopology = asset_msg_decode (&retTopology);
    assert ( cretTopology );
//    asset_msg_print (cretTopology);
//    print_frame_devices (asset_msg_devices (cretTopology));

    // check powers, should be three links
    _scoped_zlist_t* powers = asset_msg_get_powers (cretTopology);
    REQUIRE ( powers );

    char* a = NULL;
    int n = zlist_size(powers);
    for ( int i = 1; i <= n; i++ )
    {
        if ( i == 1 )
            a = (char*)zlist_first (powers);
        else
            a = (char*)zlist_next (powers);
        REQUIRE ( a != NULL );
        auto it = spowers.find (std::string(a));
        REQUIRE ( it != spowers.end() );
        spowers.erase(it);
    }
    REQUIRE ( zlist_next (powers) == NULL );
    REQUIRE ( spowers.empty() );

    zlist_destroy (&powers);

    // check the devices, should be four devices
    zframe_t* frame = asset_msg_devices (cretTopology);
#if CZMQ_VERSION_MAJOR == 3
    byte* buffer5 = zframe_data (frame);
    assert ( buffer5 );

    _scoped_zmsg_t *zmsg = zmsg_decode ( buffer5, zframe_size (frame));
#else
    _scoped_zmsg_t *zmsg = zmsg_decode (frame);
#endif
    assert ( zmsg );
    assert ( zmsg_is (zmsg) );

    _scoped_zmsg_t* pop = NULL;
    n = sdevices.size();
    for (int i = 1 ; i <= n ; i ++ )
    {
        pop = zmsg_popmsg (zmsg);
        REQUIRE ( pop != NULL );

        _scoped_asset_msg_t* item = asset_msg_decode (&pop); // pop is freed
        assert ( item );
//    asset_msg_print (item);
        auto it = sdevices.find ( std::make_tuple ( asset_msg_element_id (item),
                                                    asset_msg_name (item),
                                                    asset_msg_type_name (item) ));
        REQUIRE ( it != sdevices.end() );
        sdevices.erase (it);
        asset_msg_destroy (&item);
    }

    // there is no more devices
    pop = zmsg_popmsg (zmsg);
    REQUIRE ( pop == NULL );
    REQUIRE ( sdevices.empty() );

    asset_msg_destroy (&getmsg);
    asset_msg_destroy (&cretTopology);
    zmsg_destroy (&zmsg);
}

TEST_CASE("Power topology from #7","[db][topology][power][from][power_topology.sql][n7]")
{
    log_info ("=============== POWER FROM #7 ==================");
    _scoped_asset_msg_t* getmsg = asset_msg_new (ASSET_MSG_GET_POWER_FROM);
    assert ( getmsg );
    asset_msg_set_element_id (getmsg, 5038);
//    asset_msg_print (getmsg);

    // the expected devices
    std::set<std::tuple<int,std::string,std::string>> sdevices;
    sdevices.insert (std::make_tuple(5038, "UPSFROM7", "ups")); // id,  device_name, device_type_name
    sdevices.insert (std::make_tuple(5039, "SINK14", "sink"));

    _scoped_zmsg_t* retTopology = get_return_power_topology_from (url.c_str(), getmsg);
    assert ( retTopology );
    REQUIRE ( is_asset_msg (retTopology) );
    _scoped_asset_msg_t* cretTopology = asset_msg_decode (&retTopology);
    assert ( cretTopology );
//    asset_msg_print (cretTopology);
//    print_frame_devices (asset_msg_devices (cretTopology));

    // check powers, should be one link
    _scoped_zlist_t* powers = asset_msg_get_powers (cretTopology);
    REQUIRE ( powers );

    std::string first1  = std::string(SRCOUT_DESTIN_IS_NULL) + ":5038:" + std::string(SRCOUT_DESTIN_IS_NULL) + ":5039";//src_socket:src_id:dst_socket:dst_id
    char* first  = (char*) zlist_first  (powers);
    REQUIRE ( first  != NULL );
    REQUIRE ( zlist_next (powers)   == NULL );
    REQUIRE ( first1.compare(std::string(first)) == 0 );

    zlist_destroy (&powers);

    // check the devices, should be two devices
    zframe_t* frame = asset_msg_devices (cretTopology);
#if CZMQ_VERSION_MAJOR == 3
    byte* buffer6 = zframe_data (frame);
    assert ( buffer6 );

    _scoped_zmsg_t *zmsg = zmsg_decode ( buffer6, zframe_size (frame));
#else
    _scoped_zmsg_t *zmsg = zmsg_decode (frame);
#endif
    assert ( zmsg );
    assert ( zmsg_is (zmsg) );

    _scoped_zmsg_t* pop = NULL;
    int n = sdevices.size();
    for (int i = 1 ; i <= n ; i ++ )
    {
        pop = zmsg_popmsg (zmsg);
        REQUIRE ( pop != NULL );

        _scoped_asset_msg_t* item = asset_msg_decode (&pop); // pop is freed
        assert ( item );
//    asset_msg_print (item);
        auto it = sdevices.find ( std::make_tuple ( asset_msg_element_id (item),
                                                    asset_msg_name (item),
                                                    asset_msg_type_name (item) ));
        REQUIRE ( it != sdevices.end() );
        sdevices.erase (it);
        asset_msg_destroy (&item);
    }

    // there is no more devices
    pop = zmsg_popmsg (zmsg);
    REQUIRE ( pop == NULL );
    REQUIRE ( sdevices.empty() );

    asset_msg_destroy (&getmsg);
    asset_msg_destroy (&cretTopology);
    zmsg_destroy (&zmsg);
}

TEST_CASE("Power topology from #8","[db][topology][power][from][power_topology.sql][n8]")
{
    log_info ("=============== POWER FROM #8 ==================");
    _scoped_asset_msg_t* getmsg = asset_msg_new (ASSET_MSG_GET_POWER_FROM);
    assert ( getmsg );
    asset_msg_set_element_id (getmsg, 4998);
//    asset_msg_print (getmsg);

    _scoped_zmsg_t* retTopology = get_return_power_topology_from (url.c_str(), getmsg);
    assert ( retTopology );

    REQUIRE ( is_common_msg (retTopology) );
    _scoped_common_msg_t* cretTopology = common_msg_decode (&retTopology);
    assert ( cretTopology );
//    common_msg_print (cretTopology);
    REQUIRE ( common_msg_errtype (cretTopology) == DB_ERR );
    REQUIRE ( common_msg_errorno (cretTopology) == DB_ERROR_BADINPUT );

    asset_msg_destroy (&getmsg);
    common_msg_destroy (&cretTopology);
}


TEST_CASE("Power topology from #9","[db][topology][power][from][power_topology.sql][n9]")
{
    log_info ("=============== POWER FROM #9 ==================");
    _scoped_asset_msg_t* getmsg = asset_msg_new (ASSET_MSG_GET_POWER_FROM);
    assert ( getmsg );
    asset_msg_set_element_id (getmsg, 5040);
//    asset_msg_print (getmsg);

    // the expected devices
    std::set<std::tuple<int,std::string,std::string>> sdevices;
    sdevices.insert (std::make_tuple(5040, "UPSFROM9", "ups")); // id,  device_name, device_type_name

    // the expected links
    std::set<std::string> spowers;
    spowers.insert ("5:5040:6:5040");

    _scoped_zmsg_t* retTopology = get_return_power_topology_from (url.c_str(), getmsg);
    assert ( retTopology );
    REQUIRE ( is_asset_msg (retTopology) );
    _scoped_asset_msg_t* cretTopology = asset_msg_decode (&retTopology);
//    asset_msg_print (cretTopology);

    // check the devices, should be one device
    zframe_t* frame = asset_msg_devices (cretTopology);
#if CZMQ_VERSION_MAJOR == 3
    byte* buffer7 = zframe_data (frame);
    assert ( buffer7 );

    _scoped_zmsg_t *zmsg = zmsg_decode ( buffer7, zframe_size (frame));
#else
    _scoped_zmsg_t *zmsg = zmsg_decode (frame);
#endif
    assert ( zmsg );
    assert ( zmsg_is (zmsg) );

    _scoped_zmsg_t* pop = NULL;
    int n = sdevices.size();
    for (int i = 1 ; i <= n ; i ++ )
    {
        pop = zmsg_popmsg (zmsg);
        REQUIRE ( pop != NULL );

        _scoped_asset_msg_t* item = asset_msg_decode (&pop); // pop is freed
        assert ( item );
//    asset_msg_print (item);
        auto it = sdevices.find ( std::make_tuple ( asset_msg_element_id (item),
                                                    asset_msg_name (item),
                                                    asset_msg_type_name (item) ));
        REQUIRE ( it != sdevices.end() );
        sdevices.erase (it);
        asset_msg_destroy (&item);
    }

    // there is no more devices
    pop = zmsg_popmsg (zmsg);
    REQUIRE ( pop == NULL );
    REQUIRE ( sdevices.empty() );

    // check powers, should be one link
    _scoped_zlist_t* powers = asset_msg_get_powers (cretTopology);
    REQUIRE ( powers );

    REQUIRE ( spowers.count(std::string ((const char*)zlist_first  (powers))) == 1 );
    REQUIRE ( zlist_next (powers)   == NULL );

    zlist_destroy (&powers);

    asset_msg_destroy (&getmsg);
    asset_msg_destroy (&cretTopology);
    zmsg_destroy (&zmsg);
}

TEST_CASE("Power topology from #10","[db][topology][power][from][power_topology.sql][n10]")
{
    log_info ("=============== POWER FROM #10 ==================");
    _scoped_asset_msg_t* getmsg = asset_msg_new (ASSET_MSG_GET_POWER_FROM);
    assert ( getmsg );
    asset_msg_set_element_id (getmsg, 5041);
//    asset_msg_print (getmsg);

    // the expected devices
    std::set<std::tuple<int,std::string,std::string>> sdevices;
    sdevices.insert (std::make_tuple(5041, "UPSFROM10", "ups")); // id,  device_name, device_type_name
    sdevices.insert (std::make_tuple(5042, "SINK15", "sink"));

    // the expected links
    std::set<std::string> spowers;
    spowers.insert (std::string(SRCOUT_DESTIN_IS_NULL) + ":5041:" + std::string(SRCOUT_DESTIN_IS_NULL) + ":5042");
    spowers.insert ("5:5041:" + std::string(SRCOUT_DESTIN_IS_NULL) + ":5042");

    _scoped_zmsg_t* retTopology = get_return_power_topology_from (url.c_str(), getmsg);
    assert ( retTopology );
    REQUIRE ( is_asset_msg (retTopology) );
    _scoped_asset_msg_t* cretTopology = asset_msg_decode (&retTopology);
    assert ( cretTopology );
//    asset_msg_print (cretTopology);

    // check powers, should be one link
    _scoped_zlist_t* powers = asset_msg_get_powers (cretTopology);
    REQUIRE ( powers );

    char* a = NULL;
    int n = zlist_size(powers);
    for ( int i = 1; i <= n; i++ )
    {
        if ( i == 1 )
            a = (char*)zlist_first (powers);
        else
            a = (char*)zlist_next (powers);
        REQUIRE ( a != NULL );
        auto it = spowers.find (std::string(a));
        REQUIRE ( it != spowers.end() );
        spowers.erase(it);
    }
    REQUIRE ( zlist_next (powers) == NULL );
    REQUIRE ( spowers.empty() );

    zlist_destroy (&powers);

    // check the devices, should be two devices
    zframe_t* frame = asset_msg_devices (cretTopology);
#if CZMQ_VERSION_MAJOR == 3
    byte* buffer8 = zframe_data (frame);
    assert ( buffer8 );

    _scoped_zmsg_t *zmsg = zmsg_decode ( buffer8, zframe_size (frame));
#else
    _scoped_zmsg_t *zmsg = zmsg_decode (frame);
#endif
    assert ( zmsg );
    assert ( zmsg_is (zmsg) );

    _scoped_zmsg_t* pop = NULL;
    n = sdevices.size();
    for (int i = 1 ; i <= n ; i ++ )
    {
        pop = zmsg_popmsg (zmsg);
        REQUIRE ( pop != NULL );

        _scoped_asset_msg_t* item = asset_msg_decode (&pop); // pop is freed
        assert ( item );
//    asset_msg_print (item);
        auto it = sdevices.find ( std::make_tuple ( asset_msg_element_id (item),
                                                    asset_msg_name (item),
                                                    asset_msg_type_name (item) ));
        REQUIRE ( it != sdevices.end() );
        sdevices.erase (it);
        asset_msg_destroy (&item);
    }

    // there is no more devices
    pop = zmsg_popmsg (zmsg);
    REQUIRE ( pop == NULL );
    REQUIRE ( sdevices.empty() );

    asset_msg_destroy (&getmsg);
    asset_msg_destroy (&cretTopology);
    zmsg_destroy (&zmsg);
}

TEST_CASE("Power topology from #11","[db][topology][power][from][power_topology.sql][n11]")
{
    log_info ("=============== POWER FROM #11 ==================");
    _scoped_asset_msg_t* getmsg = asset_msg_new (ASSET_MSG_GET_POWER_FROM);
    assert ( getmsg );
    asset_msg_set_element_id (getmsg, 5043);
//    asset_msg_print (getmsg);

    // the expected devices
    std::set<std::tuple<int,std::string,std::string>> sdevices;
    sdevices.insert (std::make_tuple(5043, "UPSFROM11", "ups")); // id,  device_name, device_type_name
    sdevices.insert (std::make_tuple(5044, "SINK16", "sink"));

    // the expected links
    std::set<std::string> spowers;
    spowers.insert (std::string(SRCOUT_DESTIN_IS_NULL) + ":5043:" + std::string(SRCOUT_DESTIN_IS_NULL) + ":5044");

    _scoped_zmsg_t* retTopology = get_return_power_topology_from (url.c_str(), getmsg);
    assert ( retTopology );
    REQUIRE ( is_asset_msg (retTopology) );
    _scoped_asset_msg_t* cretTopology = asset_msg_decode (&retTopology);
    assert ( cretTopology );
//    asset_msg_print (cretTopology);

    // check powers, should be one link
    _scoped_zlist_t* powers = asset_msg_get_powers (cretTopology);
    REQUIRE ( powers );

    char* a = NULL;
    int n = zlist_size(powers);
    for ( int i = 1; i <= n; i++ )
    {
        if ( i == 1 )
            a = (char*)zlist_first (powers);
        else
            a = (char*)zlist_next (powers);
        REQUIRE ( a != NULL );
        auto it = spowers.find (std::string(a));
        REQUIRE ( it != spowers.end() );
        spowers.erase(it);
    }
    REQUIRE ( zlist_next (powers) == NULL );
    REQUIRE ( spowers.empty() );

    zlist_destroy (&powers);

    // check the devices, should be two
    zframe_t* frame = asset_msg_devices (cretTopology);
#if CZMQ_VERSION_MAJOR == 3
    byte* buffer9 = zframe_data (frame);
    assert ( buffer9 );

    _scoped_zmsg_t *zmsg = zmsg_decode ( buffer9, zframe_size (frame));
#else
    _scoped_zmsg_t *zmsg = zmsg_decode (frame);
#endif
    assert ( zmsg );
    assert ( zmsg_is (zmsg) );

    _scoped_zmsg_t* pop = NULL;
    n = sdevices.size();
    for (int i = 1 ; i <= n ; i ++ )
    {
        pop = zmsg_popmsg (zmsg);
        REQUIRE ( pop != NULL );

        _scoped_asset_msg_t* item = asset_msg_decode (&pop); // pop is freed
        assert ( item );
//    asset_msg_print (item);
        auto it = sdevices.find ( std::make_tuple ( asset_msg_element_id (item),
                                                    asset_msg_name (item),
                                                    asset_msg_type_name (item) ));
        REQUIRE ( it != sdevices.end() );
        sdevices.erase (it);
        asset_msg_destroy (&item);
    }

    // there is no more devices
    pop = zmsg_popmsg (zmsg);
    REQUIRE ( pop == NULL );
    REQUIRE ( sdevices.empty() );

    asset_msg_destroy (&getmsg);
    asset_msg_destroy (&cretTopology);
    zmsg_destroy (&zmsg);
}

