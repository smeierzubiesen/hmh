# Handmade Hero
by [Molly Rocket][]  
a complete game development in C(++) from scratch  
<small>(Visit our [Github Page][] for more information.)</small>

|                |                                                                               Build Status                                                                               |
|----------------|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
| Project Status |               [![Build status](https://ci.appveyor.com/api/projects/status/ut2kjvfxq7ceny9x?svg=true)](https://ci.appveyor.com/project/smeierzubiesen/hmh)               |
| branch:master  | [![Build status](https://ci.appveyor.com/api/projects/status/ut2kjvfxq7ceny9x/branch/master?svg=true)](https://ci.appveyor.com/project/smeierzubiesen/hmh/branch/master) |

| Agile Board | Status | on | [Waffle.io][] |
|-------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------|
| [![Stories in Backlog](https://badge.waffle.io/smeierzubiesen/hmh.svg?label=Backlog&title=Backlog)](https://waffle.io/smeierzubiesen/hmh) | [![Stories in Ready](https://badge.waffle.io/smeierzubiesen/hmh.svg?label=Ready&title=Ready)](http://waffle.io/smeierzubiesen/hmh) | [![Stories in In Progress](https://badge.waffle.io/smeierzubiesen/hmh.svg?label=In%20Progess&title=In%20Progress)](https://waffle.io/smeierzubiesen/hmh) | [![Stories in Done](https://badge.waffle.io/smeierzubiesen/hmh.svg?label=done&title=Done)](https://waffle.io/smeierzubiesen/hmh) |

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/df831b31c02e458aab74c6f77fce60ae)](https://www.codacy.com/app/smeierzubiesen/hmh?utm_source=github.com&utm_medium=referral&utm_content=smeierzubiesen/hmh&utm_campaign=badger)  
[![Code Climate](https://codeclimate.com/github/smeierzubiesen/hmh/badges/gpa.svg)](https://codeclimate.com/github/smeierzubiesen/hmh)  
[![Test Coverage](https://codeclimate.com/github/smeierzubiesen/hmh/badges/coverage.svg)](https://codeclimate.com/github/smeierzubiesen/hmh/coverage)  
[![Issue Count](https://codeclimate.com/github/smeierzubiesen/hmh/badges/issue_count.svg)](https://codeclimate.com/github/smeierzubiesen/hmh)

Participate in the development via [Waffle.io][]

## About
This is my take on and progress of the above mentioned (lol)

## Progress

[Changelog](/hmh/changelog.md)
<pre>this is not working yet</pre>
 - 09/02/2017 : Day10 done
  - Now generating timing information in debug mode
 - 04/02/2017 - 08/02/2017 : Day6-9
  - Implemented Sound Output
   - At the moment only a sine-wave output but a clearer sound an ear has never harkened!!
  - Implemented Controller and Keyboard Input
   - Alt+F4 is now handled by us as well
  - Bugfixes
  - Perfomance Improvents
   - Improved code Readability by moving stuff into a .h file.
   - Timing fixes etc.
  - Documenation Updates
   - The Documentation is now generated upon each build of the latest dayX->master pull-request
 - 03/02/2017 : Day 5
  - We have finished day 5, reviewed the code and have cleaned up the codebase.
 - 02/02/2017 : Day 4
  - We are drawing into the window.
 - 01//2/2017 : Day 3
  - We have created a backbuffer in which we can now paint.
 - 30/01/2017 : Day 2
  - we have created an actual window in which we either set white or black upon WM_SIZE
 - 29/01/17 : Day1 finished and ready for merge
  - Added simple MsgBox function
  - Compiler options set.
  - Documentation is being generated
 - 29/01/17 : We are currently on Day 1 and have not actually started yet
  - Set up dev environnment.
  - Changed approach from emacs based and cmd builds to [VS2015][] as I prefer it, but it's personal choice.

First of all I had to set up my dev environment.
So simply install [VS2015][] (Community Edition is free and does everything we need it to)

#### Step-by-step

Simply open the solution in [VS2015][] Build and Run win32_handmade.exe

  [Github Page]: https://smeierzubiesen.github.io/hmh/  "Handmade Hero Github Page"
  [Waffle.io]: https://waffle.io/smeierzubiesen/hmh     "Waffle.io Handmade Hero"
  [VS2015]: https://www.visualstudio.com/downloads/     "Visual Studio 2015 Downloads"
  [Molly Rocket]: https://mollyrocket.com               "Molly Rocket"
