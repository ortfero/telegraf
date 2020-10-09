#pragma once


#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif


namespace telegraf {


class context {
public:

  context() noexcept {
#if defined(_WIN32)
    auto const code = WSAStartup(MAKEWORD(2, 2), &wsadata);
    ready = (code == 0);
#endif
  }

  ~context() {
#if defined(_WIN32)
    WSACleanup();
#endif
  }

  explicit operator bool() const noexcept {
#if defined(_WIN32)
    return ready;
#else
    return true;
#endif
  }

private:

#if defined(_WIN32)
  WSADATA wsadata;
  bool ready;
#endif
};


class bot {
public:

  bot(context&) noexcept { }
  bot(bot const&) = delete;
  bot& operator = (bot const&) = delete;
  bot(bot&&) noexcept = default;
  bot& operator = (bot&&) noexcept = default;

private:

#if defined(_WIN32)
  SOCKET socket{INVALID_SOCKET};
#endif

};


}

