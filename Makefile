CC = gcc
CFLAGS = -Wall
OBJ = main.o regex.o lang.o
EXEC = my_program.exe

# 默认目标
# all: $(EXEC)

# 编译主程序
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

# 编译 main.c
main.o: main.c regex.h lang.h
	$(CC) $(CFLAGS) -c main.c

# 编译 regex.c
regex.o: regex.c regex.h lang.h
	$(CC) $(CFLAGS) -c regex.c

# 编译 lang.c
lang.o: lang.c lang.h
	$(CC) $(CFLAGS) -c lang.c

# 清理目标
clean:
	rm -f $(OBJ) $(EXEC)