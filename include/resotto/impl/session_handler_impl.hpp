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
#ifndef RESOTTO_SESSION_HANDLER_IMPL_HPP
#define RESOTTO_SESSION_HANDLER_IMPL_HPP


#include <chrono>
#include <vector>
#include <array>

#include "../logger.hpp"
#include "../http_request.hpp"
#include "../http_reply.hpp"
#include "../options.hpp"
#include "../server_config.hpp"

#include "../error.hpp"

namespace resotto {

namespace server {

constexpr std::size_t avg_read_size = 16 *1024;
constexpr std::size_t max_header_size = 1* 1024 * 1024;
constexpr std::size_t max_verb_size =  128;

const char* end_header_delimiter = "\r\n\r\n";


class http11_request_handler : public http::request{
public:

    using const_iterator = const network::const_buffer*;

    using const_header_iterator = const char*;

    http11_request_handler() :
        offset_buffer(0),
        header_size(0),
        header_buffer(max_header_size){
    }


    template<typename Socket>
    void read_headers(Socket & sess_socket){
        header_buffer.resize(avg_read_size, '\0');

        while(1){
            error_code sock_err;

            std::size_t read_size = sess_socket.read_some(network::buffer(buffer_ptr(), buffer_size()), sock_err);
            if(sock_err){
                throw session_error(sock_err.message());
            }

            logger(log_level::trace, log_scope::session, "read size ", read_size);

            if( header_buffer.size() >= max_header_size ||
                read_size >= max_header_size){
                throw session_error(hadoken::scat("header larger than limit of ", max_header_size));
                return;
            }

            auto it = std::search(header_buffer.begin(), header_buffer.end(), end_header_delimiter, end_header_delimiter +4);
            if(it != header_buffer.end()){
                header_size = std::distance(header_buffer.begin(), it) + 4;
                return;
            }
            offset_buffer = offset_buffer + read_size;
            if(offset_buffer *2 > std::ptrdiff_t(header_buffer.size())){
                header_buffer.resize(header_buffer.size() *2, '\0');
            }
        }
    }

    void parse_headers(){
        //auto last = parse_verb(sess_err);
       // parse_path(last, sess_err);

        logger(log_level::debug, log_scope::parser, full_header_content());
    }

    string_view full_header_content(){
        return string_view(header_buffer.data(), header_size);
    }

    string_view verb() const{
        return verb();
    }

    string_view path() const{
        return path();
    }

    void reset(){
        headers.clear();
        header_buffer.clear();
        offset_buffer = header_size = 0;
    }

private:
    http11_request_handler(const http11_request_handler&) = delete;

    char* buffer_ptr(){
        return header_buffer.data() + offset_buffer;
    }

    std::size_t buffer_size(){
        return header_buffer.size() - offset_buffer;
    }

    const_header_iterator parse_verb(error_code & err){
        auto it = std::find(header_begin(), header_end(), ' ');
        if(it == header_end()){
            err = error_code(EINVAL, boost::system::generic_category());
            return header_end();
        }
        _verb = string_view(header_begin(), std::distance(header_begin(), it));
        return it;
    }

    const_header_iterator parse_path(const_header_iterator last, error_code & err){
        auto it = std::search(last, header_end(), end_header_delimiter, end_header_delimiter+2);
        if(it == header_end()){
            err = error_code(EINVAL, boost::system::generic_category());
            return header_end();
        }
        _path = string_view(last, std::distance(last, it));
        return it;
    }

    const_header_iterator header_begin(){
        return header_buffer.data();
    }

    const_header_iterator header_end(){
        return header_buffer.data() + header_size;
    }

    std::ptrdiff_t offset_buffer, header_size;
    std::vector<char> header_buffer;
    string_view _verb, _path;

    std::vector<header> headers;
};


//
// context for connected socket
//
template<typename Socket>
class session_handler{
public:
    session_handler(Socket s, options & sopts) :
        sess_socket(std::move(s)),
        server_opts(sopts),
        creation_time()
    {
    }


    void process(){
        try{
            creation_time = std::chrono::steady_clock::now();

            std::size_t counter = 0;
            logger(log_level::debug, log_scope::session, "new session");



            while(1){
                counter++;

                logger(log_level::debug, log_scope::request, "new request ", counter);

                request11.reset();


                request11.read_headers(sess_socket);

                request11.parse_headers();
                try{

                    reply.set_body("hello\n");
                    reply.set_code(200);

                }catch(http::request_error & req_err){
                    reply.set_code(req_err.code());
                    reply.set_body(req_err.what());
                }

                serialize_reply(sess_socket, reply);

                logger(log_level::debug, log_scope::request, "end request ", counter);
            }

        }catch(std::exception & e){
            logger(log_level::debug, log_scope::session, "Session Error ", e.what());
            close();
        }
    }



    void close(){
        sess_socket.close();
    }

    Socket & socket(){
        return sess_socket;
    }

private:
    session_handler(const session_handler &) = delete;

    Socket sess_socket;
    options & server_opts;
    std::chrono::steady_clock::time_point creation_time;

    http11_request_handler request11;
    http::reply reply;



};


} // server

} // resotto

#endif // RESOTTO_SESSION_HANDLER_IMPL_HPP