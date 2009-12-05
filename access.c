#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>


#include "access.h"

static void print(struct dquot *dquot, char *name);
static char overlim(uint usage, uint softlim, uint hardlim);
static int output(struct dquot *dquot, char *name);

/* flags must be configured before calling create_handle_list() */
int fmt = -1;
struct dquot dquot_cache[MAX_CACHE_DQUOTS];
int cached_dquots;
int flags;

int getquota(void) {
  /* flags: for all users, not cached */
  flags = FL_ALL | FL_USER | FL_NOCACHE;
  int type = USRQUOTA;
  struct quota_handle **handles;
  char name[MAXNAMELEN];
  int i, j;
  struct passwd *pwent; /* For /etc/passwd entry */

  /* main */
  init_kernel_interface();
  
  handles = create_handle_list(0, NULL, type, fmt,
                               IOI_READONLY | IOI_OPENFILE, MS_LOCALONLY
                               | (flags & FL_NOAUTOFS ? MS_NO_AUTOFS : 0)
                               );
  
  for (i = 0; handles[i]; i++) {
    struct quota_handle* h;
    h->qh_ops->scan_dquots(h, output);
    setpwent();
    while ((pwent = getpwent())) {
      for (j = 0; j < cached_dquots && pwent->pw_uid != dquot_cache[j].dq_id; j++);
      if (j < cached_dquots && !(dquot_cache[j].dq_flags & DQ_PRINTED)) {
        print(dquot_cache+i, pwent->pw_name);
        dquot_cache[j].dq_flags |= DQ_PRINTED;
      }
    }
    endpwent();
  }

  return 0;
}

/* Print one quota entry */
static void print(struct dquot *dquot, char *name)
{
  char pname[MAXNAMELEN];
  char time[MAXTIMELEN];
  char numbuf[3][MAXNUMLEN];
	
  struct util_dqblk *entry = &dquot->dq_dqb;

  if (!entry->dqb_curspace && !entry->dqb_curinodes && !(flags & FL_VERBOSE))
    return;
  sstrncpy(pname, name, sizeof(pname));
  if (flags & FL_TRUNCNAMES)
    pname[PRINTNAMELEN] = 0;
  if (entry->dqb_bsoftlimit && toqb(entry->dqb_curspace) >= entry->dqb_bsoftlimit)
    if (flags & FL_RAWGRACE)
      sprintf(time, "%Lu", (unsigned long long)entry->dqb_btime);
    else
      difftime2str(entry->dqb_btime, time);
  else
    if (flags & FL_RAWGRACE)
      strcpy(time, "0");
    else
      time[0] = 0;
  space2str(toqb(entry->dqb_curspace), numbuf[0], flags & FL_SHORTNUMS);
  space2str(entry->dqb_bsoftlimit, numbuf[1], flags & FL_SHORTNUMS);
  space2str(entry->dqb_bhardlimit, numbuf[2], flags & FL_SHORTNUMS);
  printf("%-*s %c%c %7s %7s %7s %6s", PRINTNAMELEN, pname,
         overlim(qb2kb(toqb(entry->dqb_curspace)), qb2kb(entry->dqb_bsoftlimit), qb2kb(entry->dqb_bhardlimit)),
         overlim(entry->dqb_curinodes, entry->dqb_isoftlimit, entry->dqb_ihardlimit),
         numbuf[0], numbuf[1], numbuf[2], time);
  if (entry->dqb_isoftlimit && entry->dqb_curinodes >= entry->dqb_isoftlimit)
    if (flags & FL_RAWGRACE)
      sprintf(time, "%Lu", (unsigned long long)entry->dqb_itime);
    else
      difftime2str(entry->dqb_itime, time);
  else
    if (flags & FL_RAWGRACE)
      strcpy(time, "0");
    else
      time[0] = 0;
  number2str(entry->dqb_curinodes, numbuf[0], flags & FL_SHORTNUMS);
  number2str(entry->dqb_isoftlimit, numbuf[1], flags & FL_SHORTNUMS);
  number2str(entry->dqb_ihardlimit, numbuf[2], flags & FL_SHORTNUMS);
  printf(" %7s %5s %5s %6s\n", numbuf[0], numbuf[1], numbuf[2], time);
}

/* Are we over soft or hard limit? */
static char overlim(uint usage, uint softlim, uint hardlim)
{
  if ((usage > softlim && softlim) || (usage > hardlim && hardlim))
    return '+';
  return '-';
}

/* Callback routine called by scan_dquots on each dquot */
static int output(struct dquot *dquot, char *name)
{
  char namebuf[MAXNAMELEN];
  
  if (!name) {
    id2name(dquot->dq_id, dquot->dq_h->qh_type, namebuf);
    name = namebuf;
  }
  print(dquot, name);
  return 0;
}
