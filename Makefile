OUTPUT := pcm_g726_convert

CC := gcc
CFLAGS := 

# 控制程序里的DEBUG打印开关
ifeq ($(DEBUG), 1)
CFLAGS += -DENABLE_DEBUG
endif

SRC := g726.c g726.h main.c

$(OUTPUT) : $(SRC)
	$(CC) $^ $(CFLAGS) -o $@

clean : 
	rm -rf $(OUTPUT) out*
.PHONY := clean

