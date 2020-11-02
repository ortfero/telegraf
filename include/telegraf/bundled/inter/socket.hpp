/* This file is part of inter library
 * Copyright 2020 Andrei Ilin <ortfero@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once


#include <optional>
#include <string>
#include <string_view>


#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif


namespace inter {


  class socket {
#if defined(_WIN32)
    static constexpr auto invalid_handle = INVALID_SOCKET;
#else
    static constexpr auto invalid_handle = 0;
#endif
  public:

    socket() noexcept = default;
    socket(socket const&) = delete;
    socket& operator = (socket const&) = delete;


    socket(socket&& other) noexcept:
      handle_{other.handle_} {
      other.handle_ = invalid_handle;
    }


    socket& operator = (socket&& other) noexcept {
      ensure_closed();
      handle_ = other.handle_; other.handle_ = invalid_handle;
      return *this;
    }


    ~socket() {
      ensure_closed();
    }


    explicit operator bool () const noexcept {
      return handle_ != invalid_handle;
    }


    bool connect(std::string const& host, std::uint32_t port) noexcept {
      if(handle_ != invalid_handle)
        return false;
      handle_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if(handle_ == invalid_handle)
        return false;
      SOCKADDR_IN server;
      server.sin_addr.s_addr = inet_addr(host.data());
      server.sin_family = AF_INET;
      server.sin_port = htons(port);
      auto const connected = ::connect(handle_, (sockaddr*)(&server),
                                     sizeof(server));
      if(connected != SOCKET_ERROR)
        return true;
      close();
      handle_ = invalid_handle;
      return false;
    }


    bool reconnect(std::string const& host, std::uint32_t port) noexcept {
      ensure_closed();
      return connect(host, port);
    }


    bool send(std::string_view const& message) noexcept {
      auto const sent = ::send(handle_, message.data(), message.size(), 0);
      return sent != SOCKET_ERROR;
    }


    bool send(std::string const& message) noexcept {
      return send(std::string_view{message.data(), message.size()});
    }


    std::optional<std::size_t> recv(char* buffer,
                                    std::size_t size) noexcept {
      auto const received = ::recv(handle_, buffer, size, 0);
      if(received == SOCKET_ERROR)
        return std::nullopt;
      return std::size_t(received);
    }


    std::optional<std::size_t> recv(std::string& message) noexcept {
      message.resize(message.capacity());
      auto const received = recv(message.data(), message.size());
      if(!received)
        return received;
      message.resize(*received);
      return received;
    }

  private:

#if defined(_WIN32)
    SOCKET handle_{invalid_handle};
#else
    int handle_{invalid_handle};
#endif


    void close() noexcept {
      closesocket(handle_);
    }


    void ensure_closed() noexcept {
      if(handle_ != invalid_handle)
        close();
    }
  };


}
