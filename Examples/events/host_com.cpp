#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <mutex>
#include <queue>
#include <atomic>
#include "config_events.h"
#include "cg_enums.h"
#include "custom.hpp"
#include "posix_thread.hpp"
#include "StreamNode.hpp"
extern "C"
{
#include "cstream_node.h"
}
#include "EventQueue.hpp"
#include "GenericNodes.hpp"
#include "scheduler.h"

#include "cg_pack.hpp"

#include "host_com.hpp"

using namespace arm_cmsis_stream;

#if defined(CG_HOST)

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
typedef SOCKET socket_t;
#define CLOSESOCKET closesocket
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h> // For fcntl(), F_GETFL, F_SETFL, O_NONBLOCK

typedef int socket_t;
#define CLOSESOCKET close
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif

std::atomic<socket_t> server_fd{INVALID_SOCKET};
socket_t client_socket = INVALID_SOCKET;

static CG_MUTEX queue_mutex;
using Msg = std::vector<uint8_t>; // Node ID, Data

static std::queue<Msg> msg_to_host;

void send_data_to_host(const char *data, std::size_t size)
{
    CG_MUTEX_ERROR_TYPE error;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, error);
    if (client_socket != INVALID_SOCKET)
    {
        // Message is nodeid and event
        // Messages to host are ignored when there is
        // an overflow of the FIFO
        if (msg_to_host.size() < MAX_NB_HOST_MSGS)
        {
            msg_to_host.push(std::vector<uint8_t>(data, data + size));
        }
    }
    CG_EXIT_CRITICAL_SECTION(queue_mutex, error);
}

void send_event_to_host(int nodeid, Event &&evt)
{
    Pack packer;
    // event id, valued id, value
    // value can be normal value
    // or combined value
    packer.pack(nodeid, std::move(evt));

    std::vector<uint8_t> d = packer.vector();
    // for(const uint8_t &byte : d)
    //{
    //     printf("\\x%02x", byte);
    // }
    // std::cout << std::endl;

    send_data_to_host((const char *)d.data(), d.size());
}

void clean_queue()
{
    CG_MUTEX_ERROR_TYPE error;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, error);
    while (!msg_to_host.empty())
    {
        msg_to_host.pop();
    }
    CG_EXIT_CRITICAL_SECTION(queue_mutex, error);
}

std::size_t send_all(socket_t sock, const char *data, std::size_t len)
{
    std::size_t total_sent = 0;
    while (total_sent < len)
    {
        int sent = send(sock, data + total_sent, (int)(len - total_sent), 0);
        if (sent <= 0)
        {
            // handle error or disconnection
            return -1;
        }
        total_sent += sent;
    }
    return total_sent;
}

void process_queue()
{
    CG_MUTEX_ERROR_TYPE error;
    CG_ENTER_CRITICAL_SECTION(queue_mutex, error);
    // std::cout << "process_queue: msg_to_host.size()=" << msg_to_host.size() << std::endl;

    while (!msg_to_host.empty())
    {
        if (client_socket != INVALID_SOCKET)
        {
            Msg msg = msg_to_host.front();
            msg_to_host.pop();

            std::size_t bytes_sent;
            std::size_t msg_len;
            // Size of message not include the msg length field
            msg_len = msg.size();
            char *data = (char *)malloc(msg_len + 4);
            memcpy(data, &msg_len, 4);                // Copy size at the beginning
            memcpy(data + 4, msg.data(), msg.size()); // Copy data after size and node ID

            bytes_sent = send_all(client_socket, data, msg_len + 4);
            free(data);
            if (bytes_sent == SOCKET_ERROR)
            {
#if defined(_WIN32)
                int err = WSAGetLastError(); // Use errno on POSIX
#else
                int err = errno;
#endif
                printf("Send failed: %d\n", err);
            }
        }
    }
    CG_EXIT_CRITICAL_SECTION(queue_mutex, error);
    // std::cout << "process_queue: done\n";
}

template <typename T>
void disp_array(std::ostream &os, const T *obj, std::size_t nb)
{
    std::size_t k = 0;
    for (std::size_t i = 0; i < nb; i++)
    {
        if (k == 10)
        {
            os << std::endl;
            k = 0;
        }

        os << obj[i] << " ";

        k++;
    }
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const TensorPtr<T> &obj)
{
    obj.lock_shared([&os](CG_MUTEX_ERROR_TYPE error, const Tensor<T> &t)
                    {

            os << "Tensor[";
            for (uint8_t i = 0; i < t.nb_dims; ++i)
            {
                os << t.dims[i];
                if (i < t.nb_dims - 1)
                    os << ", ";
            }
            os << "]"; 
            disp_array(os, t.data.get(), t.size()); });
    return os;
}

std::ostream &operator<<(std::ostream &os, const cg_any_tensor &obj)
{
    if (std::holds_alternative<TensorPtr<uint8_t>>(obj))
    {
        os << "uint8_t ";
        auto tensor = std::get<TensorPtr<uint8_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<uint16_t>>(obj))
    {
        os << "uint16_t ";
        auto tensor = std::get<TensorPtr<uint16_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<uint32_t>>(obj))
    {
        os << "uint32_t ";
        auto tensor = std::get<TensorPtr<uint32_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<uint64_t>>(obj))
    {
        os << "uint64_t ";
        auto tensor = std::get<TensorPtr<uint64_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<int8_t>>(obj))
    {
        os << "int8_t ";
        auto tensor = std::get<TensorPtr<int8_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<int16_t>>(obj))
    {
        os << "int16_t ";
        auto tensor = std::get<TensorPtr<int16_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<int32_t>>(obj))
    {
        os << "int32_t ";
        auto tensor = std::get<TensorPtr<int32_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<int64_t>>(obj))
    {
        os << "int64_t ";
        auto tensor = std::get<TensorPtr<int64_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<double>>(obj))
    {
        os << "double ";
        auto tensor = std::get<TensorPtr<double>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<float>>(obj))
    {
        os << "float ";
        auto tensor = std::get<TensorPtr<float>>(obj);
        os << tensor;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const cg_any_const_tensor &obj)
{
    if (std::holds_alternative<TensorPtr<const uint8_t>>(obj))
    {
        os << "const uint8_t ";
        auto tensor = std::get<TensorPtr<const uint8_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const uint16_t>>(obj))
    {
        os << "const uint16_t ";
        auto tensor = std::get<TensorPtr<const uint16_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const uint32_t>>(obj))
    {
        os << "const uint32_t ";
        auto tensor = std::get<TensorPtr<const uint32_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const uint64_t>>(obj))
    {
        os << "const uint64_t ";
        auto tensor = std::get<TensorPtr<const uint64_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const int8_t>>(obj))
    {
        os << "const int8_t ";
        auto tensor = std::get<TensorPtr<const int8_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const int16_t>>(obj))
    {
        os << "const int16_t ";
        auto tensor = std::get<TensorPtr<const int16_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const int32_t>>(obj))
    {
        os << "const int32_t ";
        auto tensor = std::get<TensorPtr<const int32_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const int64_t>>(obj))
    {
        os << "const int64_t ";
        auto tensor = std::get<TensorPtr<const int64_t>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const double>>(obj))
    {
        os << "const double ";
        auto tensor = std::get<TensorPtr<const double>>(obj);
        os << tensor;
    }
    if (std::holds_alternative<TensorPtr<const float>>(obj))
    {
        os << "const float ";
        auto tensor = std::get<TensorPtr<const float>>(obj);
        os << tensor;
    }
    return os;
}


// Overload operator<<
std::ostream &operator<<(std::ostream &os, const cg_value &obj)
{
    if (std::holds_alternative<int8_t>(obj.value))
    {
        os << std::get<int8_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<int16_t>(obj.value))
    {
        os << std::get<int16_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<int32_t>(obj.value))
    {
        os << std::get<int32_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<int64_t>(obj.value))
    {
        os << std::get<int64_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<uint8_t>(obj.value))
    {
        os << std::get<uint8_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<uint16_t>(obj.value))
    {
        os << std::get<uint16_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<uint32_t>(obj.value))
    {
        os << std::get<uint32_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<uint64_t>(obj.value))
    {
        os << std::get<uint64_t>(obj.value);
        return os;
    }

    if (std::holds_alternative<float>(obj.value))
    {
        os << std::get<float>(obj.value);
        return os;
    }

    if (std::holds_alternative<double>(obj.value))
    {
        os << std::get<double>(obj.value);
        return os;
    }

    if (std::holds_alternative<BufferPtr>(obj.value))
    {
        std::get<BufferPtr>(obj.value).lock_shared([&os](CG_MUTEX_ERROR_TYPE error, const RawBuffer &buf)
                                                   { os << "Buffer(size=" << buf.buf_size << ")"; });
        return os;
    }

    if (std::holds_alternative<std::string>(obj.value))
    {
        os << std::get<std::string>(obj.value);
        return os;
    }

    if (std::holds_alternative<cg_any_tensor>(obj.value))
    {
        cg_any_tensor anyt = std::get<cg_any_tensor>(obj.value);
        os << anyt;
    }

    if (std::holds_alternative<cg_any_const_tensor>(obj.value))
    {
        cg_any_const_tensor anyt = std::get<cg_any_const_tensor>(obj.value);
        os << anyt;
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const ListValue &obj)
{
    os << "CombinedValue: "
       << "nb_values=" << obj.nb_values;
    for (uint32_t i = 0; i < obj.nb_values; ++i)
    {
        os << ", value[" << i << "]=" << obj.values[i];
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, Event obj)
{
    os << "Event: event_id=" << obj.event_id
       << ", priority=" << obj.priority;
    if (std::holds_alternative<cg_value>(obj.data))
    {
        os << ", data=" << std::get<cg_value>(obj.data);
    }
    else if (std::holds_alternative<std::shared_ptr<ListValue>>(obj.data))
    {
        os << ", combined_data=" << *std::get<std::shared_ptr<ListValue>>(obj.data);
    }
    else
    {
        os << ", data=unknown";
    }
    return os;
}

class MessageReceiver
{
    socket_t sock;
    std::vector<uint8_t> buffer;

public:
    MessageReceiver(socket_t socket_fd) : sock(socket_fd) {}

    bool receiveMessages()
    {
        char temp[4096];
        int bytes_received = recv(sock, temp, sizeof(temp), 0);
        if (bytes_received <= 0)
        {
#if defined(_WIN32)
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK)
            {
                return true; // No data available, continue waiting
            }
            else if (err == WSAECONNRESET)
            {
                std::cerr << "Connection reset by peer\n";
                return false; // Connection closed
            }
            else
            {
                std::cerr << "Receive failed: " << err << "\n";
                return false; // Error occurred
            }
#else
            if ((bytes_received < 0) && (errno == EWOULDBLOCK || errno == EAGAIN))
            {
                return true; // No data available, continue waiting
            }
            else if (bytes_received == 0)
            {
                std::cerr << "Connection closed by peer\n";
                return false; // Connection closed
            }
            else if (bytes_received < 0 && errno == ECONNRESET)
            {
                std::cerr << "Connection reset by peer\n";
                return false; // Connection closed
            }
            else
            {
                std::cerr << "Receive failed: " << strerror(errno) << "\n";
                return false; // Error occurred
            }
#endif
        }

        buffer.insert(buffer.end(), temp, temp + bytes_received);

        // Process all complete messages in buffer
        while (true)
        {
            // std::cout << "Buffer size: " << buffer.size() << std::endl;
            if (buffer.size() < 4)
                break; // Not enough data for header

            // Extract message length
            uint32_t msg_len;
            std::memcpy(&msg_len, buffer.data(), 4);
            // std::cout << "Message len: " << msg_len << std::endl;
            //  msg_len = ntohl(msg_len); // Assume network byte order

            if (buffer.size() < 4 + msg_len)
                break; // Wait for full message

            // Extract full message
            std::vector<uint8_t> message(buffer.begin() + 4, buffer.begin() + 4 + msg_len);
            handleMessage(message);
            // Remove processed message from buffer
            buffer.erase(buffer.begin(), buffer.begin() + 4 + msg_len);
            // To send data to host
            process_queue();
        }

        return true;
    }

    void handleMessage(const std::vector<uint8_t> &message)
    {
        uint32_t node_id;

        Unpack unpack(message.data(), message.size());

        Event evt = unpack.unpack(node_id);
        // std::cout << "Received event: " << evt.event_id << std::endl;
        // std::cout << "for node " << node_id << std::endl;

        CStreamNode *node = get_scheduler_node(node_id);
        if ((node) && (node->obj != nullptr))
        {
            // NodeBase is the top class in the class hierarchy
            StreamNode *n = static_cast<StreamNode *>(node->obj);

            if (n)
            {
                // Async send to the event queue
                // Host always send to port 0
                // Message from host is not shared since it is sent to only one node
                Message msg = {n, 0, std::move(evt)};
                if (EventQueue::cg_eventQueue)
                    EventQueue::cg_eventQueue->push(std::move(msg));
            }
            else
            {
                std::cout << "Can't cast node to stream interface.\n";
            }
        }
        else
        {
            std::cout << "Unknown node\n";
        }
    }
};

void close_host()
{
    socket_t sock = server_fd.exchange(INVALID_SOCKET); // atomically get and reset
    if (sock != INVALID_SOCKET)
    {
        CLOSESOCKET(sock); // This unblocks accept()
    }
}

void listen_to_host()
{
    const int PORT = 8080;
    const int BACKLOG = 5;
    const int BUF_SIZE = 1024;
    char buffer[BUF_SIZE] = {0};

#if defined(_WIN32)
    WSADATA wsaData;
    int wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaerr != 0)
    {
        std::cerr << "WSAStartup failed with error: " << wsaerr << "\n";
        return;
    }
#endif

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Socket creation failed\n";
        return;
    }

    // Allow socket address reuse
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (::bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed\n";
        CLOSESOCKET(server_fd);
#if defined(_WIN32)
        WSACleanup();
#endif
        return;
    }

    if (listen(server_fd, BACKLOG) < 0)
    {
        std::cerr << "Listen failed\n";
        CLOSESOCKET(server_fd);
#if defined(_WIN32)
        WSACleanup();
#endif
        return;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    while (true)
    {
        client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0)
        {
            std::cerr << "Accept failed\n";
            CLOSESOCKET(server_fd);
#if defined(_WIN32)
            WSACleanup();
#endif
            return;
        }
        clean_queue();
        // Set recv non blocking
#if defined(_WIN32)
        u_long mode = 1;
        ioctlsocket(client_socket, FIONBIO, &mode);
#else
        int flags = fcntl(client_socket, F_GETFL, 0);
        fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);
#endif
        std::cout << "Client connected\n";

        MessageReceiver receiver(client_socket);
        while (receiver.receiveMessages())
        {
            // To send data to host
            process_queue();
        };
        std::cout << "Client disconnected\n\n";
        CLOSESOCKET(client_socket);
        client_socket = INVALID_SOCKET; // Reset client socket
        clean_queue();
    }

    CLOSESOCKET(server_fd);

#if defined(_WIN32)
    WSACleanup();
#endif

    return;
}

#endif // CG_HOST