#include <mqueue.h>
#include <cerrno>
#include <poll.h>

static bool timeCompare(const struct timespec& time_timeout, const struct timespec& time_current)
{
  if (time_timeout.tv_sec == time_current.tv_sec)
      return time_timeout.tv_nsec < time_current.tv_nsec;
  else
      return time_timeout.tv_sec < time_current.tv_sec;
}

static ssize_t mq_timedreceive_monotonic(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio, const struct timespec * abs_timeout)
{
  ssize_t ret = -1;
  errno = ETIMEDOUT;

  if (abs_timeout->tv_sec < 0)
  {
    errno = EINVAL;
    return ret;
  }

  struct timespec tout;
  tout.tv_nsec = 0;
  tout.tv_sec = 0;

  ret = mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, &tout);

  if (ret < 0) /* FAIL case */
  {
    struct timespec time_current= {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &time_current);

    if (timeCompare(*abs_timeout, time_current))
    {
      errno = ETIMEDOUT;
      return ret;
    }
    else
    {
      int deltaTimeMs = (abs_timeout->tv_sec - time_current.tv_sec) * 1000 + (abs_timeout->tv_nsec - time_current.tv_nsec) / 1000000;
      struct pollfd fdset[1] = {0};
      short int pool_res = 0;
      int rc = -1;
      fdset[0].fd = (int) mqdes;
      fdset[0].events = POLLIN;

      rc = poll(fdset, 1, deltaTimeMs);
      pool_res = fdset[0].revents;

      if (rc > 0)
      {
        errno = 0; /* reset errno */
        /* If poll exit receiving an event of POLLIN, the queue can be read */
        ret = mq_timedreceive(mqdes, msg_ptr, msg_len, msg_prio, &tout);
      }
      else if(rc == 0) /* (rc == 0) -> TIMEOUT */
      {
        errno = ETIMEDOUT;
        ret = -1;
      }
      else /* (rc < 0) -> ERROR */
      {
        ret = -1;
      }
    }
  }
  else /* SUCCES case */
  {
    errno = 0;
  }

  return ret;
}

static int mq_timedsend_monotonic(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abs_timeout)
{
  int ret = -1;
  errno = ETIMEDOUT;

  if (abs_timeout->tv_sec < 0)
  {
    errno = EINVAL;
    return ret;
  }

  struct timespec tout;
  tout.tv_nsec = 0;
  tout.tv_sec = 0;

  ret = mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, &tout);

  if (ret < 0) /* FAIL case */
  {
    struct timespec time_current= {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &time_current);

    if (timeCompare(*abs_timeout, time_current))
    {
      errno = ETIMEDOUT;
      return ret;
    }
    else
    {
      int deltaTimeMs = (abs_timeout->tv_sec - time_current.tv_sec) * 1000 + (abs_timeout->tv_nsec - time_current.tv_nsec) / 1000000;
      struct pollfd fdset[1] = {0};
      short int pool_res = 0;
      int rc = -1;
      fdset[0].fd = (int) mqdes;
      fdset[0].events = POLLOUT;

      rc = poll(fdset, 1, deltaTimeMs);
      pool_res = fdset[0].revents;

      if (rc > 0)
      {
        errno = 0; /* reset errno */
        /* If poll exit receiving an event of POLLOUT, the queue can be writen */
        ret = mq_timedsend(mqdes, msg_ptr, msg_len, msg_prio, &tout);
      }
      else if(rc == 0) /* (rc == 0) -> TIMEOUT */
      {
        errno = ETIMEDOUT;
        ret = -1;
      }
      else /* (rc < 0) -> ERROR */
      {
        ret = -1;
      }
    }
  }
  else /* SUCCES case */
  {
    errno = 0;
  }

  return ret;
}

