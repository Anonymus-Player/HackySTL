#include <NetworkClient.hpp>

int main()
{
    hsd::udp::client client{hsd::net::protocol_type::ipv4, 48000, "127.0.0.1"};
    char raw_buf[1024];

    while(true)
    {
        hsd::io::print<"> ">();
        
        auto& stream_ref = hsd::io::read_line().unwrap();
        hsd::cstring::copy_until(raw_buf, stream_ref.c_str(), '\n');
        auto state = client.respond<"{}">(raw_buf);

        if(state == hsd::net::received_state::err)
            continue;

        auto [buf, code] = client.receive();
        
        if(code == hsd::net::received_state::ok)
            hsd::io::print<"SERVER> {}\n">(buf.data());
        
        if(code != hsd::net::received_state::ok)
            break;
    }
}