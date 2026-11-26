# CustomGraphicsTextItem

[![Qt](https://img.shields.io/badge/Qt-5-41CD52?logo=qt&logoColor=white)](https://www.qt.io/)
[![C++](https://img.shields.io/badge/C%2B%2B-11-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![qmake](https://img.shields.io/badge/build-qmake-2D2D2D)](https://doc.qt.io/qt-6/qmake-manual.html)

**English** | [简体中文](README.zh_CN.md)

An editable text item for `QGraphicsScene` — the kind of "type text anywhere on the canvas" box you expect from an annotation tool. Derived from `QAbstractGraphicsShapeItem`, it keeps its own `QTextDocument` and `QTextCursor` and paints a real blinking caret on the scene.

## What you get

- **Double-click to edit** — double-clicking the item enters edit mode; the click position is mapped to the closest text line and character (via `QTextLayout::lineAt` + `xToCursor()`), so multi-line text places the caret where you actually clicked instead of jumping to the end.
- **Real caret and selection** — the caret is drawn by the item itself and toggled by a 500 ms `QTimer`, with the caret line computed from the document layout; focus changes start and stop it, and focus-out leaves edit mode cleanly.
- **Keyboard editing** — printable input is inserted into the document, with arrow-key navigation, `Backspace`, and `Ctrl+A` (select all, with a translucent blue selection background) and `Ctrl+C` (copy the text to the clipboard). `PageUp` / `PageDown` change the font size.
- **Input method support** — `inputMethodEvent()` commits IME strings, and `inputMethodQuery()` answers `ImCursorRectangle`, `ImFont`, `ImCursorPosition`, `ImSurroundingText` and `ImCurrentSelection`, so Chinese input keeps its candidate window next to the caret instead of in a screen corner.
- **Self-sizing geometry** — the item's bounding rect follows the document size (`prepareGeometryChange()` on every change), so the text never paints outside its own area.
- **Standard item behaviour kept** — movable, selectable, focusable and input-method aware, and the usual dashed selection frame of a graphics item is preserved.

## Demo

`Widget.cpp` puts a `QGraphicsScene` (400×400) inside a `QGraphicsView` with a single `GraphicsTextItem` at (100, 100) — double-click it and start typing.

## Build

```bash
qmake CustomGraphicsTextItem.pro
make            # nmake / mingw32-make on Windows
```

Qt 5 with the `core gui widgets` modules, C++11.

## Known limitations

This is an experiment rather than a drop-in `QTextEdit`: there is no mouse-drag selection, no paste (`Ctrl+V`), and the caret line is measured character by character with `QFontMetrics`, so unusual glyph widths or automatic line wrapping are not perfect.

## Links

- Related: [ShortcutInput](https://github.com/SunnyCapturer/ShortcutInput) — shortcut capture widgets
- Related: [qt-designer-custom-widgets](https://github.com/SunnyCapturer/qt-designer-custom-widgets) — reuseable Sunny widgets
