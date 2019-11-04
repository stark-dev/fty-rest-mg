////////////////////////////////////////////////////////////////////////
// ./src/web/src/topology_xpower.cc.cpp
// generated with ecppc
//

#include <tnt/ecpp.h>
#include <tnt/convert.h>
#include <tnt/httprequest.h>
#include <tnt/httpreply.h>
#include <tnt/httpheader.h>
#include <tnt/http.h>
#include <tnt/data.h>
#include <tnt/componentfactory.h>
#include <stdexcept>

// <%pre>
#line 30 "./src/web/src/topology_xpower.ecpp"

#include <string>
#include <sstream>
#include <vector>
#include <czmq.h>
#include <fty_common.h>
#include <fty_common_macros.h>
#include <fty_common_rest_helpers.h>
#include <fty_common_db.h>

#define __malamute_topology_power__
//#undef __malamute_topology_power__

#ifdef __malamute_topology_power__
    #include <fty_common_mlm_tntmlm.h>
#else
    #include <fty_common_db_dbpath.h>
    #include "src/persist/assettopology.h"
    #include "cleanup.h"
    #include "shared/utilspp.h"
#endif

// set S if MSG popped frame (no memleak, S untouched if no msg frame)
void zmsg_pop_s (zmsg_t *msg, std::string & s)
{
    char *aux = msg ? zmsg_popstr(msg) : NULL;
    if (aux) s = aux;
    zstr_free(&aux);
}

// </%pre>

namespace
{
class _component_ : public tnt::EcppComponent
{
    _component_& main()  { return *this; }

  protected:
    ~_component_();

  public:
    _component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl);

    unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);
};

static tnt::ComponentFactoryImpl<_component_> Factory("topology_xpower");

static const char* rawData = "\010\000\000\000\t\000\000\000\n";

// <%shared>
// </%shared>

// <%config>
// </%config>

#define SET_LANG(lang) \
     do \
     { \
       request.setLang(lang); \
       reply.setLocale(request.getLocale()); \
     } while (false)

_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)
  : EcppComponent(ci, um, cl)
{
  // <%init>
  // </%init>
}

_component_::~_component_()
{
  // <%cleanup>
  // </%cleanup>
}

unsigned _component_::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)
 {
  tnt::DataChunks data(rawData);

#line 61 "./src/web/src/topology_xpower.ecpp"
  typedef UserInfo user_type;
  TNT_REQUEST_GLOBAL_VAR(user_type, user, "UserInfo user", ());   // <%request> UserInfo user
#line 62 "./src/web/src/topology_xpower.ecpp"
  typedef bool database_ready_type;
  TNT_REQUEST_GLOBAL_VAR(database_ready_type, database_ready, "bool database_ready", ());   // <%request> bool database_ready
  // <%cpp>
#line 64 "./src/web/src/topology_xpower.ecpp"

{
    // verify server is ready
    if (!database_ready) {
        log_debug ("Database is not ready yet.");
        std::string err = TRANSLATE_ME("Database is not ready yet, please try again in a while.");
        http_die ("internal-error", err.c_str ());
    }

    // check user permissions
    static const std::map <BiosProfile, std::string> PERMISSIONS = {
        {BiosProfile::Dashboard, "R"},
        {BiosProfile::Admin,     "R"}
    };
    CHECK_USER_PERMISSIONS_OR_DIE (PERMISSIONS);

#ifdef __malamute_topology_power__
    const char *ADDRESS = AGENT_FTY_ASSET; // 42ty/fty-asset/asset-agent
    const char *SUBJECT = "TOPOLOGY";
    const char *COMMAND = "POWERCHAINS";

    ftylog_setVeboseMode(ftylog_getInstance());
    log_trace ("in %s", request.getUrl().c_str ());

    // get params
    std::string parameter_name;
    std::string asset_id;
    {
        std::string from = qparam.param("from");
        std::string to = qparam.param("to");
        std::string filter_dc = qparam.param("filter_dc");
        std::string filter_gr = qparam.param("filter_group");

        // not-empty count
        int ne_count = (from.empty()?0:1) + (to.empty()?0:1) + (filter_dc.empty()?0:1) + (filter_gr.empty()?0:1);
        if (ne_count != 1) {
            std::string err = TRANSLATE_ME("Only one parameter can be specified at once: 'from' or 'to' or 'filter_dc' or 'filter_group'");
            http_die("parameter-conflict", err.c_str ());
        }

        if (!from.empty()) {
            parameter_name = "from";
            asset_id = from;
        }
        else if (!to.empty()) {
            parameter_name = "to";
            asset_id = to;
        }
        else if (!filter_dc.empty()) {
            parameter_name = "filter_dc";
            asset_id = filter_dc;
        }
        else if (!filter_gr.empty()) {
            parameter_name = "filter_group";
            asset_id = filter_gr;
        }
        else {
            log_error ("unexpected parameter");
            http_die ("internal-error", "unexpected parameter");
        }
    }

    log_trace ("%s, parameter_name: '%s', asset_id: '%s'",
        request.getUrl().c_str (), parameter_name.c_str (), asset_id.c_str ());

    // check asset_id
    {
        // asset_id valid?
        if (!persist::is_ok_name (asset_id.c_str ()) ) {
            std::string expected = TRANSLATE_ME("valid asset name");
            http_die ("request-param-bad", "id", asset_id.c_str (), expected.c_str ());
        }
        // asset_id exist?
        int64_t rv = DBAssets::name_to_asset_id (asset_id);
        if (rv == -1) {
            std::string expected = TRANSLATE_ME("existing asset name");
            http_die ("request-param-bad", "id", asset_id.c_str (), expected.c_str ());
        }
        if (rv == -2) {
            std::string err =  TRANSLATE_ME("Connection to database failed.");
            http_die ("internal-error", err.c_str ());
        }
    }

    // connect to mlm client
    MlmClientPool::Ptr client = mlm_pool.get ();
    if (!client.getPointer ()) {
        log_error ("mlm_pool.get () failed");
        http_die ("internal-error", "connect to client failed");
    }

    // set/send req, recv response
    zmsg_t *req = zmsg_new ();
    if (!req) {
        log_error ("zmsg_new () failed");
        http_die ("internal-error", "zmsg alloc. failed");
    }

    zmsg_addstr (req, COMMAND);
    zmsg_addstr (req, parameter_name.c_str ());
    zmsg_addstr (req, asset_id.c_str ());
    zmsg_t *resp = client->requestreply (ADDRESS, SUBJECT, 5, &req);
    zmsg_destroy (&req);

    #define CLEANUP { zmsg_destroy (&resp); }

    if (!resp) {
        CLEANUP;
        log_error ("client->requestreply (timeout = '5') failed");
        http_die ("internal-error", "request to client failed (timeout reached)");
    }

    // get resp. header
    std::string rx_command, rx_asset_id, rx_status;
    zmsg_pop_s(resp, rx_command);
    zmsg_pop_s(resp, rx_asset_id);
    zmsg_pop_s(resp, rx_status);

    if (rx_command != COMMAND) {
        char err[64];
        snprintf(err, sizeof(err), "inconsistent command received ('%s')", rx_command.c_str ());
        CLEANUP;
        log_error (err);
        http_die ("internal-error", err);
    }
    if (rx_asset_id != asset_id) {
        char err[64];
        snprintf(err, sizeof(err), "inconsistent assetID received ('%s')", rx_asset_id.c_str ());
        CLEANUP;
        log_error (err);
        http_die ("internal-error", err);
    }
    if (rx_status != "OK") {
        std::string reason;
        zmsg_pop_s(resp, reason);
        char err[64];
        snprintf(err, sizeof(err), "received %s status (reason: %s) from client", rx_status.c_str(), reason.c_str ());
        CLEANUP;
        log_error (err);
        http_die ("internal-error", err);
    }

    // result JSON payload
    std::string json;
    zmsg_pop_s(resp, json);
    if (json.empty()) {
        CLEANUP;
        log_error ("empty JSON payload");
        http_die ("internal-error", "result returned by client is empty");
    }
    CLEANUP;

    // set body (req. status is 200 OK)
    reply.out () << json;

#else //__malamute_topology_power__

    // checked parameters
    int64_t checked_asset_id;
    int request_type = 0;
    std::string asset_id;
    std::string parameter_name;

    // ##################################################
    // BLOCK 1
    // Sanity parameter check
    {
        std::string from = qparam.param("from");
        std::string to = qparam.param("to");
        std::string filter_dc = qparam.param("filter_dc");
        std::string filter_group = qparam.param("filter_group");

        if ( from.empty() && to.empty() && filter_dc.empty() && filter_group.empty() ) {
            http_die("request-param-required", "from/to/filter_dc/filter_group");
        }

        if (!from.empty()) {
            if (!to.empty() || !filter_dc.empty() || !filter_group.empty()) {
                std::string err =  TRANSLATE_ME("Only one parameter can be specified at once: 'from' or 'to' or 'filter_dc' or 'filter_group'");
                http_die("parameter-conflict", err.c_str ());
            }
            request_type = ASSET_MSG_GET_POWER_FROM;
            asset_id = from;
            parameter_name = "from";
        }

        if (!to.empty()) {
            if ( !filter_dc.empty() || !filter_group.empty() || !from.empty() ) {
                std::string err =  TRANSLATE_ME("Only one parameter can be specified at once: 'from' or 'to' or 'filter_dc' or 'filter_group'");
                http_die("parameter-conflict", err.c_str ());
            }
            request_type = ASSET_MSG_GET_POWER_TO;
            asset_id = to;
            parameter_name = "to";
        }

        if (!filter_dc.empty()) {
            if (!filter_group.empty() || !to.empty() || !from.empty()) {
                std::string err =  TRANSLATE_ME("Only one parameter can be specified at once: 'from' or 'to' or 'filter_dc' or 'filter_group'");
                http_die("parameter-conflict", err.c_str ());
            }
            request_type = ASSET_MSG_GET_POWER_DATACENTER;
            asset_id = filter_dc;
            parameter_name = "filter_dc";
        }

        if (!filter_group.empty()) {
            if (!to.empty() || !from.empty() || !filter_dc.empty()) {
                std::string err =  TRANSLATE_ME("Only one parameter can be specified at once: 'from' or 'to' or 'filter_dc' or 'filter_group'");
                http_die("parameter-conflict", err.c_str ());
            }
            request_type = ASSET_MSG_GET_POWER_GROUP;
            asset_id = filter_group;
            parameter_name = "filter_group";
        }

        if (!persist::is_ok_name (asset_id.c_str ()) ) {
            std::string expected = TRANSLATE_ME("valid asset name");
            http_die ("request-param-bad", "id", asset_id.c_str (), expected.c_str ());
        }

        checked_asset_id = DBAssets::name_to_asset_id (asset_id);
        if (checked_asset_id == -1) {
            std::string expected = TRANSLATE_ME("existing asset name");
            http_die ("request-param-bad", "id", asset_id.c_str (), expected.c_str ());
        }
        if (checked_asset_id == -2) {
            std::string err =  TRANSLATE_ME("Connecting to database failed.");
            http_die ("internal-error", err.c_str ());
        }
    }
    // Sanity check end

    // ##################################################
    // BLOCK 2
    _scoped_asset_msg_t *input_msg = asset_msg_new (request_type);
    if ( !input_msg ) {
        std::string err =  TRANSLATE_ME("Cannot allocate memory");
        http_die ("internal-error", err.c_str ());
    }

    // Call persistence layer
    asset_msg_set_element_id (input_msg,  (uint32_t) checked_asset_id);
    _scoped_zmsg_t *return_msg = process_assettopology (DBConn::url.c_str(), &input_msg);
    if (return_msg == NULL) {
        log_error ("Function process_assettopology() returned a null pointer");
        http_die("internal-error", "");
    }

    if (is_common_msg (return_msg)) {
        _scoped_common_msg_t *common_msg = common_msg_decode (&return_msg);
        if (common_msg == NULL) {
            if (return_msg != NULL) {
                zmsg_destroy (&return_msg);
            }
            log_error ("common_msg_decode() failed");
            http_die("internal-error", "");
        }

        if (common_msg_id (common_msg) == COMMON_MSG_FAIL) {
            log_error ("common_msg is COMMON_MSG_FAIL");
            switch(common_msg_errorno(common_msg)) {
                case(DB_ERROR_BADINPUT):
                {
                    std::string received = TRANSLATE_ME("id of the asset, that is not a device");
                    std::string expected = TRANSLATE_ME("id of the asset, that is a device");
                    http_die("request-param-bad", parameter_name.c_str(), received.c_str (), expected.c_str ());
                }
                case(DB_ERROR_NOTFOUND):
                    http_die("element-not-found", asset_id.c_str());
                default:
                    http_die("internal-error", "");
            }
        }
        else {
            log_error ("Unexpected common_msg received. ID = %" PRIu32 , common_msg_id (common_msg));
            http_die("internal-error", "");
        }
    }
    else if (is_asset_msg (return_msg)) {
        _scoped_asset_msg_t *asset_msg = asset_msg_decode (&return_msg);

        if (asset_msg == NULL) {
            if (return_msg != NULL) {
                zmsg_destroy (&return_msg);
            }
            log_error ("asset_msg_decode() failed");
            http_die("internal-error", "");
        }

        if (asset_msg_id (asset_msg) == ASSET_MSG_RETURN_POWER) {
            _scoped_zlist_t *powers = asset_msg_get_powers (asset_msg);
            _scoped_zframe_t *devices = asset_msg_get_devices (asset_msg);
            asset_msg_destroy (&asset_msg);

            std::string json = "{";
            if (devices) {
#if CZMQ_VERSION_MAJOR == 3
                byte *buffer = zframe_data (devices);
                assert (buffer);
                _scoped_zmsg_t *zmsg = zmsg_decode ( buffer, zframe_size (devices));
#else
                _scoped_zmsg_t *zmsg = zmsg_decode (devices);
#endif
                if (zmsg == NULL || !zmsg_is (zmsg)) {
                    zframe_destroy (&devices);
                    log_error ("zmsg_decode() failed");
                    http_die("internal-error", "");
                }
                zframe_destroy (&devices);

                json.append ("\"devices\" : [");
                _scoped_zmsg_t *pop = NULL;
                bool first = true;
                while ((pop = zmsg_popmsg (zmsg)) != NULL) { // caller owns zmgs_t
                    if (!is_asset_msg (pop)) {
                        zmsg_destroy (&zmsg);
                        log_error ("malformed internal structure of returned message");
                        http_die("internal-error", "");
                    }
                    _scoped_asset_msg_t *item = asset_msg_decode (&pop); // _scoped_zmsg_t is freed
                    if (item == NULL) {
                        if (pop != NULL) {
                            zmsg_destroy (&pop);
                        }
                        log_error ("asset_smg_decode() failed for internal messages");
                        http_die("internal-error", "");
                    }

                    if (first == false) {
                        json.append (", ");
                    } else {
                        first = false;
                    }
                    std::pair<std::string,std::string> asset_names = DBAssets::id_to_name_ext_name (asset_msg_element_id (item));
                    if (asset_names.first.empty () && asset_names.second.empty ()) {
                        std::string err =  TRANSLATE_ME("Database failure");
                        http_die ("internal-error", err.c_str ());
                    }

                    json.append("{ \"name\" : \"").append(asset_names.second).append("\",");
                    json.append("\"id\" : \"").append(asset_msg_name (item)).append("\",");
                    json.append("\"sub_type\" : \"").append(utils::strip (asset_msg_type_name (item))).append("\"}");

                    asset_msg_destroy (&item);
                }
                zmsg_destroy (&zmsg);
                json.append ("] ");
            }

            if (powers) {
                if (json != "{") {
                    json.append (", ");
                }
                json.append ("\"powerchains\" : [");

                const char *item = (const char*) zlist_first (powers);
                bool first = true;
                while (item != NULL) {
                    if (first == false) {
                        json.append (", ");
                    } else {
                        first = false;
                    }
                    json.append ("{");
                    std::vector<std::string> tokens;
                    std::istringstream f(item);
                    std::string tmp;
                    // "src_socket:src_id:dst_socket:dst_id"
                    while (getline(f, tmp, ':')) {
                        tokens.push_back(tmp);
                    }
                    std::pair<std::string,std::string> src_names = DBAssets::id_to_name_ext_name (std::stoi (tokens[1]));
                    if (src_names.first.empty () && src_names.second.empty ()) {
                        std::string err =  TRANSLATE_ME("Database failure");
                        http_die ("internal-error", err.c_str ());
                    }
                    json.append("\"src-id\" : \"").append(src_names.first).append("\",");

                    if (!tokens[0].empty() && tokens[0] != "999") {
                        json.append("\"src-socket\" : \"").append(tokens[0]).append("\",");
                    }
                    std::pair<std::string,std::string> dst_names = DBAssets::id_to_name_ext_name (std::stoi (tokens[3]));
                    if (dst_names.first.empty () && dst_names.second.empty ()) {
                        std::string err =  TRANSLATE_ME("Database failure");
                        http_die ("internal-error", err.c_str ());
                    }
                    json.append("\"dst-id\" : \"").append(dst_names.first).append("\"");

                    if (!tokens[2].empty() && tokens[2] != "999") {
                        json.append(",\"dst-socket\" : \"").append(tokens[2]).append("\"");
                    }
                    json.append ("}");
                    item = (const char*) zlist_next (powers);
                }
                json.append ("] ");
            }
            json.append ("}");

#line 463 "./src/web/src/topology_xpower.ecpp"
  reply.out() << ( json );
  reply.out() << data[0]; // \n
#line 464 "./src/web/src/topology_xpower.ecpp"

        }
        else {
            log_error ("Unexpected asset_msg received. ID = %" PRIu32 , asset_msg_id (asset_msg));
            http_die("internal-error", "");
        }
    }
    else {
        log_error ("Unknown protocol");
        LOG_END;
        http_die("internal-error", "");
    }
#endif //__malamute_topology_power__

}

  // <%/cpp>
  return HTTP_OK;
}

} // namespace
