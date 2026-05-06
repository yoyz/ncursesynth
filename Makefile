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
          machine/Twytch/twytchhelm_smooth_filter.cpp \
          machine/Twytch/twytchhelm_smooth_value.cpp \
          machine/Twytch/twytchhelm_state_variable_filter.cpp \
          machine/Twytch/twytchhelm_step_generator.cpp \
          machine/Twytch/twytchhelm_stutter.cpp \
          machine/Twytch/twytchhelm_trigger_operators.cpp \
          machine/Twytch/twytchhelm_trigger_random.cpp \
          machine/Twytch/twytchhelm_value.cpp \
          machine/Twytch/twytchhelm_value_switch.cpp \
          machine/Twytch/twytchhelm_voice_handler.cpp

OBJECTS = $(SOURCES:.cpp=.o)

TEST_CXXFLAGS = -std=c++14 -pthread -frtti -I. -I../machine -I../ui -I../audio -I../midi -I../bank -Wall -Wextra -Wno-unused-function -O2
TEST_LDFLAGS = -lportaudio -lpthread -lm -Wl,--no-as-needed -lstdc++

TEST_SOURCES = test/test_runner.cpp \
               test/fake_audio_driver.cpp \
               test/fft_analyzer.cpp \
               test/test_reporter.cpp \
               test/midi_simulator.cpp \
               test/test_helpers.cpp \
               test/test_engine.cpp \
               machine/MachineManager.cpp \
               machine/Ncursesynth/NcursesynthMachine.cpp \
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
               machine/Cursynth/CursynthMachine.cpp \
               machine/Cursynth/cursynth_engine.cpp \
               machine/Cursynth/cursynth_strings.cpp \
               machine/Cursynth/delay.cpp \
               machine/Cursynth/envelope.cpp \
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
               machine/Cursynth/feedback.cpp \
               machine/Cursynth/wave.cpp

TEST_MACHINE_OBJECTS = tmp/MachineManager.o \
                       tmp/ncursesynth_machine.o \
                       tmp/pbsynth_machine.o \
                       tmp/pbsynth_synthengine.o \
                       tmp/pbsynth_osctab.o \
                       tmp/pbsynth_lfo.o \
                       tmp/pbsynth_osc.o \
                       tmp/pbsynth_envtab.o \
                       tmp/pbsynth_filter.o \
                       tmp/pbsynth_coeff.o \
                       tmp/pbsynth_echo.o \
                       tmp/pbsynth_env.o \
                       tmp/pbsynth_lfotab.o \
                       tmp/ncursesynth_moog_filter.o \
                       tmp/ncursesynth_high_pass_filter.o \
                       tmp/ncursesynth_korgms20_filter.o \
                       tmp/ncursesynth_oberheim_sem_filter.o \
                       tmp/ncursesynth_svf_filter.o \
                       tmp/ncursesynth_saw_oscillator.o \
                       tmp/ncursesynth_adsr_envelope.o \
                       tmp/ncursesynth_flexible_envelope.o \
                       tmp/ncursesynth_synth_architecture.o \
                       tmp/ncursesynth_voice.o \
                       tmp/ncursesynth_delay.o \
                       tmp/ncursesynth_reverb.o \
                       tmp/ncursesynth_chorus.o \
                       tmp/ncursesynth_distortion.o \
                       tmp/ncursesynth_effect_chain.o \
                       tmp/ncursesynth_preset_manager.o \
                       tmp/cursynth_cursynth_machine.o \
                       tmp/cursynth_cursynth_engine.o \
                       tmp/cursynth_cursynth_strings.o \
                       tmp/cursynth_delay.o \
                       tmp/cursynth_envelope.o \
                       tmp/cursynth_filter.o \
                       tmp/cursynth_linear_slope.o \
                       tmp/cursynth_midi_lookup.o \
                       tmp/cursynth_mono_panner.o \
                       tmp/cursynth_operators.o \
                       tmp/cursynth_oscillator.o \
                       tmp/cursynth_processor.o \
                       tmp/cursynth_processor_router.o \
                       tmp/cursynth_send_receive.o \
                       tmp/cursynth_smooth_filter.o \
                       tmp/cursynth_smooth_value.o \
                       tmp/cursynth_step_generator.o \
                       tmp/cursynth_trigger_operators.o \
                       tmp/cursynth_value.o \
                       tmp/cursynth_voice_handler.o \
                       tmp/cursynth_feedback.o \
                       tmp/cursynth_wave.o

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
test: test_runner.o fake_audio_driver.o fft_analyzer.o test_reporter.o midi_simulator.o test_helpers.o test_engine.o $(TEST_MACHINE_OBJECTS)
	@echo "Test framework built"

test_runner.o: test/test_runner.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

fake_audio_driver.o: test/fake_audio_driver.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

fft_analyzer.o: test/fft_analyzer.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

test_reporter.o: test/test_reporter.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

midi_simulator.o: test/midi_simulator.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

test_helpers.o: test/test_helpers.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

test_engine.o: test/test_engine.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp:
	mkdir -p tmp

tmp/MachineManager.o: machine/MachineManager.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_machine.o: machine/Ncursesynth/NcursesynthMachine.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/pbsynth_machine.o: machine/PBSynth/PBSynthMachine.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/pbsynth_synthengine.o: machine/PBSynth/synthengine.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/pbsynth_osctab.o: machine/PBSynth/osctab.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/pbsynth_lfo.o: machine/PBSynth/lfo.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/pbsynth_osc.o: machine/PBSynth/osc.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/pbsynth_envtab.o: machine/PBSynth/envtab.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/pbsynth_filter.o: machine/PBSynth/filter.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/pbsynth_coeff.o: machine/PBSynth/coeff.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/pbsynth_echo.o: machine/PBSynth/echo.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/pbsynth_env.o: machine/PBSynth/env.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/pbsynth_lfotab.o: machine/PBSynth/lfotab.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_moog_filter.o: machine/Ncursesynth/filters/moog_filter.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_high_pass_filter.o: machine/Ncursesynth/filters/high_pass_filter.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_korgms20_filter.o: machine/Ncursesynth/filters/korgms20_filter.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_oberheim_sem_filter.o: machine/Ncursesynth/filters/oberheim_sem_filter.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_svf_filter.o: machine/Ncursesynth/filters/svf_filter.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_saw_oscillator.o: machine/Ncursesynth/oscillators/saw_oscillator.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_adsr_envelope.o: machine/Ncursesynth/envelopes/adsr_envelope.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_flexible_envelope.o: machine/Ncursesynth/envelopes/flexible_envelope.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_synth_architecture.o: machine/Ncursesynth/synth/synth_architecture.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_voice.o: machine/Ncursesynth/synth/voice.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_delay.o: machine/Ncursesynth/effects/delay.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_reverb.o: machine/Ncursesynth/effects/reverb.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_chorus.o: machine/Ncursesynth/effects/chorus.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_distortion.o: machine/Ncursesynth/effects/distortion.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_effect_chain.o: machine/Ncursesynth/effects/effect_chain.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/ncursesynth_preset_manager.o: machine/Ncursesynth/preset/preset_manager.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_cursynth_machine.o: machine/Cursynth/CursynthMachine.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_cursynth_engine.o: machine/Cursynth/cursynth_engine.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_cursynth_strings.o: machine/Cursynth/cursynth_strings.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_delay.o: machine/Cursynth/delay.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_envelope.o: machine/Cursynth/envelope.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_filter.o: machine/Cursynth/filter.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_linear_slope.o: machine/Cursynth/linear_slope.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_midi_lookup.o: machine/Cursynth/midi_lookup.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_mono_panner.o: machine/Cursynth/mono_panner.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_operators.o: machine/Cursynth/operators.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_oscillator.o: machine/Cursynth/oscillator.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_processor.o: machine/Cursynth/processor.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_processor_router.o: machine/Cursynth/processor_router.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_send_receive.o: machine/Cursynth/send_receive.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_smooth_filter.o: machine/Cursynth/smooth_filter.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_smooth_value.o: machine/Cursynth/smooth_value.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_step_generator.o: machine/Cursynth/step_generator.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_trigger_operators.o: machine/Cursynth/trigger_operators.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_value.o: machine/Cursynth/value.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_voice_handler.o: machine/Cursynth/voice_handler.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_feedback.o: machine/Cursynth/feedback.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

tmp/cursynth_wave.o: machine/Cursynth/wave.cpp
	$(CXX) $(TEST_CXXFLAGS) -c $< -o $@

# Test runner with execution
test_runner: test_runner.o fake_audio_driver.o fft_analyzer.o test_reporter.o midi_simulator.o test_helpers.o $(TEST_MACHINE_OBJECTS)
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
	rm -f tmp/*.o
	rm -f midi/midi_learn midi/midi_client
	rmdir tmp 2>/dev/null || true
	rm -f midi_learn midi_client

.PHONY: all clean run test test_runner midi midi_tools midi_learn midi_client tmp

run: $(TARGET)
	./$(TARGET)
