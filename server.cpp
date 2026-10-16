#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 8080;
const int BUFFER_SIZE = 4096;

int main() {
    // 1. Create a socket (IPv4, TCP)
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // Allow immediate reuse of port after stopping server
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Bind socket to localhost and port 8080
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed on port " << PORT << "\n";
        close(server_fd);
        return 1;
    }

    // 3. Start listening for incoming connections
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed\n";
        close(server_fd);
        return 1;
    }

    std::cout << "🚀 Custom C++ HTTP Server is running on http://localhost:" << PORT << " ...\n";

    // 4. Main connection handling loop
    while (true) {
        sockaddr_in client_address{};
        socklen_t client_addr_len = sizeof(client_address);
        
        int client_socket = accept(server_fd, (struct sockaddr*)&client_address, &client_addr_len);
        if (client_socket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        // Read raw incoming HTTP request from client socket
        char buffer[BUFFER_SIZE] = {0};
        ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
        if (bytes_read > 0) {
            std::cout << "\n--- Received HTTP Request ---\n" << buffer << "-----------------------------\n";
        }

        // Craft valid HTTP/1.1 response
        std::string html_body = "<html><body style='font-family:sans-serif; text-align:center; margin-top:50px;'><h1>Hello from my custom C++ Systems Server!</h1><p>Built with raw POSIX sockets and C++20.</p></body></html>";
        std::string http_response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: " + std::to_string(html_body.size()) + "\r\n"
            "Connection: close\r\n\r\n" + 
            html_body;

        // Send response back to browser/client
        write(client_socket, http_response.c_str(), http_response.size());

        // Close connection socket for this client
        close(client_socket);
    }

    // Cleanup server socket
    close(server_fd);
    return 0;
}
