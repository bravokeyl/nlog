#ifndef _SENDDATA_H_
#define _SENDDATA_H_

#ifndef SENDDATA_THREAD_STACK_SIZE
#define SENDDATA_THREAD_STACK_SIZE   1024
#endif

#ifndef SENDDATA_THREAD_PRIORITY
#define SENDDATA_THREAD_PRIORITY     (LOWPRIO + 2)
#endif


#ifndef SOCK_SERVER_HOST
#define SOCK_SERVER_HOST  "54.147.229.8"
#endif

#ifndef SOCK_SERVER_PORT
#define SOCK_SERVER_PORT  8888
#endif

#define SENDDATA_BUF_NUM	2
#define SENDDATA_BUF_SIZE	2048

extern WORKING_AREA(wa_sendata_client, SENDDATA_THREAD_STACK_SIZE);
extern Semaphore sendingbufsem;

#ifdef __cplusplus
extern "C" {
#endif
msg_t senddata_client(void *p);
void senddata(int size, char *data);
#ifdef __cplusplus
}
#endif

#endif /* _WEB_H_ */

/** @} */
