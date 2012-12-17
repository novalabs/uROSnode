/*
 * app.h
 *
 *  Created on: 08/nov/2012
 *      Author: texzk
 */

#ifndef _APP_H_
#define _APP_H_

#include <urosBase.h>
#include "urosTcpRosHandlers.h"

#define SANDBOX_WIDTH           11
#define SANDBOX_HEIGHT          11

#define MAX_TURTLES             4
#define TURTLE_THREAD_STKSIZE   (PTHREAD_STACK_MIN << 1)
#define TURTLE_THREAD_PRIO      90

/**
 * @brief   String message queue.
 */
typedef struct queue_t {
  UrosSem       freeSem;
  UrosSem       usedSem;
  unsigned      length;
  unsigned      head;
  unsigned      tail;
  UrosMutex     slotsMtx;
  void          **slots;
} fifo_t;

/**
 * @brief   Turtle slot status.
 */
typedef enum turtlestatus_t {
  TURTLE_EMPTY,     /**< @brief Empty slot, a new turtle can be spawned here.*/
  TURTLE_ALIVE,     /**< @brief A living turtle, do not touch.*/
  TURTLE_DEAD       /**< @brief A dead turtle, wait until its related threads are done.*/
} turtlestatus_t;

/**
 * @brief   Turtle descriptor and status.
 */
typedef struct turtle_t {
  UrosMutex                     lock;           /**< @brief Guard lock.*/
  unsigned                      id;             /**< @brief Turtle ID.*/
  UrosString                    name;           /**< @brief Turtle name.*/
  UrosString                    poseTopic;      /**< @brief <tt><i>turtle</i>/pose</tt> topic name.*/
  UrosString                    velTopic;       /**< @brief <tt><i>turtle</i>/command_velocity</tt> topic name.*/
  UrosString                    setpenService;  /**< @brief <tt><i>turtle</i>/set_pen</tt> service name.*/
  UrosString                    telabsService;  /**< @brief <tt><i>turtle</i>/teleport_absolute</tt> service name.*/
  UrosString                    telrelService;  /**< @brief <tt><i>turtle</i>/teleport_relative</tt> service name.*/
  struct msg__turtlesim__Pose   pose;           /**< @brief Current turtle pose.*/
  uros_cnt_t                    countdown;      /**< @brief Command countdown, in milliseconds.*/
  turtlestatus_t                status;         /**< @brief Turtle slot status.*/
  uros_cnt_t                    refCnt;         /**< @brief Reference counter.*/
  struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t width;
    uint8_t off;
  }                             pen;            /**< @brief Pen configuration.*/
} turtle_t;

extern fifo_t rosoutQueue;

extern turtle_t turtles[MAX_TURTLES];
extern UrosThreadPool turtlesThreadPool;

void fifo_init(fifo_t *queuep, unsigned length);
void fifo_enqueue(fifo_t *queuep, void *msgp);
void *fifo_dequeue(fifo_t *queuep);

void rosout_post(UrosString *strp, uros_bool_t constant, uint8_t level,
                 const char *fileszp, int line, const char *funcp);
void rosout_fetch(struct msg__rosgraph_msgs__Log **msgpp);

#define rosout_debug(strp, constant) \
  rosout_post((strp), (constant), msg__rosgraph_msgs__Log__DEBUG, \
              __FILE__, __LINE__, __PRETTY_FUNCTION__);

#define rosout_info(strp, constant) \
  rosout_post((strp), (constant), msg__rosgraph_msgs__Log__INFO, \
              __FILE__, __LINE__, __PRETTY_FUNCTION__);

#define rosout_warn(strp, constant) \
  rosout_post((strp), (constant), msg__rosgraph_msgs__Log__WARN, \
              __FILE__, __LINE__, __PRETTY_FUNCTION__);

#define rosout_error(strp, constant) \
  rosout_post((strp), (constant), msg__rosgraph_msgs__Log__ERROR, \
              __FILE__, __LINE__, __PRETTY_FUNCTION__);

#define rosout_fatal(strp, constant) \
  rosout_post((strp), (constant), msg__rosgraph_msgs__Log__FATAL, \
              __FILE__, __LINE__, __PRETTY_FUNCTION__);

uros_err_t app_subscribe_topic(const char *namep,
                               const char *typep,
                               uros_proc_f procf);
uros_err_t app_publish_topic(const char *namep,
                             const char *typep,
                             uros_proc_f procf);
void app_register_types(void);
void app_register_published_topics(void);
void app_register_subscribed_topics(void);
void app_register_subscribed_params(void);
void app_register_services(void);
void app_initialize(void);
void app_background_service(void);

void turtle_init_pools(void);
void turtle_init(turtle_t *turtlep, unsigned id);
uros_err_t turtle_brain_thread(turtle_t *turtlep);
turtle_t *turtle_spawn(const UrosString *namep,
                       float x, float y, float theta);
void turtle_kill(turtle_t *turtlep);
void turtle_unref(turtle_t *turtlep);

#endif /* _APP_H_ */
