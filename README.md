# Advanced Docking System
Manages content widgets more like Visual Studio or similar programs.
I also try to get everything done with basic Qt functionality.
Basic usage of QWidgets an QLayouts and using basic styles as much as possible.

![Layout of widgets](preview01.png)
![Dropping widgets](preview02.png)

## Tested with - Requirements
**Windows**
- \>= Qt 5.5, VC12 or MinGW
- \>= Qt 4.5.3 VC9 - *not as good...*

**Linux**
- Not yet tested

## Build
Open the `build.pro` with QtCreator and start the build, that's it.

## Release & Development
The master branch is not guaranteed to be stable or does even build, since it is the main working branch.
If you want a version that builds you should always use a release/beta tag.

## Notes
- *SectionContent* class may safe a "size-type" property, which defines how the size of the widget should be handled.
	- PerCent: Resize in proportion to other widgets.
	- Fixed: Width or height are fixed (based on orientation).

## TODOs
Sorted by priority

### Beta 0.1
- [x] Improve FloatingWidget (Remove maximize button, only support close-button which hides the widget)
- [x] Serialize and Deserialize state/size/positions of dockings
- [x] Make compatible with Qt 4.5 (\*ROFL!\*)
- [x] Save and restore FloatingWidget states
- [x] Restore: Manage new or deleted SectionContent objects, which are not available
- [x] Working with outer-edge-drops sometimes leaves empty splitters #BUG
- [x] Clean up of unused e.g. count()<=1 QSplitters doesn't work well #BUG
- [x] Show close button on right corner of SectionWidget. How to safe last section position?
- [x] Serialize state of `_hiddenSectionContents`
- [ ] It would be easier when the SectionTitleWidget and SectionContentWidget are created inside the "SectionContent::newSectionContent(..)" method.
  This would make sure, that those two objects always exists.
- [ ] `ContainerWidget::showSectionContent` needs to insert the SC at the correct preferred position of SW
- [ ] Empty splitters, if only 2 or 1 items are in container
- [ ] Restore: Handle out-of-screen geometry for floating widgets

### Some day...
- [ ] Drop indicator images should be fully visible over the DropOverlay rectangle
- [ ] Pin contents: Pins a content and its title widget to the edge and opens on click/hover as long as it has focus
- [ ] API: Make it possible to use custom drop images
- [ ] API: Add possibility to make a SectionContent element floatable (`ContainerWidget::setFloating(SectionContent*)`?)

## Credits
- Drop indicator images from:
	http://www.codeproject.com/Articles/140209/Building-a-Docking-Window-Management-Solution-in-W

## License
Not sure yet... It's in development status anyway.