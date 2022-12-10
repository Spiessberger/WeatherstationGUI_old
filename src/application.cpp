#include "application.h"

#include <QDebug>
#include <QDir>

using namespace std::chrono_literals;

static void dumpFileTree(const QDir &dir, int indent) {
    const auto fiList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
    for (const auto &fi : fiList) {
        qDebug().noquote().nospace() << QString{indent, ' '} << fi.fileName();
        if (fi.isDir()) {
            dumpFileTree({fi.absoluteFilePath()}, indent + 2);
        }
    }
}

namespace wsgui {

Application::Application(int argc, char *argv[])
    : m_app(argc, argv), m_panomaxImageProvider(std::make_unique<PanomaxImageProvider>("", 10min)) {
    const QUrl url(u"qrc:/WeatherstationGUI/qml/main.qml"_qs);
    QObject::connect(
        &m_qmlEngine, &QQmlApplicationEngine::objectCreated, &m_app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    m_qmlEngine.addImportPath(":/WeatherstationGUI/qml");

    // dumpFileTree({":"}, 0);

    qmlRegisterSingletonInstance<PanomaxImageProvider>(
        "WeatherstationGUI", 1, 0, "PanomaxImageProvider", m_panomaxImageProvider.get());
    m_qmlEngine.addImageProvider("panomax", m_panomaxImageProvider.get());

    m_qmlEngine.load(url);
}

int Application::exec() { return m_app.exec(); }

} // namespace wsgui