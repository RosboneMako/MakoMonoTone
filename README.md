# MakoMonoTone
A demo JUCE VST mono guitar synthesizer.
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

