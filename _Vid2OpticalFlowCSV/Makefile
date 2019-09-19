#	Vid2OpticalFlowCSV
#
#	This utility reads a video sequence and estimates the optical flow in a grid of WxH blocks.
#	The results are dumped to a CSV file, which is then used by other software to analyse motion patterns in the video.
#
#	Authors: Y. Poleg, C. Arora, S. Peleg

#Variaveis
COMPILADOR = g++
FLAGSC = -Wall -g -march=native -mtune=native
OBJDIR = obj
SRCDIR = src
SRCFILES = $(wildcard $(SRCDIR)/*/*/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp) $(filter-out mainfile.cpp,$(wildcard $(SRCDIR)/*.cpp))
LINKDIR = $(shell pkg-config --cflags opencv) -Isrc -Isrc/lk -Isrc/util -Isrc/util/paramio -Isrc/util/tclap -Isrc/VideoReaders
OPENCV_LIBS = $(shell pkg-config --libs opencv)
LINKING = $(OPENCV_LIBS)
EXECNAME = optflow
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCFILES))

#Regras
$(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(OBJDIR)
	if test ! -d $(@D); then mkdir -p $(@D); fi
	$(COMPILADOR) $(FLAGSC) -c $< -o $@ $(LINKDIR)

all : $(OBJS)
	$(COMPILADOR) $(FLAGSC) $(OBJS) -o $(EXECNAME) $(LINKDIR) $(LINKING) 
	
clean:
	rm -rf $(OBJDIR) $(EXECNAME) $(EXECNAME).tar.gz

compact:
	tar czf $(EXECNAME).tar.gz $(SRCDIR) Makefile default-config.xml Example

$(OBJDIR):
	if test ! -d $(OBJDIR); then mkdir $(OBJDIR); fi
