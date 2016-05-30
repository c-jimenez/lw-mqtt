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

#include "mqtt_broker.h"
#include "mqtt_error.h"
#include "mqtt_time.h"
#include "mqtt_packet_serialize.h"
#include "mqtt_packet_deserialize.h"



/** \brief Initialize a MQTT broker */
bool mqtt_broker_init(mqtt_broker_t* const mqtt_broker)
{
    bool ret = false;

    /* Check params */
    if (mqtt_broker != NULL)
    {
        /* Re-init data structure */
        memset(mqtt_broker, 0, sizeof(mqtt_broker_t));

        /* Create listen socket */
        ret = mqtt_socket_open(&mqtt_broker->listen_socket, false);


        /* Create the mutex */
        #ifdef MQTT_MULTITASKING_ENABLED
        if (ret)
        {
            ret = mqtt_mutex_create(&mqtt_broker->mutex);
        }
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* MQTT broker ready */
        if (ret)
        {
            mqtt_broker->state = MQTT_BROKER_STATE_STOPPED;
        }
    }
    else
    {
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Start the MQTT broker */
bool mqtt_broker_start(mqtt_broker_t* const mqtt_broker, const char* const ip_address, const uint16_t port)
{
    bool ret = false;

    /* Check params */
    if (mqtt_broker != NULL)
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_broker->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Check state */
        if (mqtt_broker->state == MQTT_BROKER_STATE_STOPPED)
        {
            /* Bind listen socket */
            ret = mqtt_socket_bind(&mqtt_broker->listen_socket, ip_address, port);

            /* Put the socket in listen state */
            if (ret)
            {
                ret = mqtt_socket_listen(&mqtt_broker->listen_socket);
            }

            /* MQTT broker running */
            if (ret)
            {
                mqtt_broker->state = MQTT_BROKER_STATE_RUNNING;
            }
        }
        else
        {
            mqtt_errno_set(MQTT_ERR_BROKER_INVALID_STATE);
        }

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_broker->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }
    else
    {
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Stop the MQTT broker */
bool mqtt_broker_stop(mqtt_broker_t* const mqtt_broker)
{
    bool ret = false;

    /* Check params */
    if (mqtt_broker != NULL)
    {
        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_broker->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Check state */
        if (mqtt_broker->state == MQTT_BROKER_STATE_RUNNING)
        {
            /* Close the listen socket */
            ret = mqtt_socket_close(&mqtt_broker->listen_socket);

            /* Close the connections with the clients */
            if (ret)
            {
                /* TODO */
            }

            /* MQTT broker stopped */
            if (ret)
            {
                mqtt_broker->state = MQTT_BROKER_STATE_STOPPED;
            }
        }
        else
        {
            mqtt_errno_set(MQTT_ERR_BROKER_INVALID_STATE);
        }

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_broker->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }
    else
    {
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}


/** \brief Broker periodic task */
bool mqtt_broker_task(mqtt_broker_t* const mqtt_broker)
{
    bool ret = false;

    /* Check params */
    if (mqtt_broker != NULL)
    {

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_lock(&mqtt_broker->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* Check current state */
        switch (mqtt_broker->state)
        {
            case MQTT_BROKER_STATE_STOPPED:
            {
                /* Nothing to do */
                break;
            }

            case MQTT_BROKER_STATE_RUNNING:
            {
                bool callret;
                mqtt_socket_t temp_socket;
                mqtt_socket_t* client_mqtt_socket;

                /* Check for new connected clients */
                if (mqtt_broker->first_connected_session != NULL)
                {
                    client_mqtt_socket = mqtt_broker->first_connected_session->socket;
                }
                else
                {
                    client_mqtt_socket = temp_socket;
                }
                callret = mqtt_socket_accept(&mqtt_broker->listen_socket, client_mqtt_socket);
                if (callret)
                {
                    /* New client connected, check if the connection shall be accepted */
                    if (mqtt_broker->first_connected_session == NULL)
                    {
                        /* No more clients allowed, close connection */
                        (void)mqtt_socket_close(client_mqtt_socket);
                    }
                    else
                    {
                        /* Initialize session */
                    }
                }


                break;
            }

            default:
            {
                /* Invalid state */
                mqtt_errno_set(MQTT_ERR_BROKER_INVALID_STATE);
                ret = false;
                break;
            }
        }

        #ifdef MQTT_MULTITASKING_ENABLED
        (void)mqtt_mutex_unlock(&mqtt_broker->mutex);
        #endif /* MQTT_MULTITASKING_ENABLED */
    }

    return ret;
}
