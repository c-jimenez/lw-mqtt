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

#include "mqtt_client.h"
#include "mqtt_errno.h"
#include "mqtt_log.h"

/** \brief Program version */
#define LW_MQTT_PUB_VERSION "1.0"


/** Program parameters */
struct lw_mqtt_pub_params_t
{

    /** \brief Construtor to set the default values */
    lw_mqtt_pub_params_t()
        : broker_ip("127.0.0.1")
        , broker_port(1883u)
        , keepalive(0u)
        , username("")
        , password("")
        , qos(0u)
        , retain(false)
        , topic("")
        , message("")
        , verbose(false)
    {}

    /** \brief Broker IP address */
    string broker_ip;

    /** \brief Broker port */
    uint16_t broker_port;

    /** \brief Keepalive */
    uint16_t keepalive;

    /** \brief Username */
    string username;

    /** \brief Password */
    string password;

    /** \brief QoS */
    uint8_t qos;

    /** \brier Retain flag */
    bool retain;

    /** \brief Topic */
    string topic;

    /** \brief Message */
    string message;

    /** \brief Verbose mode */
    bool verbose;
};



/** \brief Print the version message */
static void lw_mqtt_pub_print_version();

/** \brief Print the usage message */
static void lw_mqtt_pub_print_usage();

/** \brief Parse the command line parameters */
static bool lw_mqtt_pub_parse_parameters(lw_mqtt_pub_params_t& params, int argc, char* argv[]);

/** \brief MQTT client connect callback */
static void mqtt_client_connect_callback(mqtt_client_t* const mqtt_client, const bool connected, const mqtt_connack_retcode_t retcode);

/** \brief MQTT client subscribe callback */
static void mqtt_client_subscribe_callback(mqtt_client_t* const mqtt_client, const uint8_t granted_qos, const bool subscribe_succeed);

/** \brief MQTT client unsubscribe callback */
static void mqtt_client_unsubscribe_callback(mqtt_client_t* const mqtt_client, const bool unsubscribe_succeed);

/** \brief MQTT client publish callback */
static void mqtt_client_publish_callback(mqtt_client_t* const mqtt_client, const bool publish_succeed);

/** \brief MQTT client publish received callback */
static void mqtt_client_publish_received_callback(mqtt_client_t* const mqtt_client, const mqtt_string_t* topic, const void* data,
                                                  const uint32_t length, const uint8_t qos, const bool retain, const bool duplicate);

/** \brief MQTT client disconnect callback */
static void mqtt_client_disconnect_callback(mqtt_client_t* const mqtt_client, const bool expected);






/** \brief Program entry point */
int main(int argc, char* argv[])
{
    bool ret;
    lw_mqtt_pub_params_t params;

    /* Parse command line parameters */
    ret = lw_mqtt_pub_parse_parameters(params, argc, argv);
    if (ret)
    {
        string client_id;
        stringstream sstream;
        mqtt_client_t client;
        mqtt_client_callbacks_t callbacks = {
                                                mqtt_client_connect_callback,
                                                mqtt_client_subscribe_callback,
                                                mqtt_client_unsubscribe_callback,
                                                mqtt_client_publish_callback,
                                                mqtt_client_publish_received_callback,
                                                mqtt_client_disconnect_callback
                                            };

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

        /* Create unique Client Id */
        std::srand(static_cast<unsigned int>(std::time(NULL)));
        sstream << "lw-mqtt-pub-" << std::rand();
        client_id = sstream.str();

        /* Initialize client */
        mqtt_client_init(&client);
        mqtt_client_set_client_id(&client, client_id.c_str());
        mqtt_client_set_callbacks(&client, &callbacks);
        mqtt_client_set_keepalive(&client, params.keepalive);
        mqtt_client_set_user_data(&client, &params);
        mqtt_client_set_poll_period(&client, 50u);
        mqtt_client_set_broker_response_timeout(&client, 250u);

        /* Connect to broker */
        ret = mqtt_client_connect(&client, params.broker_ip.c_str(), params.broker_port);

        /* Main loop */
        if (ret)
        {
            while (true)
            {
                mqtt_client_task(&client);
            }
        }
    }    

	return ((ret)?0:1);
}


/** \brief Print the version message */
static void lw_mqtt_pub_print_version()
{
    cout << "lw-mqtt-pub version " << LW_MQTT_PUB_VERSION << " compiled with liblw-mqtt version " << lw_mqtt_lib_version() << endl;
}

/** \brief Print the usage message */
static void lw_mqtt_pub_print_usage()
{
    cout << "usage: lw-mqtt-pub [--version] [--help] [-h <broker-ip>] [-p <broker-port>]" << endl;
    cout << "                   [-k <keepalive>] [--user <username>] [--passwd <password>]" << endl;
    cout << "                   [-q <QoS>] [-r] [-t <topic>] [-m <message>] [-v]" << endl;
}


/** \brief Parse the command line parameters */
static bool lw_mqtt_pub_parse_parameters(lw_mqtt_pub_params_t& params, int argc, char* argv[])
{
    bool ret = false;
    bool unique_option = false;
    bool invalid_arg = false;
    bool topic_found = false;

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
            lw_mqtt_pub_print_usage();
        }
        else if (strcmp(*argv, "--version") == 0)
        {
            unique_option = true;
            lw_mqtt_pub_print_version();
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
        else if (strcmp(*argv, "-k") == 0)
        {
            if (argc != 0)
            {
                argv++;
                argc--;
                params.keepalive = (uint16_t)atoi(*argv);
            }
            else
            {
                cout << "The -k option must be followed by the keepalive value.";
                invalid_arg = true;
            }
        }
        else if (strcmp(*argv, "--username") == 0)
        {
            if (argc != 0)
            {
                argv++;
                argc--;
                params.username = *argv;
            }
            else
            {
                cout << "The --username option must be followed by the username for broker authentification.";
                invalid_arg = true;
            }
        }
        else if (strcmp(*argv, "--password") == 0)
        {
            if (argc != 0)
            {
                argv++;
                argc--;
                params.password = *argv;
            }
            else
            {
                cout << "The --password option must be followed by the password for broker authentification.";
                invalid_arg = true;
            }
        }
        else if (strcmp(*argv, "-q") == 0)
        {
            if (argc != 0)
            {
                argv++;
                argc--;
                params.qos = (uint8_t)atoi(*argv);
            }
            else
            {
                cout << "The -q option must be followed by the QoS value.";
                invalid_arg = true;
            }
        }
        else if (strcmp(*argv, "-r") == 0)
        {
            params.retain = true;
        }
        else if (strcmp(*argv, "-t") == 0)
        {
            if (argc != 0)
            {
                argv++;
                argc--;
                params.topic = *argv;
                topic_found = true;
            }
            else
            {
                cout << "The -t option must be followed by the topic name.";
                invalid_arg = true;
            }
        }
        else if (strcmp(*argv, "-m") == 0)
        {
            if (argc != 0)
            {
                argv++;
                argc--;
                params.message = *argv;
            }
            else
            {
                cout << "The -t option must be followed by the message to publish.";
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

    if (!invalid_arg && !unique_option && !topic_found)
    {
        cout << "Topic name is missing.";
        invalid_arg = true;
    }

    if (invalid_arg)
    {
        cout << endl << "See 'lw-mqtt-pub --help'." << endl;
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


/** \brief MQTT client connect callback */
static void mqtt_client_connect_callback(mqtt_client_t* const mqtt_client, const bool connected, const mqtt_connack_retcode_t retcode)
{
    MQTT_UNUSED_PARAM(retcode);
    MQTT_UNUSED_PARAM(connected);

    /* Get parameters stored in client user data */
    lw_mqtt_pub_params_t* params;
    mqtt_client_get_user_data(mqtt_client, (void**)&params);

    /* Publish the message */
    int32_t exit_code = 1;
    const bool ret = mqtt_client_publish(mqtt_client, params->topic.c_str(), params->message.c_str(), (uint32_t)params->message.length(), params->qos, params->retain);
    if (ret)
    {
        exit_code = 0;
    }
    else
    {
        cout << "Error " << mqtt_errno_get() << ": Failed to send [" << params->message << "] to topic [" << params->topic << "] with QoS" << static_cast<uint32_t>(params->qos) << " and retain=" << (params->retain?"true":"false") << endl;
    }

    /* Send disconnect */
    mqtt_client_disconnect(mqtt_client);

    /* Exit program */
    exit(exit_code);
}

/** \brief MQTT client subscribe callback */
static void mqtt_client_subscribe_callback(mqtt_client_t* const mqtt_client, const uint8_t granted_qos, const bool subscribe_succeed)
{
    MQTT_UNUSED_PARAM(mqtt_client);
    MQTT_UNUSED_PARAM(granted_qos);
    MQTT_UNUSED_PARAM(subscribe_succeed);
}

/** \brief MQTT client unsubscribe callback */
static void mqtt_client_unsubscribe_callback(mqtt_client_t* const mqtt_client, const bool unsubscribe_succeed)
{
    MQTT_UNUSED_PARAM(mqtt_client);
    MQTT_UNUSED_PARAM(unsubscribe_succeed);
}

/** \brief MQTT client publish callback */
static void mqtt_client_publish_callback(mqtt_client_t* const mqtt_client, const bool publish_succeed)
{
    MQTT_UNUSED_PARAM(mqtt_client);
    MQTT_UNUSED_PARAM(publish_succeed);
}

/** \brief MQTT client publish received callback */
static void mqtt_client_publish_received_callback(mqtt_client_t* const mqtt_client, const mqtt_string_t* topic, const void* data,
                                                  const uint32_t length, const uint8_t qos, const bool retain, const bool duplicate)
{
    MQTT_UNUSED_PARAM(mqtt_client);
    MQTT_UNUSED_PARAM(topic);
    MQTT_UNUSED_PARAM(duplicate);

    cout << "PUBLISH received (" << length << " bytes, QoS " << static_cast<uint32_t>(qos);
    if (retain)
    {
        cout << ", retained";
    }
    cout << ") : { ";

    const uint8_t* received_data = reinterpret_cast<const uint8_t*>(data);
    for (uint32_t i = 0u; i < length; i++)
    {
        cout << static_cast<uint32_t>(*received_data) << " ";
        received_data++;
    }

    cout << "}";
}

/** \brief MQTT client disconnect callback */
static void mqtt_client_disconnect_callback(mqtt_client_t* const mqtt_client, const bool expected)
{
    MQTT_UNUSED_PARAM(mqtt_client);
    MQTT_UNUSED_PARAM(expected);
}
