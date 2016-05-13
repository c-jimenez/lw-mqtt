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

#ifndef MQTT_BROKER_H
#define MQTT_BROKER_H

#include "stdheaders.h"
#include "mqtt.h"
#include "mqtt_socket.h"
#include "mqtt_timer.h"
#include "mqtt_mutex.h"
#include "socket_stream.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Pre-declaration of mqtt_broker_t structure which represents a MQTT broker */
typedef struct _mqtt_broker_t mqtt_broker_t;


/** \brief MQTT broker state */
typedef enum _mqtt_broker_state_t
{
    MQTT_BROKER_STATE_NOT_INITIALIZED = 0u,
    MQTT_BROKER_STATE_STOPPED = 1u,
    MQTT_BROKER_STATE_RUNNING = 2u
} mqtt_broker_state_t;

/** \brief MQTT broker session */
typedef struct _mqtt_broker_session_t
{
    /** \brief Socket */
    mqtt_socket_t socket;

    /** \brief Output stream */
    output_stream_t outstream;

    /** \brief Input stream */
    input_stream_t instream;

    /** \brief Client ID */
    mqtt_string_t client_id;

    /** \brief Buffer for the client id string */
    char client_id_topic_buffer[MQTT_BROKER_MAX_CLIENT_ID_LENGTH];

    /** \brief QoS */
    uint8_t qos;

    /** \brief Will */
    mqtt_will_t will;

    /** \brief Buffer for the will topic name string */
    char will_topic_buffer[MQTT_BROKER_MAX_WILL_TOPIC_LENGTH];

    /** \brief Buffer for the will message */
    uint8_t will_message_buffer[MQTT_BROKER_MAX_WILL_MESSAGE_SIZE];

    /** \brief Keepalive timer */
    mqtt_timer_t keepalive_timer;

    /** \brief Next session in the list */
    struct _mqtt_broker_session_t* next;

} mqtt_broker_session_t;


/** \brief MQTT broker */
typedef struct _mqtt_broker_t
{
    /** \brief Credentials */
    mqtt_const_credentials_t credentials;

    /** \brief State */
    mqtt_broker_state_t state;

    /** \brief Socket to listen to incomming connections */
    mqtt_socket_t listen_socket;

    /** \brief Session data for the connected clients */
    mqtt_broker_session_t sessions[MQTT_BROKER_MAX_CLIENT];

    /** \brief First free session */
    mqtt_broker_session_t* first_free_session;

    /** \brief First connected session */
    mqtt_broker_session_t* first_connected_session;

    /** \brief Temp var for the reception of a topic */
    mqtt_string_t topic;

    /** \brief Buffer for the topic string */
    char topic_buffer[MQTT_BROKER_MAX_TOPIC_LENGTH];

    /** \brief Buffer for the payload reception */
    uint8_t payload_buffer[MQTT_BROKER_MAX_PAYLOAD_SIZE];

    /** \brief User data */
    void* user_data;

    /** \brief Polling period in ms for the task */
    uint32_t poll_period;

    #ifdef MQTT_MULTITASKING_ENABLED

    /** \brief Mutex for the MQTT client */
    mqtt_mutex_t mutex;

    #endif /* MQTT_MULTITASKING_ENABLED */


} mqtt_broker_t;





#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MQTT_BROKER_H */
