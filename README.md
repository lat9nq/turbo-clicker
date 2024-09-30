# turbo-clicker
This watches for bound inputs to generate mouse left button presses at a specified rate.

Still VERY much a work in progress, use at your own risk.

Features:
- Burst mode limits the number of clicks per use.
- Can specify rate in milliseconds or clicks per minute.
- Multiple rates may be specified and cycled through with a separate bind.
- Uses `/dev/input` to read events, and `/dev/uinput` to create button presses. It works regardless of X11 or Wayland display servers.

Planned features:
- Keyboard support
- Only usable when specific processes are active
- Status file to show rate or burst config, for use with external HUDs
- Cycle burst lengths

I originally wanted a way to fire semi-automatic weapons in video games at the maximum rate of fire.
Of specific note was Helldivers 2, a Windows game that works in Linux over Wine.
After finding myself disatisfied with the several options presently available for Linux, I decided it best I just write my own.