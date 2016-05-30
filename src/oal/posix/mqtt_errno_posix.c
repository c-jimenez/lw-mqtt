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

#include "mqtt_errno.h"

/** \brief Initialize the MQTT errno module */
bool mqtt_errno_init(void)
{
    /* Nothing to do */
    return true;
}

/** \brief De-initialize the MQTT errno module */
bool mqtt_errno_deinit(void)
{
    /* Nothing to do */
    return true;
}

/** \brief Get the current errno */
int32_t mqtt_errno_get(void)
{
    const int32_t ret = (int32_t)GetLastError();
    return ret;
}

/** \brief Set the current errno */
void mqtt_errno_set(const int32_t errno_val)
{
    SetLastError((DWORD)errno_val);
}

