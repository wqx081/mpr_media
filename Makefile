CXXFLAGS += -I.
CXXFLAGS += -Wno-deprecated-declarations -std=c++11 -g -c -o

BASE_LIB_FILES := -lglog -lgflags
FFMPEG_LIB_FILES := -lavcodec -lavdevice -lavformat -lavfilter -lavutil
TEST_LIB_FILES := -L/usr/local/lib -lgtest -lgtest_main -lpthread

CXX=g++

CPP_SOURCES := \
	./base/status.cc \
	./base/string_piece.cc \
	./base/string_util.cc \
	./base/string_printf.cc \
	./base/time.cc \
	./base/callback_helpers.cc \
	./base/callback_internal.cc \
	./base/ref_counted.cc \
	./base/weak_ptr.cc \
	./base/aligned_memory.cc \
	\
	./media/ffmpeg.cc \
	./media/input_container.cc \
	./media/output_container.cc \
	\
	./media/pipeline_status.cc \
	./media/buffers.cc \
	./media/decoder_buffer.cc \
	./media/decoder_buffer_queue.cc \
	./media/data_source.cc \
	./media/data_buffer.cc \
	./media/ranges.cc \
	./media/demuxer_stream.cc \
	./media/demuxer.cc \
	./media/byte_queue.cc \
	./media/bit_reader.cc \
	./media/audio/sample_rates.cc \
	./media/audio/audio_decoder_config.cc \
	./media/audio/audio_parameters.cc \
	./media/audio/audio_decoder.cc \
	./media/audio/audio_bus.cc \
	./media/clock.cc \
	\
	./media/audio/channel_layout.cc \


CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := ./media/ffmpeg_unittest \
	./media/container_unittest \
	./media/buffers_unittest \
	./media/decoder_buffer_unittest \
	./media/decoder_buffer_queue_unittest \
	./media/data_buffer_unittest \
	./media/ranges_unittest \
	./media/bit_reader_unittest \
	./media/audio/audio_bus_unittest \

all: $(CPP_OBJECTS) $(TESTS)


.cc.o:
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<


./media/ffmpeg_unittest: ./media/ffmpeg_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(BASE_LIB_FILES) $(TEST_LIB_FILES) \
		$(FFMPEG_LIB_FILES)
./media/ffmpeg_unittest.o: ./media/ffmpeg_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./media/data_buffer_unittest: ./media/data_buffer_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) $(TEST_LIB_FILES) \
		$(FFMPEG_LIB_FILES)
./media/data_buffer_unittest.o: ./media/data_buffer_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./media/container_unittest: ./media/container_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) \
		$(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./media/container_unittest.o: ./media/container_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./media/buffers_unittest: ./media/buffers_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) \
		$(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./media/buffers_unittest.o: ./media/buffers_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./media/decoder_buffer_unittest: ./media/decoder_buffer_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) \
		$(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./media/decoder_buffer_unittest.o: ./media/decoder_buffer_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./media/decoder_buffer_queue_unittest: ./media/decoder_buffer_queue_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) \
		$(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./media/decoder_buffer_queue_unittest.o: ./media/decoder_buffer_queue_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./media/ranges_unittest: ./media/ranges_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) \
		$(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./media/ranges_unittest.o: ./media/ranges_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./media/bit_reader_unittest: ./media/bit_reader_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) \
		$(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./media/bit_reader_unittest.o: ./media/bit_reader_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./media/audio/audio_bus_unittest: ./media/audio/audio_bus_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(BASE_LIB_FILES) \
		$(TEST_LIB_FILES) $(FFMPEG_LIB_FILES)
./media/audio/audio_bus_unittest.o: ./media/audio/audio_bus_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<


clean:
	rm media/*.o
	rm base/*.o
	rm $(TESTS)
