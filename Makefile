VERSION=0.1
CXXFLAGS+=-O3 -pedantic -Wall -Wno-unused-variable -Wno-long-long -std=c++11 -DVERSION=\"$(VERSION)\" $(DEBUG_FLAGS)
LDFLAGS=$(DEBUG_FLAGS) -ljpeg -pthread

OBJS=PFPP.o picio.o

all: PFPP

PFPP: $(OBJS)
	$(CXX) -Wall -W $(OBJS) $(LDFLAGS) -o PFPP
