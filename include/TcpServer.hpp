#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP
#include "ClusterIncludes.hpp"
#include "SSLServerConnection.hpp"
#include <boost/asio/ssl.hpp>

namespace ClusterController
{
    using boost::asio::ip::tcp;

    class TcpServer
    {
    public:
        TcpServer(boost::asio::io_service &io_service);

        ~TcpServer() = default;

    private:
        void startAccept();

        std::string getIpAddress(); // not functional

        std::string getPassword() const;

        void onAccept(SSLServerConnection::td_tcpConnPointer newConn, const boost::system::error_code &error);

        tcp::acceptor mServerAcceptor;
        boost::asio::ssl::context mContext;
    };
}
#endif //TCPSERVER_HPP