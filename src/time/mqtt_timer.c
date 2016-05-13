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

#include "mqtt_time.h"
#include "mqtt_timer.h"



/** \brief Start a MQTT timer */
bool mqtt_timer_start(mqtt_timer_t* const mqtt_timer, const uint32_t ms_timeout, const bool auto_restart)
{
    bool ret = false;

    /* Check params */
    if (mqtt_timer != NULL)
    {
        ret = mqtt_time_get_current(&mqtt_timer->start_time);
        mqtt_timer->expiration_time = mqtt_timer->start_time + ms_timeout;
        mqtt_timer->auto_restart = auto_restart;
    }

    return ret;
}

/** \brief Reset a MQTT timer */
bool mqtt_timer_reset(mqtt_timer_t* const mqtt_timer)
{
    bool ret = false;

    /* Check params */
    if (mqtt_timer != NULL)
    {
        uint32_t current_time;
        ret = mqtt_time_get_current(&current_time);
        mqtt_timer->expiration_time = current_time + (mqtt_timer->expiration_time - mqtt_timer->start_time);
        mqtt_timer->start_time = current_time;
    }

    return ret;
}

/** \brief Check if a MQTT timer has reached its expiration time */
bool mqtt_timer_has_expired(mqtt_timer_t* const mqtt_timer, bool* const has_expired)
{
    bool ret = false;

    /* Check params */
    if ((mqtt_timer != NULL) &&
        (has_expired != NULL))
    {
        uint32_t current_time;
        ret = mqtt_time_get_current(&current_time);
        if (current_time >= mqtt_timer->expiration_time)
        {
            (*has_expired) = true;
            if (mqtt_timer->auto_restart)
            {
                mqtt_timer->expiration_time = current_time + (mqtt_timer->expiration_time - mqtt_timer->start_time);
                mqtt_timer->start_time = current_time;
            }
        }
        else
        {
            (*has_expired) = false;
        }        
    }

    return ret;
}

