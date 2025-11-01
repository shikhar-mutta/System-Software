# -------------------------------
# Banking Management System - SERVER
# -------------------------------

CC = gcc
CFLAGS = -Wall -g
INCLUDES = -I../include
TARGET = server
SRCS = server.c database.c session.c customer.c employee.c manager.c admin.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "🧹 Cleaning server..."
	rm -f $(OBJS) $(TARGET)

run: all
	@echo "🚀 Starting server..."
	./$(TARGET)
