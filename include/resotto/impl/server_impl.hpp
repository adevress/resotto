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
#ifndef RESOTTO_SERVER_IMPL_HPP
#define RESOTTO_SERVER_IMPL_HPP


#include "../server.hpp"

#include <thread>
#include <boost/asio.hpp>

#include "session_handler_impl.hpp"

namespace resotto{

namespace network = boost::asio;

namespace server{

template<typename ServerConfig>
struct server<ServerConfig>::intern{
    intern(options opt) :
        _opt(opt),
        _io_service(),
        _acceptor(_io_service),
        _executors(opt.overcommit_factor * std::thread::hardware_concurrency()),
        _terminate(false){

    }

    options _opt;

    network::io_service        _io_service;
    network::ip::tcp::acceptor _acceptor;

    std::thread _acceptor_thread;

    server<ServerConfig>::request_executor _executors;


    std::atomic<bool> _terminate;
};

template<typename ServerConfig>
server<ServerConfig>::server(options && opt) : _pimpl(new intern(std::move(opt))){

}

template<typename ServerConfig>
server<ServerConfig>::~server(){
    _pimpl->_terminate.store(true);

    _pimpl->_acceptor_thread.join();
}

template<typename ServerConfig>
void server<ServerConfig>::serve(const std::string & address, int port){

    network::ip::tcp::resolver resolver(_pimpl->_io_service);
    network::ip::tcp::endpoint endpoint = *resolver.resolve({address, std::to_string(port)});

    auto & acceptor = _pimpl->_acceptor;
    acceptor.open(endpoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.set_option(boost::asio::ip::tcp::no_delay(true));
    acceptor.set_option(boost::asio::ip::v6_only(false));
    acceptor.bind(endpoint);
    acceptor.listen();

    _pimpl->_acceptor_thread = std::thread([this](){
        auto & self = this->_pimpl;

        while(self->_terminate != true){
            auto session_ptr
                    = std::make_shared<session_handler<network::ip::tcp::socket>>(
                           network::ip::tcp::socket(_pimpl->_io_service),
                           _pimpl->_opt
                     );

            error_code err;
            self->_acceptor.accept(session_ptr->socket(), err);

            if(! err){
                _pimpl->_executors.execute([session_ptr](){
                    logger(log_level::debug, log_scope::session, "enter session handler");

                    session_ptr->process();

                    logger(log_level::debug, log_scope::session, "finish session handler");
                });
            } else{
                std::cerr << "Can not accept connection " << err.message() << std::endl;
            }



        }
    });

    while(_pimpl->_terminate == false){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

} // server

} // resotto

#endif // RESOTTO_SERVER_IMPL_HPP
