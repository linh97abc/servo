#pragma once
// MESSAGE POS_MON PACKING

#define MAVLINK_MSG_ID_POS_MON 1


typedef struct __mavlink_pos_mon_t {
 int16_t pos[4]; /*<  */
 uint8_t seq; /*<  */
 uint8_t isRunning; /*<  */
} mavlink_pos_mon_t;

#define MAVLINK_MSG_ID_POS_MON_LEN 10
#define MAVLINK_MSG_ID_POS_MON_MIN_LEN 10
#define MAVLINK_MSG_ID_1_LEN 10
#define MAVLINK_MSG_ID_1_MIN_LEN 10

#define MAVLINK_MSG_ID_POS_MON_CRC 40
#define MAVLINK_MSG_ID_1_CRC 40

#define MAVLINK_MSG_POS_MON_FIELD_POS_LEN 4

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_POS_MON { \
    1, \
    "POS_MON", \
    3, \
    {  { "seq", NULL, MAVLINK_TYPE_UINT8_T, 0, 8, offsetof(mavlink_pos_mon_t, seq) }, \
         { "isRunning", NULL, MAVLINK_TYPE_UINT8_T, 0, 9, offsetof(mavlink_pos_mon_t, isRunning) }, \
         { "pos", NULL, MAVLINK_TYPE_INT16_T, 4, 0, offsetof(mavlink_pos_mon_t, pos) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_POS_MON { \
    "POS_MON", \
    3, \
    {  { "seq", NULL, MAVLINK_TYPE_UINT8_T, 0, 8, offsetof(mavlink_pos_mon_t, seq) }, \
         { "isRunning", NULL, MAVLINK_TYPE_UINT8_T, 0, 9, offsetof(mavlink_pos_mon_t, isRunning) }, \
         { "pos", NULL, MAVLINK_TYPE_INT16_T, 4, 0, offsetof(mavlink_pos_mon_t, pos) }, \
         } \
}
#endif

/**
 * @brief Pack a pos_mon message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param seq  
 * @param isRunning  
 * @param pos  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_pos_mon_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t seq, uint8_t isRunning, const int16_t *pos)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_POS_MON_LEN];
    _mav_put_uint8_t(buf, 8, seq);
    _mav_put_uint8_t(buf, 9, isRunning);
    _mav_put_int16_t_array(buf, 0, pos, 4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_POS_MON_LEN);
#else
    mavlink_pos_mon_t packet;
    packet.seq = seq;
    packet.isRunning = isRunning;
    mav_array_memcpy(packet.pos, pos, sizeof(int16_t)*4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_POS_MON_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_POS_MON;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_POS_MON_MIN_LEN, MAVLINK_MSG_ID_POS_MON_LEN, MAVLINK_MSG_ID_POS_MON_CRC);
}

/**
 * @brief Pack a pos_mon message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param seq  
 * @param isRunning  
 * @param pos  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_pos_mon_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t seq,uint8_t isRunning,const int16_t *pos)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_POS_MON_LEN];
    _mav_put_uint8_t(buf, 8, seq);
    _mav_put_uint8_t(buf, 9, isRunning);
    _mav_put_int16_t_array(buf, 0, pos, 4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_POS_MON_LEN);
#else
    mavlink_pos_mon_t packet;
    packet.seq = seq;
    packet.isRunning = isRunning;
    mav_array_memcpy(packet.pos, pos, sizeof(int16_t)*4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_POS_MON_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_POS_MON;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_POS_MON_MIN_LEN, MAVLINK_MSG_ID_POS_MON_LEN, MAVLINK_MSG_ID_POS_MON_CRC);
}

/**
 * @brief Encode a pos_mon struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param pos_mon C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_pos_mon_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_pos_mon_t* pos_mon)
{
    return mavlink_msg_pos_mon_pack(system_id, component_id, msg, pos_mon->seq, pos_mon->isRunning, pos_mon->pos);
}

/**
 * @brief Encode a pos_mon struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param pos_mon C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_pos_mon_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_pos_mon_t* pos_mon)
{
    return mavlink_msg_pos_mon_pack_chan(system_id, component_id, chan, msg, pos_mon->seq, pos_mon->isRunning, pos_mon->pos);
}

/**
 * @brief Send a pos_mon message
 * @param chan MAVLink channel to send the message
 *
 * @param seq  
 * @param isRunning  
 * @param pos  
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_pos_mon_send(mavlink_channel_t chan, uint8_t seq, uint8_t isRunning, const int16_t *pos)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_POS_MON_LEN];
    _mav_put_uint8_t(buf, 8, seq);
    _mav_put_uint8_t(buf, 9, isRunning);
    _mav_put_int16_t_array(buf, 0, pos, 4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POS_MON, buf, MAVLINK_MSG_ID_POS_MON_MIN_LEN, MAVLINK_MSG_ID_POS_MON_LEN, MAVLINK_MSG_ID_POS_MON_CRC);
#else
    mavlink_pos_mon_t packet;
    packet.seq = seq;
    packet.isRunning = isRunning;
    mav_array_memcpy(packet.pos, pos, sizeof(int16_t)*4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POS_MON, (const char *)&packet, MAVLINK_MSG_ID_POS_MON_MIN_LEN, MAVLINK_MSG_ID_POS_MON_LEN, MAVLINK_MSG_ID_POS_MON_CRC);
#endif
}

/**
 * @brief Send a pos_mon message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_pos_mon_send_struct(mavlink_channel_t chan, const mavlink_pos_mon_t* pos_mon)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_pos_mon_send(chan, pos_mon->seq, pos_mon->isRunning, pos_mon->pos);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POS_MON, (const char *)pos_mon, MAVLINK_MSG_ID_POS_MON_MIN_LEN, MAVLINK_MSG_ID_POS_MON_LEN, MAVLINK_MSG_ID_POS_MON_CRC);
#endif
}

#if MAVLINK_MSG_ID_POS_MON_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_pos_mon_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t seq, uint8_t isRunning, const int16_t *pos)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 8, seq);
    _mav_put_uint8_t(buf, 9, isRunning);
    _mav_put_int16_t_array(buf, 0, pos, 4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POS_MON, buf, MAVLINK_MSG_ID_POS_MON_MIN_LEN, MAVLINK_MSG_ID_POS_MON_LEN, MAVLINK_MSG_ID_POS_MON_CRC);
#else
    mavlink_pos_mon_t *packet = (mavlink_pos_mon_t *)msgbuf;
    packet->seq = seq;
    packet->isRunning = isRunning;
    mav_array_memcpy(packet->pos, pos, sizeof(int16_t)*4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_POS_MON, (const char *)packet, MAVLINK_MSG_ID_POS_MON_MIN_LEN, MAVLINK_MSG_ID_POS_MON_LEN, MAVLINK_MSG_ID_POS_MON_CRC);
#endif
}
#endif

#endif

// MESSAGE POS_MON UNPACKING


/**
 * @brief Get field seq from pos_mon message
 *
 * @return  
 */
static inline uint8_t mavlink_msg_pos_mon_get_seq(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  8);
}

/**
 * @brief Get field isRunning from pos_mon message
 *
 * @return  
 */
static inline uint8_t mavlink_msg_pos_mon_get_isRunning(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  9);
}

/**
 * @brief Get field pos from pos_mon message
 *
 * @return  
 */
static inline uint16_t mavlink_msg_pos_mon_get_pos(const mavlink_message_t* msg, int16_t *pos)
{
    return _MAV_RETURN_int16_t_array(msg, pos, 4,  0);
}

/**
 * @brief Decode a pos_mon message into a struct
 *
 * @param msg The message to decode
 * @param pos_mon C-struct to decode the message contents into
 */
static inline void mavlink_msg_pos_mon_decode(const mavlink_message_t* msg, mavlink_pos_mon_t* pos_mon)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_pos_mon_get_pos(msg, pos_mon->pos);
    pos_mon->seq = mavlink_msg_pos_mon_get_seq(msg);
    pos_mon->isRunning = mavlink_msg_pos_mon_get_isRunning(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_POS_MON_LEN? msg->len : MAVLINK_MSG_ID_POS_MON_LEN;
        memset(pos_mon, 0, MAVLINK_MSG_ID_POS_MON_LEN);
    memcpy(pos_mon, _MAV_PAYLOAD(msg), len);
#endif
}
