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
        if (!ret)
        {
            mqtt_broker->last_error = mqtt_broker->listen_socket.last_error;
        }


        /* Create the mutex */
         #ifdef MQTT_MULTITASKING_ENABLED
        if (ret)
        {
            ret = mqtt_mutex_create(&mqtt_broker->mutex);
            if (!ret)
            {
                mqtt_broker->last_error = mqtt_broker->mutex.last_error;
            }
        }
        #endif /* MQTT_MULTITASKING_ENABLED */

        /* MQTT broker ready */
        if (ret)
        {
            mqtt_broker->state = MQTT_BROKER_STATE_STOPPED;
        }
    }

    return ret;
}

