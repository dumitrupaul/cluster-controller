#ifndef TCPCONNECTION_HPP
#define TCPCONNECTION_HPP
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <string>


namespace ClusterController
{
    using boost::asio::ip::tcp;

    class TcpConnection : public boost::enable_shared_from_this<TcpConnection>,
                        public boost::noncopyable
    {

    public:
        typedef boost::shared_ptr<TcpConnection> td_tcpConnPointer;

        static td_tcpConnPointer create(boost::asio::io_service &io_service);

        tcp::socket &getSocket();

        void startServerConnection();

    private:
        TcpConnection(boost::asio::io_service &io_service);

        void onRead(const boost::system::error_code &error, size_t bytes_transferred);

        tcp::socket m_socket;
        std::string m_rxBuffer;
    };
}
#endif //TCPCONNECTION_HPP