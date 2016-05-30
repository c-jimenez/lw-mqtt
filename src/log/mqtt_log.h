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

#ifndef MQTT_LOG_H
#define MQTT_LOG_H

#include "stdheaders.h"
#include "mqtt_config.h"

#ifndef MQTT_LOG_ENABLED

/** \brief Macro to log a debug string */
#define MQTT_LOG_DEBUG(msg, ...)

/** \brief Macro to log an info string */
#define MQTT_LOG_INFO(msg, ...)

/** \brief Macro to log an error string */
#define MQTT_LOG_ERROR(msg, ...)

#else /* MQTT_LOG_ENABLED */

/** \brief Macro to log a debug string */
#define MQTT_LOG_DEBUG(msg, ...) mqtt_log_add(MQTT_LOG_LVL_DEBUG, (msg), __VA_ARGS__)

/** \brief Macro to log an info string */
#define MQTT_LOG_INFO(msg, ...) mqtt_log_add(MQTT_LOG_LVL_INFO, (msg), __VA_ARGS__)

/** \brief Macro to log an error string */
#define MQTT_LOG_ERROR(msg, ...) mqtt_log_add(MQTT_LOG_LVL_ERROR, (msg), __VA_ARGS__)


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** \brief Log verbosity level */
typedef enum _mqtt_log_verbosity
{
    MQTT_LOG_LVL_SILENT = 0u,
    MQTT_LOG_LVL_DEBUG = 1u,
    MQTT_LOG_LVL_INFO = 2u,
    MQTT_LOG_LVL_ERROR = 4u
} mqtt_log_verbosity;


/** \brief Initialize the MQTT log module */
bool mqtt_log_init(void);

/** \brief De-initialize the MQTT log module */
bool mqtt_log_deinit(void);

/** \brief Add a log message */
void mqtt_log_add(const mqtt_log_verbosity verbosity, const char* const message, ...);

/** \brief Add a log message */
void mqtt_log_vadd(const mqtt_log_verbosity verbosity, const char* const message, va_list args);

/** \brief Set the current log filter */
void mqtt_log_set_filter(const uint8_t filter);

/** \brief Convert a log verbosity level into a string */
bool mqtt_log_level_to_string(const mqtt_log_verbosity verbosity, char* const dest_string, const size_t dest_string_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MQTT_LOG_ENABLED */

#endif /* MQTT_LOG_H */
