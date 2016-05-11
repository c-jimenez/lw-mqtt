/*
Copyright(c) 2016 Cedric Jimenez

This file is part of lw-mqtt.

lw-mqtt is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

lw-mqtt is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with lw-mqtt.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma warning(push, 3)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#pragma warning(pop)

#include "mqtt_socket.h"
#include "mqtt_error.h"



/** \brief Initialize the MQTT socket module */
bool mqtt_socket_init(void)
{
    WSADATA wsaData;
    const bool ret = (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);
    return ret;
}

/** \brief De-initialize the MQTT socket module */
bool mqtt_socket_deinit(void)
{
    const bool ret = (WSACleanup() == 0);
    return ret;
}

/** \brief Open a MQTT socket */
bool mqtt_socket_open(mqtt_socket_t* const mqtt_socket, const bool non_blocking)
{
    bool ret = false;

    /* Check params */
    if (mqtt_socket != NULL)
    {
        SOCKET sock;

        /* Create socket */
        sock = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
        if (sock != INVALID_SOCKET)
        {
            /* Save socket handle */
            mqtt_socket->data = (void*)sock;
            ret = true;

            /* Apply non-blocking IO option if needed */
            if (non_blocking)
            {
                u_long option_value = 1u;
                ret = (ioctlsocket(sock, FIONBIO, &option_value) == 0);
                if (!ret)
                {
                    closesocket(sock);
                }
            }
        }
        else
        {
            mqtt_socket->last_error = MQTT_ERR_SOCKET_FAILED;
        }
    }

    return ret;
}

/** \brief Close a MQTT socket */
bool mqtt_socket_close(mqtt_socket_t* const mqtt_socket)
{
    bool ret = false;

    /* Check params */
    if (mqtt_socket != NULL)
    {
        ret = (shutdown((SOCKET)(mqtt_socket->data), SD_BOTH) == 0);
        if (ret)
        {      
            ret = (closesocket((SOCKET)(mqtt_socket->data)) == 0);
        }
        if (!ret)
        {
            mqtt_socket->last_error = MQTT_ERR_SOCKET_FAILED;
        }
    }

    return ret;
}

/** \brief Connect a MQTT socket to a specific IP address and port */
bool mqtt_socket_connect(mqtt_socket_t* const mqtt_socket, const char* const ip_address, const uint16_t port)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_socket != NULL) &&
        (ip_address != NULL))
    {
        struct sockaddr_in addr = { 0 };
        addr.sin_family = PF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip_address);
        ret = (connect((SOCKET)(mqtt_socket->data), (struct sockaddr *)&addr, sizeof(addr)) == 0);
        if (!ret)
        {
            /* No connection, check error */
            int32_t err = (int32_t)WSAGetLastError();
            if ((err == WSAEWOULDBLOCK) || (err == WSAEINPROGRESS))
            {
                /* Connection pending */
                mqtt_socket->last_error = MQTT_ERR_SOCKET_PENDING;
            }
            else
            {
                /* Connection rejected */
                mqtt_socket->last_error = MQTT_ERR_SOCKET_FAILED;
            }
        }
    }

    return ret;
}

/** \brief Check if a MQTT socket is connected */
bool mqtt_socket_is_connected(mqtt_socket_t* const mqtt_socket)
{
    bool ret = false;

    /* Check params */
    if (mqtt_socket != NULL)
    {
        
        /* Check connection */
        int32_t callret;
        fd_set fd_write;
        struct timeval timeout;

        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        FD_ZERO(&fd_write);
        FD_SET((SOCKET)(mqtt_socket->data), &fd_write);

        callret = select((int)((SOCKET)(mqtt_socket->data)) + 1, NULL, &fd_write, NULL, &timeout);
        if (callret > 0)
        {
            /* Connected */
            ret = true;
        }
        else if (callret == 0)
        {
            /* Connection pending */
            mqtt_socket->last_error = MQTT_ERR_SOCKET_PENDING;
        }
        else
        {
            /* Connection rejected */
            mqtt_socket->last_error = MQTT_ERR_SOCKET_FAILED;
        }
    }

    return ret;
}

/** \brief Bind a MQTT socket to a specific IP address and port */
bool mqtt_socket_bind(mqtt_socket_t* const mqtt_socket, const char* const ip_address, const uint16_t port)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_socket != NULL) &&
        (ip_address != NULL))
    {
        struct sockaddr_in addr = { 0 };
        addr.sin_family = PF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip_address);
        ret = (bind((SOCKET)(mqtt_socket->data), (struct sockaddr *)&addr, sizeof(addr)) == 0);
        if (!ret)
        {
            mqtt_socket->last_error = MQTT_ERR_SOCKET_FAILED;
        }
    }

    return ret;
}

/** \brief Put a MQTT socket in listen state */
bool mqtt_socket_listen(mqtt_socket_t* const mqtt_socket)
{
    bool ret = false;

    /* Check params */
    if (mqtt_socket != NULL)
    {
        ret = (listen((SOCKET)(mqtt_socket->data), SOMAXCONN) == 0);
        if (!ret)
        {
            mqtt_socket->last_error = MQTT_ERR_SOCKET_FAILED;
        }
    }

    return ret;
}

/** \brief Accept an incoming connection on a MQTT socket */
bool mqtt_socket_accept(mqtt_socket_t* const mqtt_socket, mqtt_socket_t* const client_mqtt_socket)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_socket != NULL) &&
        (client_mqtt_socket != NULL))
    {
        struct sockaddr_in addr = { 0 };
        int32_t addr_size = sizeof(addr);

        const SOCKET client_socket = (int32_t)accept((SOCKET)(mqtt_socket->data), (struct sockaddr*)&addr, (int*)&addr_size);
        if (client_socket != INVALID_SOCKET)
        {
            /* Success */
            client_mqtt_socket->data = (void*)client_socket;
            ret = true;
        }
        else
        {
            int32_t err = (int32_t)WSAGetLastError();
            if ((err == WSAEWOULDBLOCK) || (err == WSAEINPROGRESS))
            {
                /* Send pending */
                mqtt_socket->last_error = MQTT_ERR_SOCKET_PENDING;
            }
            else
            {
                /* Error */
                mqtt_socket->last_error = MQTT_ERR_SOCKET_FAILED;
            }
        }

    }

    return ret;
}

/** \brief Send data on a MQTT socket */
bool mqtt_socket_send(mqtt_socket_t* const mqtt_socket, const void* data, const size_t size, size_t* const sent)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_socket != NULL) &&
        (data != NULL) &&
        (sent != NULL))
    {
        const int32_t callret = (int32_t)send((SOCKET)(mqtt_socket->data), (const char*)data, (int)size, 0);
        if (callret >= 0)
        {
            /* Success */
            (*sent) = (size_t)(callret);
            ret = true;
        }
        else
        {
            int32_t err = (int32_t)WSAGetLastError();
            if ((err == WSAEWOULDBLOCK) || (err == WSAEINPROGRESS))
            {
                /* Send pending */
                mqtt_socket->last_error = MQTT_ERR_SOCKET_PENDING;
            }
            else
            {
                /* Error */
                mqtt_socket->last_error = MQTT_ERR_SOCKET_FAILED;
            }
        }

    }

    return ret;
}

/** \brief Receive data on a MQTT socket */
bool mqtt_socket_receive(mqtt_socket_t* const mqtt_socket, void* data, const size_t size, size_t* const received)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_socket != NULL) &&
        (data != NULL) &&
        (received != NULL))
    {
        const int32_t callret = (int32_t)recv((SOCKET)(mqtt_socket->data), (char*)data, (int)size, 0);
        if (callret >= 0)
        {
            /* Success */
            (*received) = (size_t)(callret);
            ret = true;
        }
        else
        {
            int32_t err = (int32_t)WSAGetLastError();
            if ((err == WSAEWOULDBLOCK) || (err == WSAEINPROGRESS))
            {
                /* Receive pending */
                mqtt_socket->last_error = MQTT_ERR_SOCKET_PENDING;
            }
            else
            {
                /* Error */
                mqtt_socket->last_error = MQTT_ERR_SOCKET_FAILED;
            }
        }

    }

    return ret;
}

/** \brief Wait for data ready to receive on the socket */
bool mqtt_socket_select(mqtt_socket_t* const mqtt_socket, const uint32_t ms_timeout)
{
    bool ret = false;

    /* Check params */
    if (mqtt_socket != NULL)
    {

        /* Check connection */
        int32_t callret;
        fd_set fd_read;
        struct timeval timeout;

        timeout.tv_sec = (long)(ms_timeout / 1000u);
        timeout.tv_usec = (((long)ms_timeout) - (timeout.tv_sec * 1000)) * 1000;

        FD_ZERO(&fd_read);
        FD_SET((SOCKET)(mqtt_socket->data), &fd_read);

        callret = select((int)((SOCKET)(mqtt_socket->data)) + 1, &fd_read, NULL, NULL, &timeout);
        if (callret > 0)
        {
            /* Data ready */
            ret = true;
        }
        else if (callret == 0)
        {
            /* No data available */
            mqtt_socket->last_error = MQTT_ERR_SOCKET_PENDING;
        }
        else
        {
            /* Connection closed */
            mqtt_socket->last_error = MQTT_ERR_SOCKET_FAILED;
        }
    }

    return ret;
}
