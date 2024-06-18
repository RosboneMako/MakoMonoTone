/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MakoBiteAudioProcessor::MakoBiteAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
    ),
    
    //R1.00 Define our VALUE TREE paramters.
    parameters(*this, nullptr, "PARAMETERS", 
      {
        std::make_unique<juce::AudioParameterFloat>("gain","Gain",         .0f, 10.0f, 1.0f),
        std::make_unique<juce::AudioParameterInt>("voice","Voice",           0,   10, 1),
        std::make_unique<juce::AudioParameterFloat>("gliss","Gliss",       .0f, 1.0f, .24f),
        std::make_unique<juce::AudioParameterFloat>("mix","Mix",           .0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterInt>("lp","Low Pass",          50,  500, 200),
        std::make_unique<juce::AudioParameterFloat>("bal","Bal",           .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("boost","Boost",       .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("pregain","PreGain",   .0f, 1.0f, .2f),
        std::make_unique<juce::AudioParameterFloat>("attack","Attack",     .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("dtime","Delay Time", .01f, 1.0f, .4f),
        std::make_unique<juce::AudioParameterFloat>("dlen","Del Repeat",  .0f, 1.0f, .2f),
        std::make_unique<juce::AudioParameterFloat>("dmix","Delay Mix",   .0f, 1.0f, .1f),

        std::make_unique<juce::AudioParameterInt>("mono","Mono",    0, 1, 1),
        
      }
    )   

#endif
{   
}

MakoBiteAudioProcessor::~MakoBiteAudioProcessor()
{
}

//==============================================================================
const juce::String MakoBiteAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MakoBiteAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MakoBiteAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MakoBiteAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MakoBiteAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MakoBiteAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MakoBiteAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MakoBiteAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MakoBiteAudioProcessor::getProgramName (int index)
{
    return {};
}

void MakoBiteAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MakoBiteAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..


    //R1.00 Get our Sample Rate for filter calculations.
    SampleRate = MakoBiteAudioProcessor::getSampleRate();
    if (SampleRate < 21000) SampleRate = 48000;
    if (192000 < SampleRate) SampleRate = 48000;
        
    //R1.00 Update things that need updating as the program is running normally.
    //R1.00 Force every setting to be calculated.
    Settings_Update(true);    
}

void MakoBiteAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MakoBiteAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MakoBiteAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //R1.00 Our defined variables.
    float tS;  //R1.00 Temporary Sample.
    float tSOrg;

    //R1.00 Handle any changes to our Parameters in the Editor. 
    //R1.00 Dont force all updates. Just change things that have changed since last check.
    if (0 < SettingsChanged) Settings_Update(false);

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        //*********************************************************
        //R1.00 Process the AUDIO buffer data. Apply our effects.
        //*********************************************************
        if (Pedal_Mono && (channel == 1))
        {
            auto* channel0Data = buffer.getWritePointer(0);

            //R1.0 FORCE MONO - Put CHANNEL 0 data in CHANNEL 1.
            for (int samp = 0; samp < buffer.getNumSamples(); samp++) channelData[samp] = channel0Data[samp];
        }
        else
        {
            // ..do something to the data...
            for (int samp = 0; samp < buffer.getNumSamples(); samp++)
            {
                //R1.00 Get the current sample and put it in tS. 
                tS = buffer.getSample(channel, samp);
                tSOrg = tS;

                //R1.00 Apply the ATTACK effect.
                tS = Mako_FX_Attack(tS, channel);

                //R1.00 Calc pitch and create the synth sound.
                tS = Mako_FX_MonoToneSyn(tS, channel);
               
                //R1.00 Mix original sample and new modified synth sample. 
                tS = (tSOrg * (1.0f - Setting[e_Mix])) + (tS * Setting[e_Mix]);

                //R1.00 Reduce vol.We dont want to exceed - 1 / 1.
                //R1.00 If tSOrg = 1 and tS = 1 that = 2. Which is bad.
                tS = tS * .5f;

                //R1.00 Add stereo Digital Delay. 
                tS = Mako_FX_Delay(tS, channel);

                //R1.00 Write our modified sample back into the sample buffer.
                channelData[samp] = tS * Setting[e_Gain];
            }
        }
        //**************************************************

    }
}

//==============================================================================
bool MakoBiteAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MakoBiteAudioProcessor::createEditor()
{
    return new MakoBiteAudioProcessorEditor (*this);
}


//==============================================================================
void MakoBiteAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    //R1.00 Save our VALUE TREE parameters to file/DAW.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);   
}

void MakoBiteAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    //R1.00 Read our VALUE TREE parameters from file/DAW.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));

    //R1.00 Get our parameters from the new settings.
    Setting[e_Gain] = makoGetParmValue_float("gain");
    Setting[e_Voice] = makoGetParmValue_int("voice");
    Setting[e_Gliss] = makoGetParmValue_float("gliss");
    Setting[e_Mix] = makoGetParmValue_float("mix");
    Setting[e_LP] = makoGetParmValue_int("lp");    
    Setting[e_Bal] = makoGetParmValue_float("bal");
    Setting[e_Boost] = makoGetParmValue_float("boost");
    Setting[e_PreGain] = makoGetParmValue_float("pregain");
    Setting[e_Attack] = makoGetParmValue_float("attack");
    Setting[e_DTime] = makoGetParmValue_float("dtime");
    Setting[e_DLen] = makoGetParmValue_float("dlen");
    Setting[e_DMix] = makoGetParmValue_float("dmix");    
    
    Pedal_Mono = makoGetParmValue_int("mono");
    
    //R1.00 Force all settings to be updated.
    Settings_Update(true);
}

int MakoBiteAudioProcessor::makoGetParmValue_int(juce::String Pstring)
{
    //R1.00 Helper func that makes parameters easier to deal with.
    auto parm = parameters.getRawParameterValue(Pstring);
    if (parm != NULL)
        return int(parm->load());
    else
        return 0;
}

float MakoBiteAudioProcessor::makoGetParmValue_float(juce::String Pstring)
{
    //R1.00 Helper func that makes parameters easier to deal with.
    auto parm = parameters.getRawParameterValue(Pstring);
    if (parm != NULL)
        return float(parm->load());
    else
        return 0.0f;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MakoBiteAudioProcessor();
}

//R1.00 Actual filter calculation code that modifies our sample.
float MakoBiteAudioProcessor::Filter_Calc_BiQuad(float tSample, int channel, tp_filter* fn)
{
    float tS = tSample;

    fn->xn0[channel] = tS;
    tS = fn->a0 * fn->xn0[channel] + fn->a1 * fn->xn1[channel] + fn->a2 * fn->xn2[channel] - fn->b1 * fn->yn1[channel] - fn->b2 * fn->yn2[channel];
    fn->xn2[channel] = fn->xn1[channel]; fn->xn1[channel] = fn->xn0[channel]; fn->yn2[channel] = fn->yn1[channel]; fn->yn1[channel] = tS;

    return tS;
}

//R1.00 Second order parametric/peaking boost filter with constant-Q. fc=Cutoff Frequency. Q=Filter width (.707 def).
void MakoBiteAudioProcessor::Filter_BP_Coeffs(float Gain_dB, float Fc, float Q, tp_filter* fn)
{    
    float K = pi2 * (Fc * .5f) / SampleRate;
    float K2 = K * K;
    float V0 = pow(10.0, Gain_dB / 20.0);

    float a = 1.0f + (V0 * K) / Q + K2;
    float b = 2.0f * (K2 - 1.0f);
    float g = 1.0f - (V0 * K) / Q + K2;
    float d = 1.0f - K / Q + K2;
    float dd = 1.0f / (1.0f + K / Q + K2);

    fn->a0 = a * dd;
    fn->a1 = b * dd;
    fn->a2 = g * dd;
    fn->b1 = b * dd;
    fn->b2 = d * dd;
    fn->c0 = 1.0f;
    fn->d0 = 0.0f;
}

//R1.00 Second order LOW PASS filter.  fc=Cutoff Frequency.
void MakoBiteAudioProcessor::Filter_LP_Coeffs(float fc, tp_filter* fn)
{
    float c = 1.0f / (tanf(pi * fc / SampleRate));
    fn->a0 = 1.0f / (1.0f + sqrt2 * c + (c * c));
    fn->a1 = 2.0f * fn->a0;
    fn->a2 = fn->a0;
    fn->b1 = 2.0f * fn->a0 * (1.0f - (c * c));
    fn->b2 = fn->a0 * (1.0f - sqrt2 * c + (c * c));
}

//F1.00 Second order butterworth High Pass. fc=Cutoff Frequency.
void MakoBiteAudioProcessor::Filter_HP_Coeffs(float fc, tp_filter* fn)
{ 
    float c = tanf(pi * fc / SampleRate);
    fn->a0 = 1.0f / (1.0f + sqrt2 * c + (c * c));
    fn->a1 = -2.0f * fn->a0;
    fn->a2 = fn->a0;
    fn->b1 = 2.0f * fn->a0 * ((c * c) - 1.0f);
    fn->b2 = fn->a0 * (1.0f - sqrt2 * c + (c * c));
}


void MakoBiteAudioProcessor::Balance_CalcSettings(bool ForceAll)
{
    //R1.00 Create left/right volume settings based on balance setting.
    //R1.00 These are multiplied by our chorus effect in processing.

    //R1.00 BALANCE settings.
    if ((Setting[e_Bal] != Setting_Last[e_Bal]) || ForceAll)
    {
        Setting_Last[e_Bal] = Setting[e_Bal];
        if (Setting[e_Bal] < .5f)
        {
            Pedal_Bal1LR[0] = 1.0f;
            Pedal_Bal1LR[1] = Setting[e_Bal] * 2.0f;
        }
        else
        {
            Pedal_Bal1LR[0] = 1.0f - ((Setting[e_Bal] - .5f) * 2.0f);
            Pedal_Bal1LR[1] = 1.0f;
        }
    }
   
}

void MakoBiteAudioProcessor::Filter_CalcSettings(bool ForceAll)
{
    if ((Setting[e_LP] != Setting_Last[e_LP]) || ForceAll)
    {
        Setting_Last[e_LP] = Setting[e_LP];
        Filter_LP_Coeffs(Setting[e_LP], &makoF_HiCut1);        
    }    
}

void MakoBiteAudioProcessor::Mako_Update_Delay(bool ForceAll)
{
    //R2.00 Adjust the DELAY mix. 
    if ((Setting[e_DMix] != Setting_Last[e_DMix]) || ForceAll)
    {
        if (Setting[e_DMix] < .5f)
        {
            Delay_Dry = 1.0f;
            Delay_Wet = Setting[e_DMix] * 2;
        }
        else
        {
            Delay_Dry = 1.0f - ((Setting[e_DMix] - .5f) * 2.0f);
            Delay_Wet = 1.0f;
        }
    }

    //R1.00 DELAY Settings.
    if ((Setting[e_DTime] != Setting_Last[e_DTime]) || ForceAll)
    {
        Setting_Last[e_DTime] = Setting[e_DTime];        

        //R1.00 Create the Left channel Echo Index and End of Buffer (Max).
        Delay_B_Idx[0] = 2 * Setting[e_DTime] * SampleRate;
        Delay_B_Idx_Max[0] = (2 * Setting[e_DTime] * SampleRate) + 1;
        
        //R1.00 Create the Right channel Echo Index and End of Buffer (Max).
        //R1.00 Cut Delay Time in half so we have a stereo echo.
        Delay_B_Idx[1] = 2 * Setting[e_DTime] * .5f * SampleRate;
        Delay_B_Idx_Max[1] = (2 * Setting[e_DTime] * .5f * SampleRate) + 1;        
    }
}



float MakoBiteAudioProcessor::Mako_FX_MonoToneSyn(float tSample, int channel)
{
    float tS = tSample;
    float tS2 = tSample;
    float Gliss = Setting[e_Gliss] - .01f;

    //R1.00 Exit if not even using Synth.
    if ((int(Setting[e_Voice]) == 0) || (Setting[e_Mix] < .001f)) return tSample;

    // VOLUME ENVELOPE CODE ******************************************************************************
    //R1.00 Apply some psuedo compression to the peak value. To smooth out the picking dynamic range.
    //R1.00 This func does not exeed -1/1 so it is volume safe.
    float tP = abs(tanhf(tS * (.01f + Setting[e_PreGain]) * 8.0f));

    //R1.00 Slowly decrease our peak detected volume. Set to new Peak if applicable.
    Mod_Peak[channel] *= .995f;
    if (Mod_Peak[channel] < tP) Mod_Peak[channel] = tP;
    // VOLUME ENVELOPE CODE ******************************************************************************

    // PITCH DETECTION CODE ******************************************************************************
    //R1.00 Update our Sample Count since the last ZERO crossing..
    //R1.00 Our pitch is sample counts between crossings.
    Mod_PitchCnt[channel]++;

    //R1.00 Low Pass filter on incoming signal to reduce highs. The more we cut the closer to a sine
    //R1.00 wave we get and the better our tracking is. Too much and high notes stop working.
    tS = Filter_Calc_BiQuad(tS, channel, &makoF_HiCut1);
    
    //R1.00 Find Rising Edge ZERO crossing. Update Pitch change rate. Store last Sample value.
    //R1.00 Here is the heart of the app. We calc pitch from samples per crossing. Then blend the new pitch to create Glissando effect.
    if ((Mod_LastSample[channel] < 0.0f) && (0.0f < tS))
    {
        //R1.00 Blend new pitch with old for Glissando. PI2 = 6.263
        Mod_PitchInc[channel] = (Mod_PitchInc[channel] * Gliss) + ((pi2 / Mod_PitchCnt[channel]) * (1.0f - Gliss));

        //R1.00 Limit our highest pitch so noise doesnt drive it higher. Probably dont need this. Needs to be SampleRate dependent.
        //if (.16f < Mod_PitchInc[channel]) Mod_PitchInc[channel] = .16f;

        Mod_PitchCnt[channel] = 0; //R1.00 Reset our sample counter.
    }
    Mod_LastSample[channel] = tS;
    // PITCH DETECTION CODE ******************************************************************************

    // SYNTH SOUND GENERATION CODE ******************************************************************************
    //R1.00 Increment our sig gen and limit range to 0.0 - (X*PI) or the loss of floating point resolution causes errors.
    Mod_Sin[channel] += Mod_PitchInc[channel];
    if ((2.0f * pi2) < Mod_Sin[channel]) Mod_Sin[channel] -= (2.0f * pi2);

    //R1.00 Create the SINE wave gen signal.
    //R1.00 This is not the fastest/best way to do this probably. Lambda function maybe? SINF are also expensive for CPU.
    switch (int(Setting[e_Voice]))
    {
        case 1:tS2 = (sinf(Mod_Sin[channel]) + sinf(Mod_Sin[channel] * 2.0f)); break;
        case 2:tS2 = .75f * ((cosf(Mod_Sin[channel]) + sinf(Mod_Sin[channel] * 2.0f) + sinf(Mod_Sin[channel] * 1.5833333f))); break;
        case 3: (0.0f < sinf(Mod_Sin[channel])) ? tS2 = .5f : tS2 = -.5f;   break;
        case 4:
            (0.0f < sinf(Mod_Sin[channel])) ? tS2 = 1.0f : tS2 = -1.0f;
            tS2 += sinf(Mod_Sin[channel] * 4.0f);
            tS2 *= .333f;
            break;
        case 5:tS2 = sinf(Mod_Sin[channel]) + sinf(Mod_Sin[channel] * 1.5833333f); break;
        case 6:tS2 = sinf(Mod_Sin[channel]) + sinf(Mod_Sin[channel] * 2.0f); break;
        case 7:tS2 = sinf(Mod_Sin[channel] * 2.0f) + (sinf(Mod_Sin[channel] * 4.0f) * .1f); break;
        case 8:tS2 = sinf(Mod_Sin[channel]) + (sinf(Mod_Sin[channel] * 2.0f) * .1f); break;
        case 9:tS2 = sinf(Mod_Sin[channel] * .5f) + (sinf(Mod_Sin[channel] * 4.0f) * .1f); break;
        case 10: 
            (0.0f < sinf(Mod_Sin[channel] * .5f)) ? tS2 = 1.0f : tS2 = -1.0f;
            tS2 += sinf(Mod_Sin[channel] * 1.3340909f);
            tS2 *= .333f;
            break;
        //R1.00 Default for when things go horribly wrong.
        default: tS2 = 1.5f * sinf(Mod_Sin[channel]); break;
    }
    // SYNTH SOUND GENERATION CODE ******************************************************************************

    //R1.00 Scale the volume to our peak vol.
    tS2 *= Mod_Peak[channel];

    //R1.00 Apply BOOST if selected. Gain calculated in SettingsUpdate.
    if (0.0f < Setting[e_Boost]) tS2 = BOOST_Gain * sinf(tS2 * Setting[e_Boost] * 50.0f);        
    
    //R1.00 Return the BALANCE adjusted signal.
    return tS2 * Pedal_Bal1LR[channel];
}

void MakoBiteAudioProcessor::Settings_Update(bool ForceAll)
{
    //R1.00 We do changes here so we know the vars are not in use while we change them.
    //R1.00 EDITOR sets SETTING flags and we make changes here.

    //R1.00 Update our Filters.
    Filter_CalcSettings(ForceAll);

    //R1.00 Update our BALANCE settings.
    Balance_CalcSettings(ForceAll);

    //R1.00 Update the delay settings.
    Mako_Update_Delay(ForceAll);

    //R1.00 Reduce gain to compensate for the added gain and harmonics (rms value).
    //R1.00 It is put here because it needs to be more complex than this.
    //R1.00 Basically the most gain happens from 0-.25 depending on PreGain setting.
    if (0.0f < Setting[e_Boost])
    {
        BOOST_Gain = 1.0f - (Setting[e_Boost] * 5.0f);
        if (BOOST_Gain < .1f) BOOST_Gain = .1f;
    }

    //R1.00 RESET our settings flags.
    SettingsChanged -= 1;
    if (SettingsChanged < 0) SettingsChanged = 0;

}

float MakoBiteAudioProcessor::Mako_FX_Attack(float tSample, int channel)
{
    //R1.00 Attack is turned off (0.0) so skip this code and return.
    if (Setting[e_Attack] < .001f) return tSample;

    //R1.00 Calculate our average incoming signal. Blend it for some fixed amount of time. Needs SampleRate calc.
    Signal_AVG[channel] = (Signal_AVG[channel] * .995f) + (abs(tSample) * .005f);

    //R1.00 A slow envelope attack for violin/synth effects.
    //R1.00 Detect when a note is played. And retrigger the Attack fade in.
    //R1.00 This code lets players play non-attacked notes if no silence is between notes.
    if (!Signal_VolFadeOn[channel] && (.001f < tSample))
    {
        Signal_VolFadeOn[channel] = true;
        Signal_VolFade[channel] = 0.0f;
    }

    //R1.00 Check for Note off period.
    if (Signal_AVG[channel] < .0001f) Signal_VolFadeOn[channel] = false;

    //R1.00 Ramp up or down the effect volume based on if playing or not.
    if (.0005f < Signal_AVG[channel])
        Signal_VolFade[channel] += .000001f + (1.0f - Setting[e_Attack]) * .0001f; //R1.00 Fade in.
    else
        Signal_VolFade[channel] *= -.995f;                                         //R1.00 Fade out.

    //R1.00 Clip the volume near unity.
    if (.9999f < Signal_VolFade[channel]) Signal_VolFade[channel] = .9999f;

    return tSample * Signal_VolFade[channel];
}


//R1.00 DIGITAL DELAY.
float MakoBiteAudioProcessor::Mako_FX_Delay(float tSample, int channel)
{
    //R1.00 Exit if not even using Delay.
    if (Setting[e_DMix] < .001f) return tSample;

    //R1.00 Get the index into our Delay buffer where our current echo is.
    long idx = Delay_B_Idx[channel];

    //R1.00 Mix our signal with the echo.
    float NewSignal = (tSample * Delay_Dry) + ((Delay_B[channel][idx] * Delay_Wet));

    //R1.00 Update the buffer with our new sample and old echo mixed. 
    //R1.00 We cant exceed -1/1 so we put in .5f sample volume. .5+.5 = 1 (safe).
    Delay_B[channel][idx] = (.5f * tSample) + (Delay_B[channel][idx] * Setting[e_DLen]);

    //R1.00 Update delay buffer position. 
    Delay_B_Idx[channel]++;

    //R1.00 If the index is past the buffer limit, go back to the start of the buffer. Wrap around.
    if (Delay_B_Idx_Max[channel] < Delay_B_Idx[channel]) Delay_B_Idx[channel] = 0;

    return NewSignal;
}



