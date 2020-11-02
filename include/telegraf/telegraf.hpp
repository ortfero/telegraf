/* This file is part of telegraf library
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


#include <cstdint>
#include <string>
#include <string_view>
#include <system_error>
#include <chrono>
#include <optional>

#if defined(TELEGRAF_USE_SYSTEM_INTER)
#include <inter/context.hpp>
#include <inter/socket.hpp>
#include <inter/tls/context.hpp>
#else
#include "bundled/inter/context.hpp"
#include "bundled/inter/socket.hpp"
#include "bundled/inter/tls/context.hpp"
#endif


namespace telegraf {


class context {
public:

  static std::error_code last_error() {
    return inter::context::last_error();
  }

  context() noexcept = default;
  context(context const&) = delete;
  context& operator = (context const&) = delete;
 
  explicit operator bool() const noexcept {
    return bool(network_);
  }


private:
  inter::context network_;
};


class chat {
public:

  static chat from_id(std::string id) noexcept {
    return chat{std::move(id)};
  }

  chat(chat const&) = default;
  chat& operator = (chat const&) = default;

  chat(chat&&) noexcept = default;
  chat& operator = (chat&&) noexcept = default;

  std::string const& id() const noexcept {
    return id_;
  }

private:

  std::string id_;

  chat(std::string id) noexcept:
    id_{std::move(id)}
  { }
};


struct connection_options {
  using duration = std::chrono::system_clock::duration;

  duration timeout{std::chrono::minutes{1}};
  std::string host{"telegram.org"};
  std::uint32_t port{80};
};


class bot {
public:

  static bot from_id(context& context,
                     connection_options const& options,
                     std::string id) noexcept {
    return bot{context, options, std::move(id)}; 
  }


  bot(bot const&) = delete;
  bot& operator = (bot const&) = delete;
  bot(bot&&) noexcept = default;
  bot& operator = (bot&&) noexcept = default;


  explicit operator bool () const noexcept {
    return !!socket_;
  }


  std::string const& id() const noexcept {
    return id_;
  }


  bool send(std::string_view const& message, chat const& chat) noexcept {
    if(!ensure_connected())
      return false;
    
    return false; 
  }


  bool send(std::string const& message, chat const& chat) noexcept {
    return send(std::string_view{message.data(), message.size()}, chat);
  }


private:

  std::string id_;
  connection_options options_;

  inter::socket socket_;

  bot(context& context,
      connection_options const& options,
      std::string id) noexcept:
    id_{std::move(id)}, options_{options}
  { }


  bool ensure_connected() noexcept {
    if(!!socket_)
      return true;
    if(!socket_.connect(options_.host, options_.port))
      return false;
  }

};


}

