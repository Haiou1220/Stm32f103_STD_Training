#include "key_button.h"
#include "stddef.h"
#include "string.h"

//temp
uint8_t g_keyBtn_state;
uint8_t g_keyBtn_level;
uint8_t g_keyBtn_event;
//temp

#define TICKS_INTERVAL    10	//10 ms 适配任务调度周期
#define DEBOUNCE_TICKS    2	//MAX 7 (0 ~ 7) //3	//MAX 7 (0 ~ 7)
#define SHORT_TICKS       (300 /TICKS_INTERVAL) //  (300 /TICKS_INTERVAL)
#define LONG_TICKS        (1000 /TICKS_INTERVAL)
#define PRESS_REPEAT_MAX_NUM  15 /*!< The maximum value of the repeat counter */

//button handle list head.
static struct Button* sg_head_handle = NULL;
static void button_process(struct Button* handle);

#define EVENT_CB(_handle,_ev)    \
do  \
{   \
    if (_handle->cb[_ev]) \
        _handle->cb[_ev]((void *)_handle); \
} while (0)

/**
 * @brief  Register a user button, add the handle into work list.
 *
 * @param  handle: target handle struct.
 * @retval 0: succeed. -1: already exist.
 */
int key_button_register(struct Button *handle)
{
    struct Button *target = sg_head_handle;
    while (target)
    {
        if (target == handle)
            return -1; // already exist.
        target = target->next;
    }
    /**
     * First registered button is at the end of the 'linked list'.
     * btn_head points to the head of the 'linked list'.
    */
    handle->next = sg_head_handle;
    sg_head_handle = handle;
    return 0;
}




/**
  * @brief  Initializes the button struct handle obj.
  * @param  handle: the button handle struct.
  * @param  pin_level_fun: read the HAL GPIO of the connected button level.
  * @param  active_level: pressed GPIO level.
  * @param  button_id: the button id.
  * @retval None
  */
void button_object_init(struct Button* handle, uint8_t(*pin_level_fun)(uint8_t), uint8_t active_level, uint8_t button_id)
{
	memset(handle, 0, sizeof(struct Button));
	handle->event = (uint8_t)NONE_PRESS;
	handle->readButtonFun = pin_level_fun;
	handle->button_level = !active_level;
	handle->active_level = active_level;
	handle->button_id = button_id;
}


/**
  * @brief  Attach the button event callback function.
  * @param  handle: the button handle struct.
  * @param  event: trigger event type.
  * @param  cb: callback function.
  * @retval None
  */
void button_attach(struct Button* handle, PressEvent event, BtnCallback cb)
{
	handle->cb[event] = cb;
}


/**
  * @brief  process all button from button handle list. 
  *          Need to be called cyclically within the specified period.
  *          Sample cycle: 5 - 20ms
  * @param  void 
  * @retval None
  */
void button_process_all(void)
{
    /*
    按键队列
    head_btn_handle => old_btn_handle => NULL 
    */
	struct Button* target;
	for(target= sg_head_handle; target; target=target->next) {
		button_process(target); 
	}    
}


/**
  * @brief  Button driver core function, driver state machine.
  * @param  handle: the button handle struct.
  * @retval None
  */
static void button_process(struct Button* handle)
{
	uint8_t read_gpio_level;

    if(handle == NULL)
    {
        return;
    }
    /* 	read button level */ 
    if(handle->readButtonFun)
    {
        read_gpio_level = handle->readButtonFun(handle->button_id);
    }
                    
	//ticks counter working..
	if((handle->state) > 0) handle->ticks++;

//优化程序,短时间按下松开可以捕获到,中断方法
	/*------------button debounce handle---------------*/
	if(read_gpio_level != handle->button_level) { //not equal to prev one
		//continue read 3 times same new level change
		if(++(handle->debounce_cnt) >= DEBOUNCE_TICKS) {
			handle->button_level = read_gpio_level;
			handle->debounce_cnt = 0;
		}
	} else { //level not change ,counter reset.
		handle->debounce_cnt = 0;
	}
                    
g_keyBtn_level = handle->button_level;//temp
g_keyBtn_state = handle->state; //temp
g_keyBtn_event = handle->event; //temp

	/*-----------------State machine-------------------*/
	switch (handle->state) {
	case 0:
		if(handle->button_level == handle->active_level) {	//start press down
			handle->event = (uint8_t)PRESS_DOWN;
			EVENT_CB(handle,PRESS_DOWN);
			handle->ticks = 0;
			handle->repeat = 1;
			handle->state = 1;
		} else {
			handle->event = (uint8_t)NONE_PRESS;
		}
		break;

	case 1:
		if(handle->button_level != handle->active_level) { //released press up
			handle->event = (uint8_t)PRESS_UP;
			EVENT_CB(handle,PRESS_UP);
			handle->ticks = 0;
			handle->state = 2;
		} else if(handle->ticks > LONG_TICKS) {
			handle->event = (uint8_t)LONG_PRESS_START;
			EVENT_CB(handle,LONG_PRESS_START);
			handle->state = 5;
		}
		break;

	case 2:
		if(handle->button_level == handle->active_level) { //press down again,SHORT_TICKS时间内再次按下
			handle->event = (uint8_t)PRESS_DOWN;
			EVENT_CB(handle,PRESS_DOWN);
			if(handle->repeat != PRESS_REPEAT_MAX_NUM) {
				handle->repeat++;
			}
			EVENT_CB(handle,PRESS_REPEAT); // repeat hit
			handle->ticks = 0;
			handle->state = 3;
		} else if(handle->ticks > SHORT_TICKS) { //released timeout,双击 单击 判断超时,可以进行判断
			if(handle->repeat == 1) {
				handle->event = (uint8_t)SINGLE_CLICK;
				EVENT_CB(handle,SINGLE_CLICK);
			} else if((handle->repeat >= 2)) {
				handle->event = (uint8_t)DOUBLE_CLICK;
				EVENT_CB(handle,DOUBLE_CLICK); // repeat hit
			}
			handle->state = 0;
		}
		break;

	case 3:
		if(handle->button_level != handle->active_level) { //released press up
			handle->event = (uint8_t)PRESS_UP;
			EVENT_CB(handle,PRESS_UP);
			if(handle->ticks < SHORT_TICKS) {
				handle->ticks = 0;
				handle->state = 2; //repeat press
			} else {
				handle->state = 0;
			}
		} else if(handle->ticks > SHORT_TICKS) { // SHORT_TICKS < press down hold time < LONG_TICKS
			handle->state = 1;
		}
		break;

	case 5:
		if(handle->button_level == handle->active_level) {
			//continue hold trigger
			handle->event = (uint8_t)LONG_PRESS_HOLD;
			EVENT_CB(handle,LONG_PRESS_HOLD);
		} else { //released
			handle->event = (uint8_t)PRESS_UP;
			EVENT_CB(handle,PRESS_UP);
			handle->state = 0; //reset
		}
		break;
	default:
		handle->state = 0; //reset
		break;
	}
}

