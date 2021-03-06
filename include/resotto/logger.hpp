/**
 * Copyright (c) 2018, Adrien Devresse <adev@adev.name>
 *
 * Boost Software License - Version 1.0
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
*
*/
#ifndef RESOTTO_LOGGER_HPP
#define RESOTTO_LOGGER_HPP


#include <memory>
#include <bitset>


#include <hadoken/format/format.hpp>
#include "server_config.hpp"

namespace resotto {


///
/// \brief Available log level
///
enum class log_level : int{
    error = 0,
    warning = 1,
    info = 2,
    debug = 3,
    trace = 4
};

///
/// log scope
///
/// log scope defined the area ( scope ) of
/// resotto able to log
///
///


namespace log_scope{
    using set = std::bitset<64>;
    using value = int;

    constexpr int access = 1;
    constexpr int session = 2;
    constexpr int parser = 3;
    constexpr int request = 4;
    constexpr int reply = 5;
}


void set_log_level(log_level level);

void set_log_scope(log_scope::set level);


template<typename... Args>
void logger(log_level level, log_scope::value scope, Args... args);

string_view to_string_view(log_level l);



} // resotto


#include "impl/logger_impl.hpp"

#endif // RESOTTO_LOGGER_HPP
