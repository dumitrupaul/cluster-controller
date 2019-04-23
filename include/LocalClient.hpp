#ifndef LOCALCLIENT_HPP
#define LOCALCLIENT_HPP
#include <boost/asio.hpp>
#include <string>

namespace ClusterController
{
    using boost::asio::ip::tcp;

    class LocalClient
    {
    public:
        LocalClient(boost::asio::io_service &io_service, int serverPort);

        void startConnection();

    private:
        void onConnect(const boost::system::error_code &error);

        void handleMessages();

        void onWrite(const boost::system::error_code &error, size_t bytes_transferred);
        
        int m_serverPort;
        tcp::socket m_socket;
        std::string m_txBuffer;
        std::string m_ipAddress;
    };
}
#endif //LOCALCLIENT_HPP