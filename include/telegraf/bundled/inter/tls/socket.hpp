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


#include <ctsdint>
#include <string>
#include <string_view>
#include <system_error>

#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/certs.h>

#include "context.hpp"


namespace inter::tls {


  class socket {
  public:


    socket(std::string_view const& pem) noexcept {
      mbedtls_net_init(&server_fd_);
      mbedtls_x509_crt_init(&ca_cert_);
      mbedtls_ssl_config_init(&config_);
      mbedtls_ssl_init(&context_);
    }


    ~socket() {
      mbedtls_ssl_free(&context_);
      mbedtls_ssl_config_free(&config_);
      mbedtls_x509_crt_free(&ca_cert_);
      mbedtls_net_free(&server_fd_);
    }


    socket(socket const&) = delete;
    socket& operator = (socket const&) = delete;


    bool connect(context& context, std::string const& host, std::uint32_t port,
                 std::string_view const& pem) {
      int const parse_code = mbedtls_x509_crt_parse(&ca_cert_, pem.data(), pem.size());
      if(parse_code != 0)
        return false;

      int const connect_code = mbedtls_net_connect(&server_fd_, host.data(), port,
                                                MBEDTLS_NET_PROTO_TCP);
      if(connect_code != 0)
        return false;

      int const config_code = mbedtls_ssl_config_defaults(&config_,
                                  MBEDTLS_SSL_IS_CLIENT,
                                  MBEDTLS_SSL_TRANSPORT_STREAM,
                                  MBEDTLS_SSL_PRESET_DEFAULT);
      if(config_code != 0)
        return false;

      mbedtls_ssl_conf_authmode(&config_, MBEDTLS_SSL_VERIFY_OPTIONAL);
      mbedtls_ssl_conf_ca_chain(&config_, &ca_cert_, nullptr);
      mbedtls_ssl_conf_rng(&config_, mbedtls_ctr_drbg_random, &context.ctr_drbg_);
      int const setup_code = mbedtls_ssl_setup(&context_, &config_);
      if(setup_code != 0)
        return false;

      int const hostname_code = mbedtls_ssl_set_hostname(&context_, host.data());
      if(hostname_code != 0)
        return false;

      mbedtls_ssl_set_bio(&context_, &server_fd,
                          mbedtls_net_send, mbedtls_net_recv, nullptr);
      for(;;) {
        int const handshake_code = mbedtls_ssl_handshake(&context_);
        if(handshake_code == 0)
          break;
        if(handshake_code != MBEDTLS_ERR_SSL_WANT_READ
           && handshake_code != MBEDTLS_ERR_SSL_WANT_WRITE)
          return false;
      }

      return true;
    }


    bool write(std::string_view const& sv) noexcept {

    }


    bool write(std::string const& s) noexcept {
      return write(std::string_view{s.data(), s.size()};
    }


  private:

    mbedtls_net_context server_fd_;
    mbedtls_x509_crt ca_cert_;
    mbedtls_ssl_context context_;
    mbedtls_ssl_config config_;
  };


}

