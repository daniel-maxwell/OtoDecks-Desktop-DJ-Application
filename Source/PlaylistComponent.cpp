/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 6 Mar 2023 10:30:55pm
    Author:  Dan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent(juce::AudioFormatManager& _formatManager, DeckGUI* _deck1, DeckGUI* _deck2)
                                     : formatManager(_formatManager),
                                       deck1(_deck1), 
                                       deck2(_deck2)
{
    if (std::filesystem::exists("playlist.csv")) // checks if playlist file exists
    {
        std::string playlistFile = "playlist.csv";
        try
        {
            readFromPlaylistFile(playlistFile);
        }
        catch (const std::exception& e)
        {
            // If file is empty or corrupted, show error message box
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                "Playlist Error",
                "Could not open or read from playlist file. File is empty or has been corrupted."
                "OK"
            );

            writeToPlaylistFile(playlist); // then write a blank playlist to the playlist file
        }
    }
    else
    {   // populate playlist vector with default values
        playlist[0].push_back("Track 1");
        playlist[0].push_back("-");
        playlist[1].push_back("Track 2");
        playlist[1].push_back("-");
        playlist[2].push_back("Track 3");
        playlist[2].push_back("-");
        playlist[3].push_back("Track 4");
        playlist[3].push_back("-");
        playlist[4].push_back("Track 5");
        playlist[4].push_back("-");
        playlist[5].push_back("Track 6");
        playlist[5].push_back("-");
        writeToPlaylistFile(playlist);
    }
    // create table
    tableComponent.getHeader().addColumn("Track title", 1, 400);
    tableComponent.getHeader().addColumn("Track length", 2, 400);
    tableComponent.getHeader().addColumn("", 3, 500/3);
    tableComponent.getHeader().addColumn("", 4, 500/3);
    tableComponent.getHeader().addColumn("", 5, 500/3);
    tableComponent.setModel(this);
    tableComponent.setRowHeight(tableComponent.getRowHeight() * 2);

    addAndMakeVisible(tableComponent);
}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("PlaylistComponent", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
    tableComponent.setBounds(0, 0, getWidth(), getHeight());
}

void PlaylistComponent::resized()
{
    tableComponent.setBounds(0, 0, getWidth(), getHeight());
}

int PlaylistComponent::getNumRows()
{
    return playlist.size();
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g,
                                           int rowNumber,
                                           int width,
                                           int height,
                                           bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::orange);
    }
    else if (rowNumber % 2)  // Check if row is odd
    {
        g.fillAll(juce::Colours::darkgrey);
    }
    else
    {
        g.fillAll(juce::Colour::fromRGB(110, 110, 110));
    }
}

void PlaylistComponent::paintCell(juce::Graphics& g, 
                                  int rowNumber,
                                  int columnId,
                                  int width,
                                  int height,
                                  bool rowIsSelected) // draw in the values from the playlist vector on to the gui
{ 
    if (columnId == 1)
    {
        g.drawText(playlist[rowNumber][0], 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }

    if (columnId == 2)
    {
        g.drawText(playlist[rowNumber][1], 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
    std::string trackname = playlist[rowNumber][0];
}

// Creates the buttons for each playlist row
juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
    int columnId,
    bool isRowSelected,
    Component* existingComponentToUpdate)
{   
    if (columnId == 3)
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{ "DECK 1" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            juce::String title{ "Deck1LoadFromPlaylist" };
            btn->setTitle(title);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }

    if (columnId == 4)
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{ "DECK 2" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            juce::String title{ "Deck2LoadFromPlaylist" };
            btn->setTitle(title);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }

    if (columnId == 5)
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{ "LOAD" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            juce::String title{ "LoadButton" };
            btn->setTitle(title);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

// Checks which button was clicked, and performs appropriate response
void PlaylistComponent::buttonClicked(juce::Button* button)
{
    int id = std::stoi(button->getComponentID().toStdString());
    selectedTrackID = id;

    if (button->getTitle() == "Deck1LoadFromPlaylist")
    {
        if (playlist[id].size() == 3)
        {
            juce::StringArray fileToLoad;
            fileToLoad.add(playlist[id][2]);

            deck1->filesDropped(fileToLoad, 0, 0);

        }
        else
        {
            // Alert dialogue to inform user that no track is currently loaded
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                                   "Playlist Error",
                                                   "No track currently loaded in this playlist slot!",
                                                   "OK");
        } 
    }

    if (button->getTitle() == "Deck2LoadFromPlaylist")
    {
        if (playlist[id].size() == 3)
        {
            juce::StringArray fileToLoad;
            fileToLoad.add(playlist[id][2]);

            deck2->filesDropped(fileToLoad, 0, 0);

        }
        else
        {
            // Alert dialogue to inform user that no track is currently loaded
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                                   "Playlist Error",
                                                   "No track currently loaded in this playlist slot!",
                                                   "OK");
        }
    }

    if (button->getTitle() == "LoadButton") // launch a file chooser
    {
        auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles;
        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
        {
            
            updatePlaylist(selectedTrackID, chooser.getResult());
            
        });
    }
}

// update playlist with information about the new track and save path
void PlaylistComponent::updatePlaylist(int trackIndex, juce::File selectedTrack)
{
    std::string newTrackName = selectedTrack.getFileNameWithoutExtension().toStdString();
    std::string trackLength;
    std::string path = selectedTrack.getFullPathName().toStdString();

    if (auto reader = formatManager.createReaderFor(selectedTrack))
    {
        // Calculate length of the audio file in minutes/secs from sample rate
        int lengthInSeconds = (reader->lengthInSamples / reader->sampleRate);

        // Calculate minutes and seconds
        int finalMinutes = floor(lengthInSeconds / 60);
        int finalSeconds = lengthInSeconds - finalMinutes * 60;
       
        // Convert to string
        trackLength = std::to_string(finalMinutes) + "m " + std::to_string(finalSeconds) + "s";

    }

    // Update the track vector in the playlist array with the new track and repaint to display changes
    playlist[trackIndex][0] = newTrackName;
    playlist[trackIndex][1] = trackLength;
    playlist[trackIndex].size() == 3 ? playlist[trackIndex][2] = path : playlist[trackIndex].push_back(path);
    writeToPlaylistFile(playlist);
    PlaylistComponent::repaint();
}

// read from the saved playlist csv file and save it in to the playlist array
void PlaylistComponent::readFromPlaylistFile(std::string playlistFile)
{
    std::ifstream savedPlaylist(playlistFile);
    if (!savedPlaylist.is_open()) throw std::runtime_error("Could not open playlist file");

    std::string line;
    std::string delimiter = ",";
    int trackIndex = 0;

    if (savedPlaylist.good())
    {
        while (std::getline(savedPlaylist, line)) // for each line in the csv
        {
            size_t pos = 0;
            while ((pos = line.find(delimiter)) != std::string::npos) // tokenise with ',' as delimiter 
            {
                std::string token = line.substr(0, pos);
                playlist[trackIndex].push_back(token);
                line.erase(0, pos + delimiter.length());
            }
            playlist[trackIndex].push_back(line); // save to playlist array
            trackIndex += 1;
        }
    }
    else
    {
        throw std::runtime_error("Could not read playlist file");
    }
}

// Saves updated playlist array in to the playlist csv file
void PlaylistComponent::writeToPlaylistFile(std::array<std::vector<std::string>, 6> playlist)
{
    std::ofstream playlistFile("playlist.csv");

    for (int i = 0; i < playlist.size(); ++i) // for all tracks in playlist
    {
        for (int j=0; j < playlist[i].size(); ++j) // for all elements of the track vector
        {
            playlistFile << playlist.at(i)[j];
            if (j != playlist[i].size() - 1) playlistFile << ","; // No comma at end of line
        }
        playlistFile << "\n";
    }
    playlistFile.close();
}