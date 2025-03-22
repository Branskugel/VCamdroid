#include <iostream>
#include <asio.hpp>

int main()
{
    try {
        asio::io_context io_context;

        asio::ip::udp::socket socket(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 13));

        for (;;)
        {
            std::array<char, 100> buf;
            asio::ip::udp::endpoint remote_endpoint;

            int bytes = socket.receive_from(asio::buffer(buf), remote_endpoint);

            for (int i = 0; i < bytes; i++)
            {
                std::cout << buf.at(i);
            }
            std::cout << std::endl;
        }
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
