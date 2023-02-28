# **RFFmpeg**

Command-line/Console window tool for batch-processing files inside a specific directory (or more) with FFmpeg

* Currently supports *Windows* and *Linux* (it's theoretically compilable for *MacOS* but I can't do it myself :c)

* Run with `--help` to read the help page (though this page has much better documentation and examples)

<br>

**NOTE:** I made this tool with the intention of it being intuitive, versatile and robust (error handling-wise) so it could fit more than just one very specific use case, therefore, if you have any suggestions or ideas for improving it, feel free to reach out or contribute if you want :)

<br>

## **Installation**

In order to use this tool you will first need to install FFmpeg, so here are two ways to do it:

<br>

### **Windows (10+)**

If you're using Windows 10.1709 or newer, you can install it by simply opening PowerShell and running: 

    > winget install ffmpeg

<br>

### **Linux (Debian/Ubuntu-based)**

In Linux you can probably install it just by using your local package manager. Here's the command for Debian/Ubuntu-based distros: 

    > sudo apt install ffmpeg

<br>

You can then check if your installation succeeded by running the version command below and seeing if it produces any output:

    > ffmpeg -version

<br>

## **Usage**

This tool's argument syntax is not that different from FFmpeg itself, with the basic structure being:
 
    > rffmpeg -path <in-path> -in <in-fmt(s)> -opts <parameters> -out <out-ext>

Here's a basic RFFmpeg command that looks for wav files and converts them to mp3:

    > rffmpeg -path C:\Users\Music -in wav -out mp3
  

<br>

 ### **Arguments**
 
 * __\*__**path** : the directory(ies) the tool will look for files in, used as: `-path /usr/folder`

    * It can hold a list of directories separated by a delimiter, which is '*' on Windows and ':' on the other OS's. Here are two examples: `-path 'C:\Users\Music*C:\音乐'` (Windows), `-path '/usr/Music:/usr/音乐'` (Others)

 * __\*__**in**   : one or more formats you wish to transcode separated by commas, like: `-in mp4,m4v,mov`

 * **opts**       : ffmpeg conversion parameters sorrounded by quotes, as in: `-opts '-c:v vp9 -crf 32 -c:a copy'` 

 * __\*__**out**  : specifies the extension your output files will use

    * If you specify identical input and output formats, use the **--newfolder** or **--newpath** option otherwise the program won't run 

 __\*__ All arguments marked with an must not be empty for the tool to work

<br>

 ### **Additional Flags**

You can also pass any of the flags below to help you organize your batch conversions a little

 * **--newfolder**   : puts your converted files inside a new folder (which is itself created inside the input file's directory)
    * You can also give it a custom name using an equal sign, as in: `--newfolder=你好`
 * **--newpath**   : puts your converted files inside a new directory
    * You can also give it a custom name using an equal sign just like with --newfolder, as in: `--newdir=/usr/Music/你好`

 * **--cleanup**     : deletes the original files permanently after conversion (**don't use this if you're not sure the conversion will work properly**)

 * **--overwrite**   : automatically overwrites files if they conflict with the output files generated by FFmpeg  (the program appends an index by default)

 * **--norecursion** : disables file searches inside subfolders, in case your directory contains many folders you  don't want to mess with

<br>

### **A Slightly more Complex Example**

Here's an example of a more specific call to rffmpeg that looks inside two specific folders for .flac and .m4a files, converts them to .opus using the libopus codec, and finally stores them in a new subfolder named 'transcoded'

    > rffmpeg -path 'C:\Users\User\Music\Album1*C:\Users\User\Music\Album2' -in flac,m4a -opts '-c:a libopus -vbr on -b:a 128k' -out opus --newfolder=transcoded

***