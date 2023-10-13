/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 3 Mar 2023 6:23:03pm
    Author:  Dan

  ==============================================================================
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& _formatManager)
                            : formatManager(_formatManager)
{}

DJAudioPlayer::~DJAudioPlayer() 
{}

void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    lastSampleRate = sampleRate;
   
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DJAudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    resampleSource.getNextAudioBlock(bufferToFill);

    std::array<float*, 2> channels = { bufferToFill.buffer->getWritePointer(0), bufferToFill.buffer->getWritePointer(1) };
    auto totalNumSamples = bufferToFill.buffer->getNumSamples();
    for (int i = 0; i < 2; ++i)
    {
        highGainControl[i].processSamples(channels[i], totalNumSamples);
        midGainControl[i].processSamples(channels[i], totalNumSamples);
        lowGainControl[i].processSamples(channels[i], totalNumSamples);
    }
}

void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

void DJAudioPlayer::loadURL(juce::URL audioURL) 
{
    paused = false;
    transportSource.setPosition(0.0);
    juce::AudioFormatReader* reader = formatManager.createReaderFor(audioURL.createInputStream(false));

    if (reader != nullptr) // good file!
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
        trackLoaded = true;
    }
    else
    {
        DBG("Something went wrong loading the file! :( ");
    }
}

void DJAudioPlayer::setGain(double gain) 
{
    if (gain < 0 || gain > 1) // check Gain is in correct range
    {
        DBG("Warning: invalid gain value at DJAudioPlayer::setGain");
    }
    else
    {
        transportSource.setGain(gain);
    }
}

void DJAudioPlayer::setHighGain(double gain) // sets gain level for high frequency range
{
    for (int i = 0; i < 2; ++i)
    {
        highGainCoef[i] = juce::IIRCoefficients::makeHighShelf(lastSampleRate, 7000, 0.5, gain);
        highGainControl[i].setCoefficients(highGainCoef[i]);
    }
}

void DJAudioPlayer::setMidGain(double gain) // sets gain level for mid frequency range
{
    for (int i = 0; i < 2; ++i)
    {
        midGainCoef[i] = juce::IIRCoefficients::makePeakFilter(lastSampleRate, 2500, 0.555, gain);
        midGainControl[i].setCoefficients(midGainCoef[i]);
    }
}

void DJAudioPlayer::setLowGain(double gain) // sets gain level for low frequency range
{
    for (int i = 0; i < 2; ++i)
    {
        lowGainCoef[i] = juce::IIRCoefficients::makeLowShelf(lastSampleRate, 1000, 0.5, gain);
        lowGainControl[i].setCoefficients(lowGainCoef[i]);
    }
}

void DJAudioPlayer::setSpeed(double ratio)
{
    if (ratio < 0.8 || ratio > 1.2) // check tempo value is in correct range
    {
        DBG("Warning: invalid speed value at DJAudioPlayer::setSpeed");
    }
    else
    {
        tempoValue = tempoValue * ratio;
        resampleSource.setResamplingRatio(ratio);
    }
}

void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos)
{
    if (pos < 1 || pos > 1)
    {
        DBG("Warning: invalid relative position value, should be between 0 and 1. at DJAudioPlayer::setPositionRelative");
    }
    else
    {
        const double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
    transportSource.setPosition(pos);
}

void DJAudioPlayer::start()
{
    playing = true;
    if (positionAtPause != 0.0 && paused == true)
    {
        setPositionRelative(positionAtPause);
        positionAtPause = 0.0;
        paused = false;
        transportSource.start();
    }
    else
    {
        transportSource.start();
    }
}

void DJAudioPlayer::pause()
{
    playing = false;
    paused = true;
    positionAtPause = transportSource.getCurrentPosition();
    transportSource.stop();
}

void DJAudioPlayer::stop()
{
    playing = false;
    paused = false;
    transportSource.setPosition(0.0);
    transportSource.stop();
}

double const DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

double const DJAudioPlayer::getTrackLength()
{
    return transportSource.getLengthInSeconds();
}

bool DJAudioPlayer::checkIfPaused()
{
    return paused;
}

void DJAudioPlayer::movePlayheadWhilePaused(double newPos)
{
    positionAtPause = newPos;
}