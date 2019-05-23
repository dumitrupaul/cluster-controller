#ifndef LOCALCLIENT_HPP
#define LOCALCLIENT_HPP
#include "ClusterIncludes.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace ClusterController
{
    using boost::asio::ip::tcp;
    
    class LocalClient
    {
    public:
        LocalClient(boost::asio::io_service &io_service, boost::asio::ssl::context& context);

        void startConnection();

    private:
        bool verifyCertificate(bool preverified, boost::asio::ssl::verify_context& ctx);

        void onConnect(const boost::system::error_code &error);

        void handshake();

        void writeMessage();

        void handleInput();

        void onWrite(const boost::system::error_code &error, size_t bytes_transferred);
        
        int m_serverPort;
        boost::asio::ssl::stream<tcp::socket> m_socket;
        boost::asio::streambuf m_txBuffer;
        boost::asio::ip::address m_ipAddress;
    };
}
#endif //LOCALCLIENT_HPP