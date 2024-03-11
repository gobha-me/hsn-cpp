#include <catch2/catch_test_macros.hpp>

namespace hsn::socket {
  namespace detail {
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <sys/uio.h>

    template<int SOCKTYPE>
    class Socket {
    public:
      Socket() {
        m_hints.ai_family = AF_UNSPEC;
        m_hints.ai_socktype = SOCKTYPE;
        m_hints.ai_flags = AI_PASSIVE;
      }
    private:
      struct addrinfo m_hints{};
      struct addrinfo *m_servinfo{nullptr};
    };
  }

  class Stream : public detail::Socket<detail::SOCK_STREAM> {
  public:
    Stream() {
    }
  private:
  };

  class Datagram : public detail::Socket<detail::SOCK_DGRAM> {
  };

  class Raw : public detail::Socket<detail::SOCK_RAW> {
  };
}
