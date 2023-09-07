#pragma once
// MESSAGE GET_CONF PACKING

#define MAVLINK_MSG_ID_GET_CONF 33


typedef struct __mavlink_get_conf_t {
 uint8_t seq; /*<  */
} mavlink_get_conf_t;

#define MAVLINK_MSG_ID_GET_CONF_LEN 1
#define MAVLINK_MSG_ID_GET_CONF_MIN_LEN 1
#define MAVLINK_MSG_ID_33_LEN 1
#define MAVLINK_MSG_ID_33_MIN_LEN 1

#define MAVLINK_MSG_ID_GET_CONF_CRC 149
#define MAVLINK_MSG_ID_33_CRC 149



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_GET_CONF { \
    33, \
    "GET_CONF", \
    1, \
    {  { "seq", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_get_conf_t, seq) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_GET_CONF { \
    "GET_CONF", \
    1, \
    {  { "seq", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_get_conf_t, seq) }, \
         } \
}
#endif

/**
 * @brief Pack a get_conf message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param seq  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_get_conf_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t seq)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GET_CONF_LEN];
    _mav_put_uint8_t(buf, 0, seq);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GET_CONF_LEN);
#else
    mavlink_get_conf_t packet;
    packet.seq = seq;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GET_CONF_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_GET_CONF;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GET_CONF_MIN_LEN, MAVLINK_MSG_ID_GET_CONF_LEN, MAVLINK_MSG_ID_GET_CONF_CRC);
}

/**
 * @brief Pack a get_conf message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param seq  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_get_conf_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t seq)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GET_CONF_LEN];
    _mav_put_uint8_t(buf, 0, seq);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GET_CONF_LEN);
#else
    mavlink_get_conf_t packet;
    packet.seq = seq;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GET_CONF_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_GET_CONF;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GET_CONF_MIN_LEN, MAVLINK_MSG_ID_GET_CONF_LEN, MAVLINK_MSG_ID_GET_CONF_CRC);
}

/**
 * @brief Encode a get_conf struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param get_conf C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_get_conf_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_get_conf_t* get_conf)
{
    return mavlink_msg_get_conf_pack(system_id, component_id, msg, get_conf->seq);
}

/**
 * @brief Encode a get_conf struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param get_conf C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_get_conf_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_get_conf_t* get_conf)
{
    return mavlink_msg_get_conf_pack_chan(system_id, component_id, chan, msg, get_conf->seq);
}

/**
 * @brief Send a get_conf message
 * @param chan MAVLink channel to send the message
 *
 * @param seq  
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_get_conf_send(mavlink_channel_t chan, uint8_t seq)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GET_CONF_LEN];
    _mav_put_uint8_t(buf, 0, seq);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_CONF, buf, MAVLINK_MSG_ID_GET_CONF_MIN_LEN, MAVLINK_MSG_ID_GET_CONF_LEN, MAVLINK_MSG_ID_GET_CONF_CRC);
#else
    mavlink_get_conf_t packet;
    packet.seq = seq;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_CONF, (const char *)&packet, MAVLINK_MSG_ID_GET_CONF_MIN_LEN, MAVLINK_MSG_ID_GET_CONF_LEN, MAVLINK_MSG_ID_GET_CONF_CRC);
#endif
}

/**
 * @brief Send a get_conf message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_get_conf_send_struct(mavlink_channel_t chan, const mavlink_get_conf_t* get_conf)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_get_conf_send(chan, get_conf->seq);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_CONF, (const char *)get_conf, MAVLINK_MSG_ID_GET_CONF_MIN_LEN, MAVLINK_MSG_ID_GET_CONF_LEN, MAVLINK_MSG_ID_GET_CONF_CRC);
#endif
}

#if MAVLINK_MSG_ID_GET_CONF_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_get_conf_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t seq)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 0, seq);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_CONF, buf, MAVLINK_MSG_ID_GET_CONF_MIN_LEN, MAVLINK_MSG_ID_GET_CONF_LEN, MAVLINK_MSG_ID_GET_CONF_CRC);
#else
    mavlink_get_conf_t *packet = (mavlink_get_conf_t *)msgbuf;
    packet->seq = seq;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GET_CONF, (const char *)packet, MAVLINK_MSG_ID_GET_CONF_MIN_LEN, MAVLINK_MSG_ID_GET_CONF_LEN, MAVLINK_MSG_ID_GET_CONF_CRC);
#endif
}
#endif

#endif

// MESSAGE GET_CONF UNPACKING


/**
 * @brief Get field seq from get_conf message
 *
 * @return  
 */
static inline uint8_t mavlink_msg_get_conf_get_seq(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Decode a get_conf message into a struct
 *
 * @param msg The message to decode
 * @param get_conf C-struct to decode the message contents into
 */
static inline void mavlink_msg_get_conf_decode(const mavlink_message_t* msg, mavlink_get_conf_t* get_conf)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    get_conf->seq = mavlink_msg_get_conf_get_seq(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_GET_CONF_LEN? msg->len : MAVLINK_MSG_ID_GET_CONF_LEN;
        memset(get_conf, 0, MAVLINK_MSG_ID_GET_CONF_LEN);
    memcpy(get_conf, _MAV_PAYLOAD(msg), len);
#endif
}
