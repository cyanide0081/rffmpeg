# **RFFmpeg**

Command-line/Console window tool for batch-processing files inside a specific directory (or more) with FFmpeg

* Currently supports *Windows*, *Linux* and *MacOS* (though I haven't gotten to compiling or testing for mac yet)

* Run with `--help` to read the help page (this one covers more examples though)

<br>

**NOTE:** This tool's goal is being simple to use and robust, (error handling-wise) to make it easy to batch-convert files without all the manual organizational headache, so suggestions are welcome

<br>

## **Installation**

In order to use this tool you will first need to install FFmpeg, so here are two ways to do it:

<br>

### **Windows (10+)**

If you're using Windows 10.1709 or newer, you can install it by simply opening PowerShell and running: 

    winget install ffmpeg

<br>

### **Linux (Debian/Ubuntu-based)**

In Linux you can probably install it just by using your local package manager. Here's the command for Debian/Ubuntu-based distros: 

    sudo apt install ffmpeg

<br>

You can then check if your installation succeeded by running the version command below and seeing if it produces any output:

    ffmpeg -version

<br>

## **Usage**

This tool's argument syntax is not that different from FFmpeg itself, with the basic structure being:
 
    rffmpeg [PATH(S)] -i [IN-FMT(S)] -p [PARAMS] -o [OUT-FMT]

Here's a basic RFFmpeg command that looks for wav files and converts them to mp3:

    > rffmpeg C:\Users\Music -i wav -o mp3
  

<br>

 ### **Arguments**
 

 * **-i**   :   one or more formats you wish to transcode separated by commas, like: `-in mp4,m4v,mov`

 * **-p**   :   optional ffmpeg conversion parameters, as in: `-p '-c:v vp9 -crf 32 -c:a copy'` 

 * **-o**   :   extension for your output files (dot is implicit)

    * If you specify identical input and output formats, use the **-subfolder** or **-custompath** option otherwise the program won't run


<br>

 ### **Additional Flags**

You can also pass any of the flags below to help you organize your batch conversions a little

 * **-subfolder** : puts your converted files inside a new folder (which is itself created inside the input file's directory)
    * You can also give it a custom name using an equal sign, as in: `-subfolder=你好`
 * **-custompath**  : puts your converted files inside a new directory
    * Unlike --newfolder, you have to explicitly name the new path, as in: `-custompath=/home/user/Music/你好`

 * **-cl**       : deletes the original files permanently after conversion (**don't use this if you're not sure the conversion will work properly**)

 * **-ow**       : automatically overwrites files if they conflict with the output files generated by FFmpeg  (the program appends an index by default)

 * **-rn**       : disables file searches inside subfolders, in case your directory contains many folders you  don't want to mess with

<br>

### **A Slightly more Complex Example**

Here's an example of a more specific call to rffmpeg that looks inside two specific folders for .flac and .m4a files, converts them to .opus using the libopus codec, and finally stores them in a new subfolder named 'transcoded'

    rffmpeg C:\Users\User\Music C:\Users\User\Music\Album2 -i flac,m4a -p '-c:a libopus -vbr on -b:a 128k' -o opus -subfolder=transcoded

***
