#include "TcpServer.hpp"
#include <boost/bind.hpp>
#include <iostream>

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

    std::string TcpServer::getIpAddress()
    {
        boost::system::error_code error;
        boost::asio::ip::tcp::endpoint endpoint = serverAcceptor.local_endpoint(error);
        if (error)
        {
            // An error occurred.
            CLUSTER_LOG(error) << "Can't get local IP Address";
            return nullptr;
        }

        return std::string(endpoint.address().to_string().c_str());
    }
}