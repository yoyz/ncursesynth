CXX = g++
CXXFLAGS = -std=c++14 -Wall -O2 -pthread -I.
LDFLAGS = -lncurses -lportaudio -lrtmidi -lasound -lpthread -lm

TARGET = virtual_synth

SOURCES = main.cpp \
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
          audio/audio_engine.cpp \
          machine/Ncursesynth/preset/preset_manager.cpp \
          ui/ui.cpp \
          ui/ui_parameters.cpp \
          ui/ui_draw.cpp \
          ui/ui_draw_util.cpp \
          ui/ui_input.cpp \
          ui/ui_constants.cpp \
          ui/pbsynth_ui.cpp \
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
