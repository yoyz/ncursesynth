CXX = g++
CXXFLAGS = -std=c++14 -Wall -O2 -pthread -I.
LDFLAGS = -lncurses -lportaudio -lrtmidi -lasound -lpthread -lm

TARGET = virtual_synth

# =============================================================================
# CORE / COMMON
# =============================================================================
CORE_SOURCES = main.cpp \
               audio/audio_engine.cpp \
               audio/audio_capture.cpp \
               audio/audio_level.cpp \
               audio/capture_analysis.cpp \
               machine/Machine.cpp \
               machine/MachineManager.cpp \
               midi/midi_input.cpp \
               midi/midi_mapping.cpp \
               midi/midi_capture.cpp \
               midi/tcp_midi_server.cpp \
               test/fft_analyzer.cpp

# =============================================================================
# NCURSESYNTH ENGINE
# =============================================================================
ENGINE_NCURSESYNTH_ENGINE = machine/Ncursesynth/filters/moog_filter.cpp \
                           machine/Ncursesynth/filters/high_pass_filter.cpp \
                           machine/Ncursesynth/filters/korgms20_filter.cpp \
                            machine/Ncursesynth/filters/oberheim_sem_filter.cpp \
                           machine/Ncursesynth/filters/svf_filter.cpp \
                           machine/Ncursesynth/filters/diode_filter.cpp \
                           machine/Ncursesynth/filters/formant_filter.cpp \
                            machine/Ncursesynth/filters/comb_filter.cpp \
                            machine/Ncursesynth/filters/vital_sallen_key_filter.cpp \
                            machine/Ncursesynth/filters/vital_digital_svf_filter.cpp \
                            machine/Ncursesynth/filters/vital_ladder_filter.cpp \
                            machine/Ncursesynth/filters/vital_dirty_filter.cpp \
                            machine/Ncursesynth/oscillators/saw_oscillator.cpp \
                           machine/Ncursesynth/envelopes/adsr_envelope.cpp \
                           machine/Ncursesynth/envelopes/flexible_envelope.cpp \
                           machine/Ncursesynth/synth/synth_architecture.cpp \
                           machine/Ncursesynth/synth/voice.cpp \
                           machine/Ncursesynth/effects/delay.cpp \
                           machine/Ncursesynth/effects/reverb.cpp \
                           machine/Ncursesynth/effects/chorus.cpp \
                           machine/Ncursesynth/effects/distortion.cpp \
                           machine/Ncursesynth/effects/effect_chain.cpp \
                           machine/Ncursesynth/preset/preset_manager.cpp

ENGINE_NCURSESYNTH_MACHINE = machine/Ncursesynth/NcursesynthMachine.cpp

ENGINE_NCURSESYNTH_UI = ui/ncursesynth_ui.cpp

# =============================================================================
# PBSYNTH ENGINE
# =============================================================================
ENGINE_PBSYNTH_ENGINE = machine/PBSynth/synthengine.cpp \
                        machine/PBSynth/osctab.cpp \
                        machine/PBSynth/lfo.cpp \
                        machine/PBSynth/osc.cpp \
                        machine/PBSynth/envtab.cpp \
                        machine/PBSynth/filter.cpp \
                        machine/PBSynth/coeff.cpp \
                        machine/PBSynth/echo.cpp \
                        machine/PBSynth/env.cpp \
                        machine/PBSynth/lfotab.cpp

ENGINE_PBSYNTH_MACHINE = machine/PBSynth/PBSynthMachine.cpp

ENGINE_PBSYNTH_UI = ui/pbsynth_ui.cpp

# =============================================================================
# CURSYNTH ENGINE
# =============================================================================
ENGINE_CURSYNTH_ENGINE = machine/Cursynth/cursynth_engine.cpp \
                        machine/Cursynth/cursynth_strings.cpp \
                        machine/Cursynth/delay.cpp \
                        machine/Cursynth/envelope.cpp \
                        machine/Cursynth/feedback.cpp \
                        machine/Cursynth/filter.cpp \
                        machine/Cursynth/linear_slope.cpp \
                        machine/Cursynth/midi_lookup.cpp \
                        machine/Cursynth/mono_panner.cpp \
                        machine/Cursynth/operators.cpp \
                        machine/Cursynth/oscillator.cpp \
                        machine/Cursynth/processor.cpp \
                        machine/Cursynth/processor_router.cpp \
                        machine/Cursynth/send_receive.cpp \
                        machine/Cursynth/smooth_filter.cpp \
                        machine/Cursynth/smooth_value.cpp \
                        machine/Cursynth/step_generator.cpp \
                        machine/Cursynth/trigger_operators.cpp \
                        machine/Cursynth/value.cpp \
                        machine/Cursynth/voice_handler.cpp \
                        machine/Cursynth/wave.cpp

ENGINE_CURSYNTH_MACHINE = machine/Cursynth/CursynthMachine.cpp

ENGINE_CURSYNTH_UI = ui/cursynth_ui.cpp

# =============================================================================
# TWYTCH ENGINE
# =============================================================================
ENGINE_TWYTCH_ENGINE = machine/Twytch/twytchhelm_alias.cpp \
                      machine/Twytch/twytchhelm_arpeggiator.cpp \
                      machine/Twytch/twytchhelm_biquad_filter.cpp \
                      machine/Twytch/twytchhelm_bit_crush.cpp \
                      machine/Twytch/twytchhelm_bypass_router.cpp \
                      machine/Twytch/twytchhelm_dc_filter.cpp \
                      machine/Twytch/twytchhelm_delay.cpp \
                      machine/Twytch/twytchhelm_detune_lookup.cpp \
                      machine/Twytch/twytchhelm_distortion.cpp \
                      machine/Twytch/twytchhelm_envelope.cpp \
                      machine/Twytch/twytchhelm_feedback.cpp \
                      machine/Twytch/twytchhelm_fixed_point_oscillator.cpp \
                      machine/Twytch/twytchhelm_fixed_point_wave.cpp \
                      machine/Twytch/twytchhelm_formant_manager.cpp \
                      machine/Twytch/twytchhelm_gate.cpp \
                      machine/Twytch/twytchhelm_helm_common.cpp \
                      machine/Twytch/twytchhelm_helm_engine.cpp \
                      machine/Twytch/twytchhelm_helm_lfo.cpp \
                      machine/Twytch/twytchhelm_helm_module.cpp \
                      machine/Twytch/twytchhelm_helm_oscillators.cpp \
                      machine/Twytch/twytchhelm_helm_voice_handler.cpp \
                      machine/Twytch/twytchhelm_ladder_filter.cpp \
                      machine/Twytch/twytchhelm_linear_slope.cpp \
                      machine/Twytch/twytchhelm_magnitude_lookup.cpp \
                      machine/Twytch/twytchhelm_memory.cpp \
                      machine/Twytch/twytchhelm_midi_lookup.cpp \
                      machine/Twytch/twytchhelm_mono_panner.cpp \
                      machine/Twytch/twytchhelm_noise_oscillator.cpp \
                      machine/Twytch/twytchhelm_operators.cpp \
                      machine/Twytch/twytchhelm_oscillator.cpp \
                      machine/Twytch/twytchhelm_peak_meter.cpp \
                      machine/Twytch/twytchhelm_portamento_slope.cpp \
                      machine/Twytch/twytchhelm_processor.cpp \
                      machine/Twytch/twytchhelm_processor_router.cpp \
                      machine/Twytch/twytchhelm_resonance_cancel.cpp \
                      machine/Twytch/twytchhelm_resonance_lookup.cpp \
                      machine/Twytch/twytchhelm_reverb.cpp \
                      machine/Twytch/twytchhelm_reverb_all_pass.cpp \
                      machine/Twytch/twytchhelm_reverb_comb.cpp \
                      machine/Twytch/twytchhelm_sample_decay_lookup.cpp \
                      machine/Twytch/twytchhelm_simple_delay.cpp \
                      machine/Twytch/twytchhelm_smooth_value.cpp \
                      machine/Twytch/twytchhelm_state_variable_filter.cpp \
                      machine/Twytch/twytchhelm_step_generator.cpp \
                      machine/Twytch/twytchhelm_stutter.cpp \
                      machine/Twytch/twytchhelm_trigger_operators.cpp \
                      machine/Twytch/twytchhelm_trigger_random.cpp \
                      machine/Twytch/twytchhelm_value.cpp \
                      machine/Twytch/twytchhelm_value_switch.cpp \
                      machine/Twytch/twytchhelm_voice_handler.cpp \
                      machine/Twytch/twytchhelm_smooth_filter.cpp

ENGINE_TWYTCH_MACHINE = machine/Twytch/TwytchsynthMachine.cpp

ENGINE_TWYTCH_UI = ui/twytch_ui.cpp

# =============================================================================
# MACHINE UI (shared/engine selection)
# =============================================================================
MACHINE_UI = ui/machine_ui.cpp \
             ui/ncurses_renderer.cpp \
             ui/widget.cpp

# =============================================================================
# ALL SOURCES COMBINED
# =============================================================================
SOURCES = $(CORE_SOURCES) \
          $(ENGINE_NCURSESYNTH_ENGINE) \
          $(ENGINE_NCURSESYNTH_MACHINE) \
          $(ENGINE_NCURSESYNTH_UI) \
          $(ENGINE_PBSYNTH_ENGINE) \
          $(ENGINE_PBSYNTH_MACHINE) \
          $(ENGINE_PBSYNTH_UI) \
          $(ENGINE_CURSYNTH_ENGINE) \
          $(ENGINE_CURSYNTH_MACHINE) \
          $(ENGINE_CURSYNTH_UI) \
          $(ENGINE_TWYTCH_ENGINE) \
          $(ENGINE_TWYTCH_MACHINE) \
          $(ENGINE_TWYTCH_UI) \
          $(MACHINE_UI)

OBJECTS = $(SOURCES:.cpp=.o)

# =============================================================================
# TEST FRAMEWORK
# =============================================================================
TEST_CXXFLAGS = -std=c++14 -pthread -frtti -I. -I../machine -I../ui -I../audio -I../midi -I../bank -Wall -Wextra -Wno-unused-function -O2
TEST_LDFLAGS = -lportaudio -lpthread -lrtmidi -lm -lncurses -Wl,--no-as-needed -lstdc++

# UI test needs its own CXXFLAGS (includes ncurses path)
TEST_UI_CXXFLAGS = -std=c++14 -pthread -frtti -I. -I../machine -I../ui -I../audio -I../midi -I../bank -Wall -Wextra -Wno-unused-function -O2
TEST_UI_LDFLAGS = -lncurses -lportaudio -lpthread -lrtmidi -lm -Wl,--no-as-needed -lstdc++

TEST_UI_OBJECTS = ui/machine_ui.o \
                  ui/ncurses_renderer.o \
                  ui/widget.o \
                  ui/ncursesynth_ui.o \
                  ui/pbsynth_ui.o \
                  ui/cursynth_ui.o \
                  ui/twytch_ui.o \
                  audio/audio_level.o

# Convert source variables to object variables for test framework
TEST_CORE_OBJECTS = machine/Machine.o \
                    machine/MachineManager.o \
                    midi/midi_input.o \
                    midi/midi_mapping.o \
                    midi/midi_capture.o \
                    midi/tcp_midi_server.o

TEST_NCURSESYNTH_MACHINE_OBJECTS = $(ENGINE_NCURSESYNTH_ENGINE:.cpp=.o) \
                                  $(ENGINE_NCURSESYNTH_MACHINE:.cpp=.o)

TEST_PBSYNTH_MACHINE_OBJECTS = $(ENGINE_PBSYNTH_ENGINE:.cpp=.o) \
                              $(ENGINE_PBSYNTH_MACHINE:.cpp=.o)

TEST_CURSYNTH_MACHINE_OBJECTS = $(ENGINE_CURSYNTH_ENGINE:.cpp=.o) \
                              $(ENGINE_CURSYNTH_MACHINE:.cpp=.o)

TEST_TWYTCH_MACHINE_OBJECTS = $(ENGINE_TWYTCH_ENGINE:.cpp=.o) \
                             $(ENGINE_TWYTCH_MACHINE:.cpp=.o)

TEST_MACHINE_OBJECTS = $(TEST_CORE_OBJECTS) \
                    $(TEST_NCURSESYNTH_MACHINE_OBJECTS) \
                    $(TEST_PBSYNTH_MACHINE_OBJECTS) \
                    $(TEST_CURSYNTH_MACHINE_OBJECTS) \
                    $(TEST_TWYTCH_MACHINE_OBJECTS)

TEST_OBJECTS = test_runner.o \
              fft_analyzer.o \
              test_reporter.o \
              test_helpers.o \
              test_engine.o \
              fake_audio_driver.o \
              midi_simulator.o \
              $(TEST_MACHINE_OBJECTS)

# =============================================================================
# BUILD RULES
# =============================================================================
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# =============================================================================
# MIDI TOOLS
# =============================================================================
midi: midi/midi_learn midi/midi_client
	@echo "MIDI tools built"

midi/midi_learn: midi/midi_learn.cpp midi/cc_map.h midi/midi_protocol.h
	$(CXX) $(CXXFLAGS) -DUSE_RTMIDI -o $@ $< -lrtmidi -lpthread -lm

midi/midi_client: midi/midi_client.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lpthread

# =============================================================================
# TEST FRAMEWORK
# =============================================================================
test: test/test_runner.o test/fft_analyzer.o test/test_reporter.o test/test_helpers.o test/test_engine.o test/fake_audio_driver.o test/midi_simulator.o $(TEST_MACHINE_OBJECTS)
	@echo "Test framework built"

test_runner.o fft_analyzer.o test_reporter.o test_helpers.o test_engine.o fake_audio_driver.o midi_simulator.o: %.o: test/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

test_runner: test/test_runner.o test/fft_analyzer.o test/test_reporter.o test/test_helpers.o test/test_engine.o test/fake_audio_driver.o test/midi_simulator.o $(TEST_MACHINE_OBJECTS)
	$(CXX) $(TEST_CXXFLAGS) -o $@ $^ $(TEST_LDFLAGS)
	@echo ""
	@echo "Testing can be done using the following command"
	@echo "./test_runner --all-engines --all-tests --fft"

# =============================================================================
# UI TEST
# =============================================================================
test_ui: test/ui_test.o $(TEST_UI_OBJECTS) $(TEST_MACHINE_OBJECTS)
	$(CXX) $(TEST_UI_CXXFLAGS) -o test_ui $^ $(TEST_UI_LDFLAGS)
	@echo ""
	@echo "UI test built. Run with:"
	@echo "./test_ui"

test/ui_test.o: test/ui_test.cpp
	$(CXX) $(TEST_UI_CXXFLAGS) -c $< -o $@

# =============================================================================
# CLEAN
# =============================================================================
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_runner.o fft_analyzer.o test_reporter.o test_helpers.o test_engine.o fake_audio_driver.o midi_simulator.o test/ui_test.o
	rm -f test/*.o midi/*.o
	rm -f midi/midi_learn midi/midi_client
	rm -f midi_learn midi_client test_runner test_ui
	find . -name '*.o' -type f -delete 2>/dev/null || true

.PHONY: all clean run test test_runner test_ui midi midi_tools midi_learn midi_client

run: $(TARGET)
	./$(TARGET)

