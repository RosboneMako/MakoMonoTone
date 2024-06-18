# MakoMonoTone
A demo JUCE VST mono guitar synthesizer and digital delay.
* Tested on Windows only.
* Written in Visual C++ 2022.
* Written for new programmers, not complicated.
* Version: 1.00
* Posted: June 17, 2024

VERSION
------------------------------------------------------------------
1.00 - Initial release.

DISCLAIMER
------------------------------------------------------------------  
This VST was written in a very simple way. Just simple code for
people who are not programmers but want to dabble in the magic of JUCE VSTs.

If you are new to JUCE code, the Mako Thump VST may be a better
starting place. Or be brave and try this.
       
SUMMARY
------------------------------------------------------------------
A Juce/C++ VST3 written to create a monophonic synth sound from your guitar.

![Demo Image](docs/assets/makomonotonedemo.png)

# THEORY OF OPERATION<br />
PSUEDO SYNTHESIZER  
This VST calculates the pitch of a guitar note by measuring the time betweem positive edge zero crossings.

![Sine Image](docs/assets/sinepitch.png)  
FIGURE 1  

In this example the sine wave crosses at time 0s and then again at time .01s. 
* Frequency/pitch f = 1 / (.01s - 0s) = 100Hz.

Our app will count the samples between crossings to get the time.
* Time = SampleCnt / SampleRate
* .01s = 480/48000

The measured pitch value is then used to create various waveforms. 

PURE SINE WAVE  
For our VST to work the guitar notes need to closely resemble a sine wave. Guitar notes rarley resemble a sine wave. There are usually a lot
of harmonics that create strange shapes. 

To get the easiest to detect notes, the guitar should use its neck pickup and have the tone control rolled all the way down. This helps remove 
the extra harmonic content. 

The VST also adds a LOW PASS filter to further remove unwanted harmonics.
* 100 Hz Low Pass is good for lower guitar notes.
* 200 Hz Low Pass is good for higher notes. 

GLISSANDO  
By blending the pitch with the previous pitch value, we can create a Glissando effect. This slows the change from note to note
to create a sliding up/dn whistle type effect. This effect also helps smooth out the pitch detection.

ATTACK  
The VST also creates a slow attack effect. This is useful for synth type pad effects when combined with the digital delay.

VOLUME ENVELOPE  
The generated synth notes use the volume of the incoming guitar signal as their volume. A PreGain control is added to add gain and smooth out
the volume differences. This makes the notes a little easier to control. 

BOOST  
A boost control is added to allow for some dynamic signal change. This effect adds high harmonics based on how loud the guitar is being played. 

NOTE: Some compression or OverDrive before the synth can help add sustain if the signal is not too distorted. 


