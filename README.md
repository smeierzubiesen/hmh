[![Stories in Backlog](https://badge.waffle.io/smeierzubiesen/hmh.svg?label=backlog&title=Backlog)](https://waffle.io/smeierzubiesen/hmh) 
[![Stories in Ready](https://badge.waffle.io/smeierzubiesen/hmh.svg?label=ready&title=Ready)](http://waffle.io/smeierzubiesen/hmh)
[![Stories in In Progress](https://badge.waffle.io/smeierzubiesen/hmh.svg?label=in%20progess&title=In%20Progress)](https://waffle.io/smeierzubiesen/hmh) 
[![Stories in Done](https://badge.waffle.io/smeierzubiesen/hmh.svg?label=done&title=Done)](https://waffle.io/smeierzubiesen/hmh) 
 
Participate in the development [here](https://waffle.io/smeierzubiesen/hmh) (via: Waffle.io)
# Handmade Hero
by Molly Rocket

## About
This is my take on and progress of the above mentioned (lol)

## Progress
We are currently on Day 8 and have implemented some backbuffer graphics
We also managed to get the Xbox 360 PC controller working and interpreting input
(eg: Start button closes app)

First of all I had to set up my dev environment.

## Getting Started

### Prerequisites

 * Emacs 24 for Windows (at the moment)
 (this *ideally* wants to live in a directory called emacs next to the hmh\ directory)
 * Visual Studio 2015 Community Edition

### Caveats

Please make sure you check all .bat scripts for your personal paths, as these are most likely changing once you clone into your environment.

### Step-by-step

Make a shortcut to cmd.exe and append something like this to the commandline `/k "%HOMEDRIVE%%HOMEPATH%\Documents\Github\hmh\misc\shell.bat"`  
*(where `<project path>` = the path where you cloned __HMH__ to)*  
That way, when you click on that link you will end up with an almost ready-to-go dev environment on drive z:

you can open any file by simply calling emacs.bat with a filename

within emacs simply hit M-m to build the .cpp file that is currently open in your buffer
