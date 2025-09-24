// WebView2Host.cpp
#include "EdgeWebView2.h"
#include <functional> // For std::function
#include <wrl/implements.h> // 使用一个轻量级的wrl头文件来实现COM接口，避免了对wil的依赖

#include "WebView2.h"
// 使用匿名命名空间隐藏辅助类
namespace {

    // EnvironmentCompletedHandler: 创建环境完成后的回调处理类
    class EnvironmentCompletedHandler : public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>
    {
    public:
        EnvironmentCompletedHandler(HWND parentHwnd, std::function<HRESULT(ICoreWebView2Environment*)> callback)
            : m_parentHwnd(parentHwnd), m_callback(callback) {
        }

        // ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
        HRESULT STDMETHODCALLTYPE Invoke(HRESULT result, ICoreWebView2Environment* env) override {
            if (SUCCEEDED(result)) {
                return m_callback(env);
            }
            return result;
        }

    private:
        HWND m_parentHwnd;
        std::function<HRESULT(ICoreWebView2Environment*)> m_callback;
    };


    // ControllerCompletedHandler: 创建控制器完成后的回调处理类
    class ControllerCompletedHandler : public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>
    {
    public:
        ControllerCompletedHandler(std::function<void(ICoreWebView2Controller*)> callback)
            : m_callback(callback) {
        }

        // ICoreWebView2CreateCoreWebView2ControllerCompletedHandler
        HRESULT STDMETHODCALLTYPE Invoke(HRESULT result, ICoreWebView2Controller* controller) override {
            if (SUCCEEDED(result) && controller) {
                m_callback(controller);
            }
            return S_OK;
        }
    private:
        std::function<void(ICoreWebView2Controller*)> m_callback;
    };


    // NavigationStartingHandler: 导航开始事件处理
    class NavigationStartingHandler : public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        ICoreWebView2NavigationStartingEventHandler>
    {
    public:
        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) override {
            LPWSTR uri = nullptr;
            args->get_Uri(&uri);
            std::wstring source(uri);

            // 仅允许 https 协议的导航
            if (source.substr(0, 5) != L"https") {
                args->put_Cancel(TRUE);
            }

            CoTaskMemFree(uri);
            return S_OK;
        }
    };


    // WebMessageReceivedHandler: 网页消息接收事件处理
    class WebMessageReceivedHandler : public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        ICoreWebView2WebMessageReceivedEventHandler>
    {
    public:
        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) override {
            LPWSTR message = nullptr;
            args->TryGetWebMessageAsString(&message);

            // 将收到的消息原样发回
            if (message) {
                sender->PostWebMessageAsString(message);
                CoTaskMemFree(message);
            }

            return S_OK;
        }
    };

} // end anonymous namespace


// --- WebView2Host Class Implementation ---

WebView2Host::WebView2Host() {}

WebView2Host::~WebView2Host() {
    Close();
}

bool WebView2Host::Create(HWND parentHwnd, std::function<void(bool)> onCreated) {
    m_parentHwnd = parentHwnd;
    m_onCreatedCallback = onCreated;

    // 定义创建Controller的回调逻辑
    auto createControllerCallback = [this](ICoreWebView2Environment* env) -> HRESULT {
        auto controllerCompletedHandler = Microsoft::WRL::Make<ControllerCompletedHandler>(
            [this](ICoreWebView2Controller* controller) {
                InitializeWebView(controller);
            }
        );
        return env->CreateCoreWebView2Controller(m_parentHwnd, controllerCompletedHandler.Get());
        };

    // 创建环境，这是第一步
    auto environmentCompletedHandler = Microsoft::WRL::Make<EnvironmentCompletedHandler>(m_parentHwnd, createControllerCallback);
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr, environmentCompletedHandler.Get());

    return SUCCEEDED(hr);
}

void WebView2Host::InitializeWebView(ICoreWebView2Controller* controller) {
    if (!controller) {
        if (m_onCreatedCallback) m_onCreatedCallback(false);
        return;
    }

    m_controller = controller;
    m_controller->AddRef();
    m_controller->get_CoreWebView2(&m_webView);

    // 设置WebView2
    if (m_webView) {
        m_webView->AddRef();

        ICoreWebView2Settings* settings;
        m_webView->get_Settings(&settings);
        settings->put_IsScriptEnabled(TRUE);
        settings->put_AreDefaultScriptDialogsEnabled(TRUE);
        settings->put_IsWebMessageEnabled(TRUE);
        settings->Release();

        //if (!m_navigationStartingToken) {
        //    m_navigationStartingToken = new EventRegistrationToken();
        //}

        //if (!m_webMessageReceivedToken) {
        //    m_webMessageReceivedToken = new EventRegistrationToken();
        //}

        //// 注册事件处理器
        //m_webView->add_NavigationStarting(Microsoft::WRL::Make<NavigationStartingHandler>().Get(), m_navigationStartingToken);
        //m_webView->add_WebMessageReceived(Microsoft::WRL::Make<WebMessageReceivedHandler>().Get(), m_webMessageReceivedToken);

        // 添加在文档创建时执行的脚本
        m_webView->AddScriptToExecuteOnDocumentCreated(L"Object.freeze(Object);", nullptr);
        /*m_webView->AddScriptToExecuteOnDocumentCreated(
            L"window.chrome.webview.addEventListener('message', event => alert(event.data));"
            L"window.chrome.webview.postMessage(window.document.URL);",
            nullptr);*/

        Resize(); // 初始调整大小

        if (m_onCreatedCallback) m_onCreatedCallback(true); // 通知调用者创建成功
    }
    else {
        if (m_onCreatedCallback) m_onCreatedCallback(false); // 通知调用者创建失败
    }
}

void WebView2Host::Navigate(const std::wstring& url) {
    if (m_webView) {
        m_webView->Navigate(url.c_str());
    }
}

void WebView2Host::Resize() {
    if (m_controller) {
        RECT bounds;
        GetClientRect(m_parentHwnd, &bounds);
        m_controller->put_Bounds(bounds);
    }
}

void WebView2Host::Close() {
    if (m_webView) {
       /* if (m_navigationStartingToken) {
            m_webView->remove_NavigationStarting(*m_navigationStartingToken);
        }
        if (m_webMessageReceivedToken) {
            m_webView->remove_WebMessageReceived(*m_webMessageReceivedToken);
        }*/
        m_webView->Release();
        m_webView = nullptr;
    }
    if (m_controller) {
        m_controller->Close();
        m_controller->Release();
        m_controller = nullptr;
    }
}