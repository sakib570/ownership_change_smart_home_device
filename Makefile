CXXFLAGS =	-O2 -w -g -Wall -fmessage-length=0  

DIR  =		src/main

OBJS =		$(DIR)/ownership_change.o $(DIR)/packet.o $(DIR)/device_info.o $(DIR)/main.o

LIBS =		-lpthread -lbluetooth

DEPS = 		$(DIR)/ownership_change.h $(DIR)/packet.h $(DIR)/device_info.h

TARGET =	ownership_change

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) $(DEPS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)