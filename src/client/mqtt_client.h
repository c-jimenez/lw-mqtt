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

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

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


/** \brief Pre-declaration of mqtt_client_t structure which represents a MQTT client */
typedef struct _mqtt_client_t mqtt_client_t;


/** \brief MQTT client connect callback */
typedef void (*fp_mqtt_client_connect_callback_t)(mqtt_client_t* const mqtt_client, const bool connected, const mqtt_connack_retcode_t retcode);

/** \brief MQTT client subscribe callback */
typedef void(*fp_mqtt_client_subscribe_callback_t)(mqtt_client_t* const mqtt_client, const uint8_t granted_qos, const bool subscribe_succeed);

/** \brief MQTT client unsubscribe callback */
typedef void(*fp_mqtt_client_unsubscribe_callback_t)(mqtt_client_t* const mqtt_client, const bool unsubscribe_succeed);

/** \brief MQTT client publish callback */
typedef void(*fp_mqtt_client_publish_callback_t)(mqtt_client_t* const mqtt_client, const bool publish_succeed);

/** \brief MQTT client publish received callback */
typedef void(*fp_mqtt_client_publish_received_callback_t)(mqtt_client_t* const mqtt_client, const mqtt_string_t* topic, const void* data, 
                                                          const uint32_t length, const uint8_t qos, const bool retain, const bool duplicate);

/** \brief MQTT client disconnect callback */
typedef void(*fp_mqtt_client_disconnect_callback_t)(mqtt_client_t* const mqtt_client, const bool expected);



/** \brief MQTT client callbacks */
typedef struct _mqtt_client_callbacks_t
{
    /** \brief Connect callback */
    fp_mqtt_client_connect_callback_t connect;
    /** \brief Subscribe callback */
    fp_mqtt_client_subscribe_callback_t subscribe;
    /** \brief Unsubscribe callback */
    fp_mqtt_client_unsubscribe_callback_t unsubscribe;
    /** \brief Publish callback */
    fp_mqtt_client_publish_callback_t publish;
    /** \brief Publish received callback */
    fp_mqtt_client_publish_received_callback_t publish_received;
    /** \brief Disconnect callback */
    fp_mqtt_client_disconnect_callback_t disconnect;
} mqtt_client_callbacks_t;


/** \brief MQTT client state */
typedef enum _mqtt_client_state_t
{
    MQTT_CLIENT_STATE_NOT_INITIALIZED = 0u,
    MQTT_CLIENT_STATE_DISCONNECTED = 1u,
    MQTT_CLIENT_STATE_TCP_CONNECTING = 2u,
    MQTT_CLIENT_STATE_TCP_CONNECTED = 3u,
    MQTT_CLIENT_STATE_MQTT_CONNECTING = 3u,
    MQTT_CLIENT_STATE_MQTT_CONNECTED = 4u,
    MQTT_CLIENT_STATE_MQTT_DISCONNECTING = 5u
} mqtt_client_state_t;

/** \brief MQTT client */
typedef struct _mqtt_client_t
{
    /** \brief Client id */
    mqtt_const_string_t client_id;

    /** \brief Credentials */
    mqtt_const_credentials_t credentials;

    /** \brief Will */
    mqtt_const_will_t will;

    /** \brief Callbacks */
    mqtt_client_callbacks_t callbacks;

    /** \brief Keep alive */
    uint16_t keepalive;

    /** \brief State */
    mqtt_client_state_t state;

    /** \brief Packet id */
    uint16_t packet_id;

    /** \brief Socket */
    mqtt_socket_t socket;

    /** \brief Output stream */
    output_stream_t outstream;

    /** \brief Input stream */
    input_stream_t instream;

    /** \brief Last error */
    int32_t last_error;

    /** \brief Temp var for the reception of a topic */
    mqtt_string_t topic;

    /** \brief Buffer for the topic string */
    char topic_buffer[MQTT_CLIENT_MAX_TOPIC_LENGTH];

    /** \brief Buffer for the payload reception */
    uint8_t payload_buffer[MQTT_CLIENT_MAX_PAYLOAD_SIZE];

    /** \brief User data */
    void* user_data;

    /** \brief Polling period in ms for the task */
    uint32_t poll_period;

    /** \brief Keepalive timer */
    mqtt_timer_t keepalive_timer;

    /** \brief Broker response timer */
    mqtt_timer_t broker_response_timer;

    /** \brief Broker response timeout in ms */
    uint32_t broker_response_timeout;

    /** \brief Indicate if the client is waiting for a response from the broker */
    bool is_waiting_response;

    #ifdef MQTT_MULTITASKING_ENABLED

    /** \brief Mutex for the MQTT client */
    mqtt_mutex_t mutex;

    #endif /* MQTT_MULTITASKING_ENABLED */


} mqtt_client_t;



/** \brief Initialize a MQTT client */
bool mqtt_client_init(mqtt_client_t* const mqtt_client);

/** \brief Set the client id */
bool mqtt_client_set_client_id(mqtt_client_t* const mqtt_client, const char* const client_id);

/** \brief Set the credentials */
bool mqtt_client_set_credentials(mqtt_client_t* const mqtt_client, const char* const username, const uint8_t* const password, 
                                 const uint16_t password_length);

/** \brief Set the will message */
bool mqtt_client_set_will(mqtt_client_t* const mqtt_client, const char* const topic, const uint8_t* const message, 
                          const uint16_t message_length, const uint8_t qos, const bool retain);

/** \brief Set the callbacks */
bool mqtt_client_set_callbacks(mqtt_client_t* const mqtt_client, const mqtt_client_callbacks_t* const callbacks);

/** \brief Set the keepalive in sec */
bool mqtt_client_set_keepalive(mqtt_client_t* const mqtt_client, const uint16_t sec_keepalive);

/** \brief Set the broker response timeout in ms */
bool mqtt_client_set_broker_response_timeout(mqtt_client_t* const mqtt_client, const uint16_t ms_broker_response_timeout);

/** \brief Set the user data field */
bool mqtt_client_set_user_data(mqtt_client_t* const mqtt_client, void* user_data);

/** \brief Get the user data field */
bool mqtt_client_get_user_data(mqtt_client_t* const mqtt_client, void** user_data);

/** \brief Set the polling period */
bool mqtt_client_set_poll_period(mqtt_client_t* const mqtt_client, const uint32_t ms_poll_period);

/** \brief Connect to a broker */
bool mqtt_client_connect(mqtt_client_t* const mqtt_client, const char* const broker_ip, const uint16_t broker_port);

/** \brief Disconnect from the broker */
bool mqtt_client_disconnect(mqtt_client_t* const mqtt_client);

/** \brief Subscribe to a topic on the broker */
bool mqtt_client_subscribe(mqtt_client_t* const mqtt_client, const char* const topic, const uint8_t qos);

/** \brief Unsubscribe from a topic on the broker */
bool mqtt_client_unsubscribe(mqtt_client_t* const mqtt_client, const char* const topic);

/** \brief Publish a message on the broker */
bool mqtt_client_publish(mqtt_client_t* const mqtt_client, const char* const topic, const void* const message,
                         const uint32_t length, const uint8_t qos, const bool retain);

/** \brief Client periodic task */
bool mqtt_client_task(mqtt_client_t* const mqtt_client);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MQTT_CLIENT_H */
