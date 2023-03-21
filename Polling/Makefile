SHELL := /bin/bash
CXX = mpic++
CXXFLAGS = -O3

all: bfd_offload.cpp bfd_offload.h cleanfulltrip.cpp
	module load gnu_comp/9.3.0 openmpi/4.1.1; \
	module list; \
	$(CXX) $(CXXFLAGS) -std=c++11 -o fulltriphost cleanfulltrip.cpp bfd_offload.cpp
	module purge; \

clean:
	rm fulltriphost fulltripbfd onewaybfd onewayhost
