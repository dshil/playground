FROM fedora:latest

# compilers
RUN dnf install -y gcc-c++

# tools
RUN dnf install -y gengetopt wget git

# dependencies
RUN dnf install -y \
    libuv-devel \
    boost-devel \
    openssl-devel \
    make \
    cmake
