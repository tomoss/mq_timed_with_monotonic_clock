#include <mqueue.h>
#include <cerrno>

static ssize_t mq_timedreceive_monotonic(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio, const struct timespec *abs_timeout)
{
    ssize_t ret = -1;
    errno = ETIMEDOUT;

    return ret;
}

static int mq_timedsend_monotonic(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio, const struct timespec *abs_timeout)
{
    int ret = -1;
    errno = ETIMEDOUT;

    return ret;
}
