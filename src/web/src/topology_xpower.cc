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
#line 25 "./src/web/src/topology_xpower.ecpp"

#include <fty_common.h>
#include <fty_common_macros.h>
#include <fty_common_rest_helpers.h>
#include <fty_common_db.h>
#include <fty_common_mlm_tntmlm.h>

// set S with MSG popped frame (no memleak, S untouched if NULL frame)
static void zmsg_pop_s (zmsg_t *msg, std::string & s)
{
    char *aux = msg ? zmsg_popstr (msg) : NULL;
    if (aux) s = aux;
    zstr_free (&aux);
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

#line 41 "./src/web/src/topology_xpower.ecpp"
  typedef UserInfo user_type;
  TNT_REQUEST_GLOBAL_VAR(user_type, user, "UserInfo user", ());   // <%request> UserInfo user
#line 42 "./src/web/src/topology_xpower.ecpp"
  typedef bool database_ready_type;
  TNT_REQUEST_GLOBAL_VAR(database_ready_type, database_ready, "bool database_ready", ());   // <%request> bool database_ready
  // <%cpp>
#line 44 "./src/web/src/topology_xpower.ecpp"

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

    //ftylog_setVeboseMode(ftylog_getInstance());
    log_trace ("in %s", request.getUrl().c_str ());

    const char *ADDRESS = AGENT_FTY_ASSET; // "asset-agent" 42ty/fty-asset
    const char *SUBJECT = "TOPOLOGY";
    const char *COMMAND = "POWERCHAINS";

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

    // db checks
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
        std::string err =  TRANSLATE_ME("Connection to mlm client failed.");
        http_die ("internal-error", err.c_str());
    }

    // set/send req, recv response
    zmsg_t *req = zmsg_new ();
    if (!req) {
        log_error ("zmsg_new () failed");
        std::string err =  TRANSLATE_ME("Memory allocation failed.");
        http_die ("internal-error", err.c_str());
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
        std::string err =  TRANSLATE_ME("Request to mlm client failed (timeout reached).");
        http_die ("internal-error", err.c_str());
    }

    // get resp. header
    std::string rx_command, rx_asset_id, rx_status;
    zmsg_pop_s(resp, rx_command);
    zmsg_pop_s(resp, rx_asset_id);
    zmsg_pop_s(resp, rx_status);

    if (rx_command != COMMAND) {
        CLEANUP;
        log_error ("received inconsistent command ('%s')", rx_command.c_str ());
        std::string err =  TRANSLATE_ME("Received inconsistent command ('%s').", rx_command.c_str ());
        http_die ("internal-error", err.c_str());
    }
    if (rx_asset_id != asset_id) {
        CLEANUP;
        log_error ("received inconsistent assetID ('%s')", rx_asset_id.c_str ());
        std::string err =  TRANSLATE_ME("Received inconsistent asset ID ('%s').", rx_asset_id.c_str ());
        http_die ("internal-error", err.c_str());
    }
    if (rx_status != "OK") {
        std::string reason;
        zmsg_pop_s(resp, reason);
        CLEANUP;
        log_error ("received %s status (reason: %s) from mlm client", rx_status.c_str(), reason.c_str ());
        //std::string err =  TRANSLATE_ME("Received %s status (reason: %s).", rx_status.c_str(), reason.c_str ());
        //http_die ("internal-error", err.c_str());
        http_die ("internal-error", JSONIFY (reason.c_str ()).c_str ());
    }

    // result JSON payload
    std::string json;
    zmsg_pop_s(resp, json);
    if (json.empty()) {
        CLEANUP;
        log_error ("empty JSON payload");
        std::string err =  TRANSLATE_ME("Received an empty JSON payload.");
        http_die ("internal-error", err.c_str());
    }
    CLEANUP;
    #undef CLEANUP

    // set body (status is 200 OK)
    reply.out () << json;
}

  // <%/cpp>
  return HTTP_OK;
}

} // namespace
