#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP
#include "ClusterIncludes.hpp"
#include "TcpConnection.hpp"
#include <boost/asio/ssl.hpp>

namespace ClusterController
{
    using boost::asio::ip::tcp;

    class TcpServer
    {
    public:
        TcpServer(boost::asio::io_service &io_service, int serverPort);

    private:
        void startAccept();

        std::string getIpAddress(); // not functional

        std::string getPassword() const;

        void onAccept(TcpConnection::td_tcpConnPointer newConn, const boost::system::error_code &error);

        tcp::acceptor serverAcceptor;
        boost::asio::ssl::context mContext;
    };
}
#endif //TCPSERVER_HPP