import QtQuick
import QtQuick.Window
import QTEdgeWebView 1.0
Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    QmlWebView2 {
          id: webView
          anchors.fill: parent
          // 设置初始URL
          url: "https://www.google.com"
      }
}
