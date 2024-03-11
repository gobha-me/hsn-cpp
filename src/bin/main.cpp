#include <argparse/argparse.hpp>
#include <cstdlib>
#include <fmt/format.h>
#include <version.hpp>

namespace hsn::detail {
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
}

namespace hsn {
constexpr auto gethostname() -> std::string {
  std::array<char, HOST_NAME_MAX> my_name{};

  ::gethostname(my_name.data(), my_name.size());

  return {my_name.data(), my_name.size()};
}

class AddrInfo : public detail::addrinfo {
  public:
    class iterator {
    public:
      iterator() noexcept : m_current_node(nullptr) {}

      iterator(detail::addrinfo *next) noexcept : m_current_node(next) {}

      auto operator++() -> iterator& {
        if (m_current_node != nullptr) {
          m_current_node = m_current_node->ai_next;
        }

        return *this;
      }

      auto operator++(int) -> iterator {
        auto itr = *this;
        ++*this;

        return itr;
      }

      auto operator !=(const iterator& iterator) -> bool {
        return m_current_node != iterator.m_current_node;
      }

      auto operator*() -> AddrInfo {
        return AddrInfo(m_current_node);
      }
    private:
      detail::addrinfo* m_current_node;
    };

    AddrInfo() : detail::addrinfo{}, m_root{true} {}
    AddrInfo(detail::addrinfo* info) : m_addrinfo(info) {}

    ~AddrInfo() {
      if (m_root and m_addrinfo != nullptr) {
        freeaddrinfo(m_addrinfo);
      }
    }

    auto begin() -> iterator {
      return iterator(m_addrinfo);
    }

    auto end() -> iterator {
      return iterator(nullptr);
    }

    auto family() -> int {
      return m_addrinfo->ai_family;
    }

    auto family_str() -> std::string {
      if (m_addrinfo->ai_family == AF_INET) {
        return "IPv4";
      }

      return "IPv6";
    }

    auto socktype() -> int {
      return m_addrinfo->ai_socktype;
    }

    auto data() -> detail::addrinfo** {
      return &m_addrinfo;
    }

    template<typename AddrType = hsn::detail::sockaddr_in*>
    auto addr() -> AddrType {
      return reinterpret_cast<AddrType>(m_addrinfo->ai_addr);
    }

    auto addr_to_str() -> std::string {
      std::string retVal{};
      retVal.resize(INET6_ADDRSTRLEN);
      void *addr_ptr{nullptr};

      if (m_addrinfo->ai_family == AF_INET) {
        auto *ipv4 = addr();
        addr_ptr = &(ipv4->sin_addr);
      } else {
        auto *ipv6 = addr<hsn::detail::sockaddr_in6 *>();
        addr_ptr = &(ipv6->sin6_addr);
      }

      hsn::detail::inet_ntop(m_addrinfo->ai_family, addr_ptr, retVal.data(), retVal.size());

      return retVal;
    }

  private:
    bool m_root{false};
    detail::addrinfo* m_addrinfo{nullptr};
};
}

constexpr std::string_view ARG_HOSTNAME{"--hostname"};

auto main(int argc, char** argv) -> int {
  argparse::ArgumentParser program(PROGRAM_NAME.data(), fmt::format("{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH));
  program.add_argument(ARG_HOSTNAME)
    .help("")
    .default_value(hsn::gethostname());

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << '\n';
    std::cerr << program;
    std::exit(EXIT_FAILURE);
  }

  fmt::println("App running on {}", hsn::gethostname());

  hsn::AddrInfo my_info{};

  my_info.ai_family = AF_UNSPEC;
  my_info.ai_socktype = hsn::detail::SOCK_STREAM;

  if (auto status = getaddrinfo(program.get(ARG_HOSTNAME).c_str(), NULL, &my_info, my_info.data()); status != 0) {
    fmt::println("getaddrinfo: {}", hsn::detail::gai_strerror(status));
  }

  fmt::println("IP addresses for {}\n", program.get(ARG_HOSTNAME));

  for (auto addrinfo : my_info) {
    fmt::println("\t{}: {}", addrinfo.family_str(), addrinfo.addr_to_str());
  }

  return EXIT_SUCCESS;
}
