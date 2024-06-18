/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MakoBiteAudioProcessorEditor::MakoBiteAudioProcessorEditor (MakoBiteAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //R1.00 Create SLIDER ATTACHMENTS so our parameter vars get adjusted automatically for Get/Set states.
    ParAtt[e_Gain] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,  "gain",  sldKnob[e_Gain]);
    ParAtt[e_Voice] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "voice", sldKnob[e_Voice]);
    ParAtt[e_Gliss] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "gliss", sldKnob[e_Gliss]);
    ParAtt[e_Mix] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,   "mix",   sldKnob[e_Mix]);
    ParAtt[e_LP] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,    "lp",    sldKnob[e_LP]);
    ParAtt[e_Bal] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,   "bal",   sldKnob[e_Bal]);
    ParAtt[e_Boost] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "boost", sldKnob[e_Boost]);
    ParAtt[e_PreGain] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "pregain", sldKnob[e_PreGain]);
    ParAtt[e_Attack] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "attack", sldKnob[e_Attack]);
    ParAtt[e_DTime] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,   "dtime",   sldKnob[e_DTime]);
    ParAtt[e_DLen] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,    "dlen",    sldKnob[e_DLen]);
    ParAtt[e_DMix] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,    "dmix",    sldKnob[e_DMix]);    
    
    ParAtt_Mono = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "mono", jsP1_Mono);

    imgLogo = juce::ImageCache::getFromMemory(BinaryData::makologobo_png, BinaryData::makologobo_pngSize);

    //R1.00 PRESET COMBO BOX Def.
    cbPreset.setColour(juce::ComboBox::textColourId, juce::Colour(192, 192, 192));
    cbPreset.setColour(juce::ComboBox::backgroundColourId, juce::Colour(32, 32, 32));
    cbPreset.setColour(juce::ComboBox::arrowColourId, juce::Colour(192, 192, 192));
    cbPreset.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF804000));
    addAndMakeVisible(cbPreset);
    //cbPreset.addSectionHeading("Cool Header");
    cbPreset.addItem("---", 1);
    cbPreset.addItem("Sickmunk", 2);
    cbPreset.addItem("Tech Bass", 3);
    cbPreset.addItem("Pop Bass", 4);
    cbPreset.addItem("Space Boy", 5);
    cbPreset.addItem("Chellish", 6);
    cbPreset.addItem("Clean Pad", 7);
    cbPreset.onChange = [this] { cbPresetChanged(); };    //R1.00 PresetChanged is a func we create and gets called on combo selection.
    cbPreset.setSelectedId(1);

    //****************************************************************************************
    //R1.00 ADD GUI CONTROLS
    //****************************************************************************************
    //R1.00 Large Rotary Slider
    GUI_Init_Large_Slider(&sldKnob[e_Gain],  audioProcessor.Setting[e_Gain],  0.0f,  10.0f, .025f,"", 2, 0xFFc0c0c0);
    GUI_Init_Large_Slider(&sldKnob[e_Voice], audioProcessor.Setting[e_Voice],    0,  10.0,    1, "", 1, 0xFFFF8000);
    GUI_Init_Large_Slider(&sldKnob[e_Gliss],  audioProcessor.Setting[e_Gliss],  0.0f, 1.0f, .01f, "", 1, 0xFFFF8000);
    GUI_Init_Large_Slider(&sldKnob[e_Mix],  audioProcessor.Setting[e_Mix],  0.0f, 1.0f, .01f, "", 2, 0xFFFF8000);
    GUI_Init_Large_Slider(&sldKnob[e_LP],   audioProcessor.Setting[e_LP],   50,  500, 25, " Hz", 3, 0xFFFF8000);
    GUI_Init_Large_Slider(&sldKnob[e_Bal], audioProcessor.Setting[e_Bal], 0.0f, 1.0f, .01f, "", 2, 0xFFFF8000);
    GUI_Init_Large_Slider(&sldKnob[e_Boost], audioProcessor.Setting[e_Boost], 0.0f, 1.0f, .01f, "", 1, 0xFFFF8000);
    GUI_Init_Large_Slider(&sldKnob[e_PreGain], audioProcessor.Setting[e_PreGain], 0.0f, 1.0f, .01f, "", 1, 0xFFFF8000);
    GUI_Init_Large_Slider(&sldKnob[e_Attack], audioProcessor.Setting[e_Attack], 0.0f, 1.0f, .01f, "", 1, 0xFFFF8000);
    GUI_Init_Large_Slider(&sldKnob[e_DTime], audioProcessor.Setting[e_DTime], .01f, 1.0f, .01f, "", 1, 0xFFFF80FF);
    GUI_Init_Large_Slider(&sldKnob[e_DLen], audioProcessor.Setting[e_DLen], .0f, 1.0f, .01f, "", 1, 0xFFFF80FF);
    GUI_Init_Large_Slider(&sldKnob[e_DMix], audioProcessor.Setting[e_DMix], .0f, 1.0f, .01f, "", 2, 0xFFFF80FF);
    
    //R1.00 Move the DELAY sliders text box to the side of the knob instead of below (our default).
    sldKnob[e_DTime].setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 16);
    sldKnob[e_DLen].setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 16);
    sldKnob[e_DMix].setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 16);
    
    //R1.00 Keep track of how many knobs we have setup.
    Knob_Cnt = 12;
    
    //R1.00 Update the Look and Feel (Global colors) so drop down menu is the correct color. 
    getLookAndFeel().setColour(juce::DocumentWindow::backgroundColourId, juce::Colour(32, 32, 32));
    getLookAndFeel().setColour(juce::DocumentWindow::textColourId, juce::Colour(255, 255, 255));
    getLookAndFeel().setColour(juce::DialogWindow::backgroundColourId, juce::Colour(32, 32, 32));
    getLookAndFeel().setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0, 0, 0));
    getLookAndFeel().setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xFF804000));
    getLookAndFeel().setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF804000));
    getLookAndFeel().setColour(juce::TextButton::buttonColourId, juce::Colour(0, 0, 0));
    getLookAndFeel().setColour(juce::ComboBox::backgroundColourId, juce::Colour(0, 0, 0));
    getLookAndFeel().setColour(juce::ListBox::backgroundColourId, juce::Colour(32, 32, 32));
    getLookAndFeel().setColour(juce::Label::backgroundColourId, juce::Colour(32, 32, 32));

    //R1.00 Help Text object. 
    labHelp.setText("", juce::dontSendNotification);
    labHelp.setJustificationType(juce::Justification::centred);
    labHelp.setColour(juce::Label::backgroundColourId, juce::Colour(32, 32, 32));
    labHelp.setColour(juce::Label::textColourId, juce::Colour(192, 192, 192));
    labHelp.setColour(juce::Label::outlineColourId, juce::Colour(0xFF804000));
    addAndMakeVisible(labHelp);
    labHelp.setText("Mako MonoTone Version 1.0", juce::dontSendNotification);

    //R1.00 Define the knob (slider) positions on the screen/UI.
    KNOB_DefinePosition(e_Gain,   20, 75, 90, 90, "Gain");

    KNOB_DefinePosition(e_Voice, 125, 20, 70, 80, "Voice");
    KNOB_DefinePosition(e_Gliss, 200, 20, 70, 80, "Gliss");
    KNOB_DefinePosition(e_Mix,   275, 20, 70, 80, "Mix");
    KNOB_DefinePosition(e_Boost, 350, 20, 70, 80, "Boost");

    KNOB_DefinePosition(e_PreGain, 125, 130, 70, 80, "PreGain");
    KNOB_DefinePosition(e_LP,      200, 130, 70, 80, "Low Pass");
    KNOB_DefinePosition(e_Attack,  275, 130, 70, 80, "Attack");
    KNOB_DefinePosition(e_Bal,     350, 130, 70, 80, "Bal");
    
    KNOB_DefinePosition(e_DTime, 430,  20, 100, 45, "Delay Time");
    KNOB_DefinePosition(e_DLen,  430,  95, 100, 45, "Delay Repeat");
    KNOB_DefinePosition(e_DMix,  430, 170, 100, 45, "Delay Mix");
        
    //R1.00 Setup the small option sliders.
    GUI_Init_Switch_Slider(&jsP1_Mono, audioProcessor.Pedal_Mono, 0, 1, 1, "");
    
    //R1.00 Enable/Disable knobs based on VOICE setting.
    KNOB_SetVoiceEnable();

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
     
    //R1.00 Set the window size.
    setSize(540, 250);
}

MakoBiteAudioProcessorEditor::~MakoBiteAudioProcessorEditor()
{
}

//==============================================================================
void MakoBiteAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient ColGrad;

    //R1.00 Draw our GUI.
    //R1.00 Background.
    ColGrad = juce::ColourGradient(juce::Colour(0xFF202030), 0.0f, 0.0f, juce::Colour(0xFF505060), 0.0f, 80.0f, false);
    g.setGradientFill(ColGrad);
    g.fillRect(0, 0, 540, 80);
    ColGrad = juce::ColourGradient(juce::Colour(0xFF505060), 0.0f, 80.0f, juce::Colour(0xFF101020), 0.0f, 250.0f, false);
    g.setGradientFill(ColGrad);
    g.fillRect(0, 80, 540, 170);

    g.setColour(juce::Colour(0x20000000));
    g.fillRect(10, 2, 110, 210);

    //R1.00 Headers.
    //g.setColour(juce::Colour(0xFF202030));
    //for (int t = 0; t < Knob_Cnt; t++) g.fillRect(Knob_Pos[t].x, Knob_Pos[t].y - 14.0f, Knob_Pos[t].sizex, 14.0f);
    g.setFont(12.0f);
    g.setColour(juce::Colours::white); 
    for (int t = 0; t < Knob_Cnt; t++) g.drawFittedText(Knob_Name[t], Knob_Pos[t].x, Knob_Pos[t].y - 15, Knob_Pos[t].sizex, 15, juce::Justification::centred, 1);

    g.setColour(juce::Colour(0xFFF0F0F0));
    g.drawFittedText("Stereo/Mono", 0, 175, 130, 15, juce::Justification::centred, 1);
    
    //R1.00 Draw LOGO text.
    g.drawImageAt(imgLogo, 20, 5);
    //g.setColour(juce::Colour(0xFF804000));
    //g.fillRect(30, 30, 70, 15);
    //g.setColour(juce::Colour(0xFF000000));
    //g.setFont(14.0f);
    //g.drawFittedText("M a k o", 0, 30, 130, 15, juce::Justification::centred, 1);
    //g.setColour(juce::Colour(0xFFFFFFFF));
    //g.setFont(16.0f);
    //g.drawFittedText("MonoTone", 0, 14, 130, 15, juce::Justification::centred, 1);    
}

void MakoBiteAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //R1.00 Draw all of the defined KNOBS.
    for (int t = 0; t < Knob_Cnt;t++) sldKnob[t].setBounds(Knob_Pos[t].x, Knob_Pos[t].y, Knob_Pos[t].sizex, Knob_Pos[t].sizey);
    
    //R1.00 Add some switches (Sliders).
    jsP1_Mono.setBounds     (20, 190, 80, 20);
    
    //R1.00 Preset Dropdown List.
    cbPreset.setBounds  (10, 220, 110, 18);    

    //R1.00 Help Text / status bar.
    labHelp.setBounds  (125, 220, 410, 18);    
}

void MakoBiteAudioProcessorEditor::cbPresetChanged()
{
    //R1.00 JUCE calls this when a combobox item is selected. (set in defs above).
    
    //R1.00 User selected one of our premade sounds.
    int Sel = cbPreset.getSelectedId() - 1;
    if (Sel == 0) return;

    switch (Sel)
    {
        case 1:  Preset01(); break;   //R1.00 Sickmunk.
        case 2:  Preset02(); break;   //R1.00 TechBass.
        case 3:  Preset03(); break;   //R1.00 Pop Bass.    
        case 4:  Preset04(); break;   //R1.00 Space Boy.    
        case 5:  Preset05(); break;   //R1.00 Cellish.    
        case 6:  Preset06(); break;   //R1.00 Clean Pad.    
    }

    //R1.00 Flag the processor, we need to update settings.
    audioProcessor.SettingsChanged = true;    
}

void MakoBiteAudioProcessorEditor::cbPreset_UpdateSelection(int button, int idx, int editmode)
{    
}

//R1.00 Sickmunk
void MakoBiteAudioProcessorEditor::Preset01()
{
    sldKnob[e_Voice].setValue(3);
    sldKnob[e_Gliss].setValue(.9f);
    sldKnob[e_Mix].setValue(1.0f);
    sldKnob[e_Boost].setValue(.2f);
    sldKnob[e_PreGain].setValue(.4);
    sldKnob[e_LP].setValue(200);
    sldKnob[e_Attack].setValue(0);
    sldKnob[e_Bal].setValue(.5f);
    sldKnob[e_DTime].setValue(.4f);
    sldKnob[e_DLen].setValue(.25f);
    sldKnob[e_DMix].setValue(.35f);      
}

//R1.00 Tech Bass
void MakoBiteAudioProcessorEditor::Preset02()
{
    sldKnob[e_Voice].setValue(9);
    sldKnob[e_Gliss].setValue(.2f);
    sldKnob[e_Mix].setValue(1.0f);
    sldKnob[e_Boost].setValue(.5f);
    sldKnob[e_PreGain].setValue(.4);
    sldKnob[e_LP].setValue(100);
    sldKnob[e_Attack].setValue(0);
    sldKnob[e_Bal].setValue(.5f);
    sldKnob[e_DTime].setValue(.4f);
    sldKnob[e_DLen].setValue(.25f);
    sldKnob[e_DMix].setValue(.35f);
}

//R1.00 Pop Bass 
void MakoBiteAudioProcessorEditor::Preset03()
{
    sldKnob[e_Voice].setValue(10);
    sldKnob[e_Gliss].setValue(.0f);
    sldKnob[e_Mix].setValue(1.0f);
    sldKnob[e_Boost].setValue(.0f);
    sldKnob[e_PreGain].setValue(.6);
    sldKnob[e_LP].setValue(100);
    sldKnob[e_Attack].setValue(0);
    sldKnob[e_Bal].setValue(.5f);
    sldKnob[e_DTime].setValue(.4f);
    sldKnob[e_DLen].setValue(.25f);
    sldKnob[e_DMix].setValue(.35f);
}

//R1.00 Space Boy
void MakoBiteAudioProcessorEditor::Preset04()
{
    sldKnob[e_Voice].setValue(7);
    sldKnob[e_Gliss].setValue(.1f);
    sldKnob[e_Mix].setValue(1.0f);
    sldKnob[e_Boost].setValue(.0f);
    sldKnob[e_PreGain].setValue(.4);
    sldKnob[e_LP].setValue(200);
    sldKnob[e_Attack].setValue(0);
    sldKnob[e_Bal].setValue(.5f);
    sldKnob[e_DTime].setValue(.4f);
    sldKnob[e_DLen].setValue(.4f);
    sldKnob[e_DMix].setValue(.5f);
}

//R1.00 Cellish
void MakoBiteAudioProcessorEditor::Preset05()
{
    sldKnob[e_Voice].setValue(5);
    sldKnob[e_Gliss].setValue(.2f);
    sldKnob[e_Mix].setValue(1.0f);
    sldKnob[e_Boost].setValue(.0f);
    sldKnob[e_PreGain].setValue(.5);
    sldKnob[e_LP].setValue(200);
    sldKnob[e_Attack].setValue(.9);
    sldKnob[e_Bal].setValue(.5f);
    sldKnob[e_DTime].setValue(.3f);
    sldKnob[e_DLen].setValue(.6f);
    sldKnob[e_DMix].setValue(.5f);
}

//R1.00 Clean Pad
void MakoBiteAudioProcessorEditor::Preset06()
{
    //R1.00 Some settings are not used in Voice 0 (Off).
    sldKnob[e_Voice].setValue(0);
    //sldKnob[e_Gliss].setValue(.2f);
    //sldKnob[e_Mix].setValue(1.0f);
    //sldKnob[e_Boost].setValue(.0f);
    //sldKnob[e_PreGain].setValue(.4);
    //sldKnob[e_LP].setValue(200);
    sldKnob[e_Attack].setValue(.4);
    sldKnob[e_Bal].setValue(.5f);
    sldKnob[e_DTime].setValue(.2f);
    sldKnob[e_DLen].setValue(.9f);
    sldKnob[e_DMix].setValue(1.0f);
}


//R1.00 Setup the SLIDER control edit values, Text Suffix (if any), UI tick marks, and Indicator Color.
void MakoBiteAudioProcessorEditor::GUI_Init_Large_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix, int TickStyle, int ThumbColor)
{   
    //R1.00 Setup the slider edit parameters.
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    slider->setTextValueSuffix(Suffix);
    slider->setRange(Vmin, Vmax, Vinterval);
    slider->setValue(Val);    
    slider->addListener(this);
    addAndMakeVisible(slider);    

    //R1.00 Override the default Juce drawing routines and use ours.
    slider->setLookAndFeel(&otherLookAndFeel);

    //R1.00 Setup the type and colors for the sliders.
    slider->setSliderStyle(juce::Slider::SliderStyle::Rotary);
    slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xFFC08000));
    slider->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(0xFF804000));
    slider->setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0x00000000));    //R1.00 Make this SEE THRU. Alpha=0.
    slider->setColour(juce::Slider::thumbColourId, juce::Colour(ThumbColor));

    //R1.00 Cheat: We are using this color as a Tick Mark style selector in our drawing function.
    slider->setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(TickStyle));
}

void MakoBiteAudioProcessorEditor::GUI_Init_Small_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix)
{
    //R1.00 Setup the slider edit parameters.
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    slider->setRange(Vmin, Vmax, Vinterval);
    slider->setTextValueSuffix(Suffix);
    slider->setValue(Val);
    slider->addListener(this);
    addAndMakeVisible(slider);    
        
    //R1.00 Setup the type and colors for the sliders.
    slider->setSliderStyle(juce::Slider::LinearHorizontal);
    slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xFFA0A0A0));
    slider->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF202020));
    slider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(0xFFFF8000));
    slider->setColour(juce::Slider::trackColourId, juce::Colour(0xFFFF8000));
    slider->setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xFFF0F0F0));
}

void MakoBiteAudioProcessorEditor::GUI_Init_Switch_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix)
{
    //R1.00 Setup the slider edit parameters.
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    slider->setRange(Vmin, Vmax, Vinterval);
    slider->setTextValueSuffix(Suffix);
    slider->setValue(Val);
    slider->addListener(this);
    addAndMakeVisible(slider);

    //R1.00 Override the default Juce drawing routines and use ours to draw switch images.
    slider->setLookAndFeel(&otherLookAndFeel);    
}

void MakoBiteAudioProcessorEditor::KNOB_DefinePosition(int idx,float x, float y, float sizex, float sizey, juce::String name)
{
    //R1.00 Define our knob positions so we can just loop thru and draw them all easily.
    Knob_Pos[idx].x = x;
    Knob_Pos[idx].y = y;
    Knob_Pos[idx].sizex = sizex;
    Knob_Pos[idx].sizey = sizey;
    Knob_Name[idx] = name;
}

void MakoBiteAudioProcessorEditor::KNOB_SetVoiceEnable()
{    
    bool tMode = true;

    if (int(sldKnob[e_Voice].getValue()) == 0) tMode = false;

    //R1.00 Enable/Disable some SLIDERs.
    sldKnob[e_Gliss].setEnabled(tMode);
    sldKnob[e_Mix].setEnabled(tMode);
    sldKnob[e_Boost].setEnabled(tMode);
    sldKnob[e_PreGain].setEnabled(tMode);
    sldKnob[e_LP].setEnabled(tMode);    
}

void MakoBiteAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{   
    float v1;
    float v2;

    //R1.00 Special Case test for VOICE setting. Voice 0 = Off.
    if (slider == &sldKnob[e_Voice]) KNOB_SetVoiceEnable();
   
    //R1.00 Check which slider has been adjusted.
    for (int t = 0; t < Knob_Cnt; t++)
    {
        if (slider == &sldKnob[t])
        {
            //R1.00 Update HELP bar.
            labHelp.setText(HelpString[t], juce::dontSendNotification);

            //R1.00 Update the actual processor variable being edited.
            audioProcessor.Setting[t] = float(sldKnob[t].getValue());
            
            //R1.00 We need to update settings in processor.
            //R1.00 Increment changed var to be sure every change gets made. Changed var is decremented in processor.
            audioProcessor.SettingsChanged += 1; 

            //R1.00 We have captured the correct slider change, exit this function.
            return;
        }
    }
        
    //R1.00 Stereo MONO.
    if (slider == &jsP1_Mono)
    {
        labHelp.setText("Toggle Stereo or Mono operation.", juce::dontSendNotification);
        audioProcessor.Pedal_Mono = float(jsP1_Mono.getValue());
        return;
    }
    
    return;
}

