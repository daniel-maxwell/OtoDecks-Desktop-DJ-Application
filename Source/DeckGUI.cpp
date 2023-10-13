/*
  ==============================================================================

    DeckGUI.cpp
    Created: 5 Mar 2023 11:10:21am
    Author:  Dan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player,
                juce::AudioFormatManager& formatManagerToUse,
                juce::AudioThumbnailCache& cacheToUse,
                int deckNum)
                  : player(_player),
                    waveformDisplay(formatManagerToUse, cacheToUse)
{
    deckNumber = deckNum; // Deck 1 or Deck 2
    
    // Initialise and configure GUI components
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(tempoDial);
    addAndMakeVisible(highGainDial);
    addAndMakeVisible(midGainDial);
    addAndMakeVisible(lowGainDial);
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(posSlider);
 
    playButton.addListener(this);
    pauseButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);

    gainSlider.addListener(this);
    gainSlider.setRange(0.0, 1.0);
    gainSlider.setValue(0.5);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    
    posSlider.addListener(this);
    posSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    posSlider.setOpaque(false);
    posSlider.setAlpha(0.0);
    posSlider.setRange(0.0, 10);
    posSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

    tempoDial.addListener(this);
    tempoDial.setRange(0.85, 1.15);
    tempoDial.setValue(1.0);
    tempoDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    tempoDial.setPopupDisplayEnabled(true, true, this);
    tempoDial.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

    highGainDial.addListener(this);
    highGainDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    highGainDial.setRange(0.01f, 2.01f);
    highGainDial.setValue(1.0f);
    highGainDial.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);

    midGainDial.addListener(this);
    midGainDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    midGainDial.setRange(0.01f, 2.01f);
    midGainDial.setValue(1.0f);
    midGainDial.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);

    lowGainDial.addListener(this);
    lowGainDial.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    lowGainDial.setRange(0.01f, 2.01f);
    lowGainDial.setValue(1.0f);
    lowGainDial.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
    
    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (juce::Graphics& g)
{
    const double rowH = getHeight() / 8;
    const double rowW = getWidth() / 12;
    const double centreDeck = getWidth() / 4;

    juce::Rectangle<int> canvasArea = getLocalBounds();

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::lightgrey);
    g.setFont (17.0f);

    // Draw Component labels
    if (deckNumber == 1)
    {
        g.drawText("Gain",
            juce::Rectangle<float>(10.02 * rowW, rowH * 5.2, rowW * 2, rowH * 2),
            juce::Justification::centred, true);

        g.drawText("Tempo",
            juce::Rectangle<float>(8.23 * rowW, rowH*0.9, rowW * 2, rowH * 2),
            juce::Justification::centred, true);

        g.drawText("High",
            juce::Rectangle<float>(centreDeck - (3.25 * rowW), rowH*1.47, rowW * 2, rowH * 2),
            juce::Justification::centred, true);

        g.drawText("Mid",
            juce::Rectangle<float>(centreDeck - (3.25 * rowW), rowH * 2.98, rowW * 2, rowH * 2),
            juce::Justification::centred, true);

        g.drawText("Low",
            juce::Rectangle<float>(centreDeck - (3.25 * rowW), rowH * 4.48, rowW * 2, rowH * 2),
            juce::Justification::centred, true);
    }
    else
    {
        g.drawText("Gain",
            juce::Rectangle<float>(rowW/100, rowH * 5.2, rowW * 2, rowH * 2),
            juce::Justification::centred, true);

        g.drawText("Tempo",
            juce::Rectangle<float>(1.74 * rowW, rowH * 0.9, rowW * 2, rowH * 2),
            juce::Justification::centred, true);

        g.drawText("High",
            juce::Rectangle<float>(10.24 * rowW, rowH * 1.47, rowW * 2, rowH * 2),
            juce::Justification::centred, true);

        g.drawText("Mid",
            juce::Rectangle<float>(10.24 * rowW, rowH * 2.98, rowW * 2, rowH * 2),
            juce::Justification::centred, true);

        g.drawText("Low",
            juce::Rectangle<float>(10.24 * rowW, rowH * 4.48, rowW * 2, rowH * 2),
            juce::Justification::centred, true);
    }

    // Draw deck Disc
    g.setColour(juce::Colour::fromRGB(120, 120, 120));

    g.fillEllipse(juce::Rectangle< float >::Rectangle(2 * (rowW * 1.75), rowH*2.25, rowW * 5, rowW * 5));
    auto discCenter = juce::Rectangle< float >::Rectangle(2 * (rowW * 1.75), rowH * 2.25, rowW * 5, rowW * 5).getCentre();
    float diameter{ 130 };
    float radius{ diameter / 2 };
    juce::Rectangle<float> discArea{ float(2 * (rowW * 1.75)), float(rowH * 2.25), float(rowW * 5), float(rowW * 5) };

    g.drawEllipse(discArea, 5.0f);
    g.fillEllipse(discArea);
    g.setColour(juce::Colour::fromRGB(70, 70, 70));

    juce::Line<float> line(discCenter.getX() + cos(notchAngleInRadians),
                           discCenter.getY() + sin(notchAngleInRadians),
                           discCenter.getX() + cos(notchAngleInRadians) * diameter,
                           discCenter.getY() + sin(notchAngleInRadians) * diameter);
    line = line.withLengthenedEnd(7.0f);
    line = line.withShortenedStart(line.getLength() / 1.67 );

    g.drawLine(line, 17.0f);
}

void DeckGUI::resized()
{
    const double rowH = getHeight() / 8;
    const double rowW = getWidth() / 12;
    const double centreDeck = getWidth() / 4;
    const double dialWidth = rowW * 6;

    playButton.setBounds(rowW * 1.75, rowH * 6.75, rowW * 1.75, rowH/1.5);
    pauseButton.setBounds(rowW * 4, rowH * 6.75, rowW * 1.75, rowH / 1.5);
    stopButton.setBounds(rowW * 6.25, rowH * 6.75, rowW * 1.75, rowH / 1.5);
    loadButton.setBounds(rowW * 8.5, rowH * 6.75, rowW * 1.75, rowH / 1.5);
    posSlider.setBounds(0, 0, getWidth(), rowH*1.5);
    waveformDisplay.setBounds(0, 0, getWidth(), rowH*1.5);

    if (deckNumber == 1) // sets bounds for components if they differ between decks
    {
        gainSlider.setBounds(getWidth()- rowW*1.5, rowH * 2, rowW, rowH * 4);
        tempoDial.setBounds(centreDeck + 3.25 * rowW, rowH * 2, dialWidth, rowH);
        highGainDial.setBounds(centreDeck - (4.25 * rowW), rowH * 2, dialWidth, rowH);
        midGainDial.setBounds(centreDeck - (4.25 * rowW), rowH * 3.5, dialWidth, rowH);
        lowGainDial.setBounds(centreDeck - (4.25 * rowW), rowH * 5, dialWidth, rowH);
    }
    else
    {
        gainSlider.setBounds(rowW/2, rowH * 2, rowW, rowH * 4);
        tempoDial.setBounds(centreDeck - 3.25 * rowW, rowH * 2, dialWidth, rowH);
        highGainDial.setBounds(rowW * 7.25, rowH * 2, dialWidth, rowH);
        midGainDial.setBounds(rowW * 7.25, rowH * 3.5, dialWidth, rowH);
        lowGainDial.setBounds(rowW * 7.25, rowH * 5, dialWidth, rowH);
    }
}

// Configures the appearance of the rotary sliders (Dials) Called when one is painted
void DeckGUI::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, 
    const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&)
{
    auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // fill
    g.setColour(juce::Colours::orange);
    g.fillEllipse(rx, ry, rw, rw);

    // outline
    g.setColour(juce::Colours::red);
    g.drawEllipse(rx, ry, rw, rw, 1.0f);

    juce::Path p;
    auto pointerLength = radius * 0.33f;
    auto pointerThickness = 2.0f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    // pointer
    g.setColour(juce::Colours::yellow);
    g.fillPath(p);
}

void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        if (player->trackLoaded)
        {
            posSlider.setRange(0, player->getTrackLength());
            player->start();
            DBG("play button was clicked");
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                "Player Error",
                "No track loaded on this deck."
            );
        }
    }
    if (button == &pauseButton)
    {
        player->pause();
    }
    if (button == &stopButton)
    {
        player->stop();
    }
    if (button == &loadButton)
    {
        auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles;
        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
            {
                auto chosenFile = chooser.getResult();
                player->loadURL(juce::URL{ chosenFile });
                waveformDisplay.loadURL(juce::URL{ chosenFile });
                notchAngleInRadians = 0;
            });
    }
}

void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    std::ostringstream message; // debug message

    if (slider == &gainSlider)
    {
        float gain = slider->getValue(); // gets the value of the volume slider
        player->setGain(gain);
    }

    if (slider == &tempoDial)
    {
        float playbackSpeed = slider->getValue(); // gets the value of the tempo slider
        player->setSpeed(playbackSpeed);
        deckSpeed = player->tempoValue;
    }

    if (slider == &posSlider)
    {
        if (player->checkIfPaused()) player->movePlayheadWhilePaused(slider->getValue());
        player->setPositionRelative(slider->getValue()); // gets the value of the tempo slider
    }

    if (slider == &highGainDial) // pass the slider value to the setHighGain function
    {
        player->setHighGain(slider->getValue());
    }

    if (slider == &midGainDial) // pass the slider value to the setMidGain function
    {
        player->setMidGain(slider->getValue());
    }

    if (slider == &lowGainDial) // pass the slider value to the setLowGain function
    {
        player->setLowGain(slider->getValue());
    }
}

bool DeckGUI::isInterestedInFileDrag(const juce::StringArray &files) // true if user is dragging audio file
{
    return true;
}

void DeckGUI::filesDropped(const juce::StringArray &files, int x, int y) // loads on to deck once dropped
{
    if (files.size() == 1)
    {
        player->loadURL(juce::URL{ juce::File {files[0]} });
        waveformDisplay.loadURL(juce::URL{ juce::File {files[0]} });
    }
}

void DeckGUI::timerCallback() // updates waveform display playhead
{
    if (player->playing)
    {
        notchAngleInRadians >= 6.283 ? notchAngleInRadians = 2 * deckSpeed : notchAngleInRadians += deckSpeed;
        repaint();
    }
    waveformDisplay.setPositionRelative(player->getPositionRelative());
}