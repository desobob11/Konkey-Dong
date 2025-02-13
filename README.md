
<a id="readme-top"></a>





<br />
<div align="center">
  <a href="https://github.com/desobob11/SDLDoom">
  </a>

  <h3 align="center">Konkey Dong :monkey_face:</h3>

  <p align="center">
    A retro platformer game, themed after our favorite copyrighted hero! 
  </p>
</div>




https://github.com/user-attachments/assets/13654134-dfb8-4e24-98ff-aa0520e9b631




## About The Project

 Dodge falling boulders, :runner: climb jungle vines :palm_tree:, dodge insects :beetle:, and avoid pools of lava :fire: in this charming adventure!
 
  This game was originally developed for the Raspberry PI as am embedded systems project. A peripheral was added to a Raspberry PI to bypass the OS and allow access to the hardware directly.

  I am now in the process of porting this game to Windows/Mac/Linux with the help of SDL2 for managing windows and processing user inputs! This current version is buggy at the moment, but I hope to have the port finished soon.

  The port from the baremetal Raspberry PI entails changing all user inputs from GPIO code intended for the Pi's hardware, and into SDL2 calls. Luckily, the logic for writing pixel's to the Pi's framebuffer translated very smoothly to writing to an SDL2 surface's framebuffer instead! As a result, the graphics on this Windows version are identical to the baremetal version. 

### Built With

* ![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
* SDL2

<p align="right">(<a href="#readme-top">back to top</a>)</p>




## Getting Started

The repository comes with two compiled executables:
* **Main.exe** is a Windows executable compiled for x86_64 architecture. If your PC satisfies this requirement, run this executable!


### Prerequisites

* <a href="https://www.libsdl.org/"> SDL2 </a> (the required binaries and DLLs are included in the repo)
* GCC, or on Windows, <a href="https://cygwin.com/"> Cygwin with mingw-64 is preferred </a>

### Installation

This section is for compiling the program.

**Windows**
1. The **make.bat** can be used to compile your program using GCC, once your Cygwin installation has been configured.
2. The template assumes that SDLDoom is installed in your C: directory. If this is not the case, please update any paths!
3. The script will create the **Main.exe** executable.

**Mac/Linux**
1. A makefile has been included in the repo! Please execute if you would like to recompile the program.
2. The makefile will create the **Main** executable.

<p align="right">(<a href="#readme-top">back to top</a>)</p>







## Contact

Desmond O'Brien -- desmondobrien01@outlook.com

Project Link: [https://github.com/desobob11/Konkey-Dong](https://github.com/desobob11/Konkey-Dong)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

