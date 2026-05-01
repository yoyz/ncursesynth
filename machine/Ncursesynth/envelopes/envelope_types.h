#ifndef ENVELOPE_TYPES_H
#define ENVELOPE_TYPES_H

enum class EnvelopeCurve {
    LINEAR,      // Linear segments (standard ADSR)
    EXPONENTIAL, // Exponential curve for natural decay
    LOGARITHMIC, // Logarithmic curve (fast initial, slow tail)
    SINE,        // Sine-based curve for smooth shapes
    TENT,        // Fast attack, exponential decay
    REVERSE_TENT // Slow attack, fast decay
};

enum class EnvelopeStage {
    IDLE,
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE
};

#endif
