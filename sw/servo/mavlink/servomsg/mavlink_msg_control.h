#pragma once
// MESSAGE CONTROL PACKING

#define MAVLINK_MSG_ID_CONTROL 34


typedef struct __mavlink_control_t {
 uint8_t seq; /*<  */
 uint8_t start; /*<  */
} mavlink_control_t;

#define MAVLINK_MSG_ID_CONTROL_LEN 2
#define MAVLINK_MSG_ID_CONTROL_MIN_LEN 2
#define MAVLINK_MSG_ID_34_LEN 2
#define MAVLINK_MSG_ID_34_MIN_LEN 2

#define MAVLINK_MSG_ID_CONTROL_CRC 99
#define MAVLINK_MSG_ID_34_CRC 99



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_CONTROL { \
    34, \
    "CONTROL", \
    2, \
    {  { "seq", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_control_t, seq) }, \
         { "start", NULL, MAVLINK_TYPE_UINT8_T, 0, 1, offsetof(mavlink_control_t, start) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_CONTROL { \
    "CONTROL", \
    2, \
    {  { "seq", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_control_t, seq) }, \
         { "start", NULL, MAVLINK_TYPE_UINT8_T, 0, 1, offsetof(mavlink_control_t, start) }, \
         } \
}
#endif

/**
 * @brief Pack a control message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param seq  
 * @param start  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_control_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t seq, uint8_t start)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CONTROL_LEN];
    _mav_put_uint8_t(buf, 0, seq);
    _mav_put_uint8_t(buf, 1, start);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONTROL_LEN);
#else
    mavlink_control_t packet;
    packet.seq = seq;
    packet.start = start;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONTROL_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_CONTROL;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
}

/**
 * @brief Pack a control message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param seq  
 * @param start  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_control_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t seq,uint8_t start)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CONTROL_LEN];
    _mav_put_uint8_t(buf, 0, seq);
    _mav_put_uint8_t(buf, 1, start);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONTROL_LEN);
#else
    mavlink_control_t packet;
    packet.seq = seq;
    packet.start = start;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONTROL_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_CONTROL;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
}

/**
 * @brief Encode a control struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param control C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_control_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_control_t* control)
{
    return mavlink_msg_control_pack(system_id, component_id, msg, control->seq, control->start);
}

/**
 * @brief Encode a control struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param control C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_control_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_control_t* control)
{
    return mavlink_msg_control_pack_chan(system_id, component_id, chan, msg, control->seq, control->start);
}

/**
 * @brief Send a control message
 * @param chan MAVLink channel to send the message
 *
 * @param seq  
 * @param start  
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_control_send(mavlink_channel_t chan, uint8_t seq, uint8_t start)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CONTROL_LEN];
    _mav_put_uint8_t(buf, 0, seq);
    _mav_put_uint8_t(buf, 1, start);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONTROL, buf, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
#else
    mavlink_control_t packet;
    packet.seq = seq;
    packet.start = start;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONTROL, (const char *)&packet, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
#endif
}

/**
 * @brief Send a control message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_control_send_struct(mavlink_channel_t chan, const mavlink_control_t* control)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_control_send(chan, control->seq, control->start);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONTROL, (const char *)control, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
#endif
}

#if MAVLINK_MSG_ID_CONTROL_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_control_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t seq, uint8_t start)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 0, seq);
    _mav_put_uint8_t(buf, 1, start);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONTROL, buf, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
#else
    mavlink_control_t *packet = (mavlink_control_t *)msgbuf;
    packet->seq = seq;
    packet->start = start;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONTROL, (const char *)packet, MAVLINK_MSG_ID_CONTROL_MIN_LEN, MAVLINK_MSG_ID_CONTROL_LEN, MAVLINK_MSG_ID_CONTROL_CRC);
#endif
}
#endif

#endif

// MESSAGE CONTROL UNPACKING


/**
 * @brief Get field seq from control message
 *
 * @return  
 */
static inline uint8_t mavlink_msg_control_get_seq(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Get field start from control message
 *
 * @return  
 */
static inline uint8_t mavlink_msg_control_get_start(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  1);
}

/**
 * @brief Decode a control message into a struct
 *
 * @param msg The message to decode
 * @param control C-struct to decode the message contents into
 */
static inline void mavlink_msg_control_decode(const mavlink_message_t* msg, mavlink_control_t* control)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    control->seq = mavlink_msg_control_get_seq(msg);
    control->start = mavlink_msg_control_get_start(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_CONTROL_LEN? msg->len : MAVLINK_MSG_ID_CONTROL_LEN;
        memset(control, 0, MAVLINK_MSG_ID_CONTROL_LEN);
    memcpy(control, _MAV_PAYLOAD(msg), len);
#endif
}
