CC = clang
CCFLAGS =
LDFLAGS =
INCFLAGS = -iquotesrc -I.
CCFLAGS += -std=gnu2x
CCFLAGS += -fsanitize=undefined,address,leak
CCFLAGS += -g
CCFLAGS += -O0
LDFLAGS += -Wall
LDFLAGS += -lm
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: build

%.o: %.c
	$(CC) $(CCFLAGS) $(INCFLAGS) -o $@ -c $<

build: $(OBJ)
	$(CC) $(INCFLAGS) $(CCFLAGS) $(LDFLAGS) $(OBJ) -o ceec
