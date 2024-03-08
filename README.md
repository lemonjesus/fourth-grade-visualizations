# Some Windows Media Player Visualizations I Designed When I Was In 4th Grade

Back in elementary school, during my free time wondering why I had no friends, I would take blank pages of printer paper and design software UI. Among those sketches: Music visualizers. They were for a theoretical program I called "mTunes" (very creative) for my theoretical operating system.

I decided to recreate some of them in Windows Media Player and make a video about it. For 14 nights I live streamed this effort on [my Twitch Channel](https://twitch.tv/tuckerosman). I'm not sure why I did that, but I did.

[![Watch the video](https://img.youtube.com/vi/RvgOtbQnySE/maxresdefault.jpg)](https://www.youtube.com/watch?v=RvgOtbQnySE)

## Using the Visualizations

Requirements for using:
1. Windows 7 (with the Platform Update) or later
2. 64-bit version of Windows Media Player (if you're not building it yourself)

I've provided a 64-bit binary of the visualizations. Unfortunately, it seems like when you got to the start menu and type "Windows Media Player" it opens the 32-bit version. To open the 64-bit version, navigate to `C:\Program Files\Windows Media Player` and open `wmplayer.exe` from there.

To install the visualizations, simply copy the `.dll` files from the `Visualizations` folder to `C:\Program Files\Windows Media Player\Visualizations`, and then in an elevated command prompt, run `regsvr32` on it so WMP knows abou tit. It will show up in both the 32-bit and 64-bit versions of Windows Media Player, but it will only work in the 64-bit version. Compiling a 32-bit version is left to the reader (for now).

## Building the Visualizations

The visualizations are built using the Windows Media Player SDK. The SDK is included with the Windows SDK. I used Visual Studio 2008 to be period accurate. I assume importing the solution into a newer version of Visual Studio will work, but I haven't tried it. Assuming you have the Windows SDK installed, you should be able to open the solution and build it.

## Extending the Visualizations

The visualizations are built using C++ and Direct2D. The organization of the code is a bit lackluster, I'll admit. But C++ is not my first language. If you want to add a visualization to this pack, take a look at any "implemented *" commit to see how it was done. This is a good starting point for creating a new visualizations. All of your drawing is done in the `render` function. You're provided with the bitmap of the previous frame to apply effects to, and the render target to draw the current frame to.

If you want to follow along with the development of the visualizations, you can watch the 14 nights of live streams on my stream archive channel. The playlist is [here](https://www.youtube.com/playlist?list=PLOvoV9yfBz-f_N2lapcRvlT09QB_y-qBA).