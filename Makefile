APP:=./bin/FFmpeg

CXXFLAGS += -I.
CXXFLAGS += -Wno-write-strings -Wno-deprecated-declarations -std=c++11 -g -c -o

BASE_LIB_FILES := -lglog -lgflags -lm
FFMPEG_LIB_FILES := \
	-lavcodec \
	-lavdevice \
	-lavformat \
	-lavfilter \
	-lavutil \
	-lswresample \
	-lswscale \
	-lpostproc \
	-lm \
	-lpthread
TEST_LIB_FILES := -L/usr/local/lib -lgtest -lgtest_main -lpthread

CXX=g++

CPP_SOURCES := \
	./base/file_enumerator.cc \
	./base/file.cc \
	./base/file_path.cc \
	./base/scoped_file.cc \
	./base/file_util.cc \
	./base/status.cc \
	./base/string_piece.cc \
	./base/string_util.cc \
	./base/string_printf.cc \
	./base/string_split.cc \
	./base/numbers.cc \
	./base/ascii_ctype.cc \
	./base/time.cc \
	./base/callback_helpers.cc \
	./base/callback_internal.cc \
	./base/bind_helpers.cc \
	./base/ref_counted.cc \
	./base/weak_ptr.cc \
	./base/aligned_memory.cc \
	./base/memory_mapped_file.cc \
	./base/memory_mapped_file_posix.cc \
	./base/lazy_instance.cc \
	./base/lock.cc \
	./base/command_line.cc \
	./base/environment.cc \
	./base/file_descriptor_shuffle.cc \
	\
	\
	./base/process/internal_linux.cc \
	./base/process/process_handle.cc \
	./base/process/process_iterator.cc \
	./base/process/process_iterator_linux.cc \
	./base/process/kill.cc \
	./base/process/kill_posix.cc \
	./base/process/process_posix.cc \
	./base/process/process_linux.cc \
	./base/process/launch.cc \
	./base/process/launch_posix.cc \
	./base/test/multiprocess_func_list.cc \
	\
	./cc/ffmpeg_cmdutils.cc \
	./cc/ffmpeg_opt.cc \
	./cc/ffmpeg_filter.cc \
	./cc/ffmpeg_c.cc \

CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)

C_SOURCES := ./c/cmdutils.c \
	./c/ffmpeg_opt.c \
	./c/ffmpeg_filter.c \

C_OBJECTS := $(C_SOURCES:.c=.o)

CFLAGS += -I.
CFLAGS += -Werror -Wno-unused-result -Wno-deprecated-declarations -g -c -o

CC=gcc

.c.o:
	@echo "  [CC]  $@"
	@$(CC) $(CFLAGS) $@ $<

TESTS :=  \
	./base/process/process_unittest \
	./base/command_line_unittest \

#all: $(C_OBJECTS) $(APP) $(CPP_OBJECTS) $(TESTS)
all: $(CPP_OBJECTS) $(TESTS)


.cc.o:
	@echo "  [CC]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<


$(APP): ./c/ffmpeg.o
	@echo "  [LINK] $@"
	@$(CC) -o $@  $< $(C_OBJECTS) $(FFMPEG_LIB_FILES)
./c/ffmpeg.o: ./c/ffmpeg.c
	@echo "  [CC]  $@"
	@$(CC) $(CFLAGS) $@ $<


./base/command_line_unittest: ./base/command_line_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) $(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./base/command_line_unittest.o: ./base/command_line_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/process/process_unittest: ./base/process/process_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) $(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./base/process/process_unittest.o: ./base/process/process_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

clean:
	rm -fr media/*.o
	rm -fr base/*.o
	rm -fr c/*.o
	rm -fr cc/*.o
	rm -fr $(TESTS)
