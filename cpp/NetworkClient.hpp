#pragma once

#include "_NetworkDetail.hpp"

#ifdef __linux__

namespace hsd
{
    namespace tcp
    {
        namespace client_detail
        {
            class socket
            {
            private:
                int _sock = 0;
                sockaddr_in6 _hintv6;
                sockaddr_in _hintv4;
                protocol_type _protocol;

            public:
                socket(protocol_type protocol = protocol_type::ipv4, 
                    uint16_t port = 54000, const char* ip_addr = "127.0.0.1")
                {
                    _protocol = protocol;

                    if(_protocol == protocol_type::ipv4)
                    {
                        _sock = ::socket(AF_INET, SOCK_STREAM, 0);
                        _hintv4.sin_family = AF_INET;
                        _hintv4.sin_port = htons(port);
                        inet_pton(AF_INET, ip_addr, &_hintv4.sin_addr);
                        connect(_sock, reinterpret_cast<sockaddr*>(&_hintv4), sizeof(_hintv4));
                    }
                    else
                    {
                        _sock = ::socket(AF_INET6, SOCK_STREAM, 0);
                        _hintv6.sin6_family = AF_INET6;
                        _hintv6.sin6_port = htons(port);
                        inet_pton(AF_INET6, ip_addr, &_hintv6.sin6_addr);
                        connect(_sock, reinterpret_cast<sockaddr*>(&_hintv6), sizeof(_hintv6));
                    }
                }

                ~socket()
                {
                    close();
                }

                void close()
                {
                    ::close(_sock);
                }

                int get_sock()
                {
                    return _sock;
                }

                void switch_to(protocol_type protocol, uint16_t port, const char* ip_addr)
                {
                    close();
                    _protocol = protocol;

                    if(_protocol == protocol_type::ipv4)
                    {
                        _sock = ::socket(AF_INET, SOCK_STREAM, 0);
                        _hintv4.sin_family = AF_INET;
                        _hintv4.sin_port = htons(port);
                        inet_pton(AF_INET, ip_addr, &_hintv4.sin_addr);
                        connect(_sock, reinterpret_cast<sockaddr*>(&_hintv4), sizeof(_hintv4));
                    }
                    else
                    {
                        _sock = ::socket(AF_INET6, SOCK_STREAM, 0);
                        _hintv6.sin6_family = AF_INET6;
                        _hintv6.sin6_port = htons(port);
                        inet_pton(AF_INET6, ip_addr, &_hintv6.sin6_addr);
                        connect(_sock, reinterpret_cast<sockaddr*>(&_hintv6), sizeof(_hintv6));
                    }
                }
            };
        } // namespace client_detail

        class client
        {
        private:
            client_detail::socket _sock;
            hsd::u8sstream _net_buf{4095};

            void _clear_buf()
            {
                memset(_net_buf.data(), '\0', 4096);
            }

        public:
            client() = default;
            ~client() = default;

            client(protocol_type protocol, uint16_t port, const char* ip_addr)
                : _sock{protocol, port, ip_addr}
            {}

            hsd::pair< hsd::u8sstream&, received_state > receive()
            {
                _clear_buf();
                long _response = recv(_sock.get_sock(), 
                    _net_buf.data(), 4096, 0);

                if (_response == static_cast<long>(received_state::err))
                {
                    hsd::io::err_print("Error in receiving\n");
                    _clear_buf();
                    return {_net_buf, received_state::err};
                }
                if (_response == static_cast<long>(received_state::disconnected))
                {
                    hsd::io::err_print("Server down\n");
                    _clear_buf();
                    return {_net_buf, received_state::disconnected};
                }

                return {_net_buf, received_state::ok};
            }

            template< size_t N, typename... Args >
            received_state respond(const char (&fmt)[N], Args&&... args)
            {
                hsd::vector<hsd::u8string> _fmt_buf = io_detail::split(fmt, N - 1);
                hsd::vector<hsd::u8string> _args_buf = {
                    hsd::forward<hsd::u8string>(hsd::u8string::to_string(args))...
                };
                hsd::u8string _send_buf;

                if(_args_buf.size() != _fmt_buf.size() && _args_buf.size() + 1 != _fmt_buf.size())
                {
                    throw std::runtime_error("Arguments don\'t match");
                }
                else
                {
                    hsd::size_t index = 0;

                    for(; index < _args_buf.size(); index++)
                    {
                        _send_buf += _fmt_buf[index] + _args_buf[index];
                    }
                    if(_fmt_buf.size() != _args_buf.size())
                    {
                        _send_buf += _fmt_buf[index];
                    }

                    long _response = send(_sock.get_sock(), 
                        _send_buf.data(), _send_buf.size(), 0);

                    if(_response == static_cast<long>(received_state::err))
                    {
                        hsd::io::err_print("Error in sending\n");
                        return received_state::err;
                    }
                }

                return received_state::ok;
            }
        };
    } // namespace tcp
} // namespace hsd

#endif