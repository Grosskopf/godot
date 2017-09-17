#Modify this to point to the PJSIP location.
PJBASE=../../thirdparty/pjproject

include $(PJBASE)/build.mak

CC      = $(PJ_CC)
LDFLAGS = $(PJ_LDFLAGS)
LDLIBS  = $(PJ_LDLIBS)
CFLAGS  = $(PJ_CFLAGS)
CPPFLAGS= ${CFLAGS}

# If your application is in a file named myapp.cpp or myapp.c
# this is the line you will need to build the binary.
all: pjsip_client

pjsip_client: pjsip_client.cpp
	$(CC) -o $@ $< $(CPPFLAGS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f pjsip_client.o pjsip_client