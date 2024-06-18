#include <asm-generic/errno-base.h>
#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <errno.h>

#include <stdio.h> 

#include "signal.h"

static void
interrupting_signal_handler(int signo)
{
  /* Its only job is to interrupt system calls--like read()--when
   * certain signals arrive--like Ctrl-C.
   */
}

static struct sigaction ignore_action = {.sa_handler = SIG_IGN},
                        interrupt_action = {.sa_handler =
                                                interrupting_signal_handler},
                        old_sigtstp, old_sigint, old_sigttou;

/* Ignore certain signals.
 * 
 * @returns 0 on succes, -1 on failure
 *
 *
 * The list of signals to ignore:
 *   - SIGTSTP
 *   - SIGINT
 *   - SIGTTOU
 *
 * Should be called immediately on entry to main() 
 *
 * Saves old signal dispositions for a later call to signal_restore()
 */
int
signal_init(void)
{
  /* TODO Initialize signals, store old actions 
   *
   * e.g. sigaction(SIGNUM, &new_handler, &saved_old_handler);
   *
   * */
  int STP = sigaction(SIGTSTP, &ignore_action, &old_sigtstp);
  int SINT = sigaction(SIGINT, &ignore_action, &old_sigint);
  int TTOU = sigaction(SIGTTOU, &ignore_action, &old_sigttou);

  if (STP + SINT + TTOU < 0){
    errno = EPERM; /* not implemented */
    //perror(errno);
    return -1;
  }else{
    return 0;}
}

/** enable signal to interrupt blocking syscalls (read/getline, etc) 
 *
 * @returns 0 on succes, -1 on failure
 *
 * does not save old signal disposition
 */
int
signal_enable_interrupt(int sig)
{
  /* TODO set the signal disposition for signal to interrupt  */
  int result = sigaction(sig, &interrupt_action, NULL);
  if (result < 0){
    errno = EPERM; /* not implemented */
    //perror(errno);
    return -1;
  }else{
    return 0;}
}

/** ignore a signal
 *
 * @returns 0 on success, -1 on failure
 *
 * does not save old signal disposition
 */
int
signal_ignore(int sig)
{
  /* TODO set the signal disposition for signal back to its old state */
  int result = sigaction(sig, &ignore_action, NULL);
  if (result < 0){
    errno = EPERM; /* not implemented */
    //perror(errno);
    return -1;
  }else{
    return 0;}

}

/** Restores signal dispositions to what they were when bigshell was invoked
 *
 * @returns 0 on success, -1 on failure
 *
 */
int
signal_restore(void)
{
  /* TODO restore old actions 
   *
   * e.g. sigaction(SIGNUM, &saved_old_handler, NULL);
   *
   * */
  
  int STP = sigaction(SIGTSTP, &old_sigtstp, NULL);
  int SINT = sigaction(SIGINT,  &old_sigint, NULL);
  int TTOU = sigaction(SIGTTOU, &old_sigttou, NULL);

  if (STP + SINT + TTOU < 0){
    errno = EPERM; /* not implemented */
    //perror(errno);
    return -1;
  }else{
    return 0;}

}