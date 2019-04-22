#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP
#include "TcpConnection.hpp"

namespace ClusterController
{
    using boost::asio::ip::tcp;

    class TcpServer
    {
    public:
        TcpServer(boost::asio::io_service &io_service, int serverPort);

    private:
        void startAccept();

        void onAccept(TcpConnection::td_tcpConnPointer newConn, const boost::system::error_code &error);

        tcp::acceptor serverAcceptor;
    };
}
#endif //TCPSERVER_HPP