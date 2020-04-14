# User Guide

## Configuration Flags

The Advanced Docking System has a number of global configuration options to
configure the design and the functionality of the docking system. Each
configuration will be explained in detail in the following sections.

### Setting Configuration Flags

You should set the configuration flags before you create the dock manager
instance. That means, setting the configurations flags is the first thing
you do, if you use the library.

```c++
CDockManager::setConfigFlags(CDockManager::DefaultOpaqueConfig);
CDockManager::setConfigFlag(CDockManager::RetainTabSizeWhenCloseButtonHidden, true);
...
d->DockManager = new CDockManager(this);
```

If you set the configurations flags, you can set individual flags using the
function `CDockManager::setConfigFlag` or you can set all flags using
the function `CDockManager::setConfigFlags`. Instead of settings all
flags individualy, it is better to pick a predefined set of configuration
flags and then modify individual flags. The following predefined
configurations are avilable

- `DefaultNonOpaqueConfig` - uses non opaque splitter resizing and non opaque docking
- `DefaultOpaqueConfig` - uses opaque splitter resizing and opaque docking

Pick one of those predefined configurations and then modify the following
configurations flags to adjust the docking system to your needs.

### `ActiveTabHasCloseButton`

If this flag is set (default configuration), the active tab in a tab area has
a close button.

![ActiveTabHasCloseButton true](cfg_flag_ActiveTabHasCloseButton_true.png)

If this flag is cleared, the active tab has no close button. You can combine
this with the flag `DockAreaCloseButtonClosesTab` to use the close button
of the dock are to close the single tabs.

![ActiveTabHasCloseButton true](cfg_flag_ActiveTabHasCloseButton_false.png)

### `DockAreaHasCloseButton`

If the flag is set (default configuration) each dock area has a close button.

![DockAreaHasCloseButton true](cfg_flag_DockAreaHasCloseButton_true.png)

If this flag is cleared, dock areas do not have a close button.

![DockAreaHasCloseButton true](cfg_flag_DockAreaHasCloseButton_false.png)