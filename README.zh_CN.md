# CustomGraphicsTextItem

[![Qt](https://img.shields.io/badge/Qt-5-41CD52?logo=qt&logoColor=white)](https://www.qt.io/)
[![C++](https://img.shields.io/badge/C%2B%2B-11-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![qmake](https://img.shields.io/badge/build-qmake-2D2D2D)](https://doc.qt.io/qt-6/qmake-manual.html)

[English](README.md) | **简体中文**

`QGraphicsScene` 里的可编辑文本图元 —— 也就是标注工具中“在画布任意位置输入文字”的那种文本框。它继承自 `QAbstractGraphicsShapeItem`，自带 `QTextDocument` 与 `QTextCursor`，并在场景中绘制真正会闪烁的光标。

## 内容组成

- **双击进入编辑** — 双击图元即进入编辑状态；点击位置会通过 `QTextLayout::lineAt` 与 `xToCursor()` 映射到最近的一行与字符，多行文本时也能把光标放在鼠标点击处，而不是跳到文末。
- **真实的光标与选区** — 光标由图元自己绘制，用 500 ms 的 `QTimer` 控制闪烁，光标线位置由文档布局算出；焦点进出时相应地启动与停止，失焦后干净地退出编辑。
- **键盘编辑** — 可打印字符直接插入文档，支持方向键移动、`Backspace` 退格，`Ctrl+A` 全选（带半透明蓝色选区底色）、`Ctrl+C` 复制文本到剪贴板；`PageUp` / `PageDown` 调整字号。
- **输入法支持** — `inputMethodEvent()` 提交输入法上屏字符串，`inputMethodQuery()` 回答 `ImCursorRectangle`、`ImFont`、`ImCursorPosition`、`ImSurroundingText`、`ImCurrentSelection`，让中文输入的候选框贴着光标显示，而不是飘到屏幕角落。
- **自适应尺寸** — 包围盒随文档尺寸变化（每次改动前调用 `prepareGeometryChange()`），文字不会画到自身区域之外。
- **保留图元常规行为** — 可移动、可选中、可获得焦点并接受输入法，图形项默认的虚线选中框也保留。

## 示例

`Widget.cpp` 在一个 `QGraphicsView` 中放置 400×400 的 `QGraphicsScene`，并在 (100, 100) 处添加一个 `GraphicsTextItem`；双击即可开始输入。

## 构建

```bash
qmake CustomGraphicsTextItem.pro
make            # Windows 下为 nmake / mingw32-make
```

依赖 Qt 5 的 `core gui widgets` 模块，C++11。

## 已知限制

这是一个探索性实现，并非可直接替代 `QTextEdit` 的控件：不支持鼠标拖选，未实现粘贴（`Ctrl+V`），光标线按 `QFontMetrics` 逐字符累加计算，遇到特殊字形宽度或自动换行时会有少量偏差。

## 相关链接

- 相关项目：[ShortcutInput](https://github.com/SunnyCapturer/ShortcutInput) — 快捷键捕获控件
- 相关项目：[qt-designer-custom-widgets](https://github.com/SunnyCapturer/qt-designer-custom-widgets) — 可复用的 Sunny 控件集
