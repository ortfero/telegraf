#pragma once


namespace telegraf {

class bot {
public:

  bot() noexcept = default;
  bot(bot const&) = delete;
  bot& operator = (bot const&) = delete;
  bot(bot&&) noexcept = default;
  bot& operator = (bot&&) noexcept = default;
};


}

