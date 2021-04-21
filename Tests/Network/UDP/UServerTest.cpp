#include <NetworkServer.hpp>

int main()
{
    hsd::udp::server server{hsd::net::protocol_type::ipv4, 48000, "0.0.0.0"};
    char raw_buf[1024];

    while(true)
    {
        auto [buf, code] = server.receive();
        
        if(code == hsd::net::received_state::ok)
        {
            hsd::io::print<"CLIENT> {}\n">(buf.data());
            
            // Copy the data to a buffer
            hsd::cstring::copy(raw_buf, buf.c_str());
            server.respond<"{}">(raw_buf);
        }
        
        // Because newline will be sent as well
        if(hsd::cstring::compare(buf.c_str(), "exit\n") == 0)
            break;

        if(code != hsd::net::received_state::ok)
            break;
    }
}