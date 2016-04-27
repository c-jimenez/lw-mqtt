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

#ifndef SOCKET_STREAM_H
#define SOCKET_STREAM_H

#include "input_stream.h"
#include "output_stream.h"
#include "mqtt_socket.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** \brief Initialize an input stream from a socket */
bool socket_stream_input_from_socket(input_stream_t* const stream, mqtt_socket_t* const mqtt_socket);

/** \brief Initialize an output stream from a socket */
bool socket_stream_output_from_socket(output_stream_t* const stream, mqtt_socket_t* const mqtt_socket);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SOCKET_STREAM_H */
