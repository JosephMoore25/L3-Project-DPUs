SHELL := /bin/bash
CXX = mpic++
CXXFLAGS = -O3 -march=native
ARCH:=$(shell uname -m)

ifeq ($(ARCH),x86_64)
	TARGET=host
else ifeq ($(ARCH),aarch64)
	TARGET=bfd
endif

all: $(TARGET)

host: bfd_offload.cpp bfd_offload.h latency.cpp singletasksend.cpp baselinemodel.cpp baseline_latency.cpp

	$(CXX) $(CXXFLAGS) -std=c++11 -o latencyhost latency.cpp bfd_offload.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -o singletaskhost singletasksend.cpp bfd_offload.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -o baseline baselinemodel.cpp bfd_offload.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -o baselinelatency baseline_latency.cpp bfd_offload.cpp


bfd: bfd_offload.cpp bfd_offload.h latency.cpp singletasksend.cpp

	$(CXX) $(CXXFLAGS) -std=c++11 -o latencybfd latency.cpp bfd_offload.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -o singletaskbfd singletasksend.cpp bfd_offload.cpp

clean:
	rm latencyhost singletaskhost latencybfd singletaskbfd baseline baselinelatency
