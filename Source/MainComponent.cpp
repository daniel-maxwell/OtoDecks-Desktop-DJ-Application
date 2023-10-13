#include "MainComponent.h"
#include <sstream>
#include <iomanip> // std::setprecision

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (1300, 900);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }

    addAndMakeVisible((deckGUI1));
    addAndMakeVisible((deckGUI2));
    addAndMakeVisible(playlistComponent);
    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.


    // Define dimensions for the decks
    int deckWidth = getWidth() / 2;
    int deckHeight = static_cast<int>(getHeight() * (2.0 / 3.0));  // Allocate 2/3 of the height for the decks


    // Set bounds for the decks
    deckGUI1.setBounds(0, 0, deckWidth, deckHeight);
    deckGUI2.setBounds(deckWidth, 0, deckWidth, deckHeight);

    //deckGUI1.setBounds(0, 0, getWidth() / 2, getHeight()/1.5);
    //deckGUI2.setBounds(getWidth() / 2, 0, getWidth()/2, getHeight()/1.5);

    // Define dimensions and position for the playlistComponent
    int playlistYPosition = deckHeight;
    int playlistHeight = getHeight() - deckHeight;  // Remaining height after positioning the decks

    // Set bounds for the playlistComponent
    playlistComponent.setBounds(0, playlistYPosition, getWidth(), playlistHeight);

    //playlistComponent.setBounds(0, getHeight()/1.5, getWidth(), getHeight());
    // playlistComponent.setBounds(0, getHeight()/1.5, getWidth(), getHeight()/2);
}
