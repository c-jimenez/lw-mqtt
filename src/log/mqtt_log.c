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


#include "mqtt_log.h"
#include "mqtt_log_output.h"
#include "mqtt_mutex.h"

/** \brief Current verbosity filter */
static uint8_t s_current_verbosity_filter;

#ifdef MQTT_MULTITASKING_ENABLED
static mqtt_mutex_t s_log_mutex;
#endif /* MQTT_MULTITASKING_ENABLED */


/** \brief Initialize the MQTT log module */
bool mqtt_log_init(void)
{
    bool ret = true;

    /* Default verbosity filter = INFO + ERROR */
    s_current_verbosity_filter = (uint8_t)(MQTT_LOG_LVL_INFO) | (uint8_t)(MQTT_LOG_LVL_ERROR);


    #ifdef MQTT_MULTITASKING_ENABLED
    /* Initialize log mutex */
    ret = mqtt_mutex_create(&s_log_mutex);
    #endif /* MQTT_MULTITASKING_ENABLED */

    return ret;
}

/** \brief De-initialize the MQTT log module */
bool mqtt_log_deinit(void)
{
    bool ret = true;

    #ifdef MQTT_MULTITASKING_ENABLED
    /* Release log mutex */
    ret = mqtt_mutex_delete(&s_log_mutex);
    #endif /* MQTT_MULTITASKING_ENABLED */

    return ret;
}

/** \brief Add a log message */
void mqtt_log_add(const mqtt_log_verbosity verbosity, const char* const message, ...)
{
    va_list args;

    va_start(args, message);
    mqtt_log_vadd(verbosity, message, args);
    va_end(args);
}

/** \brief Add a log message */
void mqtt_log_vadd(const mqtt_log_verbosity verbosity, const char* const message, va_list args)
{
    /* Check params */
    if (message != NULL)
    {
        /* Filter message */
        if (((uint8_t)(verbosity)& s_current_verbosity_filter) != 0u)
        {
            #ifdef MQTT_MULTITASKING_ENABLED
            (void)mqtt_mutex_lock(&s_log_mutex);
            #endif /* MQTT_MULTITASKING_ENABLED */

            /* Output message */
            mqtt_log_output_message(verbosity, message, args);

            #ifdef MQTT_MULTITASKING_ENABLED
            (void)mqtt_mutex_unlock(&s_log_mutex);
            #endif /* MQTT_MULTITASKING_ENABLED */
        }
    }
}

/** \brief Set the current log filter */
void mqtt_log_set_filter(const uint8_t filter)
{
    /* No need for a mutex here because uint8_t access is atomic on all platforms */
    s_current_verbosity_filter = filter;
}

/** \brief Convert a log verbosity level into a string */
bool mqtt_log_level_to_string(const mqtt_log_verbosity verbosity, char* const dest_string, const size_t dest_string_size)
{
    bool ret = false;

    /* Check params */
    if (dest_string != NULL)
    {
        char* verbosity_string = NULL;
        switch (verbosity)
        {
            case MQTT_LOG_LVL_DEBUG:
                verbosity_string = "DEBUG";
                break;
            case MQTT_LOG_LVL_INFO:
                verbosity_string = "INFO";
                break;
            case MQTT_LOG_LVL_ERROR:
                verbosity_string = "ERROR";
                break;
            default:
                /* No string */
                break;
        }
        if (verbosity_string != NULL)
        {
            strncpy(dest_string, verbosity_string, dest_string_size);
        }
    }

    return ret;
}
