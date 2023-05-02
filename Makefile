CXX = mpic++
CXXFLAGS = -O3 -march=native
ARCH:=$(shell uname -m)
BUILDDIR = build
COREDIR = Core-Files
LATENCYDIR = Latency-Tests
MINIAPPDIR = Task-Offload-Miniapp

$(shell mkdir -p $(BUILDDIR))
$(shell mkdir -p $(BUILDDIR)/$(ARCH))

all:
	(cd $(LATENCYDIR); make all)
	(cd $(MINIAPPDIR); make all)

clean:
	rm $(BUILDDIR) -r