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


#include <system_error>

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif


namespace inter {

class context {
public:

  static std::error_code last_error() {
#if defined(_WIN32)
    return std::error_code(int(WSAGetLastError()), std::system_category());
#endif
  }

  context() noexcept {
#if defined(_WIN32)
    auto const code = WSAStartup(MAKEWORD(2, 2), &wsadata_);
    ready_ = (code == 0);
#endif
  }

  ~context() {
#if defined(_WIN32)
    WSACleanup();
#endif
  }


  context(context const&) = delete;
  context& operator = (context const&) = delete;

  explicit operator bool() const noexcept {
#if defined(_WIN32)
    return ready_;
#else
    return true;
#endif
  }


private:

#if defined(_WIN32)
  WSADATA wsadata_;
  bool ready_;
#endif
};


}

