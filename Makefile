CXXFLAGS =	-O2 -g -Wall -fmessage-length=0  

OBJS =		ownership_change.o

LIBS =		-lbluetooth

DEPS = 		ownership_change.h

TARGET =	ownership_change

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) $(DEPS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
