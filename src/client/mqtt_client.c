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

#include "mqtt_client.h"
#include "mqtt_error.h"
#include "mqtt_time.h"
#include "mqtt_packet_serialize.h"
#include "mqtt_packet_deserialize.h"

/** \brief Initialize a MQTT client */
bool mqtt_client_init(mqtt_client_t* const mqtt_client)
{
    bool ret = false;

    /* Check params */
    if (mqtt_client != NULL)
    {
        /* Re-init data structure */
        memset(mqtt_client, 0, sizeof(mqtt_client_t));

        /* Create socket */
        ret = mqtt_socket_open(&mqtt_client->socket, false);

        /* Initialize input and output streams */
        if (ret)
        {
            ret = socket_stream_output_from_socket(&mqtt_client->outstream, &mqtt_client->socket);
        }
        if (ret)
        {
            ret = socket_stream_input_from_socket(&mqtt_client->instream, &mqtt_client->socket);
        }

        /* Create the mutex */
        #ifdef MQTT_MULTITASKING_ENABLED
        if (ret)
        {
            ret = mqtt_mutex_create(&mqtt_client->mutex);
        }
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Initialize temp vars for reception */
        mqtt_client->topic.str = mqtt_client->topic_buffer;
        mqtt_client->topic.size = sizeof(mqtt_client->topic_buffer);

        /* MQTT client ready */
        if (ret)
        {
            mqtt_client->state = MQTT_CLIENT_STATE_DISCONNECTED;
        }
    }

    return ret;
}



/** \brief Set the client id */
bool mqtt_client_set_client_id(mqtt_client_t* const mqtt_client, const char* const client_id)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_client != NULL) &&
        (client_id != NULL))
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Save client id */
        mqtt_client->client_id.str = client_id;
        mqtt_client->client_id.size= (uint16_t)strnlen(client_id, MQTT_MAXIMUM_STRING_SIZE);

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Set the credentials */
bool mqtt_client_set_credentials(mqtt_client_t* const mqtt_client, const char* const username, const uint8_t* const password,
                                 const uint16_t password_length)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_client != NULL) &&
        (username != NULL) &&
        !((password == NULL) && (password_length!=0u)))
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Save credentials */
        mqtt_client->credentials.username.str = username;
        mqtt_client->credentials.username.size = (uint16_t)strnlen(username, MQTT_MAXIMUM_STRING_SIZE);
        mqtt_client->credentials.password.str = (const char*)password;
        mqtt_client->credentials.password.size = password_length;

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Set the will message */
bool mqtt_client_set_will(mqtt_client_t* const mqtt_client, const char* const topic, const uint8_t* const message,
                          const uint16_t message_length, const uint8_t qos, const bool retain)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_client != NULL) &&
        (topic != NULL) &&
        !((message == NULL) && (message_length != 0u)) &&
        (qos <= MQTT_CFG_MAX_QOS_LEVEL))
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Save will */
        mqtt_client->will.topic.str = topic;
        mqtt_client->will.topic.size = (uint16_t)strnlen(topic, MQTT_MAXIMUM_STRING_SIZE);
        mqtt_client->will.message.str = (const char*)message;
        mqtt_client->will.message.size = message_length;
        mqtt_client->will.qos = qos;
        mqtt_client->will.retain = retain;

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Set the callbacks */
bool mqtt_client_set_callbacks(mqtt_client_t* const mqtt_client, const mqtt_client_callbacks_t* const callbacks)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_client != NULL) &&
        (callbacks != NULL))
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Save callbacks */
        memcpy(&mqtt_client->callbacks, callbacks, sizeof(mqtt_client_callbacks_t));

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Set the keepalive in sec */
bool mqtt_client_set_keepalive(mqtt_client_t* const mqtt_client, const uint16_t sec_keepalive)
{
    bool ret = false;

    /* Check params */
    if (mqtt_client != NULL)
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Save keepalive */
        mqtt_client->keepalive = sec_keepalive;

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }

    return ret;
}

/** \brief Set the broker response timeout in ms */
bool mqtt_client_set_broker_response_timeout(mqtt_client_t* const mqtt_client, const uint16_t ms_broker_response_timeout)
{
    bool ret = false;

    /* Check params */
    if (mqtt_client != NULL)
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
        
        /* Save broker response timeout */
        mqtt_client->broker_response_timeout = ms_broker_response_timeout;

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }

    return ret;
}

/** \brief Set the user data field */
bool mqtt_client_set_user_data(mqtt_client_t* const mqtt_client, void* user_data)
{
    bool ret = false;

    /* Check params */
    if (mqtt_client != NULL)
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Save user data */
        mqtt_client->user_data = user_data;

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }

    return ret;
}

/** \brief Get the user data field */
bool mqtt_client_get_user_data(mqtt_client_t* const mqtt_client, void** user_data)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_client != NULL) &&
        (user_data != NULL))
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Copy user data */
        (*user_data) = mqtt_client->user_data;

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }

    return ret;
}

/** \brief Set the polling period */
bool mqtt_client_set_poll_period(mqtt_client_t* const mqtt_client, const uint32_t ms_poll_period)
{
    bool ret = false;

    /* Check params */
    if (mqtt_client != NULL)
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Save poll period */
        mqtt_client->poll_period = ms_poll_period;

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }

    return ret;
}

/** \brief Connect to a broker */
bool mqtt_client_connect(mqtt_client_t* const mqtt_client, const char* const broker_ip, const uint16_t broker_port)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_client != NULL) &&
        (broker_ip != NULL))
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Check disconnected state */
        if (mqtt_client->state == MQTT_CLIENT_STATE_DISCONNECTED)
        {
            /* Connect to broker */
            ret = mqtt_socket_connect(&mqtt_client->socket, broker_ip, broker_port);
            if (ret)
            {
                mqtt_client->state = MQTT_CLIENT_STATE_TCP_CONNECTING;
            }
            else
            {
                const int32_t err = mqtt_errno_get();
                if (err == MQTT_ERR_SOCKET_PENDING)
                {
                    mqtt_client->state = MQTT_CLIENT_STATE_TCP_CONNECTING;
                    ret = true;
                }
            }
        }
        else
        {
            mqtt_errno_set(MQTT_ERR_CLIENT_INVALID_STATE);
        }

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Disconnect from the broker */
bool mqtt_client_disconnect(mqtt_client_t* const mqtt_client)
{
    bool ret = false;

    /* Check params */
    if (mqtt_client != NULL)
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Check connected state */
        if (mqtt_client->state == MQTT_CLIENT_STATE_MQTT_CONNECTED)
        {
            /* Disconnect from broker */
            ret = mqtt_packet_serialize_disconnect(&mqtt_client->outstream);
            mqtt_client->state = MQTT_CLIENT_STATE_MQTT_DISCONNECTING;

            /* Close TCP connection */
            ret = mqtt_socket_close(&mqtt_client->socket);
        }
        else
        {
            mqtt_errno_set(MQTT_ERR_CLIENT_INVALID_STATE);
        }

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Subscribe to a topic on the broker */
bool mqtt_client_subscribe(mqtt_client_t* const mqtt_client, const char* const topic, const uint8_t qos)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_client != NULL) &&
        (topic != NULL) &&
        (qos <= MQTT_CFG_MAX_QOS_LEVEL))
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Check connected state */
        if (mqtt_client->state == MQTT_CLIENT_STATE_MQTT_CONNECTED)
        {
            /* Send SUBSCRIBE packet */
            mqtt_const_string_t const_topic;
            const_topic.str = topic;
            const_topic.size = (uint16_t)strnlen(topic, MQTT_MAXIMUM_STRING_SIZE);
            ret = mqtt_packet_serialize_subscribe(&mqtt_client->outstream, &const_topic, qos, mqtt_client->packet_id);
            if (!ret)
            {
                const int32_t err = mqtt_errno_get();
                if (err == MQTT_ERR_SOCKET_FAILED)
                {
                    /* Connection lost : close socket and notify application */
                    (void)mqtt_socket_close(&mqtt_client->socket);
                    if (mqtt_client->callbacks.disconnect != NULL)
                    {
                        mqtt_client->callbacks.disconnect(mqtt_client, false);
                    }
                    mqtt_client->state = MQTT_CLIENT_STATE_DISCONNECTED;
                }
            }
            else
            {
                /* Reset keepalive timer */
                (void)mqtt_timer_reset(&mqtt_client->keepalive_timer);

                /* Reset broker response timer */
                (void)mqtt_timer_reset(&mqtt_client->broker_response_timer);
                mqtt_client->is_waiting_response = true;
            }

            /* Next packet id */
            mqtt_client->packet_id++;
        }
        else
        {
            mqtt_errno_set(MQTT_ERR_CLIENT_INVALID_STATE);
        }

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Unsubscribe from a topic on the broker */
bool mqtt_client_unsubscribe(mqtt_client_t* const mqtt_client, const char* const topic)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_client != NULL) &&
        (topic != NULL))
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Check connected state */
        if (mqtt_client->state == MQTT_CLIENT_STATE_MQTT_CONNECTED)
        {
            /* Send SUBSCRIBE packet */
            mqtt_const_string_t const_topic;
            const_topic.str = topic;
            const_topic.size = (uint16_t)strnlen(topic, MQTT_MAXIMUM_STRING_SIZE);
            ret = mqtt_packet_serialize_unsubscribe(&mqtt_client->outstream, &const_topic, mqtt_client->packet_id);
            if (!ret)
            {
                const int32_t err = mqtt_errno_get();
                if (err == MQTT_ERR_SOCKET_FAILED)
                {
                    /* Connection lost : close socket and notify application */
                    (void)mqtt_socket_close(&mqtt_client->socket);
                    if (mqtt_client->callbacks.disconnect != NULL)
                    {
                        mqtt_client->callbacks.disconnect(mqtt_client, false);
                    }
                    mqtt_client->state = MQTT_CLIENT_STATE_DISCONNECTED;
                }
            }
            else
            {
                /* Reset keepalive timer */
                (void)mqtt_timer_reset(&mqtt_client->keepalive_timer);

                /* Reset broker response timer */
                (void)mqtt_timer_reset(&mqtt_client->broker_response_timer);
                mqtt_client->is_waiting_response = true;
            }

            /* Next packet id */
            mqtt_client->packet_id++;
        }
        else
        {
            mqtt_errno_set(MQTT_ERR_CLIENT_INVALID_STATE);
        }

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Publish a message on the broker */
bool mqtt_client_publish(mqtt_client_t* const mqtt_client, const char* const topic, const void* const message,
                         const uint32_t length, const uint8_t qos, const bool retain)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_client != NULL) && 
        (topic != NULL) &&
        (!((message == NULL) && (length != 0u))) &&
        (qos <= MQTT_CFG_MAX_QOS_LEVEL))
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Check connected state */
        if (mqtt_client->state == MQTT_CLIENT_STATE_MQTT_CONNECTED)
        {
            /* Send PUBLISH packet */
            mqtt_const_string_t const_topic;
            const_topic.str = topic;
            const_topic.size = (uint16_t)strnlen(topic, MQTT_MAXIMUM_STRING_SIZE);
            ret = mqtt_packet_serialize_publish(&mqtt_client->outstream, &const_topic, message, length, qos, retain, false, mqtt_client->packet_id);
            if (!ret)
            {
                const int32_t err = mqtt_errno_get();
                if (err == MQTT_ERR_SOCKET_FAILED)
                {
                    /* Connection lost : close socket and notify application */
                    (void)mqtt_socket_close(&mqtt_client->socket);
                    if (mqtt_client->callbacks.disconnect != NULL)
                    {
                        mqtt_client->callbacks.disconnect(mqtt_client, false);
                    }
                    mqtt_client->state = MQTT_CLIENT_STATE_DISCONNECTED;
                }
            }
            else
            {
                /* Reset keepalive timer */
                (void)mqtt_timer_reset(&mqtt_client->keepalive_timer);
            }

            /* Next packet id */
            mqtt_client->packet_id++;
        }
        else
        {
            mqtt_errno_set(MQTT_ERR_CLIENT_INVALID_STATE);
        }

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Client periodic task */
bool mqtt_client_task(mqtt_client_t* const mqtt_client)
{
    bool ret = false;

    /* Check params */
    if (mqtt_client != NULL)
    {
        bool disconnected = false;

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Check current state */
        switch (mqtt_client->state)
        {
            case MQTT_CLIENT_STATE_DISCONNECTED:
            {
                /* Nothing to do */
                break;
            }

            case MQTT_CLIENT_STATE_MQTT_DISCONNECTING:
            {
                /* Transit to disconnected state */
                mqtt_client->state = MQTT_CLIENT_STATE_DISCONNECTED;
                break;
            }

            case MQTT_CLIENT_STATE_TCP_CONNECTING:
            {
                /* Check tcp connection state */
                bool callret = mqtt_socket_is_connected(&mqtt_client->socket);
                if (callret)
                {
                    /* Send a CONNECT message to the broker */
                    mqtt_const_will_t* will = NULL;
                    mqtt_const_credentials_t* credentials = NULL;
                    if (mqtt_client->will.topic.str != NULL)
                    {
                        will = &mqtt_client->will;
                    }
                    if (mqtt_client->credentials.username.str != NULL)
                    {
                        credentials = &mqtt_client->credentials;
                    }
                    callret = mqtt_packet_serialize_connect(&mqtt_client->outstream, &mqtt_client->client_id, credentials,
                                                            will, true, mqtt_client->keepalive);
                    if (callret)
                    {
                        mqtt_client->state = MQTT_CLIENT_STATE_MQTT_CONNECTING;

                        /* Start keepalive timer */
                        (void)mqtt_timer_start(&mqtt_client->keepalive_timer, mqtt_client->keepalive * 1000U, true);

                        /* Start broker response timer */
                        (void)mqtt_timer_start(&mqtt_client->broker_response_timer, mqtt_client->broker_response_timeout, false);
                    }
                    else
                    {
                        /* Error, disconnect */
                        disconnected = true;
                    }
                }
                else
                {
                    const int32_t err = mqtt_errno_get();
                    if (err == MQTT_ERR_SOCKET_FAILED)
                    {
                        /* Connection lost */
                        disconnected = true;
                    }
                }
                break;
            }

            case MQTT_CLIENT_STATE_MQTT_CONNECTING:
            {
                /* Wait for the CONNACK reply */
                bool callret;
                uint8_t packet_flags;
                uint32_t packet_length;
                mqtt_control_packet_type_t packet_type;

                /* Check if data is available */
                #ifdef MQTT_MULTITASKING_ENABLED
                (void)mqtt_mutex_unlock(&mqtt_client->mutex);
                #endif /* MQTT_MULTITASKING_ENABLED */
                callret = mqtt_socket_select(&mqtt_client->socket, mqtt_client->poll_period);
                #ifdef MQTT_MULTITASKING_ENABLED
                (void)mqtt_mutex_lock(&mqtt_client->mutex);
                #endif /* MQTT_MULTITASKING_ENABLED */
                if (callret)
                {
                    callret = mqtt_packet_deserialize_packet_header(&mqtt_client->instream, &packet_type, &packet_flags, &packet_length);
                    if (callret)
                    {
                        /* Deserialize packet */
                        bool session_present;
                        mqtt_connack_retcode_t retcode = MQTT_CONNACK_RET_DISCONNECTED;

                        callret = mqtt_packet_deserialize_connack(&mqtt_client->instream, &session_present, &retcode);
                        if (callret && (retcode == MQTT_CONNACK_RET_ACCEPTED))
                        {
                            /* Connected */
                            mqtt_client->state = MQTT_CLIENT_STATE_MQTT_CONNECTED;
                            if (mqtt_client->callbacks.connect != NULL)
                            {
                                mqtt_client->callbacks.connect(mqtt_client, true, retcode);
                            }
                            mqtt_client->is_waiting_response = false;
                        }
                        else
                        {
                            /* Connection failed */
                            if (mqtt_client->callbacks.connect != NULL)
                            {
                                mqtt_client->callbacks.connect(mqtt_client, false, retcode);
                            }
                            disconnected = true;
                        }
                    }
                    else
                    {
                        /* Connection lost */
                        disconnected = true;
                    }
                }
                else
                {
                    /* Check response timeout */
                    bool has_expired;
                    (void)mqtt_timer_has_expired(&mqtt_client->broker_response_timer, &has_expired);
                    if (has_expired)
                    {
                        /* Disconnect */
                        disconnected = true;
                    }
                    else
                    {
                        const int32_t err = mqtt_errno_get();
                        if (err != MQTT_ERR_SOCKET_PENDING)
                        {
                            /* Connection lost */
                            disconnected = true;
                        }
                    }
                }

                break;
            }

            case MQTT_CLIENT_STATE_MQTT_CONNECTED:
            {
                /* Wait for a packet */
                bool callret;
                uint8_t packet_flags;
                uint32_t packet_length;
                mqtt_control_packet_type_t packet_type;

                /* Check keepalive */
                if (mqtt_client->keepalive != 0u)
                {
                    bool has_expired;
                    (void)mqtt_timer_has_expired(&mqtt_client->keepalive_timer, &has_expired);
                    if (has_expired)
                    {
                        /* Send ping request */
                        (void)mqtt_packet_serialize_pingreq(&mqtt_client->outstream);
                    }
                }
                

                /* Check if data is available */
                #ifdef MQTT_MULTITASKING_ENABLED
                (void)mqtt_mutex_unlock(&mqtt_client->mutex);
                #endif /* MQTT_MULTITASKING_ENABLED */
                callret = mqtt_socket_select(&mqtt_client->socket, mqtt_client->poll_period);
                #ifdef MQTT_MULTITASKING_ENABLED
                (void)mqtt_mutex_lock(&mqtt_client->mutex);
                #endif /* MQTT_MULTITASKING_ENABLED */
                if (callret)
                {
                    callret = mqtt_packet_deserialize_packet_header(&mqtt_client->instream, &packet_type, &packet_flags, &packet_length);
                    if (callret)
                    {
                        switch (packet_type)
                        {
                            case MQTT_PKT_PUBLISH:
                            {
                                uint32_t length;
                                uint8_t qos;
                                bool retain;
                                bool duplicate;
                                uint16_t packet_id;
                                callret = mqtt_packet_deserialize_publish(&mqtt_client->instream, packet_flags, packet_length, &mqtt_client->topic, 
                                                                          mqtt_client->payload_buffer, &length, &qos, &retain, &duplicate, &packet_id);
                                if (callret)
                                {
                                    if (mqtt_client->callbacks.publish_received != NULL)
                                    {
                                        mqtt_client->callbacks.publish_received(mqtt_client, &mqtt_client->topic, mqtt_client->payload_buffer, length, 
                                                                                qos, retain, duplicate);
                                    }
                                }
                                break;
                            }

                            case MQTT_PKT_SUBACK:
                            {
                                uint8_t qos;
                                uint16_t packet_id;
                                callret = mqtt_packet_deserialize_suback(&mqtt_client->instream, &qos, &packet_id);
                                if (callret)
                                {
                                    if (mqtt_client->callbacks.subscribe != NULL)
                                    {
                                        mqtt_client->callbacks.subscribe(mqtt_client, qos, true);
                                    }
                                }
                                mqtt_client->is_waiting_response = false;
                                break;
                            }

                            case MQTT_PKT_UNSUBACK:
                            { 
                                uint16_t packet_id;
                                callret = mqtt_packet_deserialize_unsuback(&mqtt_client->instream, &packet_id);
                                mqtt_client->is_waiting_response = false;
                                break;
                            }

                            case MQTT_PKT_PINGRESP:
                            {
                                callret = mqtt_packet_deserialize_pingresp(&mqtt_client->instream, packet_length);
                                mqtt_client->is_waiting_response = false;
                                break;
                            }

                            default:
                            {
                                /* Invalid packet */
                                mqtt_client->state = MQTT_ERR_INVALID_PACKET_TYPE;
                                disconnected = true;
                                break;
                            }
                        }
                        if (!callret)
                        {
                            /* Disconnect */
                            disconnected = true;
                        }
                    }
                    else
                    {
                        /* Connection lost */
                        disconnected = true;
                    }
                }
                else
                {
                    /* Check response timeout */
                    const int32_t err = mqtt_errno_get();
                    if (mqtt_client->is_waiting_response)
                    {
                        bool has_expired = false;
                        (void)mqtt_timer_has_expired(&mqtt_client->broker_response_timer, &has_expired);
                        if (has_expired)
                        {
                            /* Disconnect */
                            disconnected = true;
                        }
                    }

                    if (err != MQTT_ERR_SOCKET_PENDING)
                    {
                        /* Connection lost */
                        disconnected = true;
                    }
                }

                break;
            }

            default:
            {
                /* Invalid state */
                mqtt_client->state = MQTT_ERR_CLIENT_INVALID_STATE;
                ret = false;
                break;
            }
        }

        /* Check disconnection */
        if (disconnected)
        {
            /* Close socket and notify application */
            (void)mqtt_socket_close(&mqtt_client->socket);
            if ((mqtt_client->state == MQTT_CLIENT_STATE_MQTT_CONNECTED) ||
                (mqtt_client->state == MQTT_CLIENT_STATE_MQTT_DISCONNECTING))
            {
                if (mqtt_client->callbacks.disconnect != NULL)
                {
                    const bool expected_disconnection = (mqtt_client->state == MQTT_CLIENT_STATE_MQTT_DISCONNECTING);
                    mqtt_client->callbacks.disconnect(mqtt_client, expected_disconnection);
                }
            }
            else
            {
                if (mqtt_client->callbacks.connect != NULL)
                {
                    mqtt_client->callbacks.connect(mqtt_client, false, MQTT_CONNACK_RET_DISCONNECTED);
                }
            }

            /* Transit to disconnected state */
            mqtt_client->state = MQTT_CLIENT_STATE_DISCONNECTED;
        }

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_client->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }

    return ret;
}
