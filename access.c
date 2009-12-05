#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>


#include "access.h"

static void print(struct dquot *dquot, char *name);
static int output(struct dquot *dquot, char *name);

/* flags must be configured before calling create_handle_list() */
int fmt = -1;
struct dquot dquot_cache[MAX_CACHE_DQUOTS];
int cached_dquots;
int flags = FL_ALL | FL_USER; /* flags: for all users */

int getquota(void) {
  int type = 0;
  struct quota_handle **handles;
  int i;

  /* main */
  init_kernel_interface();
  
  handles = create_handle_list(0, NULL, type, fmt,
                               IOI_READONLY | IOI_OPENFILE, MS_LOCALONLY
                               | (flags & FL_NOAUTOFS ? MS_NO_AUTOFS : 0)
                               );
  
  for (i = 0; handles[i]; i++) {
    struct quota_handle* h = handles[i];
    h->qh_ops->scan_dquots(h, output);
  }

  return 0;
}

/* Print one quota entry */
static void print(struct dquot *dquot, char *name)
{
  char pname[MAXNAMELEN];
  char numbuf[MAXNUMLEN];
	
  struct util_dqblk *entry = &dquot->dq_dqb;

  if (!entry->dqb_curspace && !entry->dqb_curinodes && !(flags & FL_VERBOSE))
    return;
  sstrncpy(pname, name, sizeof(pname));
  space2str(toqb(entry->dqb_curspace), numbuf, flags & FL_SHORTNUMS);
  printf("%-*s %7s\n", PRINTNAMELEN, pname, numbuf);
}

/* Callback routine called by scan_dquots on each dquot */
static int output(struct dquot *dquot, char *name)
{
  char namebuf[MAXNAMELEN];
  
  id2name(dquot->dq_id, dquot->dq_h->qh_type, namebuf);
  name = namebuf;
  print(dquot, name);
  return 0;
}
