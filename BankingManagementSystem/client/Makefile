# -------------------------------
# Banking Management System - CLIENT
# -------------------------------

CC = gcc
CFLAGS = -Wall -g
INCLUDES = -I../include
TARGET = client
SRCS = client.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "🧹 Cleaning client..."
	rm -f $(OBJS) $(TARGET)

run: all
	@echo "🚀 Starting client..."
	./$(TARGET)
