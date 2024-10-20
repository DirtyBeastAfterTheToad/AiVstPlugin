# VST Plugin Project (C++)

This project is a VST plugin created using C++ and [Projucer](https://juce.com). 

Follow the instructions below to set up the environment and install the necessary dependencies to get everything working.

PLEASE, note that as of now, I'd say that the samples are usable ~1/8 of the time, the current models absolutely want to generate a "whole music", which is crap, but whatever.

## Table of Contents
- [External Libraries Used](#external-libraries-used)
- [Installation](#installation)
  - [Windows Setup](#windows-setup)
  - [Mac Setup](#mac-setup)
- [Installing the VST](#installing-the-vst)
- [System Prompt Edition](#system-prompt-edition)
- [Usage](#usage)
- [Editing the VST](#editing-the-vst)
- [TODO List](#todo-list)

## External Libraries Used
This project uses the following external libraries to function properly:

- [Audiocraft](https://github.com/facebookresearch/audiocraft): A library developed by Facebook Research for high-quality audio synthesis and music generation.
- [PyTorch](https://pytorch.org/): A deep learning framework used to support CUDA and efficient audio processing.
- [FFmpeg](https://ffmpeg.org/): A multimedia framework used for handling audio and video processing.
  
## Installation
To ensure everything works as expected, follow the setup guide for your operating system.

### Windows Setup
1. **Install Anaconda**
   - [Download and install Anaconda](https://www.anaconda.com/download/success#) if you haven't already.

2. **Add Conda Path to Environment Variables**
   - Add the following path to your system's environment variables:  
     ```
     C:\Users\<your_username>\anaconda3\Scripts
     ```
   - Replace `<your_username>` with your actual username, or replace the whole path with where your installation is.

3. **Create and Activate Conda Environment**
   - Open a command prompt and run the following commands:
     ```
     conda create -n MusicGen python=3.9
     conda activate MusicGen
     ```

4. **Install Dependencies**
   - Install `ffmpeg` via conda:
     ```
     conda install ffmpeg
     ```
   - Install the `audiocraft` library from GitHub:
     ```
     pip install git+https://github.com/facebookresearch/audiocraft.git
     ```

5. **Install PyTorch**
   - Uninstall any previous versions of `torch` to avoid conflicts:
     ```
     pip uninstall torch
     pip cache purge
     ```
   - Install PyTorch 2.1.0 with CUDA support:
     ```
     conda install pytorch==2.1.0 pytorch-cuda=12.1 -c pytorch -c nvidia
     ```

### Mac Setup
The process for Mac is similar to Windows, but the paths may vary (not verified!!!) :
1. **Install Anaconda**
   - [Download and install Anaconda](https://www.anaconda.com/download/success#).

2. **Create and Activate Conda Environment**
   - Open a terminal and run the following commands:
     ```
     conda create -n MusicGen python=3.9
     conda activate MusicGen
     ```

3. **Install Dependencies**
   - Install `ffmpeg` via conda:
     ```
     conda install ffmpeg
     ```
   - Install the `audiocraft` library from GitHub:
     ```
     pip install git+https://github.com/facebookresearch/audiocraft.git
     ```

4. **Install PyTorch**
   - Uninstall any previous versions of `torch` to avoid conflicts:
     ```
     pip uninstall torch
     pip cache purge
     ```
   - Install PyTorch 2.1.0:
     ```
     conda install pytorch==2.1.0 -c pytorch
     ```

## Installing the VST
Once you have followed the setup instructions, you can take the `.vst3` file from the `/VST` directory and install it in your DAW (Digital Audio Workstation) as you would with any VST plugin.

- **For Windows Users:** Copy the `.vst3` file to your VST plugins folder, which is usually located at:
  - `C:\Program Files\Common Files\VST3`

- **For Mac Users:** Copy the `.vst3` file to your VST plugins folder, usually at:
  - `/Library/Audio/Plug-Ins/VST3`
## System Prompt Edition
To edit the system prompt, you need to modify line 40 of the `Generate.py` script. This script will be located in your `Documents` folder AFTER the first generation.
It can be found within the project directory BEFORE building the VST IF you edit it yourself.
## Usage
After installing the VST plugin, open your DAW and add the plugin as an effect or instrument. It should now be ready to use!

Feel free to explore and tweak the settings to get the desired sound.

## Editing the VST
To edit the VST plugin, open the `AiAudioPlugin.jucer` file using [Projucer](https://juce.com/). Everything should be preconfigured from there, allowing you to make adjustments to the plugin as needed.

## TODO List
Here are some tasks that are left to complete:
- Change UI- I thrive in the back, who cares about the UI -uhh ... ;) 
- Display the root note in another color (~UI thing, yuk!)
- React to stop message from the DAW (Important)
- Add a sample display
- Wait for an actually good open source AI music sample model, rather than these "music" generating models, that want to generate drums every time.


- Sample Modification:- > To be able to modify the sample directly in the plugin, but eh, you can do that in any DAW right?
  - Add "start sample"
  - Add "end sample"
  - Implement pitch control (?pitch?)
  - Implement time control (?time?)
  - Add fade in
  - Add fade out
  - Implement envelope functionality (?)

 ### Issues and Feedback
If you encounter any issues or have suggestions for improvements, please open an issue on the GitHub repository, I certainly will not read it, just like you may not read this note :D !

Don't forget that I DID NOT make these models, i've just setup a way to talk with them directly from within your DAW. 

Enjoy making "music" with AI ! 

Oh and if you use parts of my code, well, noice! But don't lie to yourself saying you made it, it will mostly hurt you rather than anyone else. 

Love. 

DBAT
