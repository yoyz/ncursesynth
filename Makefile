CXX = g++
CXXFLAGS = -std=c++14 -Wall -O2 -pthread -I.
LDFLAGS = -lncurses -lportaudio -lrtmidi -lasound -lpthread -lm

TARGET = virtual_synth

SOURCES = main.cpp audio/audio_engine.cpp \
          machine/Ncursesynth/NcursesynthMachine.cpp \
          machine/MachineManager.cpp \
          machine/PBSynth/PBSynthMachine.cpp \
          machine/PBSynth/synthengine.cpp \
          machine/PBSynth/osctab.cpp \
          machine/PBSynth/lfo.cpp \
          machine/PBSynth/osc.cpp \
          machine/PBSynth/envtab.cpp \
          machine/PBSynth/filter.cpp \
          machine/PBSynth/coeff.cpp \
          machine/PBSynth/echo.cpp \
          machine/PBSynth/env.cpp \
          machine/PBSynth/lfotab.cpp \
          machine/Ncursesynth/filters/moog_filter.cpp \
          machine/Ncursesynth/filters/high_pass_filter.cpp \
          machine/Ncursesynth/filters/korgms20_filter.cpp \
          machine/Ncursesynth/filters/oberheim_sem_filter.cpp \
          machine/Ncursesynth/filters/svf_filter.cpp \
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
          machine/Ncursesynth/preset/preset_manager.cpp \
          ui/ui.cpp \
          ui/ui_parameters.cpp \
          ui/ui_draw.cpp \
          ui/ui_draw_util.cpp \
          ui/ui_input.cpp \
          ui/ui_constants.cpp \
          ui/machine_ui.cpp \
          ui/pbsynth_ui.cpp \
          ui/twytch_ui.cpp \
          ui/ncursesynth_ui.cpp \
          ui/cursynth_ui.cpp \
          midi/midi_input.cpp \
          midi/midi_mapping.cpp \
          machine/Cursynth/CursynthMachine.cpp \
          machine/Cursynth/cursynth_engine.cpp \
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
          machine/Cursynth/wave.cpp \
          machine/Twytch/TwytchsynthMachine.cpp \
          machine/Twytch/twytchhelm_alias.cpp \
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
          machine/Twytch/twytchhelm_smooth_filter.cpp \

OBJECTS = $(SOURCES:.cpp=.o)

TEST_CXXFLAGS = -std=c++14 -pthread -frtti -I. -I../machine -I../ui -I../audio -I../midi -I../bank -Wall -Wextra -Wno-unused-function -O2
TEST_LDFLAGS = -lportaudio -lpthread -lm -Wl,--no-as-needed -lstdc++

TEST_MACHINE_OBJECTS = machine/MachineManager.o \
                       machine/Ncursesynth/NcursesynthMachine.o \
                       machine/PBSynth/PBSynthMachine.o \
                       machine/PBSynth/synthengine.o \
                       machine/PBSynth/osctab.o \
                       machine/PBSynth/lfo.o \
                       machine/PBSynth/osc.o \
                       machine/PBSynth/envtab.o \
                       machine/PBSynth/filter.o \
                       machine/PBSynth/coeff.o \
                       machine/PBSynth/echo.o \
                       machine/PBSynth/env.o \
                       machine/PBSynth/lfotab.o \
                       machine/Ncursesynth/filters/moog_filter.o \
                       machine/Ncursesynth/filters/high_pass_filter.o \
                       machine/Ncursesynth/filters/korgms20_filter.o \
                       machine/Ncursesynth/filters/oberheim_sem_filter.o \
                       machine/Ncursesynth/filters/svf_filter.o \
                       machine/Ncursesynth/oscillators/saw_oscillator.o \
                       machine/Ncursesynth/envelopes/adsr_envelope.o \
                       machine/Ncursesynth/envelopes/flexible_envelope.o \
                       machine/Ncursesynth/synth/synth_architecture.o \
                       machine/Ncursesynth/synth/voice.o \
                       machine/Ncursesynth/effects/delay.o \
                       machine/Ncursesynth/effects/reverb.o \
                       machine/Ncursesynth/effects/chorus.o \
                       machine/Ncursesynth/effects/distortion.o \
                       machine/Ncursesynth/effects/effect_chain.o \
                       machine/Ncursesynth/preset/preset_manager.o \
                       machine/Cursynth/CursynthMachine.o \
                       machine/Cursynth/cursynth_engine.o \
                       machine/Cursynth/cursynth_strings.o \
                       machine/Cursynth/delay.o \
                       machine/Cursynth/envelope.o \
                       machine/Cursynth/feedback.o \
                       machine/Cursynth/filter.o \
                       machine/Cursynth/linear_slope.o \
                       machine/Cursynth/midi_lookup.o \
                       machine/Cursynth/mono_panner.o \
                       machine/Cursynth/operators.o \
                       machine/Cursynth/oscillator.o \
                       machine/Cursynth/processor.o \
                       machine/Cursynth/processor_router.o \
                       machine/Cursynth/send_receive.o \
                       machine/Cursynth/smooth_filter.o \
                       machine/Cursynth/smooth_value.o \
                       machine/Twytch/twytchhelm_smooth_filter.o \
                       machine/Cursynth/step_generator.o \
                       machine/Cursynth/trigger_operators.o \
                       machine/Cursynth/value.o \
                       machine/Cursynth/voice_handler.o \
                       machine/Cursynth/wave.o \
                       machine/Twytch/twytchhelm_helm_engine.o \
                       machine/Twytch/twytchhelm_helm_module.o \
                       machine/Twytch/twytchhelm_helm_oscillators.o \
                       machine/Twytch/twytchhelm_helm_voice_handler.o \
                       machine/Twytch/twytchhelm_helm_lfo.o \
                       machine/Twytch/twytchhelm_envelope.o \
                       machine/Twytch/twytchhelm_processor.o \
                       machine/Twytch/twytchhelm_processor_router.o \
                       machine/Twytch/twytchhelm_ladder_filter.o \
                       machine/Twytch/twytchhelm_state_variable_filter.o \
                       machine/Twytch/twytchhelm_mono_panner.o \
                       machine/Twytch/twytchhelm_delay.o \
                       machine/Twytch/twytchhelm_simple_delay.o \
                       machine/Twytch/twytchhelm_reverb.o \
                       machine/Twytch/twytchhelm_reverb_comb.o \
                       machine/Twytch/twytchhelm_reverb_all_pass.o \
                       machine/Twytch/twytchhelm_noise_oscillator.o \
                       machine/Twytch/twytchhelm_fixed_point_oscillator.o \
                       machine/Twytch/twytchhelm_oscillator.o \
                       machine/Twytch/twytchhelm_operators.o \
                       machine/Twytch/twytchhelm_value.o \
                       machine/Twytch/twytchhelm_smooth_value.o \
                       machine/Twytch/twytchhelm_linear_slope.o \
                       machine/Twytch/twytchhelm_step_generator.o \
                       machine/Twytch/twytchhelm_midi_lookup.o \
                       machine/Twytch/twytchhelm_magnitude_lookup.o \
                       machine/Twytch/twytchhelm_resonance_lookup.o \
                       machine/Twytch/twytchhelm_resonance_cancel.o \
                       machine/Twytch/twytchhelm_formant_manager.o \
                       machine/Twytch/twytchhelm_gate.o \
                       machine/Twytch/twytchhelm_arpeggiator.o \
                       machine/Twytch/twytchhelm_stutter.o \
                       machine/Twytch/twytchhelm_portamento_slope.o \
                       machine/Twytch/twytchhelm_feedback.o \
                       machine/Twytch/twytchhelm_distortion.o \
                       machine/Twytch/twytchhelm_bit_crush.o \
                       machine/Twytch/twytchhelm_dc_filter.o \
                       machine/Twytch/twytchhelm_biquad_filter.o \
                       machine/Twytch/twytchhelm_bypass_router.o \
                       machine/Twytch/twytchhelm_fixed_point_wave.o \
                       machine/Twytch/twytchhelm_memory.o \
                       machine/Twytch/twytchhelm_peak_meter.o \
                       machine/Twytch/twytchhelm_detune_lookup.o \
                       machine/Twytch/twytchhelm_sample_decay_lookup.o \
                       machine/Twytch/twytchhelm_trigger_operators.o \
                       machine/Twytch/twytchhelm_trigger_random.o \
                       machine/Twytch/twytchhelm_value_switch.o \
                       machine/Twytch/twytchhelm_helm_common.o \
                       machine/Twytch/twytchhelm_voice_handler.o \
                       machine/Twytch/twytchhelm_alias.o \
                       machine/Twytch/TwytchsynthMachine.o

TEST_OBJECTS = test_runner.o \
               fake_audio_driver.o \
               fft_analyzer.o \
               test_reporter.o \
               midi_simulator.o \
               test_helpers.o \
               test_engine.o \
               $(TEST_MACHINE_OBJECTS)

# Main build targets
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# MIDI targets
midi: midi/midi_learn midi/midi_client
	@echo "MIDI tools built"

midi/midi_learn: midi/midi_learn.cpp midi/cc_map.h midi/midi_protocol.h
	$(CXX) $(CXXFLAGS) -DUSE_RTMIDI -o $@ $< -lrtmidi -lpthread -lm

midi/midi_client: midi/midi_client.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lpthread

# Test framework build (doesn't run tests)
test: test/test_runner.o test/fake_audio_driver.o test/fft_analyzer.o test/test_reporter.o test/midi_simulator.o test/test_helpers.o test/test_engine.o $(TEST_MACHINE_OBJECTS)
	@echo "Test framework built"

test_runner.o fake_audio_driver.o fft_analyzer.o test_reporter.o midi_simulator.o test_helpers.o test_engine.o: %.o: test/%.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@


# Test runner with execution
test_runner: test/test_runner.o test/fake_audio_driver.o test/fft_analyzer.o test/test_reporter.o test/midi_simulator.o test/test_helpers.o $(TEST_MACHINE_OBJECTS)
	$(CXX) $(TEST_CXXFLAGS) -o $@ $^ $(TEST_LDFLAGS)
	./test_runner --all-engines --all-tests --fft

# MIDI tools target
midi_tools: midi/midi_learn midi/midi_client
	@echo "MIDI tools built"

# Clean targets
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f test_runner.o fake_audio_driver.o fft_analyzer.o test_reporter.o midi_simulator.o test_helpers.o test_engine.o midi/midi_learn midi/midi_client
	rm -f test/*.o midi/*.o
	rm -f midi/midi_learn midi/midi_client
	rm -f midi_learn midi_client

.PHONY: all clean run test test_runner midi midi_tools midi_learn midi_client

run: $(TARGET)
	./$(TARGET)
