/*
 *  This is the body of the test. It does not do much except ensure
 *  that the target is alive after initializing the TCP/IP stack.
 */

#include <stdlib.h>
#include <stdio.h>

#include <rtems/bsd/sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TEST_NAME "LIBBSD LOOPBACK 1"

#include <rtems/error.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sysexits.h>

#include <machine/rtems-bsd-commands.h>

#include <rtems.h>

static rtems_id masterTask;

/*
 * Thread-safe output routines
 */
static rtems_id printMutex;
static void printSafe(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    rtems_semaphore_obtain(printMutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    vprintf(fmt, args);
    rtems_semaphore_release(printMutex);
    va_end(args);
}
#define printf printSafe

static void
setSelfPrio(rtems_task_priority prio)
{
	rtems_status_code sc;

	sc = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
	assert(sc == RTEMS_SUCCESSFUL);
}

static rtems_event_set argToClientEvent(rtems_task_argument arg)
{
    return 1U << arg;
}

static void
sendClientEventToMasterTask(rtems_task_argument arg)
{
    rtems_status_code sc;

    sc = rtems_event_send(masterTask, argToClientEvent(arg));
    assert(sc == RTEMS_SUCCESSFUL);
}

static void
waitForClientEvents(rtems_event_set which)
{
    rtems_status_code sc;
    rtems_event_set events;

    sc = rtems_event_receive(
        which,
        RTEMS_EVENT_ALL | RTEMS_WAIT,
        RTEMS_NO_TIMEOUT,
        &events
    );
    assert(sc == RTEMS_SUCCESSFUL);
}

/*
 * Spawn a task
 */
static void spawnTask(rtems_task_entry entryPoint, rtems_task_priority priority, rtems_task_argument arg)
{
    rtems_status_code sc;
    rtems_id tid;

    sc = rtems_task_create(rtems_build_name('t','a','s','k'),
            priority,
            RTEMS_MINIMUM_STACK_SIZE+(8*1024),
            RTEMS_PREEMPT|RTEMS_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
            RTEMS_FLOATING_POINT|RTEMS_LOCAL,
            &tid);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_panic("Can't create task: %s", rtems_status_text(sc));
    sc = rtems_task_start(tid, entryPoint, arg);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_panic("Can't start task: %s", rtems_status_text(sc));
}

/*
 * Server subtask
 */
static rtems_task workerTask(rtems_task_argument arg)
{
    int s = arg;
    char msg[80];
    char reply[100];
    int i;

    for (;;) {
        if ((i = read(s, msg, sizeof msg)) < 0) {
            printf("Server couldn't read message from client: %s\n", strerror(errno));
            break;
        }
        if (i == 0)
            break;
        i = sprintf(reply, "Server received %d (%s)", i, msg);
        if ((i = write(s, reply, i+1)) < 0) {
            printf("Server couldn't write message to client: %s\n", strerror(errno));
            break;
        }
    }
    if (close(s) < 0)
        printf("Can't close worker task socket: %s\n", strerror(errno));
    printf("Worker task terminating.\n");
    rtems_task_delete(RTEMS_SELF);
}

/*
 * Server Task
 */
static rtems_task serverTask(rtems_task_argument arg)
{
    int s, s1;
    socklen_t addrlen;
  struct sockaddr_in myAddr, farAddr;
    rtems_task_priority myPriority;

  printf("Create socket.\n");
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0)
    rtems_panic("Can't create socket: %s\n", strerror(errno));
    memset(&myAddr, 0, sizeof myAddr);
  myAddr.sin_family = AF_INET;
  myAddr.sin_port = htons(1234);
  myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  printf("Bind socket.\n");
  if (bind(s, (struct sockaddr *)&myAddr, sizeof myAddr) < 0)
    rtems_panic("Can't bind socket: %s\n", strerror(errno));
  if (listen(s, 5) < 0)
    printf("Can't listen on socket: %s\n", strerror(errno));
    rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &myPriority);
    for(;;) {
        addrlen = sizeof farAddr;
        s1 = accept(s, (struct sockaddr *)&farAddr, &addrlen);
        if (s1 < 0)
            rtems_panic("Can't accept connection: %s", strerror(errno));
        else
            printf("ACCEPTED:%lX\n", ntohl(farAddr.sin_addr.s_addr));
        spawnTask(workerTask, myPriority, s1);
    }
}

/*
 * The real part of the client
 */
static rtems_task clientWorker(int arg)
{
  int s;
  struct sockaddr_in myAddr, farAddr;
  char cbuf[50];
  int i;

  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    printf("Can't create client socket: %s\n", strerror(errno));
    return;
  }
  memset(&myAddr, 0, sizeof myAddr);
  myAddr.sin_family = AF_INET;
  myAddr.sin_port = htons(0);
  myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(s, (struct sockaddr *)&myAddr, sizeof myAddr) < 0) {
    printf("Can't bind socket: %s\n", strerror(errno));
    goto close;
  }

  memset(&farAddr, 0, sizeof farAddr);
  farAddr.sin_family = AF_INET;
  farAddr.sin_port = htons(1234);
  farAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  printf("Connect to server.\n");
  if (connect(s, (struct sockaddr *)&farAddr, sizeof farAddr) < 0) {
    printf("Can't connect to server: %s\n", strerror(errno));
        goto close;
    }
    i = sprintf(cbuf, "Hi there, server (%d).", arg);
    i++;    /* Send the '\0', too */
    printf("Write %d-byte message to server.\n", i);
    if (write(s, cbuf, i) < 0) {
      printf("Can't write to server: %s\n", strerror(errno));
      goto close;
    }
    if ((i = read(s, cbuf, sizeof cbuf)) < 0) {
      printf("Can't read from server: %s\n", strerror(errno));
      goto close;
    }
    printf("Read %d from server: %.*s\n", i, i, cbuf);
  close:
    printf("Client closing connection.\n");
    if (close(s) < 0)
        printf("Can't close client task socket: %s\n", strerror(errno));
}

/*
 * Client Task
 */
static rtems_task clientTask(rtems_task_argument arg)
{
    clientWorker(arg);
    sendClientEventToMasterTask(arg);
    printf("Client task terminating.\n");
    rtems_task_delete( RTEMS_SELF );
}

/*
 * RTEMS Startup Task
 */
static void test_main(void)
{
  rtems_status_code    sc;
  rtems_task_priority  old;
  int                  exit_code;
  char *lo0[] = {
    "ifconfig",
    "lo0",
    "inet",
    "127.0.0.1",
    "netmask",
    "255.255.255.0",
    NULL
  };

  masterTask = rtems_task_self();

  setSelfPrio(RTEMS_MAXIMUM_PRIORITY - 1);

  sc = rtems_semaphore_create(
    rtems_build_name('P','m','t','x'),
    1,
    RTEMS_PRIORITY|RTEMS_BINARY_SEMAPHORE|RTEMS_INHERIT_PRIORITY|
      RTEMS_NO_PRIORITY_CEILING|RTEMS_LOCAL,
    0,
    &printMutex
  );
  if (sc != RTEMS_SUCCESSFUL)
    rtems_panic("Can't create printf mutex:", rtems_status_text(sc));

  sc = rtems_task_set_priority( RTEMS_SELF, 50, &old );
  if (sc != RTEMS_SUCCESSFUL)
    rtems_panic("Can't create printf mutex:", rtems_status_text(sc));

  /*
   *  Network is initialized when we get her
   */

  printf("Try running client with no server present.\n");
  printf("Should fail with `connection refused'.\n");
  clientWorker(0);

  exit_code = rtems_bsd_command_ifconfig(nitems(lo0) - 1, lo0);
  assert(exit_code == EX_OK);

  printf("\nStart server.\n");
  spawnTask(serverTask, 110, 0);

  printf("\nTry running client with server present.\n");
  spawnTask(clientTask, 120, 1);
  waitForClientEvents(argToClientEvent(1));

  printf("\nTry running two clients.\n");
  spawnTask(clientTask, 120, 2);
  spawnTask(clientTask, 120, 3);
  waitForClientEvents(argToClientEvent(2) | argToClientEvent(3));

  printf("\nTry running three clients.\n");
  spawnTask(clientTask, 120, 4);
  spawnTask(clientTask, 120, 5);
  spawnTask(clientTask, 120, 6);
  waitForClientEvents(argToClientEvent(4) | argToClientEvent(5) | argToClientEvent(6));

  exit( 0 );
}

#include <rtems/bsd/test/default-init.h>
