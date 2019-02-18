CXXFLAGS =	-O2 -g -Wall -fmessage-length=0  

DIR  =		src/main

OBJS =		$(DIR)/ownership_change.o

LIBS =		

DEPS = 		$(DIR)/ownership_change.h

TARGET =	ownership_change

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) $(DEPS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)