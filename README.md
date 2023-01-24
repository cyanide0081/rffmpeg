# rffmpeg

command-line tool for automation of ffmpeg commands through whole file paths on Windows

- run with **--help** for instructions

#### dependencies

- this tool uses the *ffmpeg* framework, you can download it [here](https://ffmpeg.org/download.html)

- to run it, either:

  a. add ffmpeg\bin to your environment variables

  b. copy the *ffmpeg* executable to the same directory as *rffmpeg*
  

- to compile this with *MingW*, use the [-municode](https://gcc.gnu.org/onlinedocs/gcc/x86-Windows-Options.html) option

- **p.s.:** currently, the executable is blocked by smartscreen by default (probably due to its `system()` calls)
