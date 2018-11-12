#ifndef OWD_HTTP_SERVER_CONFIG_H_
#define OWD_SERVER_CONFIG_H_

namespace owd {
namespace http {

class ServerConfig;

int listen_and_serv(const ServerConfig&);

class ServerConfig {
public:
    ServerConfig(const char* host, int port) : host_(host), port_(port) {
    }

    int get_port() const {
        return port_;
    }

    const char* get_host() const {
        return host_;
    }
private:
    const char* host_;
    int port_;
};

} // namespace http
} // namespace owd

#endif // OWD_SERVER_CONFIG_H_
