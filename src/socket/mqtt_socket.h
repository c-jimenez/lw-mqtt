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

#ifndef MQTT_SOCKET_H
#define MQTT_SOCKET_H

#include "stdheaders.h"
#include "mqtt_socket_t.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Initialize the MQTT socket module */
bool mqtt_socket_init(void);

/** \brief De-initialize the MQTT socket module */
bool mqtt_socket_deinit(void);

/** \brief Open a MQTT socket */
bool mqtt_socket_open(mqtt_socket_t* const mqtt_socket, const bool non_blocking);

/** \brief Close a MQTT socket */
bool mqtt_socket_close(mqtt_socket_t* const mqtt_socket);

/** \brief Connect a MQTT socket to a specific IP address and port */
bool mqtt_socket_connect(mqtt_socket_t* const mqtt_socket, const char* const ip_address, const uint16_t port);

/** \brief Check if a MQTT socket is connected */
bool mqtt_socket_is_connected(mqtt_socket_t* const mqtt_socket);

/** \brief Bind a MQTT socket to a specific IP address and port */
bool mqtt_socket_bind(mqtt_socket_t* const mqtt_socket, const char* const ip_address, const uint16_t port);

/** \brief Put a MQTT socket in listen state */
bool mqtt_socket_listen(mqtt_socket_t* const mqtt_socket);

/** \brief Accept an incoming connection on a MQTT socket */
bool mqtt_socket_accept(mqtt_socket_t* const mqtt_socket, mqtt_socket_t* const client_mqtt_socket);

/** \brief Send data on a MQTT socket */
bool mqtt_socket_send(mqtt_socket_t* const mqtt_socket, const void* data, const size_t size, size_t* const sent);

/** \brief Receive data on a MQTT socket */
bool mqtt_socket_receive(mqtt_socket_t* const mqtt_socket, void* data, const size_t size, size_t* const received);

/** \brief Wait for data ready to receive on the socket */
bool mqtt_socket_select(mqtt_socket_t* const mqtt_socket, const uint32_t ms_timeout);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MQTT_SOCKET_H */
