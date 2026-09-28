# Padphin

A gamepad-first file explorer, built with C++20, ImGui, SDL2, and OpenGL3.
Ranger-style Miller columns (parent, current, preview) with keyboard fallback.

## Features

- Miller-column layout (parent, current, preview), scaled for 720p / 1080p / 1440p / 4K
- Gamepad navigation (D-pad + left stick) and keyboard navigation (arrows + WASD)
- Per-device footer hints: Xbox / PlayStation / Switch / keyboard icon sets switch automatically
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
  - `fmtlog`
  - `nanosvg`
  - `libsdl2_mixer`

## Building

```sh
xmake
```


## Running

```sh
xmake run padphin
```

Open a specific directory on start:

```sh
xmake run padphin -- /path/to/dir
xmake run padphin -- ~/Documents
```

Quit with `Alt+Esc` or the window-close event.

## Controls

| Action | Keyboard | Gamepad |
|--------|----------|---------|
| Move selection up / down | Up / Down, `W` / `S` | D-pad Up / Down, left stick Y |
| Go to parent / open directory | Left / Right, `A` / `D` | D-pad Left / Right, left stick X |
| Open directory (enter) | `Enter` or `Space` | A button |
| Back to parent | `Esc` | B button |
| Add to clipboard | `C` | X button |
| Menu (reserved, no action yet) | `Alt` | Y button |

Footer hints show the icons for the last active device type.

## Notes / current limitations

- `OPEN_MENU_BAR` (`Alt` / `Y`) is polled and shown in the footer but has no action wired up yet.
- `Clipboard` implements copy / move / delete, but only `append` (add to clipboard) is currently called from the UI.
- `show_hidden` exists in code but is not toggleable yet; dotfiles are skipped when listing directories.

## Project layout

- `src/main.cpp` — SDL / OpenGL / ImGui setup, main loop, start-path handling
- `src/include/App.hpp`, `src/lib/App.cpp` — browser state, Miller columns, header/footer rendering
- `src/include/IControl.hpp`, `GamepadControl`, `KeyboardControl`, `CompositeControl` — unified input (`Dir` + `ACCEPT` / `DENY` / `ADD_TO_CLIPBOARD` / `OPEN_MENU_BAR`)
- `src/include|lib/Clipboard`, `SoundPlayer`, `IconLoader`, `Gate`, `DisplaySettings` — helpers
- `resources/font/` — `BigBlueTerm.ttf` (currently loaded at 16pt), `PressStart2P.ttf` (bundled)
- `resources/icon/kenney/` — per-device SVG prompt sets
- `resources/sounds/` — `hover.wav` (used); other `.wav` files are currently unused

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Credits

- [Dear ImGui](https://github.com/ocornut/imgui) - Immediate Mode Graphical User Interface (MIT License)
- [fmt / fmtlog](https://github.com/fmtlib/fmt) - Formatting and logging (MIT License)
- [nanosvg](https://github.com/memononen/nanosvg) - Simple stupid SVG parser (zlib License)
- [SDL](https://www.libsdl.org) + [SDL_mixer](https://github.com/libsdl-org/SDL_mixer) - Window, input, and audio (zlib License)
- [BigBlueTerm](resources/font/) - Bitmap-style terminal font used by the app
- [Press Start 2P](https://zone38.net/font/) - Retro bitmap font bundled in `resources/font/` (SIL Open Font License 1.1)
- [Kenney Input Prompts](https://www.kenney.nl/assets/input-prompts) - Gamepad and keyboard icons (CC0 1.0 Universal)
