#ifndef __KEY_BUTTON_H
#define __KEY_BUTTON_H
#include "stdint.h"

typedef enum {
	PRESS_DOWN = 0,
	PRESS_UP,
	PRESS_REPEAT,
	SINGLE_CLICK,
	DOUBLE_CLICK,
	LONG_PRESS_START,
	LONG_PRESS_HOLD,
	NUMBER_OF_EVENT,
	NONE_PRESS
}PressEvent;


typedef void (*BtnCallback)(void*);
typedef uint8_t (*BtnReadPinFun)(uint8_t);  

typedef struct Button {
	uint16_t ticks;
	uint8_t  repeat : 4;
	uint8_t  event : 4;
	uint8_t  state : 3;
	uint8_t  debounce_cnt : 3;
	uint8_t  active_level : 1;
	uint8_t  button_level : 1;
	uint8_t  button_id;
	BtnReadPinFun  readButtonFun;
	BtnCallback  cb[NUMBER_OF_EVENT];
	struct Button* next;
}Button;


void button_attach(struct Button* handle, PressEvent event, BtnCallback cb);
void button_object_init(struct Button* handle, uint8_t(*pin_level_fun)(uint8_t), uint8_t active_level, uint8_t button_id);
int key_button_register(struct Button *handle);
void button_process_all(void);
#endif

