#include <iostream>

#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "catch2/catch.hpp"
#include "json/json.hpp"
#include <cpr/cpr.h>

#include "http/server_config.h"

using namespace owd;

namespace {

void teardown();
void die(const char *msg);
void start_server(http::ServerConfig& config);

void start_server(http::ServerConfig& config) {
    const pid_t pid = fork();
    if (pid == -1)
        die("fork");

    if (pid != 0) {
        if (atexit(teardown) == -1)
            die("atexit");
        return;
    }

    if (close(STDIN_FILENO) == -1)
        die("close(STDIN_FILENO)");
    if (close(STDOUT_FILENO) == -1)
        die("close(STDOUT_FILENO)");
    if (close(STDERR_FILENO) == -1)
        die("close(STDERR_FILENO)");
    if (open("/dev/null", O_RDONLY) == -1)
        die("open(stdin)");
    if (open("/dev/null", O_WRONLY) == -1)
        die("open(stdout)");
    if (open("/dev/null", O_RDWR) == -1)
        die("open(stderr)");

    if (listen_and_serv(config) == -1)
        die("listen_and_serv");
}

void teardown() {
    if (kill(-getpid(), SIGINT) == -1) {
        die("kill");
    } else {
        int status = 0;
        pid_t child = 0;
        while ((child = wait(&status)) > 0);
    }
}

void die(const char *msg) {
    perror(msg);
    exit(1);
}

} // namespace

TEST_CASE("get server info") {
    const char* host = "localhost";
    const int port = 8080;

    http::ServerConfig config(host, port);
    start_server(config);

    auto response = cpr::Get(cpr::Url{"localhost:8080/info"});
    auto json = nlohmann::json::parse(response.text);

    REQUIRE(json["name"] == "owd");
    REQUIRE(json["version"] != "");
    REQUIRE(json["build-data"] != "");
    REQUIRE(json["build-host"] != "");
    REQUIRE(json["build-user"] != "");
    REQUIRE(json["description"] != "");
}
