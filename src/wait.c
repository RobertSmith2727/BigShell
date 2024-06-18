#include <time.h>
#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include <signal.h>

#include "jobs.h"
#include "params.h"
#include "wait.h"

#include "util/gprintf.h"

int
wait_on_fg_gid(gid_t gid)
{
  if (gid < 0) return -1;
  /* Make sure the foreground group is running */
  /* TODO send the "continue" signal to the process group 'gid'
   * XXX review kill(2)
   */
  gprintf("killing %d", gid);
  int killSuccess = kill(gid, SIGCONT);

  if (killSuccess < 0) {
      return -1;
  }

  /* TODO make 'gid' the foreground process group
   * XXX review tcsetpgrp(3)
   */
  pid_t originalPgrp = tcgetpgrp(STDIN_FILENO);
  pid_t pid = tcsetpgrp(STDIN_FILENO, gid);
  gprintf("pid is %d", pid);


  int status = 0;
  for (;;) {
    /* Wait on ALL processes in the process group 'gid' 
     * TODO XXX TODO*/
    pid_t res = waitpid(gid, &status, 0);
    gprintf("res in wait.c is %d",res);
    if (res < 0) {
      if (errno == ECHILD) {
        /* No unwaited-for children. The job is done! */
        errno = 0;
        
        if (WIFEXITED(status)) {
          /* TODO set params.status to the correct value */
          params.status = WEXITSTATUS(status);
          gprintf("exit status %d and status %d", WEXITSTATUS(status), status);
          gprintf("exit param set to %d, in wait.c", params.status);

        } else if (WIFSIGNALED(status)) {
          /* TODO set params.status to the correct value */
          params.status = WEXITSTATUS(status);
          gprintf("signal param set to %d, in wait.c", params.status);

        }

        /* TODO remove the job for this group from the job list
         *  see jobs.h
         */

        jobs_remove_gid(gid);
        break;
        
      }
      return -1;
    }

    /* TODO handle case where a child process is stopped */
    if (WIFSTOPPED(status) != 0) {

      params.status = WEXITSTATUS(status); 
      fprintf(stderr, "[%jd] Stopped\n", (intmax_t)jobs_get_jid(gid));

      break;
    }

    /* A child exited, but others remain. Loop! */
  }
  /* TODO Make bigshell the foreground process group again
   * using tcsetpgrp */
  tcsetpgrp(STDIN_FILENO, originalPgrp);
  return 0;
}

/* XXX DO NOT MODIFY XXX */
int
wait_on_fg_job(jid_t jid)
{
  gid_t gid = jobs_get_gid(jid);
  if (gid < 0) return -1;
  return wait_on_fg_gid(gid);
}

int
wait_on_bg_jobs()
{
  size_t job_count = jobs_get_joblist_size();
  struct job const *jobs = jobs_get_joblist();
  for (size_t i = 0; i < job_count; ++i) {
    gid_t gid = jobs[i].gid;
    jid_t jid = jobs[i].jid;
    for (;;) {
      /* TODO: wait for process group
       *  (Nonblocking wait) 
       */
      int status = 0;
      pid_t pid = waitpid(gid,&status,WNOHANG);
      if (pid == 0) break;
      if (pid < 0) {
        if (errno == ECHILD) {
          errno = 0;
          if (WIFEXITED(status)) {
            fprintf(stderr, "[%jd] Done\n", (intmax_t)jid);

          } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "[%jd] Terminated\n", (intmax_t)jid);
          }
          jobs_remove_gid(gid);
          job_count = jobs_get_joblist_size();
          jobs = jobs_get_joblist();
          break;
        }

        if (WIFSTOPPED(status)) {
          fprintf(stderr, "[%jd] Stopped\n", (intmax_t)jid);
          break;
        }
      }
    }
  }
  return 0;
}
