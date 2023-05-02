CXX = mpic++
CXXFLAGS = -O3 -march=native
ARCH:=$(shell uname -m)
COREDIR = ./../Core-Files
LINKS = -I$(COREDIR)

ifeq ($(ARCH),x86_64)
	TARGET=host
else ifeq ($(ARCH),aarch64)
	TARGET=bfd
endif

BUILDDIR = ./../build/$(ARCH)

all: $(TARGET)

host: $(COREDIR)/bfd_offload.cpp $(COREDIR)/bfd_offload.h latency.cpp baseline_latency.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -o $(BUILDDIR)/latencyhost latency.cpp $(COREDIR)/bfd_offload.cpp $(LINKS)
	$(CXX) $(CXXFLAGS) -std=c++11 -o $(BUILDDIR)/baselinelatency baseline_latency.cpp $(COREDIR)/bfd_offload.cpp $(LINKS)

bfd: $(COREDIR)/bfd_offload.cpp $(COREDIR)/bfd_offload.h latency.cpp singletasksend.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -o $(BUILDDIR)/latencybfd latency.cpp $(COREDIR)/bfd_offload.cpp $(LINKS)