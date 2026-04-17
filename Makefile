CC = clang
CCFLAGS =
LDFLAGS =
INCFLAGS = -iquotesrc -I.
CCFLAGS += -std=c23
CCFLAGS += -g
CCFLAGS += -O0
#CCFLAGS += -fsanitize=undefined,address,leak
CCFLAGS += -Wall
CCFLAGS += -Wextra
CCFLAGS += -DDEBUG
LDFLAGS += -lm
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

all: build

%.o: %.c
	$(CC) $(CCFLAGS) $(INCFLAGS) -o $@ -c $<

build: $(OBJ)
	$(CC) $(INCFLAGS) $(CCFLAGS) $(LDFLAGS) $(OBJ) -o ceec
