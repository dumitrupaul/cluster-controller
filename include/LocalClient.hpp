#ifndef LOCALCLIENT_HPP
#define LOCALCLIENT_HPP
#include "ClusterIncludes.hpp"
#include <boost/asio.hpp>
#include <vector>
#include "Features.hpp"

namespace ClusterController
{
    using boost::asio::ip::tcp;

    class LocalClient
    {
    public:
        LocalClient(boost::asio::io_service &io_service, int serverPort, bool aMode);

        void startConnection();

    private:
        void onConnect(const boost::system::error_code &error);

        void writeMessage();

        void handleInput();
        
        void handleGPIO();
        
        bool scanButtons(std::vector<Button>& buttons);

        void onWrite(const boost::system::error_code &error, size_t bytes_transferred);
        
        int m_serverPort;
        const bool autoMode;
        tcp::socket m_socket;
        uint32_t sentMessageTimeInMs;
        boost::asio::streambuf m_txBuffer;
        boost::asio::ip::address m_ipAddress;
    };
}
#endif //LOCALCLIENT_HPP
