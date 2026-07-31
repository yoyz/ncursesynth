#ifndef MASTER_EFFECTS_H
#define MASTER_EFFECTS_H

#include <memory>
#include <vector>

class EffectBase;

// Shared post-mix effects chain applied to the final output regardless of
// which synth engine is active. The chain is editable at runtime: effects can
// be added, removed and reordered. Parameters are exposed through a generic
// normalized (0..1) interface so the UI can render any chain uniformly.
class MasterEffects {
public:
    enum class EffectType { DELAY = 0, REVERB = 1, CHORUS = 2, DISTORTION = 3 };

    MasterEffects();
    ~MasterEffects();

    void setSampleRate(float rate);
    void reset();
    float process(float input);

    // --- Chain editing -----------------------------------------------------
    int getEffectCount() const { return (int)chain.size(); }
    EffectBase* getEffect(int index);
    const EffectBase* getEffect(int index) const;
    bool addEffect(EffectType type);
    bool insertEffect(int afterIndex, EffectType type);
    bool removeEffect(int index);
    bool moveEffect(int from, int to);
    bool setEffectEnabled(int index, bool enabled);
    bool isEffectEnabled(int index) const;
    const char* getEffectName(int index) const;

    // --- Generic parameter access (normalized 0..1) ------------------------
    int getEffectParamCount(int index) const;
    const char* getEffectParamName(int index, int param) const;
    float getEffectParam(int index, int param) const;
    void setEffectParam(int index, int param, float normalized);

    static const char* effectTypeName(EffectType type);

private:
    std::vector<std::unique_ptr<EffectBase>> chain;
    float sampleRate;
};

#endif
