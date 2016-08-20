CXXFLAGS += -I.
CXXFLAGS += -Wno-write-strings -Wno-deprecated-declarations -std=c++11 -g -c -o

BASE_LIB_FILES := -lglog -lgflags -lm
FFMPEG_LIB_FILES := -lavcodec -lavdevice -lavformat -lavfilter -lavutil
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
	\
	./ffmpeg/ffmpeg_common.cc \
	./ffmpeg/ffmpeg_glue.cc \
	\
	./media/bit_reader_core.cc \
	./media/bit_reader.cc \
	./media/container_names.cc \
	./media/ranges.cc \
	\
	./media/decoder_buffer.cc \
	./media/decoder_buffer_queue.cc \
	./media/demuxer_stream_provider.cc \
	./media/demuxer_stream.cc \
	./media/input_stream.cc \
	./media/file_input_stream.cc \
	./media/memory_input_stream.cc \
	\
	\
	./cc/command_line.cc \

CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)

C_SOURCES := ./c/cmdutils.c \
	./c/ffmpeg_opt.c \
	./c/ffmpeg_filter.c \
	./c/ffmpeg.c \

C_OBJECTS := $(C_SOURCES:.c=.o)

CFLAGS += -I.
CFLAGS += -Werror -Wno-deprecated-declarations -g -c -o

CC=gcc

.c.o:
	@echo "  [CC]  $@"
	@$(CC) $(CFLAGS) $@ $<

TESTS :=  \
	./ffmpeg/demuxing_decoding_c \
	./cc/command_line_unittest \

all: $(C_OBJECTS) $(CPP_OBJECTS) $(TESTS)


.cc.o:
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<


./ffmpeg/demuxing_decoding_c: ./ffmpeg/demuxing_decoding_c.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) \
		$(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./ffmpeg/demuxing_decoding_c.o: ./ffmpeg/demuxing_decoding_c.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./cc/command_line_unittest: ./cc/command_line_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) \
		$(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./cc/command_line_unittest.o: ./cc/command_line_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

clean:
	rm -fr media/*.o
	rm -fr base/*.o
	rm -fr c/*.o
	rm -fr $(TESTS)
