# get all src files
src = $(wildcard *.cpp)
obj = $(patsubst %.cpp, %.o, $(src))

# include
LIB_INC	:= -L./
SRC_INC := -I./


# library
LIB += -lssl -lcrypto

# compile flags
CFLAGS = -g
CFLAGS += -DLINUX
CFLAGS += $(LIB_INC) $(SRC_INC)

CC = g++

# bin file
#DATE := $(shell date +"%Y%m%d")
TARGET = libbtcrypto.a


all::	$(TARGET)

${TARGET}:$(obj)
	ar crv $@ $^

%.o: %.cpp
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -rf $(obj) $(TARGET)
