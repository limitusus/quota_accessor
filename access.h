#ifndef __INCLUDE_ACCESS_H
#define __INCLUDE_ACCESS_H

#include <quotaio.h>
#include <quotasys.h>
#include <quota.h>
#include <common.h>
#include <quotaops.h>

/* Flag values defined in repquota.c */

#define FL_USER 1
#define FL_GROUP 2
#define FL_VERBOSE 4
#define FL_ALL 8		/* Dump quota files on all filesystems */
#define FL_TRUNCNAMES 16	/* Truncate names to fit into the screen */
#define FL_SHORTNUMS 32	/* Try to print space in appropriate units */
#define FL_NONAME 64	/* Don't translate ids to names */
#define FL_NOCACHE 128	/* Don't cache dquots before resolving */
#define FL_NOAUTOFS 256	/* Ignore autofs mountpoints */
#define FL_RAWGRACE 512	/* Print grace times in seconds since epoch */

#define PRINTNAMELEN 9	/* Number of characters to be reserved for name on screen */
#define MAX_CACHE_DQUOTS 1024	/* Number of dquots in cache */

int getquota(void);

#endif /* __INCLUDE_ACCESS_H */

