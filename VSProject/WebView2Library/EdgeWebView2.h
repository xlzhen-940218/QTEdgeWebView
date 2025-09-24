// WebView2Host.h
#pragma once

#include <windows.h>
#include <string>
#include <functional>
class ICoreWebView2Controller;
class ICoreWebView2;
//struct EventRegistrationToken;
class __declspec(dllexport) WebView2Host {
public:
    WebView2Host();
    ~WebView2Host();

    // 创建并初始化WebView2实例
    // parentHwnd: 用于承载WebView2的父窗口句柄
    // onCreated: WebView2创建成功后的回调函数，可以开始执行Navigate等操作
    bool Create(HWND parentHwnd, std::function<void(bool)> onCreated);

    // 导航到指定URL
    void Navigate(const std::wstring& url);

    // 调整WebView2控件的大小以适应父窗口
    void Resize();

    // 关闭并清理WebView2资源
    void Close();

private:
    // 供回调函数使用的内部方法，用于完成初始化
    void InitializeWebView(ICoreWebView2Controller* controller);

    HWND m_parentHwnd = nullptr;
    ICoreWebView2Controller* m_controller = nullptr;
    ICoreWebView2* m_webView = nullptr;
    std::function<void(bool)> m_onCreatedCallback;

    // 事件注册Token，用于后续注销
    //EventRegistrationToken* m_navigationStartingToken;
    //EventRegistrationToken* m_webMessageReceivedToken;
};