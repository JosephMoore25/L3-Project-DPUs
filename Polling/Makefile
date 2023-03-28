SHELL := /bin/bash
CXX = mpic++
CXXFLAGS = -O3

all: bfd_offload.cpp bfd_offload.h latency.cpp singletasksend.cpp
	module load gnu_comp/9.3.0 openmpi/4.1.1; \
	module list; \
	$(CXX) $(CXXFLAGS) -std=c++11 -o latencyhost latency.cpp bfd_offload.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -o singletaskhost singletasksend.cpp bfd_offload.cpp
	module purge; \

clean:
	rm latencyhost singletaskhost onewaybfd onewayhost
