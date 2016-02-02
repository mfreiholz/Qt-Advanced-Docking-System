# Advanced Docking System
Manages content widgets a lot like Visual Studio or similar programs.
I also try to get everything done with basic Qt functionality.
Basic usage of QWidgets an QLayouts and using basic styles as much as possible.

## Requirements (Tested!)
**Windows**
- \>= Qt 5.5
- VC12 or MinGW (-std=c++11)

**Linux**
- Not yet tested

## Build
Open the `build.pro` with QtCreator and start the build, that's it.

## Notes
- *SectionContent* class may safe a "size-type" property, which defines how the size of the widget should be handled.
	- PerCent: Resize in proportion to other widgets.
	- Fixed: Width or height are fixed (based on orientation).

## TODOs
Sorted by priority
- Serialize state/size/positions of dockings
- Deserialize state/size/positions of dockings
- Make compatible with Qt 4.5 (\*ROFL!\*)
- Drop indicator images should be fully visible over the DropOverlay rectangle
- Pin contents: Pins a content and its title widget to the edge and opens on click/hover as long as it has focus

## Bugs
- Working with outer-edge-drops sometimes leaves empty splitters
- **[DONE]** Clean up of unused e.g. count()<=1 QSplitters doesn't work well

## Credits
- Drop indicator images from:
	http://www.codeproject.com/Articles/140209/Building-a-Docking-Window-Management-Solution-in-W

## License
Not sure yet... It's in development status anyway.