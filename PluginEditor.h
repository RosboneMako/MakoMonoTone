/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

struct t_KnobCoors {
    float x;
    float y;
    float sizex;
    float sizey;    
};

//R1.00 Create a new LnF class based on Juces LnF class. This lets us modify how objects are drawn to the screen.
//R1.00 Custom Controls.
class MakoLookAndFeel : public juce::LookAndFeel_V4
{
public:
    //R1.00 Let the user select a knob style.
    int MakoSliderKnobStyle = 2;
    
private:
    //R1.00 Ten tick mark angles around a slider.
    float TICK_Angle[11] = { 8.79645920, 8.29380417, 7.79114914, 7.28849411, 6.78583908, 6.28318405, 5.78052902, 5.27787399, 4.77521896, 4.27256393, 3.76 }; //3.76990914
    float TICK_Cos[11] = {};
    float TICK_Sin[11] = {};

    juce::Image imgSwitchOn;
    juce::Image imgSwitchOff;
        
public:
    MakoLookAndFeel()
    {
        imgSwitchOff = juce::ImageCache::getFromMemory(BinaryData::switchoff01_png, BinaryData::switchoff01_pngSize);
        imgSwitchOn = juce::ImageCache::getFromMemory(BinaryData::switchon01_png, BinaryData::switchon01_pngSize);

        //R1.00 Do some PRECALC on Sin/Cos since they are expensive on CPU.
        for (int t = 0; t < 11; t++)
        {
            TICK_Cos[t] = std::cosf(TICK_Angle[t]);
            TICK_Sin[t] = std::sinf(TICK_Angle[t]);
        }        
    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle, juce::Slider& sld) override
    {
        //R1.00 Are using this func to draw Switches (On (1)/Off(0) only).
        if(sld.getValue() < .5f)
            g.drawImageAt(imgSwitchOff, x, y);
        else
            g.drawImageAt(imgSwitchOn, x, y);
    }
    
    //R1.00 Override the Juce SLIDER drawing function so our code gets called instead of Juces code.
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& sld) override
    {
        //R1.00 Most of these are from JUCE demo code. Could be reduced if not used.
        //R1.00 Could PRECALC if they were all the same size control. 
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle); //R1.00 Bizarre values here 216(36) to 504(324).

        //R1.00 Mako Var defs.
        float sinA;
        float cosA;
        juce::ColourGradient ColGrad;
        int col;
       
        //1.00 Draw the KNOB face.
        //ColGrad = juce::ColourGradient(juce::Colour(0xFFF0F0E0), 0.0f, y, juce::Colour(0xFFA0A0A0), 0.0f, y + height, false);
        ColGrad = juce::ColourGradient(juce::Colour(0xFF808090), 0.0f, y, juce::Colour(0xFF303040), 0.0f, y + height, false);
        g.setGradientFill(ColGrad);
        g.fillEllipse(rx, ry, rw, rw);
        
        //R1.00 Draw shading around knob face.
        g.setColour(juce::Colour(0xFF000000));
        g.drawEllipse(rx - 1.0f, ry - 1.0f, rw + 2.0f, rw + 2.0f, 1.0f);

        //R1.00 Dont draw anymore objects if the control is disabled.
        if (sld.isEnabled() == false) return;

        //R1.00 Knob notches. Not used but left here for other coders to use.
        //ColGrad = juce::ColourGradient(juce::Colour(0xFFE0E0E0), 0.0f, y, juce::Colour(0xFF404040), 0.0f, y + height, false);
        //g.setGradientFill(ColGrad);
        //for (float ang = .7854f; ang < 6.0f; ang += .7854f)
        //{
        //    sinA = std::sinf(ang + angle) * radius;
        //    cosA = std::cosf(ang + angle) * radius;
        //    g.drawLine(centreX + (sinA * .9f), centreY - (cosA * .9f), centreX + sinA , centreY - cosA, 1.0f);
        //}

        //R1.00 TICK marks on background.
        //R1.00 We are cheating and using the rotarySliderOutlineColourId as a tick mark style selector.
        g.setColour(juce::Colour(0xFFC0C0C0));
        juce::Colour C1 = sld.findColour(juce::Slider::rotarySliderOutlineColourId);
        if (C1 == juce::Colour(0x1))
        {            
            for (int t = 0; t < 11; t++)
            {
                sinA = TICK_Sin[t] * radius;
                cosA = TICK_Cos[t] * radius;
                g.drawLine(centreX + (sinA * 1.2f), centreY - (cosA * 1.2f), centreX + sinA * 1.1f, centreY - cosA * 1.1f, 1.0f);
            }
        }
        if (C1 == juce::Colour(0x2))
        {
            sinA = TICK_Sin[0] * radius; cosA = TICK_Cos[0] * radius; g.drawLine(centreX + (sinA * 1.2f), centreY - (cosA * 1.2f), centreX + sinA * 1.1f, centreY - cosA * 1.1f, 1.0f);
            sinA = TICK_Sin[5] * radius; cosA = TICK_Cos[5] * radius; g.drawLine(centreX + (sinA * 1.2f), centreY - (cosA * 1.2f), centreX + sinA * 1.1f, centreY - cosA * 1.1f, 1.0f);
            sinA = TICK_Sin[10] * radius; cosA = TICK_Cos[10] * radius; g.drawLine(centreX + (sinA * 1.2f), centreY - (cosA * 1.2f), centreX + sinA * 1.1f, centreY - cosA * 1.1f, 1.0f);            
        }
        if (C1 == juce::Colour(0x3))
        {
            sinA = TICK_Sin[0] * radius; cosA = TICK_Cos[0] * radius; g.drawLine(centreX + (sinA * 1.2f), centreY - (cosA * 1.2f), centreX + sinA * 1.1f, centreY - cosA * 1.1f, 1.0f);
            sinA = TICK_Sin[10] * radius; cosA = TICK_Cos[10] * radius; g.drawLine(centreX + (sinA * 1.2f), centreY - (cosA * 1.2f), centreX + sinA * 1.1f, centreY - cosA * 1.1f, 1.0f);
        }

        //R1.00 Draw finger adjust dent/indicator.
        sinA = std::sinf(angle);
        cosA = std::cosf(angle);
        g.setColour(sld.findColour(juce::Slider::thumbColourId));
        g.drawLine(centreX + sinA * radius * .5f, centreY - cosA * radius * .5f, centreX + sinA * radius, centreY - cosA * radius, 4.0f);
       
    }
};


//R1.00 Add SLIDER listener. BUTTON or TIMER listeners also go here if needed. Must add ValueChanged overrides!
class MakoBiteAudioProcessorEditor  : public juce::AudioProcessorEditor , public juce::Slider::Listener //, public juce::Button::Listener , public juce::Timer
{
public:
    MakoBiteAudioProcessorEditor (MakoBiteAudioProcessor&);
    ~MakoBiteAudioProcessorEditor() override;

    //R1.00 OUR override functions.
    void sliderValueChanged(juce::Slider* slider) override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MakoBiteAudioProcessor& audioProcessor;

    //R1.00 Define the Look and Feel class we created.
    MakoLookAndFeel otherLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MakoBiteAudioProcessorEditor)

    juce::Image imgLogo;

    juce::ComboBox cbPreset;
    void cbPresetChanged();
    void cbPreset_UpdateSelection(int button, int idx, int editmode);
    void Preset01();
    void Preset02();
    void Preset03();
    void Preset04();
    void Preset05();
    void Preset06();
    
    void GUI_Init_Large_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix, int TickStyle, int ThumbColor);
    void GUI_Init_Small_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix);
    void GUI_Init_Switch_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix);
    
    //R1.00 Define our UI Juce Slider controls.
    int Knob_Cnt = 0;
    juce::Slider sldKnob[20];
    juce::Slider jsP1_Mono;
    
    //R1.00 Define the coords and text for our knobs. Not JUCE related. 
    t_KnobCoors Knob_Pos[20] = {};
    juce::String Knob_Name[20] = {};
    void KNOB_DefinePosition(int t, float x, float y, float sizex, float sizey, juce::String name);
    void KNOB_SetVoiceEnable();

    //R1.00 Add some context sensitive help text. 
    int ctrlHelp = 0;
    int ctrlHelpLast = 0;
    juce::Label labHelp;
    juce::String HelpString[20] =
    {
        "Adjust the volume for this effect.",
        "Select 1 of 10 different synth sounds. 0=bypass. Neck pickup best.",
        "Adjust Glissando(slide to pitch). Some helps avg pitch.",
        "Adjust mix between clean and synth signals.",
        "Low Pass. For low notes playing 100Hz is best. High notes 200 Hz.",
        "Adjust the balance between left/Right channels.",
        "Add dynamic sine modulation for more expressive playing.",
        "Compress/Smooth the picking dynamic range.",
        "Change note attack. 0-Fast. 1-Slow.",
        "Adjust the time between delay echoes.",
        "Adjust the number of echo repeats.",
        "Adjust the output to echo mix.",        
    };

    //R1.00 These are the indexes into our Settings var.
    enum { e_Gain, e_Voice, e_Gliss, e_Mix, e_LP, e_Bal, e_Boost, e_PreGain, e_Attack, e_DTime, e_DLen, e_DMix };

public:
    
    //R1.00 Define our SLIDER attachment variables.
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt[20];
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_Mono;

};
