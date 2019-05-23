#ifndef TCPCONNECTION_HPP
#define TCPCONNECTION_HPP
#include "ClusterIncludes.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace ClusterController
{
    using boost::asio::ip::tcp;

    class TcpConnection : public boost::enable_shared_from_this<TcpConnection>,
                          public boost::noncopyable
    {

    public:
        typedef boost::shared_ptr<TcpConnection> td_tcpConnPointer;

        static td_tcpConnPointer create(boost::asio::io_service &io_service, boost::asio::ssl::context& context);

        boost::asio::ssl::stream<tcp::socket> &getSocket();

        void startServerConnection();

        void startHandshake();

        ~TcpConnection() = default;

    private:
        TcpConnection(boost::asio::io_service &io_service, boost::asio::ssl::context& context);

        void onRead(const boost::system::error_code &error, size_t bytes_transferred);

        boost::asio::ssl::stream<tcp::socket> m_socket;
        boost::asio::streambuf m_rxBuffer;
    };
}
#endif //TCPCONNECTION_HPP