# Padphin

A file explorer for gamepad, built with C++20, ImGui, SDL2, and OpenGL3.

## Features

- Miller-column layout (parent, current, preview)
- Gamepad and keyboard navigation
- File preview for directories
- Clipboard support
- Custom icons and retro font (PressStart2P)

## Dependencies

- xmake
- ImGui (SDL2 + OpenGL3 backend)
- fmt
- nanosvg

## Building

```sh
xmake
```

## Running

```sh
xmake run padphin
```

## Controls

| Action | Keyboard | Gamepad |
|--------|----------|---------|
| Navigate | Arrow keys | D-pad |
| Accept | Enter | A button |
| Deny / Back | Escape | B button |
| Copy to clipboard | Ctrl+C | Y button |
| Open menu bar | Ctrl+M | — |

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Credits

- [Dear ImGui](https://github.com/ocornut/imgui) - Immediate Mode Graphical User Interface (MIT License)
- [fmt](https://github.com/fmtlib/fmt) - A modern formatting library (MIT License)
- [nanosvg](https://github.com/memononen/nanosvg) - Simple stupid SVG parser (zlib License)
- [Press Start 2P](https://zone38.net/font/) - Retro bitmap font (SIL Open Font License 1.1)
- [Kenney Input Prompts](https://www.kenney.nl/assets/input-prompts) - Gamepad and keyboard icons (CC0 1.0 Universal)
