# get all src files
src_c = $(wildcard *.c ./utility/*.c)
src_cpp = $(wildcard *.cpp ./utility/*.cpp)
obj_c = $(patsubst %.c, %.o, $(src_c))
obj_cpp = $(patsubst %.cpp, %.o, $(src_cpp))
obj = $(obj_c) $(obj_cpp)

# include
LIB_INC	:= -L./
SRC_INC := -I./ -I./utility/


# library
LIB += -lssl -lcrypto

# compile flags
CFLAGS = -g
CFLAGS += -DLINUX -DDEBUG
CFLAGS += $(LIB_INC) $(SRC_INC)

CC = g++

# bin file
#DATE := $(shell date +"%Y%m%d")
TARGET = libbtquery_com.a


all::	$(TARGET)

${TARGET}:$(obj)
	ar crv $@ $^

%.o: %.cpp
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -rf $(obj) $(TARGET)
