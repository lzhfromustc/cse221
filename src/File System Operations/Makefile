SOURCE  := cache_size.c 
TARGET  := File_System_Measurement.o
WARN    := -W -Wall -Wstrict-prototypes -Wmissing-prototypes
INCLUDE := 
CFLAGS  := -O0 -DMODULE -D__KERNEL__ ${INCLUDE} ${WARN}
CC      := gcc

install:
	${CC} ${CFLAGS} ${SOURCE} -o build/${TARGET}

clean:
	rm -rf build/*
