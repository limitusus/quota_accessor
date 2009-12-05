# export QUOTADIR=/data/1/kabe/quota/quota-3.16

OBJS = access.o main.o

CFLAGS = -O3 -g -Wall -I$(QUOTADIR)
LCFLAGS = -O3 -g -Wall -L$(QUOTADIR)

QUOTAOBJS = quotaops.o bylabel.o common.o quotasys.o pot.o quotaio.o quotaio_v1.o quotaio_v2.o quotaio_rpc.o quotaio_xfs.o quotaio_generic.o rquota_xdr.o rquota_client.o rquota_clnt.o

EXES = $(OBJS:.o=)

CC = /usr/bin/gcc
RM = /bin/rm

all : objs test

objs : $(OBJS)

test : $(OBJS)
	echo $(addprefix $(QUOTADIR)/,QUOTAOBJS)
	$(CC) -o main $(LCFLAGS) $(OBJS) $(addprefix $(QUOTADIR)/, $(QUOTAOBJS))

$(OBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY : all clean

clean :
	-$(RM) *~ *.o
