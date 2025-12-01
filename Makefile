CC = gcc
STD = -std=c17
G2X_PATH = ../g2x
G2X_INC  = -I$(G2X_PATH)/include
G2X_LIB  = -L$(G2X_PATH)/lib -L$(G2X_PATH)/bin -lg2x -lGL -lGLU -lX11 -lm
PFLAGS = $(G2X_INC) -I./src -I./include
CFLAGS = -Wpointer-arith -Wall -O2
LFLAGS = $(G2X_LIB)
SRC = src/

all : pgmtodif diftopgm

%.o : $(SRC)%.c
	@echo "Compil $<"
	@$(CC) $(STD) $(CFLAGS) $(PFLAGS) -c $< -o $@

pgmtodif : pgmtodif.o codec.o
	@echo "Link pgmtodif"
	@$(CC) $^ $(LFLAGS) -o $@

diftopgm : diftopgm.o codec.o
	@echo "Link diftopgm"
	@$(CC) $^ $(LFLAGS) -o $@

clean :
	rm -f *.o pgmtodif diftopgm