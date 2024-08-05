#pragma once

#include <platform.hpp>

#ifdef BCF_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <WinError.h>
    #include <Ws2tcpip.h>
    #include <errno.h>
    #include <winsock.h>
    #include <winsock2.h>

#elif defined BCF_PLATFORM_LINUX
    #include <arpa/inet.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <signal.h>
    #include <sys/epoll.h>
    #include <sys/eventfd.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <sys/uio.h>
    #include <unistd.h>

#elif defined BCF_PLATFORM_DARWIN || defined BCF_PLATFORM_FREEBSD
    #include <arpa/inet.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <signal.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/event.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <sys/uio.h>
    #include <unistd.h>

#else
    #error "Unsupported OS, please commit an issuse."
#endif
