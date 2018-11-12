#include <iostream>

#include "clara/clara.hpp"

#include "http/server_config.h"

using namespace owd;

namespace {

const char* default_host = "http://localhost";
const int default_port = 8080;

void print_help(clara::Parser const& p) {
    std::ostringstream oss;
    oss << p;
    std::cerr << oss.str() << std::endl;
}

} // namespace

int main(int argc, char** argv)
{
    int port = 0;
    bool show_help = false;

    auto cli
        = clara::Help(show_help)
        | clara::Opt(port, "port")
            ["-p"]["--port"]
            ("HTTP port");

    auto result = cli.parse(clara::Args(argc, argv));
    if (!result) {
        std::cerr
            << "failed to parse command line options: "
            << result.errorMessage()
            << std::endl;
        return (EXIT_FAILURE);
    }

    if (show_help) {
        print_help(cli);
        return (EXIT_SUCCESS);
    }
    if (!port)
        port = default_port;

    http::ServerConfig config(default_host, port);
    return http::listen_and_serv(config);
}
