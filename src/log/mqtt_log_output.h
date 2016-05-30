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

#ifndef MQTT_LOG_OUTPUT_H
#define MQTT_LOG_OUTPUT_H

#include "mqtt_log.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** \brief Output a log message */
void mqtt_log_output_message(const mqtt_log_verbosity verbosity, const char* const message, va_list args);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MQTT_LOG_OUTPUT_H */
