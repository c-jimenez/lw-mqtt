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

#ifndef MQTT_MUTEX_H
#define MQTT_MUTEX_H

#include "stdheaders.h"
#include "mqtt_mutex_t.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Initialize the MQTT mutex module */
bool mqtt_mutex_init(void);

/** \brief De-initialize the MQTT mutex module */
bool mqtt_mutex_deinit(void);

/** \brief Create a MQTT mutex */
bool mqtt_mutex_create(mqtt_mutex_t* const mqtt_mutex);

/** \brief Close a MQTT mutex */
bool mqtt_mutex_delete(mqtt_mutex_t* const mqtt_mutex);

/** \brief Lock a MQTT mutex */
bool mqtt_mutex_lock(mqtt_mutex_t* const mqtt_mutex);

/** \brief Release a MQTT mutex */
bool mqtt_mutex_unlock(mqtt_mutex_t* const mqtt_mutex);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MQTT_MUTEX_H */
