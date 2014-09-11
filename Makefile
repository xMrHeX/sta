# sta - simple terminal, augmented
# See LICENSE file for copyright and license details.

include config.mk

SRC = src/sta.c
OBJ = ${SRC:.c=.o}

all: options sta

options:
	@echo sta build options:
	@echo "CFLAGS   = ${CFLAGS} -DALIENFX"
	@echo "LDFLAGS  = ${LDFLAGS} -usb-1.0"
	@echo "CC       = ${CC}"

config.h:
	cp src/config.def.h src/config.h

.c.o:
	@echo CC $< -o ${OBJ}
	@${CC} -c ${CFLAGS} $< -o ${OBJ}

${OBJ}: config.h config.mk

# sta: CFLAGS += -DALIENFX
# sta: LDFLAGS += -lusb-1.0
sta: clean ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning...
	@rm -f sta ${OBJ} sta-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p sta-${VERSION}
	@cp -R LICENSE Makefile README config.mk config.def.h st.info st.1 ${SRC} st-${VERSION}
	@tar -cf sta-${VERSION}.tar sta-${VERSION}
	@gzip sta-${VERSION}.tar
	@rm -rf sta-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f sta ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/sta
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < man/sta.1 > ${DESTDIR}${MANPREFIX}/man1/st.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/st.1
	@echo Please see the README file regarding the terminfo entry of st.
	@tic -s man/sta.info

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/sta
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/st.1

.PHONY: all options clean dist install uninstall

