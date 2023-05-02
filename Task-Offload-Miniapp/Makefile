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

host: $(COREDIR)/bfd_offload.cpp $(COREDIR)/bfd_offload.h singletasksend.cpp baselinemodel.cpp

	$(CXX) $(CXXFLAGS) -std=c++11 -o $(BUILDDIR)/singletaskhost singletasksend.cpp $(COREDIR)/bfd_offload.cpp $(LINKS)
	$(CXX) $(CXXFLAGS) -std=c++11 -o $(BUILDDIR)/baseline baselinemodel.cpp $(COREDIR)/bfd_offload.cpp $(LINKS)


bfd: $(COREDIR)/bfd_offload.cpp $(COREDIR)/bfd_offload.h singletasksend.cpp

	$(CXX) $(CXXFLAGS) -std=c++11 -o $(BUILDDIR)/singletaskbfd singletasksend.cpp $(COREDIR)/bfd_offload.cpp $(LINKS)
