# SpacerNET_Union

## Table of contents

- [About](#about)
- [Download](#download)
- [Usage requirements](#build-requirements)
- [Build requirements](#build-requirements)
- [Contributing](#contributing)

## About

### Spacer.NET

**Spacer.NET** is a world editor for **Gothic II: Night of the Raven** that allows you to edit .ZEN world files.  
It's mainly inspired by the original **Spacer** editor, it offers almost the same functionallity from the original editor and some additional features. The main purpose of it is to give a better vobbing experience without many inconveniences of original editor.

### SpacerNET_Union

This project is a back-end plugin written in **cpp** for **Spacer.NET** editor using **Union SDK**.  
It's responsibility is to integrate editor features into the game and communicate with the interface.

The **Spacer.NET** contains also two additional files that can be only obtained from **SpacerNET.mod**:
- **zwe_controller.dll**  
    Contains code responsible for implementing alternative controller for camera movement / picking vobs.
- **SpacerNET.patch**  
    Contains memory patches required to run the editor.

#### Project details

- **NOTE!** Do not add new Union Files via Visual Studio 2015 UI, because it won't work.
- **NOTE!** This project uses custom `vdfs.h` and `vdfs32g.lib` that contain additional function `vdf_getvolumename`.
- Most of the hooks are located in **Spacer_Hooks.cpp**.
- Hooks using `HOOK .... AS` syntax should work for all gothic engines versions.
- Hooks using `CInvoke` will work only for G2A (they contain a specific address of function)
- `void SpacerApp::PluginLoop()` is a main loop of SpacerNET
- **Export.cpp** contains export files for using them in **C#**
- All of the communication between Union plugin and interface work via **Stack system**, like `Stack_PushInt` function.
- The project might contain some test/unused code.
- For printing use `print.PrintRed("Some text")`;

## Download

You can download the latest release of Spacer.NET from [worldofplayers.de](https://forum.worldofplayers.de/forum/threads/1557793-WORLD-EDITOR-Spacer-NET) forum.  
Or you can use Google drive: [Link](https://drive.google.com/drive/folders/1shwV7oFBP2vQfxZkBCw9KjScDlx-Wueq)

## Manual (SpacerNET lessons)
It is in Russian, but all the screenshots are English, so just use any browser translator

https://worldofplayers.ru/forums/1179/?order=title&direction=asc

## Usage requirements

In order to use this project you need'll to install:
- [Visual C++ Redistributable Package](https://aka.ms/vs/17/release/vc_redist.x86.exe)  

- **Union 1.0l or newer**  
    Install it via [Union Resource Manager](https://worldofplayers.ru/threads/41415/).

## Build requirements

In order to build this project, you'll need to install:
- **Visual Studio 2015-2019**  
    You can use the newer version, but you'll have to retarget the **Platform toolset** version to newer one (v14x).  
    This is because 2019 release is the last one to support **v140_xp** toolset.  
- **v140_xp toolset**  
    Install it via **Visual Studio Installer**.
- **Union SDK 1.0l**  
    Install it via [Union Resource Manager](https://worldofplayers.ru/threads/41415/).


## Contributing

Any contribution to this repository is welcome.  
In order to add your own changes to the project, just open a pull request.
