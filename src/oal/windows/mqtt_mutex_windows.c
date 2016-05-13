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

#include <windows.h>

#include "mqtt_mutex.h"
#include "mqtt_error.h"


/** \brief Initialize the MQTT mutex module */
bool mqtt_mutex_init(void)
{
    /* Nothing to do */
    return true;
}

/** \brief De-initialize the MQTT mutex module */
bool mqtt_mutex_deinit(void)
{
    /* Nothing to do */
    return true;
}

/** \brief Create a MQTT mutex */
bool mqtt_mutex_create(mqtt_mutex_t* const mqtt_mutex)
{
    bool ret = false;

    /* Check params */
    if (mqtt_mutex != NULL)
    {
        /* Initialise mutex */
        InitializeCriticalSection(&mqtt_mutex->data);

        ret = true;
    }

    return ret;
}

/** \brief Close a MQTT mutex */
bool mqtt_mutex_delete(mqtt_mutex_t* const mqtt_mutex)
{
    bool ret = false;

    /* Check params */
    if (mqtt_mutex != NULL)
    {
        /* Delete mutex */
        DeleteCriticalSection(&mqtt_mutex->data);

        ret = true;
    }

    return ret;
}

/** \brief Lock a MQTT mutex */
bool mqtt_mutex_lock(mqtt_mutex_t* const mqtt_mutex)
{
    bool ret = false;

    /* Check params */
    if (mqtt_mutex != NULL)
    {
        /* Lock mutex */
        EnterCriticalSection(&mqtt_mutex->data);

        ret = true;
    }

    return ret;
}

/** \brief Release a MQTT mutex */
bool mqtt_mutex_unlock(mqtt_mutex_t* const mqtt_mutex)
{
    bool ret = false;

    /* Check params */
    if (mqtt_mutex != NULL)
    {
        /* Unlock mutex */
        LeaveCriticalSection(&mqtt_mutex->data);

        ret = true;
    }

    return ret;
}
