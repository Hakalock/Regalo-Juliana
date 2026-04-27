#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string mime_type(const std::string& path) {
    if (path.ends_with(".html")) return "text/html; charset=utf-8";
    if (path.ends_with(".css")) return "text/css; charset=utf-8";
    if (path.ends_with(".js")) return "application/javascript; charset=utf-8";
    if (path.ends_with(".png")) return "image/png";
    if (path.ends_with(".jpg") || path.ends_with(".jpeg")) return "image/jpeg";
    return "application/octet-stream";
}

std::string read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void send_response(int client, const std::string& status, const std::string& type, const std::string& body) {
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n";
    response << "Content-Type: " << type << "\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Connection: close\r\n\r\n";
    response << body;
    std::string data = response.str();
    send(client, data.c_str(), data.size(), 0);
}

std::string normalize_path(const std::string& request_path) {
    std::string path = request_path;
    size_t query = path.find('?');
    if (query != std::string::npos) path = path.substr(0, query);
    if (path == "/") path = "/index.html";
    if (path.find("..") != std::string::npos) return "";
    return "." + path;
}

int main() {
    const int port = 8080;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "No se pudo crear el socket.\n";
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        std::cerr << "No se pudo abrir el puerto " << port << ".\n";
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "No se pudo iniciar el servidor.\n";
        close(server_fd);
        return 1;
    }

    std::cout << "Servidor listo en http://localhost:" << port << "\n";
    std::cout << "Presiona Ctrl+C para detenerlo.\n";

    while (true) {
        int client = accept(server_fd, nullptr, nullptr);
        if (client < 0) continue;

        char buffer[4096] = {0};
        read(client, buffer, sizeof(buffer) - 1);

        std::istringstream request(buffer);
        std::string method, path, version;
        request >> method >> path >> version;

        if (method != "GET") {
            send_response(client, "405 Method Not Allowed", "text/plain; charset=utf-8", "Metodo no permitido");
            close(client);
            continue;
        }

        std::string file_path = normalize_path(path);
        if (file_path.empty()) {
            send_response(client, "403 Forbidden", "text/plain; charset=utf-8", "Ruta no permitida");
            close(client);
            continue;
        }

        std::string body = read_file(file_path);
        if (body.empty()) {
            send_response(client, "404 Not Found", "text/plain; charset=utf-8", "Archivo no encontrado");
        } else {
            send_response(client, "200 OK", mime_type(file_path), body);
        }

        close(client);
    }

    close(server_fd);
    return 0;
}
