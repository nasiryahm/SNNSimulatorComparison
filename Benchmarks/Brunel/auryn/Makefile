# Update the following lines to your system requirements
AURYNPATH=../../../Simulators/auryn

# Path to Auryn include files
AURYNINC=$(AURYNPATH)/src

# Path to Auryn library
AURYNLIB=$(AURYNPATH)/build/release/src

# The following should not require updating in most cases 
CXX = mpicxx
CXXFLAGS=-ansi -pipe -O3 -march=native -ffast-math -pedantic -I/usr/include -I$(AURYNINC)
LDFLAGS=$(AURYNLIB)/libauryn.a -lboost_filesystem -lboost_system -lboost_program_options -lboost_mpi -lboost_serialization

# Add your simulation's file name here as default target
all: sim_brunel2k_pl

sim_%: sim_%.o
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -o $(subst .o,,$<)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<
