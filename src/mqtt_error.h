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

#ifndef MQTT_ERROR_H
#define MQTT_ERROR_H

#include "mqtt_errno.h"

/** \brief No error */
#define MQTT_ERR_SUCCESS                    0

/** \brief Invalid parameter */
#define MQTT_ERR_INVALID_PARAM              -1

/** \brief Output stream is full or not big enough */
#define MQTT_ERR_OUTPUT_STREAM_FULL         -2

/** \brief Input stream is empty or doesn't contains enough data */
#define MQTT_ERR_INPUT_STREAM_EMPTY         -3

/** \brief Invalid packet type */
#define MQTT_ERR_INVALID_PACKET_TYPE        -4

/** \brief Invalid packet size */
#define MQTT_ERR_INVALID_PACKET_SIZE        -5

/** \brief Invalid packet payload */
#define MQTT_ERR_INVALID_PACKET_PAYLOAD     -6

/** \brief Invalid packet QoS */
#define MQTT_ERR_INVALID_PACKET_QOS         -7

/** \brief Invalid protocol name */
#define MQTT_ERR_INVALID_PROTOCOL_NAME      -8

/** \brief Provided MQTT string is too small to contain the data */
#define MQTT_ERR_MQTT_STRING_TOO_SMALL      -9

/** \brief Provided buffer is too small to contain the data */
#define MQTT_ERR_BUFFER_TOO_SMALL           -10

/** \brief Current action is in progress */
#define MQTT_ERR_IN_PROGRESS                -11

/** \brief Socket operation is pending */
#define MQTT_ERR_SOCKET_PENDING             -12

/** \brief Socket operation failed */
#define MQTT_ERR_SOCKET_FAILED              -13

/** \brief Invalid MQTT client state */
#define MQTT_ERR_CLIENT_INVALID_STATE       -14

#endif /* MQTT_ERROR_H */
