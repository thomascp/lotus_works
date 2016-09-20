

#ifndef ERROR_H
#define ERROR_H



#define LOS_OK 0

#define TASK_ERROR_SHIFT 		24
#define TASK_NORESOURCE 		 -(1<<TASK_ERROR_SHIFT) 


#define MSG_ERROR_SHIFT			20
#define MSG_PARAM_INVALID   		-(1<<MSG_ERROR_SHIFT)
#define MSG_GIVEBUF_TOOSMALL		-(2<<MSG_ERROR_SHIFT)
#define MSG_BUF_FULL				-(3<<MSG_ERROR_SHIFT)

#endif


