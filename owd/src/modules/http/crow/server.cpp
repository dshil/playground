#include "crow/crow.h"

#include "version/version.h"
#include "http/server_config.h"

namespace owd {
namespace http {

int listen_and_serv(const ServerConfig &config) {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/info")([](){
        crow::json::wvalue ret;
        ret["name"] = APPLICATION_CODENAME;
        ret["description"] = APPLICATION_NAME;
        ret["version"] = APPLICATION_VERSION_STRING;
        ret["build-date"] = APPLICATION_BUILD_DATE;
        ret["build-user"] = APPLICATION_BUILD_USER;
        ret["build-host"] = APPLICATION_BUILD_HOST;
        return ret;
    });

    app.loglevel(crow::LogLevel::Warning);
    app.port(config.get_port()).multithreaded().run();

    return 0;
}

} // namespace http
} // namespace owd
