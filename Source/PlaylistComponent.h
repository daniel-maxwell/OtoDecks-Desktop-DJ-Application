/*
  ==============================================================================

    PlaylistComponent.h
    Created: 6 Mar 2023 10:30:55pm
    Author:  Dan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include <string.h>
#include <array>
#include "DeckGUI.h"
#include <fstream>
#include <filesystem>


//==============================================================================
/*
*/
class PlaylistComponent  : public juce::Component,
                           public juce::TableListBoxModel,
                           public juce::Button::Listener

{
public:
    PlaylistComponent(juce::AudioFormatManager& _formatManager, DeckGUI* deck1, DeckGUI* deck2);
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int getNumRows() override;

    void paintRowBackground(juce::Graphics&, 
                            int rowNumber, 
                            int width, 
                            int height,
                            bool rowIsSelected) override;

    void paintCell(juce::Graphics&,
                   int rowNumber,
                   int columnId,
                   int width,
                   int height,
                   bool rowIsSelected) override;

    juce::Component* refreshComponentForCell(int rowNumber,
                                       int columnId,
                                       bool isRowSelected,
                                       Component* existingComponentToUpdate) override;

    void buttonClicked(juce::Button* button);

    void readFromPlaylistFile(std::string playlistFile);

    void writeToPlaylistFile(std::array<std::vector<std::string>, 6> playlist);

    void updatePlaylist(int trackIndex, juce::File selectedTrack);

private:
    juce::TableListBox tableComponent;

    std::array<std::vector<std::string>, 6> playlist;

    juce::FileChooser fChooser{ "Select a file..." };

    juce::AudioFormatManager& formatManager;

    int selectedTrackID = 0;

    DeckGUI* deck1;
    DeckGUI* deck2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
