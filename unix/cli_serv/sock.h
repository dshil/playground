#define MAX_HOST_LEN 256

int make_stream_sock(int port);
int make_dgram_sock(int port);
int proccess_req(int sock_fd);
