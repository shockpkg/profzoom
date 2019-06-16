ifeq ($(DEBUG), 1)
CFLAGSTARGET = -g3 -O0
else
CFLAGSTARGET = -g0 -O3
endif

DIRSEP = /
BINEXT =
SOPRE =
SOEXT = .so
MKDIR = mkdir -p
RMDIR = rm -rf
CP = cp
CD = cd
CAT = cat
SHA256 = shasum -a 256
AS3COMPILE = as3compile

BINDIR = bin
BINDIRS = $(BINDIR)$(DIRSEP)

SRCDIR = src
SRCDIRS = $(SRCDIR)$(DIRSEP)

AS3DIR = as3
AS3DIRS = $(AS3DIR)$(DIRSEP)

DISTDIR = dist
DISTDIRS = $(DISTDIR)$(DIRSEP)

CFLAGSL = $(CFLAGS) -std=c99 -D_XOPEN_SOURCE=600 \
	-Wall -Wextra -Wno-unused-parameter -pedantic \
	$(CFLAGSTARGET)

PZSRC = $(SRCDIRS)b64.c \
	$(SRCDIRS)cmd.c \
	$(SRCDIRS)hook.c \
	$(SRCDIRS)hooks/*.c

all: profzoom \
	testunit \
	testfake \
	testreal

clean: clean-bin\
	clean-dist

clean-bin:
	$(RMDIR) $(BINDIR)

clean-dist:
	$(RMDIR) $(DISTDIR)

$(BINDIR):
	$(MKDIR) $(BINDIR)

profzoom: $(BINDIR)
	$(CC) $(CFLAGSL) -o$(BINDIRS)$(SOPRE)$@$(SOEXT) \
		-shared -fPIC \
		$(PZSRC) $(SRCDIRS)$@.c -ldl

testunit: $(BINDIR)
	$(CC) $(CFLAGSL) -o$(BINDIRS)$@$(BINEXT) $(PZSRC) $(SRCDIRS)$@.c
	$(CP) $(SRCDIRS)$@.sh $(BINDIR)

testfake: $(BINDIR)
	$(CC) $(CFLAGSL) -o$(BINDIRS)$@$(BINEXT) $(PZSRC) $(SRCDIRS)$@.c
	$(CP) $(SRCDIRS)$@.sh $(BINDIR)

testreal: $(BINDIR)
	$(AS3COMPILE) -X 600 -Y 400 -r 30 -T 9 \
		-I $(AS3DIR) \
		-o $(BINDIRS)$@.swf \
		$(AS3DIRS)$@.as
	$(CP) $(AS3DIRS)$@.sh $(BINDIR)

$(DISTDIR):
	$(MKDIR) $(DISTDIR)

dist-as3: $(DISTDIR)
	$(MKDIR) $(DISTDIRS)as3
	$(CP) -r as3/profzoom $(DISTDIRS)as3

dist-i386: $(DISTDIR)
	$(MKDIR) $(DISTDIRS)i386
	$(CP) bin/profzoom.so $(DISTDIRS)i386

dist-x86_64: $(DISTDIR)
	$(MKDIR) $(DISTDIRS)x86_64
	$(CP) bin/profzoom.so $(DISTDIRS)x86_64

dist-archive: $(DISTDIR)
	$(CD) $(DISTDIR); tar -cz --owner=0 --group=0 \
		-f 'profzoom.tar.gz' \
		as3 \
		i386 \
		x86_64
	$(CD) $(DISTDIR); $(SHA256) 'profzoom.tar.gz' > 'profzoom.tar.gz.sha256'
	$(CD) $(DISTDIR); $(CAT) 'profzoom.tar.gz.sha256'
