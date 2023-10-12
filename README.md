# **RFFmpeg**

Command-line/Console window tool for batch-processing files inside a specific directory (or more) with FFmpeg

* Currently ships binaries for *Windows* and *Linux*
* Building for *MacOS* and *Free/OpenBSD* should work without problems but I haven't tested that yet

* Run with `--help` to read the help page (this one covers more examples though)

<br>

**NOTE:** This tool's goal is being simple to use and robust, (error handling-wise) to make it easy to batch-convert files without all the manual organizational headache, so suggestions are welcome

<br>

# **Table of contents**

1. [Installation](#installation)
2. [Usage](#usage)
3. [Building](#building)

<br>

<a name="installation"></a>
# **Installation**

**1.** Installing *FFmpeg*

In order to use this tool you will first need to install FFmpeg, either by getting it from your systems's package manager (examples shown below), or by downloading the binaries from a [stable release](https://ffmpeg.org/download.html), extracting them and adding the `bin` folder to your `PATH`

<br>

### **Windows (10+)**

If you're using Windows 10.1709 or newer, you can install it with *WinGet* by running: 

    winget install ffmpeg

<br>

### **Linux (Debian/Ubuntu-based)**

On Linux you can probably install it just by using your local package manager. Here's the command for Debian/Ubuntu-based distros: 

    sudo apt install ffmpeg

<br>

You can then check if your installation succeeded by running the version command below and seeing if it produces any output:

    ffmpeg -version

<br>

**2.** Installing *RFFmpeg* 

To install this tool, just head to the [releases](https://github.com/cyanide0081/rffmpeg/releases) page and download the latest binaries for your system (if there are no pre-built ones for your system you can alternatively [build](#building) the program from source)

(You can also optionally place the *rffmpeg* executable in the same directory as your *ffmpeg* binaries so you can run the tool from anywhere in your OS)

<a name="usage"></a>
# **Usage**

This tool's argument syntax is not that different from FFmpeg itself, with the basic structure being:
 
    rffmpeg [PATH(S)] -i [IN-FMT(S)] -p [PARAMS] -o [OUT-FMT]

Here's a basic RFFmpeg command that looks for wav files and converts them to mp3:

    rffmpeg C:\Users\Music -i wav -o mp3

Here's an example of a more specific command that looks inside two folders for .flac and .m4a files, converts them
to OPUS using the libopus codec, and finally stores them in a new subfolder named 'transcoded':

    rffmpeg /home/Music /home/Videos -i flac,m4a -p "-c:a libopus -vbr on -b:a 128k" -o opus -subfolder:transcoded

<br>

 ### **Arguments**
 

 * **-i**   :   one or more formats you wish to transcode separated by commas, like: `-i mp4,m4v,mkv`

 * **-p**   :   optional ffmpeg conversion parameters, as in: `-p "-c:v vp9 -crf 32 -c:a copy"` 

 * **-o**   :   extension for your output files (dot is implicit) `-o webm`

    * If you specify identical input and output formats, use the **-subfolder** or **-newpath** option otherwise the program won't run

<br>

 ### **Flags/Switches**

You can also pass any of the flags below to help you organize your batch conversions a little

 * **-subfolder**  :   puts your converted files inside a new folder (which is itself created inside the input file's directory)
    * You can also give it a custom name like this: `-subfolder:你好`
 * **-outpath**    :   puts your converted files inside a new absolute directory
    * Unlike -subfolder, you have to explicitly name the new path, as in: `-outpath:/home/Music/你好`

 * **-cl**         :   deletes the original files permanently after conversion (**don't use this if you're not sure the conversion will work properly**)

 * **-ow**         :   automatically overwrites files if they conflict with the output files generated by FFmpeg  (the program appends an index by default)

 * **-rn**         :   disables file searches inside subfolders, in case your directory contains many folders you  don't want to mess with

<br>

<a name="building"></a>
# **Building**

You can build this program by simply having the LLVM [clang](https://releases.llvm.org/download.html) compiler installed

<br>

### **POSIX** (Linux, MacOS, FreeBSD, ...)

To build it on (probably) most 'POSIX' systems you can run the bash build script:

    ./build.sh 

You can also build in release mode with:

    ./build.sh rel

<br>

### **Windows**

On Windows the build links with the [MSVC](https://visualstudio.microsoft.com/vs/features/cplusplus/) libraries so you'll have to install that
if you don't have it already (though it should be pretty easy to build with something like MinGW if you modify the batch file a little)

<br>

To build it you can run the batch build script:

    .\build.cmd

Or build it in release mode like this:

    .\build.cmd rel

***
