/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 3 Mar 2023 6:23:03pm
    Author:  Dan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DJAudioPlayer : public juce::AudioSource
{
    public:
        DJAudioPlayer(juce::AudioFormatManager& _formatManager);
        ~DJAudioPlayer();

        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
        void releaseResources() override;

        void loadURL(juce::URL audioURL);
        void setGain(double gain);
        void setHighGain(double gain);
        void setMidGain(double gain);
        void setLowGain(double gain);
        void setSpeed(double ratio);
        void setPosition(double posInSecs);
        void setPositionRelative(double pos);

        void start();
        void pause();
        void stop();
        bool checkIfPaused();
        double tempoValue = 0.10471;
        void movePlayheadWhilePaused(double newPos);

        //** gets the relative pos of the playhead
        double const getPositionRelative();
        double const getTrackLength();
        bool trackLoaded = false;
        bool playing = false;

    private:
        juce::AudioFormatManager& formatManager;
        std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
        juce::AudioTransportSource transportSource;
        juce::ResamplingAudioSource resampleSource{&transportSource, false, 2};

        float lastSampleRate = 48000;
        
        bool paused = false;
        double positionAtPause = 0.0;

        std::array<juce::IIRCoefficients, 2> highGainCoef;
        std::array<juce::IIRFilter, 2> highGainControl;

        std::array<juce::IIRCoefficients, 2> midGainCoef;
        std::array<juce::IIRFilter, 2> midGainControl;

        std::array<juce::IIRCoefficients, 2> lowGainCoef;
        std::array<juce::IIRFilter, 2> lowGainControl;

};