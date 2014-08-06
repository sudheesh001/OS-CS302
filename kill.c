#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern  char *__progname;

void nosig(char *);
void printsignals(FILE *);
int signame_to_signum(char *);
void usage(void);

int
main(int argc, char *argv[])
{
  int errors, numsig, pid;
  char *ep;

  if (argc < 2)
    usage();

  numsig = SIGTERM;

  argc--, argv++;
  if (!strcmp(*argv, "-l")) {
    argc--, argv++;
    if (argc > 0 && !strcmp(*argv, "--"))
      argc--, argv++;
    if (argc > 1)
      usage();
    if (argc == 1) {
      if (!isdigit((unsigned char)**argv))
        usage();
      numsig = strtol(*argv, &ep, 10);
      if (*ep)
        errx(1, "illegal signal number: %s", *argv);
      if (numsig >= 128)
        numsig -= 128;
      if (numsig <= 0 || numsig >= NSIG)
        nosig(*argv);
      printf("%s\n", sys_signame[numsig]);
      exit(0);
    }
    printsignals(stdout);
    exit(0);
  }

  if (!strcmp(*argv, "-s")) {
    argc--, argv++;
    if (argc > 0 && !strcmp(*argv, "--"))
      argc--, argv++;
    if (argc < 1) {
      warnx("option requires an argument -- s");
      usage();
    }
    if (strcmp(*argv, "0")) {
      if ((numsig = signame_to_signum(*argv)) < 0)
        nosig(*argv);
    } else
      numsig = 0;
    argc--, argv++;
  } else if (**argv == '-') {
    if (strcmp(*argv, "--")) {
      ++*argv;
      if (isalpha((unsigned char)**argv)) {
        if ((numsig = signame_to_signum(*argv)) < 0)
          nosig(*argv);
      } else if (isdigit((unsigned char)**argv)) {
        numsig = strtol(*argv, &ep, 10);
        if (*ep)
          errx(1, "illegal signal number: %s", *argv);
        if (numsig < 0 || numsig >= NSIG)
          nosig(*argv);
      } else
        nosig(*argv);
    }
    argc--, argv++;
  }

  if (argc == 0)
    usage();

  for (errors = 0; argc; argc--, argv++) {
    pid = strtol(*argv, &ep, 10);
    if (!**argv || *ep) {
      warnx("illegal process id: %s", *argv);
      errors = 1;
    } else if (kill(pid, numsig) == -1) {
      warn("%s", *argv);
      errors = 1;
    }
  }

  exit(errors);
}

int
signame_to_signum(char *sig)
{
  int n;

  if (!strncasecmp(sig, "sig", 3))
    sig += 3;
  for (n = 1; n < NSIG; n++) {
    if (!strcasecmp(sys_signame[n], sig))
      return (n);
  }
  return (-1);
}

void
nosig(char *name)
{

  warnx("unknown signal %s; valid signals:", name);
  printsignals(stderr);
  exit(1);
}

void
printsignals(FILE *fp)
{
  int n;

  for (n = 1; n < NSIG; n++) {
    (void)fprintf(fp, "%s", sys_signame[n]);
    if (n == (NSIG / 2) || n == (NSIG - 1))
      (void)fprintf(fp, "\n");
    else
      (void)fprintf(fp, " ");
  }
}

void
usage(void)
{
  (void)fprintf(stderr, "usage: %s [-s signal_name] pid ...\n",
      __progname);
  (void)fprintf(stderr, "       %s -l [exit_status]\n", __progname);
  (void)fprintf(stderr, "       %s -signal_name pid ...\n",
      __progname);
  (void)fprintf(stderr, "       %s -signal_number pid ...\n",
      __progname);
  exit(1);
}
