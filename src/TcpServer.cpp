#include "TcpServer.hpp"
#include <boost/bind.hpp>

namespace ClusterController
{

    TcpServer::TcpServer(boost::asio::io_service &io_service, int serverPort) : 
                        serverAcceptor(io_service, tcp::endpoint(tcp::v4(), serverPort))
    {
        startAccept();
    }

    void TcpServer::startAccept()
    {
        TcpConnection::td_tcpConnPointer newConn = TcpConnection::create(serverAcceptor.get_io_service());

        serverAcceptor.async_accept(newConn->getSocket(),
                                    boost::bind(&TcpServer::onAccept, this, newConn,
                                                boost::asio::placeholders::error));
    }

    void TcpServer::onAccept(TcpConnection::td_tcpConnPointer newConn, const boost::system::error_code &error)
    {
        if (!error)
        {
            newConn->startServerConnection();
        }

        startAccept();
    }
}