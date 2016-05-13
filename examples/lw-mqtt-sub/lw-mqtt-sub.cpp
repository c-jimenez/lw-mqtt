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
#include <vector>
#include <iostream>
using namespace std;

#include "mqtt_client.h"
#include "mqtt_error.h"

/** \brief Program version */
#define LW_MQTT_SUB_VERSION "1.0"


/** Program parameters */
struct lw_mqtt_sub_params_t
{

    /** \brief Construtor to set the default values */
    lw_mqtt_sub_params_t()
        : broker_ip("127.0.0.1")
        , broker_port(1883u)
        , keepalive(0u)
        , username("")
        , password("")
        , qos(0u)
        , topics()
        , current_topic(0u)
        , is_disconnected(false)
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

    /** \brief Topics */
    vector<string> topics;

    /** \brief Current topic for subscription */
    uint32_t current_topic;

    /** \brief Disconnection indication */
    bool is_disconnected;
};



/** \brief Print the version message */
static void lw_mqtt_sub_print_version();

/** \brief Print the usage message */
static void lw_mqtt_sub_print_usage();

/** \brief Parse the command line parameters */
static bool lw_mqtt_sub_parse_parameters(lw_mqtt_sub_params_t& params, int argc, char* argv[]);

/** \brief MQTT client connect callback */
static void mqtt_client_connect_callback(mqtt_client_t* const mqtt_client, const bool connected, const mqtt_connack_retcode_t retcode);

/** \brief MQTT client subscribe callback */
static void mqtt_client_subscribe_callback(mqtt_client_t* const mqtt_client, const uint8_t granted_qos, const bool subscribe_succeed);

/** \brief MQTT client publish received callback */
static void mqtt_client_publish_received_callback(mqtt_client_t* const mqtt_client, const mqtt_string_t* topic, const void* data,
                                                  const uint32_t length, const uint8_t qos, const bool retain, const bool duplicate);

/** \brief MQTT client disconnect callback */
static void mqtt_client_disconnect_callback(mqtt_client_t* const mqtt_client, const bool expected);




/** \brief Program entry point */
int main(int argc, char* argv[])
{
    bool ret;
    lw_mqtt_sub_params_t params;

    /* Parse command line parameters */
    ret = lw_mqtt_sub_parse_parameters(params, argc, argv);
    if (ret)
    {
        string client_id;
        stringstream sstream;
        mqtt_client_t client;
        mqtt_client_callbacks_t callbacks = {
                                                mqtt_client_connect_callback,
                                                mqtt_client_subscribe_callback,
                                                NULL,
                                                NULL,
                                                mqtt_client_publish_received_callback,
                                                mqtt_client_disconnect_callback
                                            };

        /* Initialize low level layers */
        mqtt_mutex_init();
        mqtt_socket_init();

        /* Create unique Client Id */
        std::srand(static_cast<unsigned int>(std::time(NULL)));
        sstream << "lw-mqtt-sub-" << std::rand();
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
            while (!params.is_disconnected)
            {
                mqtt_client_task(&client);
            }
        }
    }    

	return ((ret)?0:1);
}


/** \brief Print the version message */
static void lw_mqtt_sub_print_version()
{
    cout << "lw-mqtt-sub version " << LW_MQTT_SUB_VERSION << " compiled with liblw-mqtt version " << lw_mqtt_lib_version() << endl;
}

/** \brief Print the usage message */
static void lw_mqtt_sub_print_usage()
{
    cout << "usage: lw-mqtt-sub [--version] [--help] [-h <broker-ip>] [-p <broker-port>]" << endl;
    cout << "                   [-k <keepalive>] [--user <username>] [--passwd <password>]" << endl;
    cout << "                   [-q <QoS>] [-t <topic1> [<topic2> [... <topicN>]]]" << endl;
}


/** \brief Parse the command line parameters */
static bool lw_mqtt_sub_parse_parameters(lw_mqtt_sub_params_t& params, int argc, char* argv[])
{
    bool ret = false;
    bool unique_option = false;
    bool invalid_arg = false;
    bool topic_found = false;
    bool topic_listing = false;

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
            lw_mqtt_sub_print_usage();
        }
        else if (strcmp(*argv, "--version") == 0)
        {
            unique_option = true;
            lw_mqtt_sub_print_version();
        }
        else if (strcmp(*argv, "-h") == 0)
        {
            if (argc != 0)
            {
                argv++;
                argc--;
                params.broker_ip = *argv;
                topic_listing = false;
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
                topic_listing = false;
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
                topic_listing = false;
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
                topic_listing = false;
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
                topic_listing = false;
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
                topic_listing = false;
            }
            else
            {
                cout << "The -q option must be followed by the QoS value.";
                invalid_arg = true;
            }
        }
        else if (strcmp(*argv, "-t") == 0)
        {
            if (argc != 0)
            {
                argv++;
                argc--;
                params.topics.push_back(*argv);
                topic_found = true;
                topic_listing = true;
            }
            else
            {
                cout << "The -t option must be followed by at least one topic name.";
                invalid_arg = true;
            }
        }
        else
        {
            if (topic_listing && (*argv[0] != '-'))
            {
                params.topics.push_back(*argv);
            }
            else
            {
                cout << "Invalid parameter : '" << *argv << "'.";
                invalid_arg = true;
            }
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
        cout << endl << "See 'lw-mqtt-sub --help'." << endl;
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

    /* Get parameters stored in client user data */
    lw_mqtt_sub_params_t* params;
    mqtt_client_get_user_data(mqtt_client, (void**)&params);

    if (connected)
    {
        /* Subscribe to the first topic */
        const bool ret = mqtt_client_subscribe(mqtt_client, params->topics[params->current_topic].c_str(), params->qos);
        if (!ret)
        {
            cout << "Error " << mqtt_errno_get() << ": Failed to subscribe to topic [" << params->topics[params->current_topic] << "] with QoS" << static_cast<uint32_t>(params->qos) << endl;
        }
        params->current_topic++;
    }
    else
    {
        params->is_disconnected = true;
    }
}

/** \brief MQTT client subscribe callback */
static void mqtt_client_subscribe_callback(mqtt_client_t* const mqtt_client, const uint8_t granted_qos, const bool subscribe_succeed)
{
    /* Get parameters stored in client user data */
    lw_mqtt_sub_params_t* params;
    mqtt_client_get_user_data(mqtt_client, (void**)&params);

    /* Display result */
    if (subscribe_succeed)
    {
        cout << "Successful subscribe to topic [" << params->topics[params->current_topic - 1u] << "] with QoS" << static_cast<uint32_t>(granted_qos) << endl;
    }
    else
    {
        cout << "Broker : Failed to subscribe to topic [" << params->topics[params->current_topic - 1u] << "] with QoS" << static_cast<uint32_t>(params->qos) << endl;
    }

    /* Subscribe to the next topic */
    if (params->current_topic != params->topics.size())
    {
        const bool ret = mqtt_client_subscribe(mqtt_client, params->topics[params->current_topic].c_str(), params->qos);
        if (!ret)
        {
            cout << "Error " << mqtt_errno_get() << ": Failed to subscribe to topic [" << params->topics[params->current_topic] << "] with QoS" << static_cast<uint32_t>(params->qos) << endl;
        }
        params->current_topic++;
    }
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

    cout << "}" << endl;
}

/** \brief MQTT client disconnect callback */
static void mqtt_client_disconnect_callback(mqtt_client_t* const mqtt_client, const bool expected)
{
    MQTT_UNUSED_PARAM(expected);

    /* Get parameters stored in client user data */
    lw_mqtt_sub_params_t* params;
    mqtt_client_get_user_data(mqtt_client, (void**)&params);

    params->is_disconnected = true;
}
