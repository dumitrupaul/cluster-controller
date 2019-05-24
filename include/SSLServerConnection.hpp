#ifndef SSLServerConnection_HPP
#define SSLServerConnection_HPP
#include "ClusterIncludes.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace ClusterController
{
    using boost::asio::ip::tcp;

    class SSLServerConnection : public boost::enable_shared_from_this<SSLServerConnection>,
                          public boost::noncopyable
    {

        public:
            typedef boost::shared_ptr<SSLServerConnection> td_tcpConnPointer;
            typedef boost::asio::ssl::stream<tcp::socket> td_streamSocket;

            static td_tcpConnPointer create(boost::asio::io_service &io_service, boost::asio::ssl::context& context);

            td_streamSocket &getSocket();

            void startServerConnection();

            void startHandshake();

            ~SSLServerConnection() = default;

        private:
            SSLServerConnection(boost::asio::io_service &io_service, boost::asio::ssl::context& context);

            void onRead(const boost::system::error_code &error, size_t bytes_transferred);

            td_streamSocket mSocket;
            boost::asio::streambuf mRxBuffer;
    };
}
#endif //SSLServerConnection_HPP