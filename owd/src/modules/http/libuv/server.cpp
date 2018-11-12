#include <stdio.h>
#include <string>

#include <uv.h>
#include "http-parser/http_parser.h"

#include "core/stddefs.h"
#include "core/heap_allocator.h"
#include "http/server_config.h"

// TODO: use logger instead of fprintf.

namespace owd {
namespace http {

namespace {

const std::string healthcheck =
  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/plain\r\n"
  "Content-Length: 18\r\n"
  "\r\n"
  "open wheater data\n";

core::HeapAllocator allocator;
http_parser_settings parser_settings;

struct connection_t {
    uv_tcp_t client;
    http_parser parser;
};

void new_conn_cb(uv_stream_t*, int);
void alloc_cb(uv_handle_t*, size_t, uv_buf_t*);
void read_cb(uv_stream_t*, ssize_t, uv_buf_t*);
void write_cb(uv_write_t*, int);
void shutdown_cb(uv_shutdown_t*, int);
void close_cb(uv_handle_t*);

int headers_complete_cb(http_parser*);
int on_url(http_parser*, const char* ptr, size_t len);
int on_message_complete_cb(http_parser* parser);

void alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf) {
    buf->base = (char*)allocator.alloc(size);
    buf->len = size;
}

void new_conn_cb(uv_stream_t* stream, int status) {
    if (status < 0) {
        fprintf(stderr, "new_conn_cb: %s\n", uv_strerror(status));
        return;
    }

    connection_t *conn = new(allocator) connection_t;
    memset(conn, 0, sizeof(connection_t));

    uv_tcp_init(stream->loop, &conn->client);
    http_parser_init(&conn->parser, HTTP_REQUEST);

    conn->client.data = conn;
    conn->parser.data = conn;

    if (!uv_accept(stream, (uv_stream_t*)&conn->client)) {
        uv_read_start((uv_stream_t*)&conn->client,
                (uv_alloc_cb)&alloc_cb,
                (uv_read_cb)&read_cb);
    } else {
        uv_shutdown_t* s = new(allocator) uv_shutdown_t;
        uv_shutdown(s,
                (uv_stream_t*)&conn->client,
                (uv_shutdown_cb)&shutdown_cb);
    }
}

void read_cb(uv_stream_t *stream, ssize_t nread, uv_buf_t *buf) {
    connection_t* conn = (connection_t*)stream;

    if (nread < 0) {
        uv_shutdown_t* s = new(allocator) uv_shutdown_t;
        uv_shutdown(s,
                (uv_stream_t*)&conn->client,
                (uv_shutdown_cb)&shutdown_cb);
    } else {
        const size_t nparsed = http_parser_execute(
                &conn->parser,
                &parser_settings,
                buf->base,
                nread);

        if (nparsed != nread) {
            fprintf(stderr, "failed to parse request\n");
            uv_close((uv_handle_t*)&conn->client, (uv_close_cb)&close_cb);
        }
    }

    allocator.free(buf->base);
}

void write_cb(uv_write_t* resp, int status) {
    uv_close((uv_handle_t*)resp->handle, (uv_close_cb)&close_cb);
}

void shutdown_cb(uv_shutdown_t* s, int status) {
    uv_close((uv_handle_t*)s->handle, (uv_close_cb)&close_cb);
    allocator.free(s);
}

void close_cb(uv_handle_t* handle) {
    connection_t *conn = (connection_t*)handle->data;
    allocator.free(conn);
}

int headers_complete_cb(http_parser* parser) {
    return 0;
}

int on_url_cb(http_parser*, const char* ptr, size_t length) {
    return 0;
}

int on_message_complete_cb(http_parser* parser) {
    uv_write_t* req = new(allocator) uv_write_t;
    uv_buf_t resp = {
        .base = (char*)healthcheck.c_str(),
        .len = healthcheck.size(),
    };

    connection_t* conn = (connection_t*)parser->data;
    uv_write(req, (uv_stream_t*)&conn->client, &resp, 1,
            (uv_write_cb)&write_cb);
}

} // namespace

int listen_and_serv(const ServerConfig &config) {
    parser_settings.on_headers_complete = headers_complete_cb;
    parser_settings.on_url = on_url_cb;
    parser_settings.on_message_complete = on_message_complete_cb;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    int code = uv_ip4_addr(config.get_host(), config.get_port(), &addr);
    if (code) {
        fprintf(stderr, "uv_ip4_addr: %s\n", uv_strerror(code));
        return code;
    }

    uv_tcp_t socket;
    uv_loop_t* loop = uv_default_loop();

    code = uv_tcp_init(loop, &socket);
    if (code) {
        fprintf(stderr, "uv_tcp_init: %s\n", uv_strerror(code));
        return code;
    }

    code = uv_tcp_bind(&socket, (const struct sockaddr*)&addr, 0);
    if (code) {
        fprintf(stderr, "uv_tcp_bind: %s\n", uv_strerror(code));
        return code;
    }

    code = uv_listen((uv_stream_t*) &socket, SOMAXCONN, new_conn_cb);
    if (code) {
        fprintf(stderr, "uv_list: %s\n", uv_strerror(code));
        return code;
    }

    code = uv_run(loop, UV_RUN_DEFAULT);
    if (code) {
        fprintf(stderr, "uv_run: %s\n", uv_strerror(code));
        return code;
    }

    return 0;
}

} // namespace http
} // namespace owd
