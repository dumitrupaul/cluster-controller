#ifndef LOCALCLIENT_HPP
#define LOCALCLIENT_HPP
#include <boost/asio.hpp>
#include <string>
#include <vector>

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

        void writeMessage();

        void handleInput();

        void onWrite(const boost::system::error_code &error, size_t bytes_transferred);

        void connectionTimerExpired();
        
        int m_serverPort;
        tcp::socket m_socket;
        boost::asio::streambuf m_txBuffer;
        boost::asio::deadline_timer connectionTimer;
        boost::asio::ip::address m_ipAddress;
    };
}
#endif //LOCALCLIENT_HPP