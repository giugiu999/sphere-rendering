CC = gcc
CFLAGS = -Wall -std=c99
LDLIBS = -lm

# 默认目标
all: MS1 MS2 FS
	rm -r *.o

# MS1 可执行文件
MS1: assg1.o vector.o spheres.o color.o
	$(CC) $(CFLAGS) -o MS1_assg $^ $(LDLIBS)

# MS2 可执行文件
MS2: assg2.o vector.o spheres.o color.o
	$(CC) $(CFLAGS) -o MS2_assg $^ $(LDLIBS)

# FS 可执行文件
FS: assg3.o vector.o spheres.o color.o
	$(CC) $(CFLAGS) -o FS_assg $^ $(LDLIBS)

# 生成目标文件
assg1.o: src/assg.c src/vector.h src/spheres.h src/color.h
	$(CC) $(CFLAGS) -DMS1 -c $< -o $@

assg2.o: src/assg.c src/vector.h src/spheres.h src/color.h
	$(CC) $(CFLAGS) -DMS2 -c $< -o $@

assg3.o: src/assg.c src/vector.h src/spheres.h src/color.h
	$(CC) $(CFLAGS) -DFS -c $< -o $@

vector.o: src/vector.c src/vector.h
	$(CC) $(CFLAGS) -c $< -o $@

spheres.o: src/spheres.c src/spheres.h
	$(CC) $(CFLAGS) -c $< -o $@

color.o: src/color.c src/color.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o MS1_assg MS2_assg FS_assg
