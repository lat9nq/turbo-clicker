# turbo-clicker
This watches for bound inputs to generate mouse left button presses at a specified rate. Hold time, burst lengths, multiple binds, and binds for multiple settings can be bound.

I originally wanted a way to fire semi-automatic weapons in video games at the maximum rate of fire.
Of specific note was Helldivers 2, a Windows game that works in Linux over Wine.
After finding myself disatisfied with the several options presently available for Linux, strangely including Logitech's macro support for mice, I decided it best I just write my own.

A work-in-progress, but I find myself using it very often. Have fun at your own risk.

User needs to be in the `input` group to use event input devices.

Features:
- Burst mode limits the number of clicks per use.
- Can specify rate in milliseconds or clicks per minute.
- Multiple rates may be specified and cycled through with a separate bind.
- Uses `/dev/input` to read events, and `/dev/uinput` to create button presses. It works regardless of X11 or Wayland display servers.
- Status file to show rate or burst config, for use with external HUDs
- Keyboard support (up to 96% at the moment)

Planned features:
- Joystick support
- Activate only when specific processes are running
- Cycle burst lengths

Possible features, no commitment:
- Windows backends
- GTK+ GUI

Building:

```
mkdir build
cd build
cmake ..
make
```

Build products will be in the `build/bin` directory.

I'll write more detailed build instructions at a later time, but once cloned and in the necessary directory, this will get you going.
