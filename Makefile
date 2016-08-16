CXXFLAGS += -I.
CXXFLAGS += -Wno-deprecated-declarations -std=c++11 -g -c -o

BASE_LIB_FILES := -lglog -lgflags
FFMPEG_LIB_FILES := -lavcodec -lavdevice -lavformat -lavfilter -lavutil
TEST_LIB_FILES := -L/usr/local/lib -lgtest -lgtest_main -lpthread

CXX=g++

CPP_SOURCES := \
	base/status.cc \
	base/string_piece.cc \
	\
	./ffmpeg/ffmpeg.cc \
	./ffmpeg/input_container.cc \
	./ffmpeg/output_container.cc \


CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := ./ffmpeg/ffmpeg_unittest \
	./ffmpeg/container_unittest \

all: $(CPP_OBJECTS) $(TESTS)


.cc.o:
	$(CXX) $(CXXFLAGS) $@ $<


./ffmpeg/ffmpeg_unittest: ./ffmpeg/ffmpeg_unittest.o
	$(CXX) -o $@ $< $(BASE_LIB_FILES) $(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./ffmpeg/ffmpeg_unittest.o: ./ffmpeg/ffmpeg_unittest.cc
	$(CXX) $(CXXFLAGS) $@ $<

./ffmpeg/container_unittest: ./ffmpeg/container_unittest.o
	$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) $(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./ffmpeg/container_unittest.o: ./ffmpeg/container_unittest.cc
	$(CXX) $(CXXFLAGS) $@ $<

clean:
	rm ffmpeg/*.o
	rm base/*.o
	rm $(TESTS)
