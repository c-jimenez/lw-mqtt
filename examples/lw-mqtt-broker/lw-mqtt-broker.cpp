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


#include <random>
#include <ctime>
#include <sstream>
#include <iostream>
using namespace std;

#include "mqtt_broker.h"
#include "mqtt_errno.h"
#include "mqtt_log.h"
/** \brief Program version */
#define LW_MQTT_BROKER_VERSION "1.0"


/** Program parameters */
struct lw_mqtt_broker_params_t
{

    /** \brief Construtor to set the default values */
    lw_mqtt_broker_params_t()
        : broker_ip("127.0.0.1")
        , broker_port(1883u)
        , verbose(false)
    {}

    /** \brief Broker IP address */
    string broker_ip;

    /** \brief Broker port */
    uint16_t broker_port;

    /** \brief Verbose mode */
    bool verbose;
};



/** \brief Print the version message */
static void lw_mqtt_broker_print_version();

/** \brief Print the usage message */
static void lw_mqtt_broker_print_usage();

/** \brief Parse the command line parameters */
static bool lw_mqtt_broker_parse_parameters(lw_mqtt_broker_params_t& params, int argc, char* argv[]);




/** \brief Program entry point */
int main(int argc, char* argv[])
{
    bool ret;
    lw_mqtt_broker_params_t params;

    /* Parse command line parameters */
    ret = lw_mqtt_broker_parse_parameters(params, argc, argv);
    if (ret)
    {
        mqtt_broker_t broker;

        /* Initialize low level layers */
        mqtt_mutex_init();
        mqtt_socket_init();
        mqtt_log_init();

        /* Set log verbosity */
        if (params.verbose)
        {
            mqtt_log_set_filter(MQTT_LOG_LVL_INFO | MQTT_LOG_LVL_ERROR);
        }
        else
        {
            mqtt_log_set_filter(MQTT_LOG_LVL_ERROR);
        }

        /* Initialize broker */
        mqtt_broker_init(&broker);

        /* Start broker */
        ret = mqtt_broker_start(&broker, params.broker_ip.c_str(), params.broker_port);

        /* Main loop */
        if (ret)
        {
            while (true)
            {
                mqtt_broker_task(&broker);
            }
        }
    }    

	return ((ret)?0:1);
}


/** \brief Print the version message */
static void lw_mqtt_broker_print_version()
{
    cout << "lw-mqtt-broker version " << LW_MQTT_BROKER_VERSION << " compiled with liblw-mqtt version " << lw_mqtt_lib_version() << endl;
}

/** \brief Print the usage message */
static void lw_mqtt_broker_print_usage()
{
    cout << "usage: lw-mqtt-broker [--version] [--help] [-h <broker-ip>] [-p <broker-port>] [-v]" << endl;
}


/** \brief Parse the command line parameters */
static bool lw_mqtt_broker_parse_parameters(lw_mqtt_broker_params_t& params, int argc, char* argv[])
{
    bool ret = false;
    bool unique_option = false;
    bool invalid_arg = false;

    /* Skip first parameter */
    argc--;
    argv++;

    /* Check parameters */
    while ((argc != 0) && !invalid_arg && !unique_option)
    {
        argc--;

        if (strcmp(*argv, "--help") == 0)
        {
            unique_option = true;
            lw_mqtt_broker_print_usage();
        }
        else if (strcmp(*argv, "--version") == 0)
        {
            unique_option = true;
            lw_mqtt_broker_print_version();
        }
        else if (strcmp(*argv, "-h") == 0)
        {
            if (argc != 0)
            {
                argv++;
                argc--;
                params.broker_ip = *argv;
            }
            else
            {
                cout << "The -h option must be followed by the IP address of the broker.";
                invalid_arg = true;
            }
        }
        else if (strcmp(*argv, "-p") == 0)
        {
            if (argc != 0)
            {
                argv++;
                argc--;
                params.broker_port = (uint16_t)atoi(*argv);
            }
            else
            {
                cout << "The -p option must be followed by the TCP port of the broker.";
                invalid_arg = true;
            }
        }
        else if (strcmp(*argv, "-v") == 0)
        {
            params.verbose = true;
        }
        else
        {
            cout << "Invalid parameter : '" << *argv << "'.";
            invalid_arg = true;
        }

        /* Next param */
        argv++;
    }

    if (invalid_arg)
    {
        cout << endl << "See 'lw-mqtt-broker --help'." << endl;
    }
    else
    {
        if (!unique_option)
        {
            ret = true;
        }
    }

    return ret;
}

