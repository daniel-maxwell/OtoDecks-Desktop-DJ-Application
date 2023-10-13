/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 6 Mar 2023 2:36:05pm
    Author:  Dan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
                                 juce::AudioThumbnailCache& cacheToUse) : 
                                 audioThumb(1000, formatManagerToUse, cacheToUse),
                                 fileLoaded(false),
                                 position(0)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::orange);

    if (fileLoaded)
    {
        audioThumb.drawChannel( g,
                                getLocalBounds(),
                                0,
                                audioThumb.getTotalLength(),
                                0,
                                1.0f);
        g.setColour(juce::Colours::lightgreen);
        if (position >= 0)
        {
            g.drawRect(position * getWidth(), 0, getWidth() / 80, getHeight());
        }

    }
    else
    {
        g.setFont(24.0f);
        g.drawText("No file currently loaded...", getLocalBounds(),
        juce::Justification::centred, true);   // draw some placeholder text
    }
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}


void WaveformDisplay::loadURL(juce::URL audioURL)
{
    DBG("Waveformdisplay loadURL");
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));

    if (fileLoaded)
    {
        DBG("WFD loaded successfully!"); 
    }
    else 
    {
        DBG("WFD Failed to load WF.");
    }
}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    DBG("Change received");
    repaint();
}

void  WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }
}