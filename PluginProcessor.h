/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class MakoBiteAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    MakoBiteAudioProcessor();
    ~MakoBiteAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //R1.00 Add a Parameters variable.
    juce::AudioProcessorValueTreeState parameters;                           
    
    //R1.00 Our public variables.
    int SettingsChanged = 0;
    int SettingsType = 0;
    float Setting[30] = {};
    float Setting_Last[30] = {};

    int Pedal_Mono = 1;
    
    //R1.00 These are the indexes into our Settings var.
    enum { e_Gain, e_Voice, e_Gliss, e_Mix, e_LP, e_Bal, e_Boost, e_PreGain, e_Attack, e_DTime, e_DLen, e_DMix };

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MakoBiteAudioProcessor)
   
    //R1.00 Functions to clean up parameter gets.
    int makoGetParmValue_int(juce::String Pstring);
    float makoGetParmValue_float(juce::String Pstring);

    //R1.00 We need a gain adjuster for BOOST.
    float BOOST_Gain = 1.0f;

    //R1.00 Balance settings are non linear so we need separate vars to track it.
    float Pedal_Bal1LR[2] = { 1.0f, 1.0f };
    float Pedal_Bal2LR[2] = { 1.0f, 1.0f };

    //R1.00 This VST uses LOW PASS filters to try and get the guitar signal as close to a sine wave as possible.
    //R1.00 We can then measure the period of the waveform to get the note being played. 
    //R1.00 We measure as the signal goes from negative to positive.
    int Mod_PitchCnt[2] = {  };       //R1.00 How many samples per Zero Crossing.
    float Mod_PitchInc[2] = {  };     //R1.00 How many samples to make a SIN wave over.
    float Mod_Sin[2] = {  };          //R1.00 Current angle of our sine wave generator. 
    float Mod_Peak[2] = {  };         //R1.00 Need to track how loud the person is playing and scale our sig gen value to it.      
    float Mod_LastSample[2] = {};     //R1.00 Store last vals so we can check if we are going NEG to POS.

    //R1.00 These variables are used for the ATTACK envelope code.
    float Signal_VolFade[2] = {};
    float Signal_AVG[2] = {};
    bool Signal_VolFadeOn[2] = { false, false };
    bool Signal_VolFadeReset[2] = { false, false };

    //R1.00 Digital Delay.
    float Delay_Dry = 1.0f;
    float Delay_Wet = 1.0f;
    float Delay_B[2][192010] = { };   //R1.00 Delay Buffer. 1 second @ 192kHz sample rate. 
    int Delay_B_Idx[2] = { };
    int Delay_B_Idx_Max[2] = { };

    //R1.00 Some Constants and vars.
    const float pi = 3.14159265f;
    const float pi2 = 6.2831853f;
    const float sqrt2 = 1.4142135f;
    float SampleRate = 48000.0f;

    //R1.00 OUR FILTER VARIABLES
    struct tp_coeffs {
        float a0;
        float a1;
        float a2;
        float b1;
        float b2;
        float c0;
        float d0;
    };

    struct tp_filter {
        float a0;
        float a1;
        float a2;
        float b1;
        float b2;
        float c0;
        float d0;
        float xn0[2];
        float xn1[2];
        float xn2[2];
        float yn1[2];
        float yn2[2];
        float offset[2];
    };

    //R1.00 FILTERS
    float Filter_Calc_BiQuad(float tSample, int channel, tp_filter* fn);
    void Filter_BP_Coeffs(float Gain_dB, float Fc, float Q, tp_filter* fn);
    void Filter_LP_Coeffs(float fc, tp_filter* fn);
    void Filter_HP_Coeffs(float fc, tp_filter* fn);

    //R1.00 Our filters and function def.
    tp_filter makoF_HiCut1 = {};
    tp_filter makoF_HiCut2 = {};
    tp_filter makoF_LoCut = {};
    
    void Balance_CalcSettings(bool ForceAll);
    void Filter_CalcSettings(bool ForceAll);
    void Mako_Update_Delay(bool ForceAll);

    float Mako_FX_MonoToneSyn(float tSample, int channel);
    float Mako_FX_Attack(float tSample, int channel);
    float Mako_FX_Delay(float tSample, int channel);

    //R1.00 Handle any paramater changes.
    void Settings_Update(bool ForceAll);
        
};
