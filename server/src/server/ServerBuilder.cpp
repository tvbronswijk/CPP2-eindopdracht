#include "server/Server.hpp"

void command_thread(Server& server)
{
    try {
        while (server.is_running()) {
            ClientCommand command = server.dequeue_command();
            if (auto clientInfo = command.get_client_info().lock()) {
                auto& client = clientInfo->get_socket();
                try {
                    server._handler->on_command(command);
                }
                catch (const std::exception& ex) {
                    server << "*** exception in consumer thread for socket " << client.get_socket() << ": " << ex.what() << '\n';
                    if (client.is_open()) client << "Sorry, something went wrong during handling of your request.\r\n";
                }
                catch (...) {
                    server << "*** exception in consumer thread for socket " << client.get_socket() << '\n';
                    if (client.is_open()) client << "Sorry, something went wrong during handling of your request.\r\n";
                }
            }
        }
    }
    catch (...) {
        server << "The command thread crashed.\n";
    }
}

std::unique_ptr<Server> Server::Builder::build(std::unique_ptr<ServerCallbackHandler> handler) {
    auto server = std::make_unique<Server>(_server_name, _prompt, _port, std::move(handler));
    server->_command_thread = std::thread{ command_thread, std::ref(*server) };
    return std::move(server);
}

Server::Builder create_server() { return Server::Builder(); }