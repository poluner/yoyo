#get all src files
src = $(wildcard *.cpp)
obj = $(patsubst %.cpp, %.o, $(src))

# include
LIB_INC	:= -L../../ -L../base/
SRC_INC := -I./ -I../ -I../base -I../../


# library
LIB += -lssl -lcrypto -lbtcrypto -lbtquery_com

# compile flags
CFLAGS = -g
CFLAGS += -DLINUX
CFLAGS += $(LIB_INC) $(SRC_INC)

CC = g++

# bin file
#DATE := $(shell date +"%Y%m%d")
TARGET = test

all::	$(TARGET)

${TARGET}:$(obj)
	${CC} -o $@ ${CFLAGS} $^ $(LIB)

%.o: %.cpp
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	@rm -rf $(obj) $(TARGET)
