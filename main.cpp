#include <iostream>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <fcntl.h>
#include <sys/stat.h>
#include "mq_monotonic.h"

#define SERVER_QUEUE_NAME "/mq-5"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

void client_thread()
{
    std::cout << "Thread START" << std::endl;

    /* Create and open a message queue for writing */
    mqd_t mqd = mq_open (SERVER_QUEUE_NAME, O_CREAT | O_EXCL | O_WRONLY,  0600, NULL);

    /* Ensure the creation was successful */
    if (mqd == -1)
    {
        perror ("mq_open");
        exit (1);
    }

    /* Send "HELLO" as a message with priority 10, then close the queue.
    Note the size is 6 to include the null byte '\0'. */
    while(1)
    {
        int recv = mq_send (mqd, "HELLO", 6, 10);
        std::cout << "mq_send recv: " << recv << std::endl;

        sleep(1);
    }
    
    mq_close (mqd);
    mq_unlink(SERVER_QUEUE_NAME);
}



int main(int, char**) {
    std::cout << "Hello, world!\n";

    mq_unlink(SERVER_QUEUE_NAME);

    std::thread t1(client_thread);
    
    sleep(1);

    /* Open the message queue for reading */
    mqd_t mqd = mq_open (SERVER_QUEUE_NAME, O_RDONLY);
    if (mqd == -1)
    {
        perror("mq_open error");
        exit(1);
    }

    /* Get the message queue attribut2es */
    struct mq_attr attr;
    //assert (mq_getattr (mqd, &attr) != -1);

    //char *buffer = calloc (attr.mq_msgsize, 1);
    //assert (buffer != NULL);
    char buffer[MAX_MSG_SIZE];



    /* Retrieve message from the queue and get its priority level */
    unsigned int priority = 0;
    while(1)
    {
        if ((mq_receive (mqd, buffer, attr.mq_msgsize, &priority)) == -1)
        printf ("Failed to receive message: %s\n",strerror(errno));
        else
        printf ("Received [priority %u]: '%s'\n", priority, buffer);

        sleep(1);
    }

    mq_close (mqd);

    mq_unlink(SERVER_QUEUE_NAME);

    t1.join();

    std::cout << "Closed!\n";
}