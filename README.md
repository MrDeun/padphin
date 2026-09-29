# Padphin

A gamepad-first file explorer, built with C++20, ImGui, SDL2, and OpenGL3.
Ranger-style Miller columns (parent, current, preview) with keyboard fallback.

## Features

- Miller-column layout (parent, current, preview); the parent and preview columns are read-only, only the current column is interactive
- Continuous UI scaling relative to a 1920x1080 baseline (`min(w/1920, h/1080)`), so 720p / 1440p / 4K and non-16:9 screens all work
- Gamepad navigation (D-pad + left stick, hot-plug supported) and keyboard navigation (arrows + WASD)
- Tweened highlight bars that slide between rows via [ImAnim](https://github.com/soufianekhiat/ImAnim) (0.12s, out-cubic, crossfaded after the first move)
- Per-device footer hints: Xbox gamepad and keyboard icon sets switch automatically based on the last active device
- Directory preview (child listing) and file preview (name, size, type, text preview up to 64 KiB / ~100 lines, with binary and too-large handling)
- Directories sorted first (case-insensitive); dotfiles are currently hidden
- Clipboard staging: add entries with a button press, count shown in the header
- Navigation sound effects via SDL_mixer
- Optional start directory from the command line, with `~` expansion
- Fullscreen-desktop window, mouse input disabled

## Dependencies

- [xmake](https://xmake.io) + C++20 compiler
- SDL2 and OpenGL3 (system, `GL` linked on Linux)
- xmake packages (resolved automatically):
  - `imgui` (with `sdl2` + `opengl3` backends)
  - `fmtlog` (pulls in `fmt`)
  - `nanosvg`
  - `libsdl2_mixer`
- Vendored in-tree: [ImAnim](https://github.com/soufianekhiat/ImAnim) in `src/include/im_anim.h` + `src/lib/im_anim.cpp` (MIT, see `Third party Licenses/LICENSE`)

## Building

```sh
xmake
```


## Running

```sh
xmake run padphin
```

Open a specific directory on start (only the first argument is read; it must be a
directory, otherwise the app falls back to the current working directory):

```sh
xmake run padphin -- /path/to/dir
xmake run padphin -- ~/Documents
```

Quit with `Alt+Esc` or the window-close event. There is no gamepad quit binding.

Resources are resolved relative to the executable (`<exe>/resources`, then one and
two levels up). `resources/` is copied next to the binary after each build; if the
folder cannot be found the app logs an error and exits.

## Controls

| Action | Keyboard | Gamepad |
|--------|----------|---------|
| Move selection up / down | Up / Down, `W` / `S` | D-pad Up / Down, left stick Y |
| Go to parent / open directory | Left / Right, `A` / `D` | D-pad Left / Right, left stick X |
| Open directory (enter) | `Enter` or `Space` | A button |
| Back to parent | `Esc` | B button |
| Add to clipboard | `C` | X button |
| Menu (reserved, no action yet) | `Alt` (either) | Y button |
| Quit | `Alt+Esc` | — |

Footer hints show the icons for the last active device type. Held directions
auto-repeat after 0.35s every 0.10s; the left stick uses a 0.5 deadzone and is
ignored while a D-pad direction is held.

## Notes / current limitations

- `OPEN_MENU_BAR` (`Alt` / `Y`) is polled and shown in the footer (with the `Tab` keyboard icon) but has no action wired up yet.
- `Clipboard` implements copy / move / delete, but only `append` (add to clipboard) is currently called from the UI, so there is no paste.
- `show_hidden` exists in code but is not toggleable yet; dotfiles are skipped when listing directories.
- The PlayStation and Switch icon sets are loaded at startup but never rendered — only the Xbox and keyboard sets are used.
- Opening a file does nothing; there is no rename, search, or settings UI.

## Project layout

- `src/main.cpp` — SDL / OpenGL / ImGui setup, main loop, start-path handling
- `src/include/App.hpp`, `src/lib/App.cpp` — browser state, Miller columns, header/footer rendering
- `src/include/IControl.hpp`, `GamepadControl`, `KeyboardControl`, `CompositeControl` — unified input (`Dir` + `ACCEPT` / `DENY` / `ADD_TO_CLIPBOARD` / `OPEN_MENU_BAR`)
- `src/include/EntryHighlight.hpp`, `src/lib/EntryHighlight.cpp` — ImAnim-tweened selection bar
- `src/include|lib/Clipboard`, `SoundPlayer`, `IconLoader`, `Gate`, `DisplaySettings` — helpers
- `src/include/im_anim.h`, `src/lib/im_anim.cpp` — vendored ImAnim
- `resources/font/` — `BigBlueTerm.ttf` (the only font loaded, at 16pt); `DepartureMono.otf` and `PressStart2P.ttf` are bundled but unused
- `resources/icon/kenney/` — per-device SVG prompt sets (only 8 icons per set are loaded; the whole `gamepad/` set is unused)
- `resources/sounds/` — `hover.wav` (used on direction changes); the other 9 `.wav` files are currently unused

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Credits

- [Dear ImGui](https://github.com/ocornut/imgui) - Immediate Mode Graphical User Interface (MIT License)
- [fmt / fmtlog](https://github.com/fmtlib/fmt) - Formatting and logging (MIT License)
- [nanosvg](https://github.com/memononen/nanosvg) - Simple stupid SVG parser (zlib License)
- [SDL](https://www.libsdl.org) + [SDL_mixer](https://github.com/libsdl-org/SDL_mixer) - Window, input, and audio (zlib License)
- [ImAnim](https://github.com/soufianekhiat/ImAnim) - Tween animation engine for ImGui, vendored in `src/` (MIT License)
- [BigBlueTerm](resources/font/) - Bitmap-style terminal font used by the app
- [Press Start 2P](https://zone38.net/font/) - Retro bitmap font bundled in `resources/font/` (SIL Open Font License 1.1)
- [Kenney Input Prompts](https://www.kenney.nl/assets/input-prompts) - Gamepad and keyboard icons (CC0 1.0 Universal)
