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

#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>


namespace inter::tls {


class socket;

class context {
friend class socket;
public:

  static std::error_code last_error() {
    return std::error_code{};
  }

  context() noexcept {
    mbedtls_ctr_drbg_init(&ctr_drbg_);
    mbedtls_entropy_init(&entropy_);
    int const seeded = mbedtls_ctr_drbg_seed(&ctr_drbg_, mbedtls_entropy_func,
                                             &entropy_, nullptr, 0);
    if(seeded != 0)
      return;

    ready_ = true;
  }

  ~context() {
    mbedtls_entropy_free(&entropy_);
    mbedtls_ctr_drbg_free(&ctr_drbg_);
  }


  context(context const&) = delete;
  context& operator = (context const&) = delete;

  explicit operator bool() const noexcept {
    return ready_;
  }


private:

  mbedtls_ctr_drbg_context ctr_drbg_;
  mbedtls_entropy_context entropy_;
  bool ready_{false};

};



}

