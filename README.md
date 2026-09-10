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

See `resources/font/LICENSE.txt` for font licensing.
