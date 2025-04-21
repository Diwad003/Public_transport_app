# Makefile for Trafikverket_program

# Compiler and flags
CFLAGS = -Wall -ggdb -std=c99 -pedantic
LIB = `xml2-config --cflags --libs` -lcurl 

# Directories
SRC_DIR = ./src
INCLUDE_DIR = ./include

# Target executable
TARGET = trafikverket_program

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)

# Default target
all:
	gcc $(CFLAGS) -I$(INCLUDE_DIR) -o $(TARGET) $(HEADERS) $(SRCS) $(LIB)