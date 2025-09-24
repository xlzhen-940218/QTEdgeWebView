// QmlWebView2.cpp
#include "qmlwebview2.h"
#include <QWindow>
#include <QQuickWindow>
#ifdef Q_OS_WIN
#include <windows.h>

QmlWebView2::QmlWebView2(QQuickItem* parent)
    : QQuickItem(parent), m_webViewHost(std::make_unique<WebView2Host>())
{
    // 当QML Item的位置或大小变化时，更新原生窗口
    connect(this, &QQuickItem::xChanged, this, &QmlWebView2::updateWebViewBounds);
    connect(this, &QQuickItem::yChanged, this, &QmlWebView2::updateWebViewBounds);
    connect(this, &QQuickItem::widthChanged, this, &QmlWebView2::updateWebViewBounds);
    connect(this, &QQuickItem::heightChanged, this, &QmlWebView2::updateWebViewBounds);
}

QmlWebView2::~QmlWebView2()
{
}

QUrl QmlWebView2::url() const
{
    return m_url;
}

void QmlWebView2::setUrl(const QUrl& url)
{
    if (m_url != url) {
        m_url = url;
        emit urlChanged();
        if (m_isWebViewCreated && m_url.isValid()) {
            m_webViewHost->Navigate(m_url.toString().toStdWString());
        }
    }
}

// Q_INVOKABLE 方法的实现 (可选，但推荐)
void QmlWebView2::goBack() { /* ...需要扩展WebView2Host来实现... */ }
void QmlWebView2::goForward() { /* ...需要扩展WebView2Host来实现... */ }
void QmlWebView2::reload() { /* ...需要扩展WebView2Host来实现... */ }


void QmlWebView2::itemChange(ItemChange change, const ItemChangeData& data)
{
    QQuickItem::itemChange(change, data);
    // 当Item首次被关联到一个窗口时，初始化WebView2
    if (change == ItemSceneChange && data.window) {
        initializeWebView();
    }
}

void QmlWebView2::initializeWebView() {
    if (m_isWebViewCreated || !window()) return;

    // 获取父窗口的HWND
    HWND parentHwnd = reinterpret_cast<HWND>(window()->winId());
    if (!parentHwnd) return;

    // 创建WebView2实例
    m_webViewHost->Create(parentHwnd, [this](bool success) {
        if (success) {
            m_isWebViewCreated = true;
            // 创建成功后，设置初始大小并导航到URL
            updateWebViewBounds();
            if (m_url.isValid()) {
                m_webViewHost->Navigate(m_url.toString().toStdWString());
            }
        }
    });
}


void QmlWebView2::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
    updateWebViewBounds();
}

void QmlWebView2::updateWebViewBounds()
{
    if (!m_isWebViewCreated || !window()) {
        return;
    }

    // 获取QML Item在窗口中的绝对坐标和大小
    QPointF itemPos = mapToScene(QPointF(0, 0));
    RECT bounds;
    bounds.left = static_cast<LONG>(itemPos.x());
    bounds.top = static_cast<LONG>(itemPos.y());
    bounds.right = static_cast<LONG>(itemPos.x() + width());
    bounds.bottom = static_cast<LONG>(itemPos.y() + height());

    m_webViewHost->Resize();
}

#endif // Q_OS_WIN
