SOURCE  := main.c cpu.c
TARGET  := CPU_Measurement.o
WARN    := -W -Wall -Wstrict-prototypes -Wmissing-prototypes
INCLUDE := 
LINK    := -pthread
CFLAGS  := -O0 -DMODULE -D__KERNEL__ ${INCLUDE} ${LINK} ${WARN}
CC      := gcc

install:
	${CC} ${CFLAGS} ${SOURCE} -o build/${TARGET}

clean:
	rm -rf build/*
