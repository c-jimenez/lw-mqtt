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

#include <stdio.h>

#include "mqtt_log_output.h"


/** \brief Output a log message */
void mqtt_log_output_message(const mqtt_log_verbosity verbosity, const char* const message, va_list args)
{
    /* No null-pointer test for the parameters because this function
    is meant to be called from the inside of the library where the
    parameters are already checked.
    */

    char verbosity_string[32];   
    (void)mqtt_log_level_to_string(verbosity, verbosity_string, sizeof(verbosity_string));

    printf("[%s]:", verbosity_string);
    vprintf(message, args);
    printf("\n");
}
