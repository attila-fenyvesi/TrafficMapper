
#include <TrafficMapper/Media/MediaPlayer>
#include <TrafficMapper/Media/VideoFilter>
#include <TrafficMapper/Modules/CameraCalibration>
#include <TrafficMapper/Modules/GateModel>
#include <TrafficMapper/Modules/StatModel>
#include <TrafficMapper/Modules/Tracker>
#include <TrafficMapper/Modules/VehicleModel>
#include <TrafficMapper/Types/Gate>
#include <TrafficMapper/Types/Vehicle>
#include <TrafficMapper/Types/VideoMeta>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSplashScreen>
#include <QtWebEngine>


using TrackerAlg = Tracker::OpticalTrackerAlgorithm;
using VType      = Vehicle::VehicleType;


inline void
  preliminaryInitialization()
{
    QtWebEngine::initialize();
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickStyle::setStyle("Fusion");
}

inline void
  applicationSetup(QApplication & app)
{
    app.setApplicationName("TrafficMapper");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("ELTE - IK");
    app.setOrganizationDomain("www.inf.elte.hu");
}

inline void
  registerMetaTypes()
{
    qRegisterMetaType<VideoMeta>("VideoMeta");
    qRegisterMetaType<Vehicle>("Vehicle");
    qRegisterMetaType<TrackerAlg>("TrackerAlg");
}

inline void
  registerQmlTypes()
{
    qmlRegisterType<MediaPlayer>("TrafficMapper", 1, 0, "MediaPlayer");
    qmlRegisterType<VideoFilter>("TrafficMapper", 1, 0, "VideoFilter");
    qmlRegisterType<CameraCalibration>("TrafficMapper", 1, 0, "CameraCalibration");
    qmlRegisterType<GateModel>("TrafficMapper", 1, 0, "GateModel");
    qmlRegisterType<Tracker>("TrafficMapper", 1, 0, "Tracker");
    qmlRegisterType<Gate>("TrafficMapper", 1, 0, "Gate");
}

inline void
  establishConnections(
    MediaPlayer & mediaPlayer,
    VideoFilter & videoFilter,
    Tracker & tracker,
    VehicleModel & vehicleModel,
    GateModel & gateModel,
    StatModel & statModel)
{
    QObject::connect(
      &tracker, &Tracker::analysisStarted, &vehicleModel, &VehicleModel::onAnalysisStarted);
    QObject::connect(
      &tracker, &Tracker::analysisStarted, &gateModel, &GateModel::onAnalysisStarted);
    QObject::connect(
      &tracker, &Tracker::analysisStarted, &statModel, &StatModel::onAnalysisStarted);

    QObject::connect(
      &tracker,
      &Tracker::vehicleTrackingFinished,
      &vehicleModel,
      &VehicleModel::onVehicleTrackingFinished);

    // Tracker -> VehicleModel
    QObject::connect(
      &tracker, &Tracker::analysisEnded, &vehicleModel, &VehicleModel::onAnalysisEnded);
    // VehicleModel -> GateModel
    QObject::connect(
      &vehicleModel,
      &VehicleModel::vehiclePostProcessingFinished,
      &gateModel,
      &GateModel::onVehiclePostProcessingFinished);
    // GateModel -> StatModel
    QObject::connect(
      &gateModel,
      &GateModel::vehiclePassedThroughGate,
      &statModel,
      &StatModel::onVehiclePassedThroughGate);

    // Flip gate counter during playback.
    QObject::connect(
      &videoFilter, &VideoFilter::frameDisplayed, &gateModel, &GateModel::onFrameDisplayed);

    // Reset Tracker on video loading.
    QObject::connect(
      &mediaPlayer, &QMediaPlayer::mediaStatusChanged, &tracker, &Tracker::onMediaStatusChanged);
    // Reset GateModel on video loading.
    QObject::connect(
      &mediaPlayer,
      &QMediaPlayer::mediaStatusChanged,
      &gateModel,
      &GateModel::onMediaStatusChanged);
}

inline void
  setEngineContextProperties(
    QQmlApplicationEngine & engine,
    MediaPlayer & mediaPlayer,
    VideoFilter & videoFilter,
    Tracker & tracker,
    VehicleModel & vehicleModel,
    GateModel & gateModel,
    StatModel & statModel)
{
    engine.rootContext()->setContextProperty(QStringLiteral("mediaPlayer"), &mediaPlayer);
    engine.rootContext()->setContextProperty(QStringLiteral("videoFilter"), &videoFilter);
    engine.rootContext()->setContextProperty(QStringLiteral("tracker"), &tracker);
    engine.rootContext()->setContextProperty(QStringLiteral("vehicleModel"), &vehicleModel);
    engine.rootContext()->setContextProperty(QStringLiteral("gateModel"), &gateModel);
    engine.rootContext()->setContextProperty(QStringLiteral("statModel"), &statModel);
    // engine.rootContext()->setContextProperty(QStringLiteral("proxyModel"), &proxyModel);
    engine.load(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));
}

int
  main(int argc, char * argv[])
{
    preliminaryInitialization();

    QApplication app(argc, argv);    // Maybe: QGuiApplication
    applicationSetup(app);

    QSplashScreen splashScreen(QPixmap(QStringLiteral(":/img/splash.png")).scaledToHeight(540));
    splashScreen.show();

    registerMetaTypes();
    registerQmlTypes();

    MediaPlayer mediaPlayer;
    VideoFilter videoFilter;
    Tracker tracker;
    VehicleModel vehicleModel;
    GateModel gateModel;
    StatModel statModel;

    videoFilter.setVehicleModel(&vehicleModel);

    establishConnections(mediaPlayer, videoFilter, tracker, vehicleModel, gateModel, statModel);

    QQmlApplicationEngine engine;
    setEngineContextProperties(
      engine, mediaPlayer, videoFilter, tracker, vehicleModel, gateModel, statModel);

    splashScreen.finish(nullptr);

    return app.exec();
}