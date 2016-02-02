# Advanced Docking System

## Requirements
- C++11 Compiler (e.g.: \>=VC12)
- \>= Qt 5.5

## Notes
- SectionContent may safe a "size-type" property, which defines how the size of the widget should be handled.
	- PerCent: Resize in proportion to other widgets.
	- Fixed: Width or height are fixed (based on orientation).

## TODOs
- [] Serialize state/size/positions of dockings
- [] Deserialize state/size/positions of dockings
- [] Drop indicator images should be fully visible over the DropOverlay rectangle

## Bugs
- Working with outer-edge-drops sometimes leaves empty splitters
- [DONE] Clean up of unused e.g. count()<=1 QSplitters doesn't work well

## License notes
- Copied drop images from
	http://www.codeproject.com/Articles/140209/Building-a-Docking-Window-Management-Solution-in-W
