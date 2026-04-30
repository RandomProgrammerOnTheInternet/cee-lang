CC = clang

INCFLAGS = -iquotesrc -I.

CCFLAGS =
CCFLAGS += -std=c23
CCFLAGS += -g
CCFLAGS += -O0
#CCFLAGS += -fsanitize=undefined,address,leak
CCFLAGS += -Wall
CCFLAGS += -Wextra
CCFLAGS += -DDEBUG

LDFLAGS =
LDFLAGS += -lm

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
OUT = ./ceec

all: $(OUT)

%.o: %.c
	$(CC) $(CCFLAGS) $(INCFLAGS) -o $@ -c $<

$(OUT): $(OBJ)
	$(CC) $(INCFLAGS) $(CCFLAGS) $(LDFLAGS) $(OBJ) -o $(@)

clean:
	rm -f $(OUT) $(O)
