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

#ifndef MQTT_TIME_H
#define MQTT_TIME_H

#include "stdheaders.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/** \brief Initialize the MQTT time module */
bool mqtt_time_init(void);

/** \brief De-initialize the MQTT time module */
bool mqtt_time_deinit(void);

/** \brief Get the current time in milliseconds */
bool mqtt_time_get_current(uint32_t* const current_time);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MQTT_TIME_H */
