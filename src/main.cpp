#include <boost/asio.hpp>
#include <iostream>
#include <algorithm>
#include "task_manager.h"
#include "elf_viewer.h"

using boost::asio::ip::tcp;

void handle_client(tcp::socket& socket) {
    try {
        char buffer[1024];
        boost::system::error_code error;

        // Чтение команды от клиента
        size_t length = socket.read_some(boost::asio::buffer(buffer), error);
        if (error == boost::asio::error::eof) return; // Соединение закрыто клиентом
        if (error) throw boost::system::system_error(error);

        std::string command(buffer, length);

        // Удаление лишних символов новой строки и пробелов
        command.erase(std::remove(command.begin(), command.end(), '\n'), command.end());
        command.erase(std::remove(command.begin(), command.end(), '\r'), command.end());

        // Отладочный вывод полученной команды
        std::cout << "Received command: '" << command << "'" << std::endl;

        std::string response;

        // Обработка команды
        if (command == "tasks") {
            response = get_task_list(); // Вызов функции из task_manager.cpp
        } else if (command.rfind("elf ", 0) == 0) { // Если команда начинается с "elf "
            std::string file_path = command.substr(4); // Извлекаем путь к файлу
            response = parse_elf_file(file_path);      // Вызов функции из elf_viewer.cpp
        } else {
            response = "Unknown command.";
        }

        // Отправка ответа клиенту
        boost::asio::write(socket, boost::asio::buffer(response), error);

    } catch (std::exception& e) {
        std::cerr << "Error handling client: " << e.what() << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_context io_context; // Создаём объект контекста для работы с сетевыми операциями

        // Создание TCP сервера, который прослушивает порт 12345
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));
        std::cout << "Server is running on port 12345..." << std::endl;

        while (true) {
            tcp::socket socket(io_context); // Создаём сокет для подключения клиента
            acceptor.accept(socket);       // Ожидание подключения клиента

            std::cout << "Client connected." << std::endl;
            handle_client(socket);         // Обработка подключившегося клиента
            std::cout << "Client disconnected." << std::endl;
        }

    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    return 0;
}