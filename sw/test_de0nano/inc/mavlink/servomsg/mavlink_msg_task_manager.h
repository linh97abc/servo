#pragma once
// MESSAGE TASK_MANAGER PACKING

#define MAVLINK_MSG_ID_TASK_MANAGER 3


typedef struct __mavlink_task_manager_t {
 uint32_t stk_used; /*<  */
 uint32_t stk_size; /*<  */
 uint32_t sw_cnt; /*<  */
 char task_name[32]; /*<  */
 uint8_t prio; /*<  */
} mavlink_task_manager_t;

#define MAVLINK_MSG_ID_TASK_MANAGER_LEN 45
#define MAVLINK_MSG_ID_TASK_MANAGER_MIN_LEN 45
#define MAVLINK_MSG_ID_3_LEN 45
#define MAVLINK_MSG_ID_3_MIN_LEN 45

#define MAVLINK_MSG_ID_TASK_MANAGER_CRC 61
#define MAVLINK_MSG_ID_3_CRC 61

#define MAVLINK_MSG_TASK_MANAGER_FIELD_TASK_NAME_LEN 32

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_TASK_MANAGER { \
    3, \
    "TASK_MANAGER", \
    5, \
    {  { "task_name", NULL, MAVLINK_TYPE_CHAR, 32, 12, offsetof(mavlink_task_manager_t, task_name) }, \
         { "prio", NULL, MAVLINK_TYPE_UINT8_T, 0, 44, offsetof(mavlink_task_manager_t, prio) }, \
         { "stk_used", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_task_manager_t, stk_used) }, \
         { "stk_size", NULL, MAVLINK_TYPE_UINT32_T, 0, 4, offsetof(mavlink_task_manager_t, stk_size) }, \
         { "sw_cnt", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_task_manager_t, sw_cnt) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_TASK_MANAGER { \
    "TASK_MANAGER", \
    5, \
    {  { "task_name", NULL, MAVLINK_TYPE_CHAR, 32, 12, offsetof(mavlink_task_manager_t, task_name) }, \
         { "prio", NULL, MAVLINK_TYPE_UINT8_T, 0, 44, offsetof(mavlink_task_manager_t, prio) }, \
         { "stk_used", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_task_manager_t, stk_used) }, \
         { "stk_size", NULL, MAVLINK_TYPE_UINT32_T, 0, 4, offsetof(mavlink_task_manager_t, stk_size) }, \
         { "sw_cnt", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_task_manager_t, sw_cnt) }, \
         } \
}
#endif

/**
 * @brief Pack a task_manager message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param task_name  
 * @param prio  
 * @param stk_used  
 * @param stk_size  
 * @param sw_cnt  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_task_manager_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               const char *task_name, uint8_t prio, uint32_t stk_used, uint32_t stk_size, uint32_t sw_cnt)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_TASK_MANAGER_LEN];
    _mav_put_uint32_t(buf, 0, stk_used);
    _mav_put_uint32_t(buf, 4, stk_size);
    _mav_put_uint32_t(buf, 8, sw_cnt);
    _mav_put_uint8_t(buf, 44, prio);
    _mav_put_char_array(buf, 12, task_name, 32);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_TASK_MANAGER_LEN);
#else
    mavlink_task_manager_t packet;
    packet.stk_used = stk_used;
    packet.stk_size = stk_size;
    packet.sw_cnt = sw_cnt;
    packet.prio = prio;
    mav_array_memcpy(packet.task_name, task_name, sizeof(char)*32);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_TASK_MANAGER_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_TASK_MANAGER;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_TASK_MANAGER_MIN_LEN, MAVLINK_MSG_ID_TASK_MANAGER_LEN, MAVLINK_MSG_ID_TASK_MANAGER_CRC);
}

/**
 * @brief Pack a task_manager message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param task_name  
 * @param prio  
 * @param stk_used  
 * @param stk_size  
 * @param sw_cnt  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_task_manager_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   const char *task_name,uint8_t prio,uint32_t stk_used,uint32_t stk_size,uint32_t sw_cnt)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_TASK_MANAGER_LEN];
    _mav_put_uint32_t(buf, 0, stk_used);
    _mav_put_uint32_t(buf, 4, stk_size);
    _mav_put_uint32_t(buf, 8, sw_cnt);
    _mav_put_uint8_t(buf, 44, prio);
    _mav_put_char_array(buf, 12, task_name, 32);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_TASK_MANAGER_LEN);
#else
    mavlink_task_manager_t packet;
    packet.stk_used = stk_used;
    packet.stk_size = stk_size;
    packet.sw_cnt = sw_cnt;
    packet.prio = prio;
    mav_array_memcpy(packet.task_name, task_name, sizeof(char)*32);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_TASK_MANAGER_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_TASK_MANAGER;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_TASK_MANAGER_MIN_LEN, MAVLINK_MSG_ID_TASK_MANAGER_LEN, MAVLINK_MSG_ID_TASK_MANAGER_CRC);
}

/**
 * @brief Encode a task_manager struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param task_manager C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_task_manager_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_task_manager_t* task_manager)
{
    return mavlink_msg_task_manager_pack(system_id, component_id, msg, task_manager->task_name, task_manager->prio, task_manager->stk_used, task_manager->stk_size, task_manager->sw_cnt);
}

/**
 * @brief Encode a task_manager struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param task_manager C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_task_manager_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_task_manager_t* task_manager)
{
    return mavlink_msg_task_manager_pack_chan(system_id, component_id, chan, msg, task_manager->task_name, task_manager->prio, task_manager->stk_used, task_manager->stk_size, task_manager->sw_cnt);
}

/**
 * @brief Send a task_manager message
 * @param chan MAVLink channel to send the message
 *
 * @param task_name  
 * @param prio  
 * @param stk_used  
 * @param stk_size  
 * @param sw_cnt  
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_task_manager_send(mavlink_channel_t chan, const char *task_name, uint8_t prio, uint32_t stk_used, uint32_t stk_size, uint32_t sw_cnt)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_TASK_MANAGER_LEN];
    _mav_put_uint32_t(buf, 0, stk_used);
    _mav_put_uint32_t(buf, 4, stk_size);
    _mav_put_uint32_t(buf, 8, sw_cnt);
    _mav_put_uint8_t(buf, 44, prio);
    _mav_put_char_array(buf, 12, task_name, 32);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_TASK_MANAGER, buf, MAVLINK_MSG_ID_TASK_MANAGER_MIN_LEN, MAVLINK_MSG_ID_TASK_MANAGER_LEN, MAVLINK_MSG_ID_TASK_MANAGER_CRC);
#else
    mavlink_task_manager_t packet;
    packet.stk_used = stk_used;
    packet.stk_size = stk_size;
    packet.sw_cnt = sw_cnt;
    packet.prio = prio;
    mav_array_memcpy(packet.task_name, task_name, sizeof(char)*32);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_TASK_MANAGER, (const char *)&packet, MAVLINK_MSG_ID_TASK_MANAGER_MIN_LEN, MAVLINK_MSG_ID_TASK_MANAGER_LEN, MAVLINK_MSG_ID_TASK_MANAGER_CRC);
#endif
}

/**
 * @brief Send a task_manager message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_task_manager_send_struct(mavlink_channel_t chan, const mavlink_task_manager_t* task_manager)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_task_manager_send(chan, task_manager->task_name, task_manager->prio, task_manager->stk_used, task_manager->stk_size, task_manager->sw_cnt);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_TASK_MANAGER, (const char *)task_manager, MAVLINK_MSG_ID_TASK_MANAGER_MIN_LEN, MAVLINK_MSG_ID_TASK_MANAGER_LEN, MAVLINK_MSG_ID_TASK_MANAGER_CRC);
#endif
}

#if MAVLINK_MSG_ID_TASK_MANAGER_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_task_manager_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  const char *task_name, uint8_t prio, uint32_t stk_used, uint32_t stk_size, uint32_t sw_cnt)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint32_t(buf, 0, stk_used);
    _mav_put_uint32_t(buf, 4, stk_size);
    _mav_put_uint32_t(buf, 8, sw_cnt);
    _mav_put_uint8_t(buf, 44, prio);
    _mav_put_char_array(buf, 12, task_name, 32);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_TASK_MANAGER, buf, MAVLINK_MSG_ID_TASK_MANAGER_MIN_LEN, MAVLINK_MSG_ID_TASK_MANAGER_LEN, MAVLINK_MSG_ID_TASK_MANAGER_CRC);
#else
    mavlink_task_manager_t *packet = (mavlink_task_manager_t *)msgbuf;
    packet->stk_used = stk_used;
    packet->stk_size = stk_size;
    packet->sw_cnt = sw_cnt;
    packet->prio = prio;
    mav_array_memcpy(packet->task_name, task_name, sizeof(char)*32);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_TASK_MANAGER, (const char *)packet, MAVLINK_MSG_ID_TASK_MANAGER_MIN_LEN, MAVLINK_MSG_ID_TASK_MANAGER_LEN, MAVLINK_MSG_ID_TASK_MANAGER_CRC);
#endif
}
#endif

#endif

// MESSAGE TASK_MANAGER UNPACKING


/**
 * @brief Get field task_name from task_manager message
 *
 * @return  
 */
static inline uint16_t mavlink_msg_task_manager_get_task_name(const mavlink_message_t* msg, char *task_name)
{
    return _MAV_RETURN_char_array(msg, task_name, 32,  12);
}

/**
 * @brief Get field prio from task_manager message
 *
 * @return  
 */
static inline uint8_t mavlink_msg_task_manager_get_prio(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  44);
}

/**
 * @brief Get field stk_used from task_manager message
 *
 * @return  
 */
static inline uint32_t mavlink_msg_task_manager_get_stk_used(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field stk_size from task_manager message
 *
 * @return  
 */
static inline uint32_t mavlink_msg_task_manager_get_stk_size(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  4);
}

/**
 * @brief Get field sw_cnt from task_manager message
 *
 * @return  
 */
static inline uint32_t mavlink_msg_task_manager_get_sw_cnt(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  8);
}

/**
 * @brief Decode a task_manager message into a struct
 *
 * @param msg The message to decode
 * @param task_manager C-struct to decode the message contents into
 */
static inline void mavlink_msg_task_manager_decode(const mavlink_message_t* msg, mavlink_task_manager_t* task_manager)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    task_manager->stk_used = mavlink_msg_task_manager_get_stk_used(msg);
    task_manager->stk_size = mavlink_msg_task_manager_get_stk_size(msg);
    task_manager->sw_cnt = mavlink_msg_task_manager_get_sw_cnt(msg);
    mavlink_msg_task_manager_get_task_name(msg, task_manager->task_name);
    task_manager->prio = mavlink_msg_task_manager_get_prio(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_TASK_MANAGER_LEN? msg->len : MAVLINK_MSG_ID_TASK_MANAGER_LEN;
        memset(task_manager, 0, MAVLINK_MSG_ID_TASK_MANAGER_LEN);
    memcpy(task_manager, _MAV_PAYLOAD(msg), len);
#endif
}
