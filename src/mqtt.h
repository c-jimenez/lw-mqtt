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

#ifndef MQTT_H
#define MQTT_H

#include "stdheaders.h"
#include "mqtt_config.h"


/** \brief lw-mqtt library version */
#define LW_MQTT_LIB_VERSION "0.1"



#ifdef MQTT_CFG_CPU_TYPE_BE

/** \brief Create a big endian uint16_t from an uint16_t value */
#define MQTT_BIG_ENDIAN_UINT16(value) (value)

#else /* MQTT_CFG_CPU_TYPE_BE */

/** \brief Create a big endian uint16_t from an uint16_t value */
#define MQTT_BIG_ENDIAN_UINT16(value) ((uint16_t)(((value) >> 8u) | ((value) << 8u)))

#endif /* MQTT_CFG_CPU_TYPE_BE */

/* Check config CPU type */
#ifndef MQTT_CFG_CPU_TYPE_BE
#ifndef MQTT_CFG_CPU_TYPE_LE
#error "Invalid CPU type in configuration file"
#endif
#endif


/** \brief Maximum MQTT string size in bytes => 2 bytes encoding : 0xFFFF */
#define MQTT_MAXIMUM_STRING_SIZE    0xFFFFu

/** \brief Maximum level of QoS (shall be less or equal than 2) */
#define MQTT_MAX_QOS_LEVEL  2u

/** \brief QoS value which represents a failure for some responses */
#define MQTT_FAILURE_QOS 0x80u

/* Check config maximum level of qos set in config */
#if (MQTT_CFG_MAX_QOS_LEVEL > MQTT_MAX_QOS_LEVEL)
#error "Invalid maximum level of QoS in configuration file"
#endif


/** \brief Minimum encoded string size => 2 bytes (length)*/
#define MQTT_MIN_ENCODED_STRING_SIZE 2u


/** \brief Minimum size of a MQTT CONNECT packet :
           => 6 (protocol name) + 1 (version) + 1 (flags) + 2 (keepalive)
*/
#define MQTT_CONNECT_PACKET_MIN_SIZE 10u

/** \brief Size of a MQTT CONNACK packet :
           => 1 (flags) + 1 (return code)
*/
#define MQTT_CONNACK_PACKET_SIZE 2u

/** \brief Size of a MQTT packet id only response packet 
           => 2 (packet id)
*/
#define MQTT_PACKET_ID_ONLY_PACKET_SIZE 2u

/** \brief Header value specific to the SUBSCRIBE and UNSUBSCRIBE packets */
#define MQTT_SUB_UNSUBSCRIBE_HEADER_VALUE 2u

/** \brief Minimum size of a MQTT SUBSCRIBE packet
           => 2 (packet id) + 1 (qos)
*/
#define MQTT_SUBSCRIBE_PACKET_MIN_SIZE 3u

/** \brief Size of a MQTT SUBACK packet
           => 2 (packet id) + 1 (qos)
*/
#define MQTT_SUBACK_PACKET_SIZE 3u

/** \brief Minimum size of a MQTT UNSUBSCRIBE packet
           => 2 (packet id)
*/
#define MQTT_UNSUBSCRIBE_PACKET_MIN_SIZE 2u

/** \brief Size of a MQTT UNSUBACK packet
           => 2 (packet id)
*/
#define MQTT_UNSUBACK_PACKET_SIZE 3u

/** \brief Size of a MQTT PINGREQ packet
           => 0 (no payload)
*/
#define MQTT_PINGREQ_PACKET_SIZE 0u

/** \brief Size of a MQTT PINGRESP packet
           => 0 (no payload)
*/
#define MQTT_PINGRESP_PACKET_SIZE 0u

/** \brief Size of a MQTT DISCONNECT packet
           => 0 (no payload)
*/
#define MQTT_DISCONNECT_PACKET_SIZE 0u


/** \brief MQTT Protocol name */
#define MQTT_PROTOCOL_NAME { 0x00u, 0x04u, (uint8_t)'M', (uint8_t)'Q', (uint8_t)'T', (uint8_t)'T'}

/** \brief MQTT Protocol name size */
#define MQTT_PROTOCOL_NAME_SIZE 6u

/** \brief MQTT Protocol level */
#define MQTT_PROTOCOL_LEVEL 4u



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief MQTT string */
typedef struct _mqtt_string_t
{
    /** \brief Not NULL terminated string */
    char* str;
    /** \brief Size of the string in bytes */
    uint16_t size;
} mqtt_string_t;

/** \brief MQTT const string */
typedef struct _mqtt_const_string_t
{
    /** \brief Not NULL terminated string */
    const char* str;
    /** \brief Size of the string in bytes */
    uint16_t size;
} mqtt_const_string_t;


/** \brief MQTT credentials */
typedef struct _mqtt_credentials_t
{
    /** \brief Username */
    mqtt_string_t username;
    /** \brief Password */
    mqtt_string_t password;
} mqtt_credentials_t;

/** \brief MQTT const credentials */
typedef struct _mqtt_const_credentials_t
{
    /** \brief Username */
    mqtt_const_string_t username;
    /** \brief Password */
    mqtt_const_string_t password;
} mqtt_const_credentials_t;


/** \brief MQTT will */
typedef struct _mqtt_will_t
{
    /** \brief Topic */
    mqtt_string_t topic;
    /** \brief Message */
    mqtt_string_t message;
    /** \brief QoS */
    uint8_t qos;
    /** \brief Retain flag */
    bool retain;
} mqtt_will_t;

/** \brief MQTT const will */
typedef struct _mqtt_const_will_t
{
    /** \brief Topic */
    mqtt_const_string_t topic;
    /** \brief Message */
    mqtt_const_string_t message;
    /** \brief QoS */
    uint8_t qos;
    /** \brief Retain flag */
    bool retain;
} mqtt_const_will_t;


/** \brief MQTT control packet types */
typedef enum _mqtt_control_packet_type_t
{
    MQTT_PKT_CONNECT = 1u,
    MQTT_PKT_CONNACK = 2u,
    MQTT_PKT_PUBLISH = 3u,
    MQTT_PKT_PUBACK = 4u,
    MQTT_PKT_PUBREC = 5u,
    MQTT_PKT_PUBREL = 6u,
    MQTT_PKT_PUBCOMP = 7u,
    MQTT_PKT_SUBSCRIBE = 8u,
    MQTT_PKT_SUBACK = 9u,
    MQTT_PKT_UNSUBSCRIBE = 10u,
    MQTT_PKT_UNSUBACK = 11u,
    MQTT_PKT_PINGREQ = 12u,
    MQTT_PKT_PINGRESP = 13u,
    MQTT_PKT_DISCONNECT = 14u
} mqtt_control_packet_type_t;


/** \brief MQTT CONNECT packet flags */
typedef enum _mqtt_connect_packet_flags
{
    MQTT_CONNECT_FLAG_USERNAME = (1u << 7u),
    MQTT_CONNECT_FLAG_PASSWORD = (1u << 6u),
    MQTT_CONNECT_FLAG_WILL_RETAIN = (1u << 5u),
    MQTT_CONNECT_FLAG_WILL_QOS = (3u << 3u),
    MQTT_CONNECT_FLAG_WILL_QOS_POSITION = 3u,
    MQTT_CONNECT_FLAG_WILL_FLAG = (3u << 2u),
    MQTT_CONNECT_FLAG_CLEAN_SESSION = (1u << 1u)

} mqtt_connect_packet_flags;

/** \brief MQTT CONNACK return codes */
typedef enum _mqtt_connack_retcode_t
{
    MQTT_CONNACK_RET_ACCEPTED = 0u,
    MQTT_CONNACK_RET_REFUSED_PROTOCOL = 1u,
    MQTT_CONNACK_RET_REFUSED_CLIENT_ID = 2u,
    MQTT_CONNACK_RET_REFUSED_SERVER_UNAVAILABLE = 3u,
    MQTT_CONNACK_RET_REFUSED_BAD_CREDENTIALS = 4u,
    MQTT_CONNACK_RET_REFUSED_NOT_AUTHORIZED = 5u,
    MQTT_CONNACK_RET_DISCONNECTED = 0xFFu
} mqtt_connack_retcode_t;

/** \brief MQTT PUBLISH packet flags */
typedef enum _mqtt_publish_packet_flags
{
    MQTT_PUBLISH_FLAG_DUP = (1u << 3u),
    MQTT_PUBLISH_FLAG_QOS = (3u << 1u),
    MQTT_PUBLISH_FLAG_QOS_POSITION = 1u,
    MQTT_PUBLISH_FLAG_RETAIN = (1u << 0u)

} _mqtt_publish_packet_flags;




/** \brief Get the library version */
const char* lw_mqtt_lib_version(void);



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MQTT_H */
