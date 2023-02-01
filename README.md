# RFFmpeg

> Windows command-line/console window tool for automating *FFmpeg* file conversions inside a specific directory
>
> - Run with **--help** to get the full manual



## Installation
 
> In order to use this tool you have to:
> 
> 1. download a build of [**FFmpeg**](https://ffmpeg.org/download.html)
> 
> 2. add `*/ffmpeg/bin` to your PATH environment variable
> 
> 3. download the latest build of this tool from [**here**](https://github.com/cyanide0081/rffmpeg/releases)
> 
> 4. (**optional**) put *rffmpeg.exe* in your `*_/ffmpeg/bin_` folder so you can call it from any directory



## Usage

> This tool's argument syntax is not that different from FFmpeg itself, with the basic structure being:
> 
> `rffmpeg -path <in-path> -fmt <in-fmt(s)> -opts <parameters> -ext <out-ext>`
>
> Here's an example of a basic RFFmpeg command:
>
> `rffmpeg -path C:\Users\Music -fmt wav -opts "-c:a libmp3lame -b:a 256k" -ext mp3`
>
> ### Remarks:
>
> - `path` : must hold a valid directory or else the application will close (__*__)
>
> - `fmt`  : must hold one or more formats you wish to transcode separated by commas, as in `-fmt wav,aif,flac` (__*__)
>
> - `opts` : can hold ffmpeg options that must be sorrounded by double-quotes (**and any other argument containing whitespaces**)
>
> - `ext`  : must hold the extension your output files will use



## Additional Flags

> You can also pass additional flags to the program to help you organize your batch conversions a little
>
> `--newfolder`   : puts your converted files inside a new folder to which you can assign a custom name, as in `--newfolder=안녕하세요`
>
> `--delete`      : deletes you original files permanently
>
> `--overwrite`   : automatically overwrites files if they conflict with the output files generated by FFmpeg (the program appends an index by default)
>
> `--norecursion` : disables file searches inside subfolders, in case your directory contains many folders you don't want to mess with



> - **p.s.:** you will get a Windows SmartScreen warning the first time you run it (assuming you have it enabled) 