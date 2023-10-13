/*
  ==============================================================================

    DeckGUI.h
    Created: 5 Mar 2023 11:10:21am
    Author:  Dan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <math.h>
#include <numbers>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"

//==============================================================================
/*
*/
class DeckGUI  : public juce::Component,
                 public juce::Button::Listener,
                 public juce::Slider::Listener,
                 public juce::FileDragAndDropTarget,
                 public juce::Timer,
                 public juce::LookAndFeel_V4          
{
public:
    DeckGUI(DJAudioPlayer* player,
            juce::AudioFormatManager& formatManagerToUse,
            juce::AudioThumbnailCache& cacheToUse,
            int deckNum);

    ~DeckGUI() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;

    /* Implement Button::Listener*/
    void buttonClicked(juce::Button*) override;

    /** implement Slider::Listener */
    void sliderValueChanged(juce::Slider* slider);

    /** function to create a Dial appearance for a slider */
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override;

    bool isInterestedInFileDrag(const juce::StringArray &files) override;

    void filesDropped(const juce::StringArray &files, int x, int y) override;

    void timerCallback() override;

private:
    juce::TextButton playButton{ "PLAY" };
    juce::TextButton pauseButton{ "PAUSE" };
    juce::TextButton stopButton{ "STOP" };
    juce::TextButton loadButton{ "LOAD" };
    
    juce::Slider gainSlider;
    juce::Slider posSlider;
    juce::Slider tempoDial;
    juce::Slider highGainDial;
    juce::Slider midGainDial;
    juce::Slider lowGainDial;

    DJAudioPlayer* player;

    WaveformDisplay waveformDisplay;

    int deckNumber;

    float notchAngleInRadians = 0;
    float deckSpeed = player->tempoValue;

    juce::FileChooser fChooser{ "Select a file..." };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI);
};
