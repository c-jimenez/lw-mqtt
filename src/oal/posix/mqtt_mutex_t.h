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

#ifndef MQTT_MUTEX_T_H
#define MQTT_MUTEX_T_H

#include "stdheaders.h"
#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief MQTT mutex */
typedef struct _mqtt_mutex_t
{
    /** Implementation specific data */
    CRITICAL_SECTION data;

    /** \brief Last error */
    int32_t last_error;
} mqtt_mutex_t;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MQTT_MUTEX_T_H */
