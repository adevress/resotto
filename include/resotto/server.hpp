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
#ifndef RESOTTO_SERVER_HPP
#define RESOTTO_SERVER_HPP


#include <memory>


#include "options.hpp"
#include "server_config.hpp"

namespace resotto {

namespace server {

///
/// \brief resotto server
///
///  main entry point for API creation
///
template<typename ServerConfig = config::std_thread>
class server{
    struct intern;
public:
    using config = ServerConfig;
    using thread_model = typename config::thread_model;
    using request_executor = typename config::executor;

    server(options && opt = options());

    virtual ~server();


    void serve(const std::string & address, int port);

private:
    server(const server & ) = delete;
    server & operator=(const server & ) = delete;

    std::unique_ptr<intern> _pimpl;
};

} // server

} // resotto


#include "impl/server_impl.hpp"

#endif // RESOTTO_SERVER_HPP
