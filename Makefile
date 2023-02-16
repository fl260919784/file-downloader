.SUFFIXES: .cpp .h

CXX=g++
CPPFLAGS=-std=c++17 -O2 -g -Wall -Werror
INCFLAGS=-I./
LDFLAGS=
LIBS=-lcurl -lboost_program_options

.PHONLY: all clean

%.o:%.cpp
	$(CXX) -c $(CPPFLAGS) $(INCFLAGS) $< -o $@

all: file-downloader

clean:
	@echo "clean all"
	@find . -name "*.o" -delete

objs = ./utils/CurlHolder.o              			\
			 ./utils/FileGurad.o               			\
       ./downloader/EasyHTTPFileDownloader.o  \
			 ./downloader/QuickHTTPFileDownloader.o \
			 ./main.o

file-downloader: $(objs) main.o
	$(CXX) $^ $(LDFLAGS) $(LIBS) -o $@
	@find . -name "*.o" -delete
	@mkdir -p ./build && mv file-downloader ./build/file-downloader
