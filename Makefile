CXX = g++
CXXFLAGS = -std=c++14 -Wall -O2 -pthread -I.
LDFLAGS = -lncurses -lportaudio -lrtmidi -lasound -lpthread -lm

TARGET = virtual_synth

SOURCES = main.cpp \
          filters/moog_filter.cpp \
          filters/korgms20_filter.cpp \
          filters/oberheim_sem_filter.cpp \
          oscillators/saw_oscillator.cpp \
          envelopes/adsr_envelope.cpp \
          envelopes/flexible_envelope.cpp \
          synth/synth_architecture.cpp \
          synth/voice.cpp \
          effects/delay.cpp \
          effects/reverb.cpp \
          effects/chorus.cpp \
          effects/distortion.cpp \
          effects/effect_chain.cpp \
          audio/audio_engine.cpp \
          ui/ui.cpp \
          ui/ui_parameters.cpp \
          ui/ui_draw.cpp \
          ui/ui_input.cpp \
          ui/ui_constants.cpp \
          midi/midi_input.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
