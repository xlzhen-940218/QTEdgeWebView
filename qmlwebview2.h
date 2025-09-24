#ifndef QMLWEBVIEW2_H
#define QMLWEBVIEW2_H

// QmlWebView2.h
#pragma once

#include <QQuickItem>
#include <QUrl>
#include <memory>

#include "EdgeWebView2.h"


class QmlWebView2 : public QQuickItem
{
    Q_OBJECT
    // 注册到QML，模块名为 "WebView2", 版本 1.0
    QML_ELEMENT

    // 暴露给QML的url属性
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)

public:
    QmlWebView2(QQuickItem* parent = nullptr);
    ~QmlWebView2();

    QUrl url() const;
    void setUrl(const QUrl& url);

    // Q_INVOKABLE 暴露给QML的方法
    Q_INVOKABLE void goBack();
    Q_INVOKABLE void goForward();
    Q_INVOKABLE void reload();

signals:
    void urlChanged();

protected:
    // 当QML Item的几何属性改变时调用
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
    // 当Item被添加到窗口中时调用
    void itemChange(ItemChange change, const ItemChangeData& data) override;

private slots:
    // 更新原生窗口的位置和大小
    void updateWebViewBounds();

private:
    void initializeWebView();

    std::unique_ptr<WebView2Host> m_webViewHost;
    QUrl m_url;
    bool m_isWebViewCreated = false;
    bool m_isItemVisible = false;
};

#endif
