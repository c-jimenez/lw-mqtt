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

#ifndef MQTT_TIMER_H
#define MQTT_TIMER_H

#include "stdheaders.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief MQTT timer data */
typedef struct _mqtt_timer_t
{
    /** \brief Start time in ms */
    uint32_t start_time;
    /** \brief Expiration time in ms */
    uint32_t expiration_time;
    /** \brief Auto restart */
    bool auto_restart;
} mqtt_timer_t;



/** \brief Start a MQTT timer */
bool mqtt_timer_start(mqtt_timer_t* const mqtt_timer, const uint32_t ms_timeout, const bool auto_restart);

/** \brief Reset a MQTT timer */
bool mqtt_timer_reset(mqtt_timer_t* const mqtt_timer);

/** \brief Check if a MQTT timer has reached its expiration time */
bool mqtt_timer_has_expired(mqtt_timer_t* const mqtt_timer, bool* const has_expired);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MQTT_TIMER_H */
