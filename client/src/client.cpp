#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("192.168.0.137", "12345");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        std::string command = "tasks\n"; 
        boost::asio::write(socket, boost::asio::buffer(command));

        char reply[1024];
        boost::system::error_code error;
        size_t reply_length = socket.read_some(boost::asio::buffer(reply), error);

        if (error == boost::asio::error::eof) {
            std::cerr << "Server closed the connection.\n";
        } else if (error) {
            throw boost::system::system_error(error);
        } else {
            std::cout << "Reply from server:\n";
            std::cout.write(reply, reply_length);
            std::cout << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }


    return 0;
}