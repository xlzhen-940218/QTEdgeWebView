/****************************************************************************
** Generated QML type registration code
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <QtQml/qqml.h>
#include <QtQml/qqmlmoduleregistration.h>

#include <qmlwebview2.h>


#if !defined(QT_STATIC)
#define Q_QMLTYPE_EXPORT Q_DECL_EXPORT
#else
#define Q_QMLTYPE_EXPORT
#endif
Q_QMLTYPE_EXPORT void qml_register_types_QTEdgeWebView()
{
    qmlRegisterTypesAndRevisions<QmlWebView2>("QTEdgeWebView", 1);
    qmlRegisterAnonymousType<QQuickItem, 254>("QTEdgeWebView", 1);
    qmlRegisterModule("QTEdgeWebView", 1, 0);
}

static const QQmlModuleRegistration registration("QTEdgeWebView", qml_register_types_QTEdgeWebView);
