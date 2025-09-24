# Qt WebView2 模块

这是一个为 **Qt/QML** 应用程序提供 **WebView2** 控件的模块。它使你能够在 Windows 平台上，利用现代的 **Edge (Chromium)** 内核，在你的 Qt 应用中无缝地嵌入 Web 内容。

这个模块通过创建并管理一个原生 WebView2 实例，并将其作为子窗口嵌入到 QML 的 `Window` 或其他 `QQuickItem` 中来实现。

## 核心功能

  * **无缝集成**: 在 Qt/QML 界面中嵌入基于 WebView2 的高性能 Web 视图。
  * **灵活控制**: 通过 QML 属性和方法，可以轻松地进行页面导航 (`goBack`, `goForward`, `reload`) 和 URL 设置。
  * **跨平台**: 主要面向 Windows 平台，充分利用 Windows 原生 WebView2 API 的优势。

## 如何使用

### 1\. 注册 QML 类型

首先，你需要在 QML 文件中通过 `import` 语句引入这个模块。在 `QmlWebView2.h` 文件中，我们使用了 `QML_ELEMENT` 宏，它会自动将类注册到 QML 中。

```qml
import QtQuick
import QtQuick.Window
import QTEdgeWebView 1.0 // 注册的模块名和版本
```

### 2\. 在 QML 中创建 WebView2 实例

你可以在任何 `Window` 或 `Item` 中使用 `QmlWebView2` 控件，并像使用其他 QML 元素一样对其进行布局和属性设置。

```qml
// main.qml
Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello WebView2")

    QmlWebView2 {
        id: webView
        anchors.fill: parent // 让 WebView2 填充父窗口
        url: "https://www.google.com" // 设置初始 URL
    }
}
```

### 3\. 使用 QML API

`QmlWebView2` 暴露了一些方便的 API，你可以直接在 QML 中调用它们。

**属性 (Properties)**

  * `url`: `QUrl` 类型，用于获取或设置当前页面的 URL。当 URL 改变时，会发出 `urlChanged` 信号。

**方法 (Methods)**

  * `goBack()`: 导航到上一页。
  * `goForward()`: 导航到下一页。
  * `reload()`: 重新加载当前页面。

这是一个简单的例子，展示如何通过按钮控制 WebView 的导航：

```qml
// 假设你有一个按钮
Button {
    text: "Go Back"
    onClicked: webView.goBack()
}
```

## 代码结构

  * **`QmlWebView2.h`**: 这是 QML 接口的头文件，它继承自 `QQuickItem`。它定义了暴露给 QML 的属性 (`url`) 和方法 (`goBack`, `goForward`, `reload`)，并处理 QML 元素的生命周期和几何变化。
  * **`WebView2Host.h`**: 这是一个 C++ 类，它封装了所有 Windows 原生 **WebView2 API** 的调用。它负责创建、管理、调整大小和清理 WebView2 实例。

### 工作原理简述

1.  当 `QmlWebView2` 被添加到 `Window` 中时 (`itemChange` 信号)，它会调用 `WebView2Host::Create` 来创建原生的 WebView2 实例。
2.  `WebView2Host` 在内部使用 `CreateCoreWebView2Controller` 函数创建 WebView2 控件，并将其作为子窗口 (`m_parentHwnd`) 放置。
3.  `QmlWebView2` 监听自己的 `geometryChange` 事件，一旦尺寸或位置发生变化，它就会通知 `WebView2Host`，后者会调用 `Resize()` 方法，确保原生 WebView2 窗口的大小与 QML 元素同步。
4.  QML 中的 `url` 属性变化时，会调用 `WebView2Host::Navigate` 方法，从而实现页面的加载和跳转。
