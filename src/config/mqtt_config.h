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

#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

/** \brief Define the CPU type :
           - MQTT_CFG_CPU_TYPE_BE : big endian
           - MQTT_CFG_CPU_TYPE_LE : little endian
*/
#define MQTT_CFG_CPU_TYPE_LE


/** \brief Maximum level of QoS (shall be less or equal than 2) */
#define MQTT_CFG_MAX_QOS_LEVEL  2u

/** \brief Enable the multitasking */
#define MQTT_MULTITASKING_ENABLED



/** \brief Maximum length in bytes of a topic string for the MQTT client */
#define MQTT_CLIENT_MAX_TOPIC_LENGTH    512u

/** \brief Maximum length in byte of the payload of a PUBLISH message for the MQTT client */
#define MQTT_CLIENT_MAX_PAYLOAD_SIZE    1024u



/** \brief Maximum length in bytes of a topic string for the MQTT broker */
#define MQTT_BROKER_MAX_TOPIC_LENGTH    512u

/** \brief Maximum length in byte of the payload of a PUBLISH message for the MQTT broker */
#define MQTT_BROKER_MAX_PAYLOAD_SIZE    2048u

/** \brief Maximum number of simultaneous clients for the MQTT broker */
#define MQTT_BROKER_MAX_CLIENT          10u

/** \brief Maximum length in bytes of a will topic string for the MQTT broker */
#define MQTT_BROKER_MAX_WILL_TOPIC_LENGTH    512u

/** \brief Maximum length in byte of a will message for the MQTT broker */
#define MQTT_BROKER_MAX_WILL_MESSAGE_SIZE    2048u

/** \brief Maximum length in bytes of a client id string for the MQTT broker */
#define MQTT_BROKER_MAX_CLIENT_ID_LENGTH     32u








#endif /* MQTT_CONFIG_H */
