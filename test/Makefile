CXX = g++
CXXFLAGS = -std=c++14 -pthread -frtti -I../. -I../machine -I../ui -I../audio -I../midi -I../bank -Wall -Wextra -Wno-unused-function -O2
LDFLAGS = -lportaudio -lpthread -lm -Wl,--no-as-needed -lstdc++

.PHONY: all clean

all: test_runner

test_runner: test_runner.o fake_audio_driver.o fft_analyzer.o test_reporter.o midi_simulator.o test_engine.o test_helpers.o tmp/MachineManager.o tmp/ncursesynth_machine.o tmp/pbsynth_machine.o tmp/pbsynth_synthengine.o tmp/pbsynth_osctab.o tmp/pbsynth_lfo.o tmp/pbsynth_osc.o tmp/pbsynth_envtab.o tmp/pbsynth_filter.o tmp/pbsynth_coeff.o tmp/pbsynth_echo.o tmp/pbsynth_env.o tmp/pbsynth_lfotab.o tmp/ncursesynth_moog_filter.o tmp/ncursesynth_high_pass_filter.o tmp/ncursesynth_korgms20_filter.o tmp/ncursesynth_oberheim_sem_filter.o tmp/ncursesynth_svf_filter.o tmp/ncursesynth_saw_oscillator.o tmp/ncursesynth_adsr_envelope.o tmp/ncursesynth_flexible_envelope.o tmp/ncursesynth_synth_architecture.o tmp/ncursesynth_voice.o tmp/ncursesynth_delay.o tmp/ncursesynth_reverb.o tmp/ncursesynth_chorus.o tmp/ncursesynth_distortion.o tmp/ncursesynth_effect_chain.o tmp/ncursesynth_preset_manager.o tmp/cursynth_cursynth_machine.o tmp/cursynth_cursynth_engine.o tmp/cursynth_cursynth_strings.o tmp/cursynth_delay.o tmp/cursynth_envelope.o tmp/cursynth_filter.o tmp/cursynth_linear_slope.o tmp/cursynth_midi_lookup.o tmp/cursynth_mono_panner.o tmp/cursynth_operators.o tmp/cursynth_oscillator.o tmp/cursynth_processor.o tmp/cursynth_processor_router.o tmp/cursynth_send_receive.o tmp/cursynth_smooth_filter.o tmp/cursynth_smooth_value.o tmp/cursynth_step_generator.o tmp/cursynth_trigger_operators.o tmp/cursynth_value.o tmp/cursynth_voice_handler.o tmp/cursynth_feedback.o tmp/cursynth_wave.o

# Remove test_runner_main.cpp from build to avoid conflicts
# test_runner_main.o: test_runner_main.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(CXX) $^ -o $@ $(LDFLAGS)

test_runner.o: test_runner.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

fake_audio_driver.o: fake_audio_driver.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

fft_analyzer.o: fft_analyzer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_reporter.o: test_reporter.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

midi_simulator.o: midi_simulator.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_helpers.o: test_helpers.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_engine.o: test_engine.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/MachineManager.o: ../machine/MachineManager.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_machine.o: ../machine/Ncursesynth/NcursesynthMachine.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/pbsynth_machine.o: ../machine/PBSynth/PBSynthMachine.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/pbsynth_synthengine.o: ../machine/PBSynth/synthengine.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/pbsynth_osctab.o: ../machine/PBSynth/osctab.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/pbsynth_lfo.o: ../machine/PBSynth/lfo.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/pbsynth_osc.o: ../machine/PBSynth/osc.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/pbsynth_envtab.o: ../machine/PBSynth/envtab.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/pbsynth_filter.o: ../machine/PBSynth/filter.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/pbsynth_coeff.o: ../machine/PBSynth/coeff.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/pbsynth_echo.o: ../machine/PBSynth/echo.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/pbsynth_env.o: ../machine/PBSynth/env.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/pbsynth_lfotab.o: ../machine/PBSynth/lfotab.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_moog_filter.o: ../machine/Ncursesynth/filters/moog_filter.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_high_pass_filter.o: ../machine/Ncursesynth/filters/high_pass_filter.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_korgms20_filter.o: ../machine/Ncursesynth/filters/korgms20_filter.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_oberheim_sem_filter.o: ../machine/Ncursesynth/filters/oberheim_sem_filter.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_svf_filter.o: ../machine/Ncursesynth/filters/svf_filter.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_saw_oscillator.o: ../machine/Ncursesynth/oscillators/saw_oscillator.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_adsr_envelope.o: ../machine/Ncursesynth/envelopes/adsr_envelope.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_flexible_envelope.o: ../machine/Ncursesynth/envelopes/flexible_envelope.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_synth_architecture.o: ../machine/Ncursesynth/synth/synth_architecture.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_voice.o: ../machine/Ncursesynth/synth/voice.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_delay.o: ../machine/Ncursesynth/effects/delay.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_reverb.o: ../machine/Ncursesynth/effects/reverb.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_chorus.o: ../machine/Ncursesynth/effects/chorus.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_distortion.o: ../machine/Ncursesynth/effects/distortion.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_effect_chain.o: ../machine/Ncursesynth/effects/effect_chain.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/ncursesynth_preset_manager.o: ../machine/Ncursesynth/preset/preset_manager.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_cursynth_machine.o: ../machine/Cursynth/CursynthMachine.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_cursynth_engine.o: ../machine/Cursynth/cursynth_engine.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_cursynth_strings.o: ../machine/Cursynth/cursynth_strings.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_delay.o: ../machine/Cursynth/delay.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_envelope.o: ../machine/Cursynth/envelope.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_filter.o: ../machine/Cursynth/filter.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_linear_slope.o: ../machine/Cursynth/linear_slope.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_midi_lookup.o: ../machine/Cursynth/midi_lookup.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_mono_panner.o: ../machine/Cursynth/mono_panner.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_operators.o: ../machine/Cursynth/operators.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_oscillator.o: ../machine/Cursynth/oscillator.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_processor.o: ../machine/Cursynth/processor.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_processor_router.o: ../machine/Cursynth/processor_router.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_send_receive.o: ../machine/Cursynth/send_receive.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_smooth_filter.o: ../machine/Cursynth/smooth_filter.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_smooth_value.o: ../machine/Cursynth/smooth_value.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_step_generator.o: ../machine/Cursynth/step_generator.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_trigger_operators.o: ../machine/Cursynth/trigger_operators.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_value.o: ../machine/Cursynth/value.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_voice_handler.o: ../machine/Cursynth/voice_handler.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/cursynth_feedback.o: ../machine/Cursynth/feedback.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
tmp/cursynth_wave.o: ../machine/Cursynth/wave.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f test_runner test/*.o tmp/*.o

