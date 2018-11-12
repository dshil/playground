#include <cpprest/http_listener.h> // HTTP server
#include <cpprest/json.h>          // JSON processing

#include <iostream>

#include "core/sigint_waiter.h"
#include "http/server_config.h"

namespace owd {
namespace http {

namespace {

void handle_get(web::http::http_request request);

void handle_get(web::http::http_request request) {
    auto resp = web::json::value::object();
    resp["healthcheck"] = web::json::value::string("alive");
    request.reply(web::http::status_codes::OK, resp);
}

} // namespace

int listen_and_serv(const ServerConfig &config) {
    core::SigintWaiter waiter;
    if (waiter.setup() == -1)
        return -1;

    char uri[BUFSIZ];
    std::sprintf(uri, "%s:%d/beta", config.get_host(), config.get_port());

    web::http::experimental::listener::http_listener listener(uri);
    listener.support(web::http::methods::GET, handle_get);

    try {
        listener
            .open()
            .then([&listener, &config]() {
                auto info = web::json::value::object();
                info["host"] = web::json::value::string(config.get_host());
                info["port"] =
                    web::json::value::string(std::to_string(config.get_port()));
                std::cerr
                    << info.serialize()
                    << std::endl;
            })
            .wait();

        waiter.wait_interrupt();
        listener.close().wait();
    } catch (const std::exception& e) {
        std::wcerr << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}

} // namespace owd
} // namespace http
