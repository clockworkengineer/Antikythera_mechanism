#include "HOST.hpp"
/*
 * File:   CSocket.cpp
 * 
 * Author: Robert Tizzard
 * 
 * Created on October 10, 2017, 2:33 PM
 *
 * Copyright 2017.
 *
 */

//
// Class: CSocket
// 
// Description: Class for connecting to / listening for connections from remote peers
// and the reading/writing of data using sockets. It supports both plain and TLS/SSL 
// connections and  is implemented using BOOST:ASIO synchronous API calls.
//
// Note: TLS/SSL connections are supported.
//
// Dependencies:   C11++        - Language standard features used.
//                 BOOST ASIO   - Used to talk to FTP server.
//

// =================
// CLASS DEFINITIONS
// =================

#include "CSocket.hpp"

// ====================
// CLASS IMPLEMENTATION
// ====================

//
// C++ STL
//

#include <iostream>
#include <fstream>

// =======
// IMPORTS
// =======

// =========
// NAMESPACE
// =========

namespace Antik {
    namespace Network {

        // ===========================
        // PRIVATE TYPES AND CONSTANTS
        // ===========================

        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================

        // ========================
        // PRIVATE STATIC VARIABLES
        // ========================

        // =======================
        // PUBLIC STATIC VARIABLES
        // =======================

        // ===============
        // PRIVATE METHODS
        // ===============

        //
        // Socket listener thread method for incoming connections. At present it listens
        // on a random port but sets m_hostPort.
        //

        void CSocket::connectionListener() {

            boost::asio::ip::tcp::acceptor acceptor(m_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0));

            m_hostPort = std::to_string(acceptor.local_endpoint().port());

            m_isListenThreadRunning = true;

            m_socket.reset(new SSLSocket(m_ioService, m_sslContext));
            if (!m_socket) {
                std::cerr << "Failure : Could not create socket." << std::endl;
                throw Exception("Could not create socket.");
            }

            acceptor.accept(m_socket->next_layer(), m_socketError);
            if (m_socketError) {
                m_socket.reset();
                m_isListenThreadRunning = false;
                throw Exception(m_socketError.message());
            }

            m_isListenThreadRunning = false;


        }

        // ==============
        // PUBLIC METHODS
        // ==============

        //
        // Cleanup after socket connection. This includes stopping any unused listener
        // thread and closing the socket if still open.
        //

        void CSocket::cleanup() {

            if (m_isListenThreadRunning && m_socketListenThread) {
                m_isListenThreadRunning = false;
                try {
                    boost::asio::ip::tcp::socket socket{ m_ioService};
                    boost::asio::ip::tcp::resolver::query query(m_hostAddress, m_hostPort);
                    boost::asio::connect(socket, m_ioQueryResolver.resolve(query));
                    socket.close();
                } catch (std::exception &e) {
                    throw Exception("Listener thread running when it should not be.");
                }
                m_socketListenThread->join();
            }

            close();

        }

        //
        // Listen for connections
        //

        void CSocket::listenForConnection() {

            m_socketListenThread.reset(new std::thread(&CSocket::connectionListener, this));
            while (!m_isListenThreadRunning) { // Wait for until listening before sending PORT command
                continue; // Could use conditional but use existing flag for now
            }

        }

        //
        // Wait until a socket is connected.
        //

        void CSocket::waitUntilConnected() {

            // No socket present

            if (!m_socket) {
                throw Exception("No socket present.");
            }

            // Listener thread is running (wait for it to finish)

            if (m_socketListenThread) {
                m_socketListenThread->join();
            }

            // TLS handshake

            if (m_sslActive) {
                tlsHandshake();
            }

        }

        //
        // Connect to a given host and port.
        //

        void CSocket::connect() {

            m_socket.reset(new SSLSocket(m_ioService, m_sslContext));
            if (!m_socket) {
                throw Exception("Could not create socket.");
            }
            
            boost::asio::ip::tcp::resolver::query query(m_hostAddress, m_hostPort);
            m_socket->next_layer().connect(*m_ioQueryResolver.resolve(query), m_socketError);
            if (m_socketError) {
                throw Exception(m_socketError.message());
            }

        }

        //
        // Read data from socket into buffer
        //

        size_t CSocket::read(char *readBuffer, size_t bufferLength) {

            // No socket present

            if (!m_socket) {
                throw Exception("No socket present.");
            }

            if (m_sslActive) {
                return (m_socket->read_some(boost::asio::buffer(readBuffer, bufferLength), m_socketError));
            } else {
                return (m_socket->next_layer().read_some(boost::asio::buffer(readBuffer, bufferLength), m_socketError));
            }

        }

        //
        // Write data to socket
        //

        size_t CSocket::write(const char *writeBuffer, size_t writeLength) {

            size_t bytesWritten = 0;

            // No socket present

            if (!m_socket) {
                throw Exception("No socket present.");
            }

            if (m_sslActive) {
                bytesWritten = m_socket->write_some(boost::asio::buffer(writeBuffer, writeLength), m_socketError);
            } else {
                bytesWritten = m_socket->next_layer().write_some(boost::asio::buffer(writeBuffer, writeLength), m_socketError);
            }

            if (getSocketError()) {
                throw Exception(getSocketError().message());
            }

            return (bytesWritten);

        }

        //
        // Perform TLS handshake on to enable SSL
        //

        void CSocket::tlsHandshake() {

            // No socket present

            if (!m_socket) {
                throw Exception("No socket present.");
            }

            m_socket->handshake(SSLSocket::client, m_socketError);
            if (m_socketError) {
                throw Exception(m_socketError.message());
            }
            m_sslActive = true;
        }

        //
        // Closedown any running SSL and close socket.
        //

        void CSocket::close() {

            if (m_socket && m_socket->next_layer().is_open()) {
                if (m_sslActive) {
                    m_socket->shutdown(m_socketError);
                }
                m_socket->next_layer().close();
                m_socket.reset();
            }

            if (m_socketListenThread) {
                m_socketListenThread.reset();
            }

        }

        //
        // Return true if socket closed by server otherwise false.
        // Also throw exception for any socket error detected,
        //

        bool CSocket::closedByRemotePeer() {

            if (m_socketError == boost::asio::error::eof) {
                return (true); // Connection closed cleanly by peer.
            } else if (m_socketError) {
                throw Exception(m_socketError.message());
            }

            return (false);

        }

        //
        // Work out ip address for local machine. This is quite difficult to achieve but
        // this is the best code i have seen for doing it. It just tries to connect to
        // google.com with a udp connect to get the local socket endpoint.
        // Note: Fall back of localhost on failure.
        // 

        std::string CSocket::localIPAddress() {

            static std::string localIPAddress;

            if (localIPAddress.empty()) {
                try {
                    boost::asio::io_service ioService;
                    boost::asio::ip::udp::resolver resolver(ioService);
                    boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), "google.com", "");
                    boost::asio::ip::udp::socket socket(ioService);
                    socket.connect(*resolver.resolve(query));
                    localIPAddress = socket.local_endpoint().address().to_string();
                    socket.close();
                } catch (std::exception &e) {
                    return ("127.0.0.1");
                }
            }

            return (localIPAddress);

        }

        // ============================
        // CLASS PRIVATE DATA ACCESSORS
        // ============================

        void CSocket::setSslActive(bool sslActive) {
            m_sslActive = sslActive;
        }

        bool CSocket::isSslActive() const {
            return m_sslActive;
        }

        boost::system::error_code CSocket::getSocketError() const {
            return m_socketError;
        }

        void CSocket::setHostAddress(std::string hostAddress) {
            m_hostAddress = hostAddress;
        }

        std::string CSocket::getHostAddress() const {
            return m_hostAddress;
        }

        void CSocket::setHostPort(std::string hostPort) {
            m_hostPort = hostPort;
        }

        std::string CSocket::getHostPort() const {
            return m_hostPort;
        }


    } // namespace Network
} // namespace Antik
