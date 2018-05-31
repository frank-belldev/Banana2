#ifndef DOWNLOADPROGRESS_H
#define DOWNLOADPROGRESS_H

#include <QtCore>
#include "CurlEasy.h"

class DownloadProgress : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
public:
    explicit DownloadProgress(QObject *parent = 0);

public slots:
    void startDownload();
    void cancelDownload();

    void onTransferProgress(qint64 downloadTotal, qint64 downloadNow, qint64 uploadTotal, qint64 uploadNow);
    void onTransferDone();
    void onTransferAborted();

    void safeRemove();

private:
    QString m_szId;
    CurlEasy *transfer = nullptr;
    QFile *downloadFile = nullptr;

signals:
    void clearLog();
    void appendLog(QString);

//methods for properties
private:
    QString m_szUrl;
    QString m_szFile;
    double m_rProgressValue;
    bool m_bLoading;

signals:
    void urlChanged();
    void dirChanged();
    void fileChanged();
    void progressChanged();
    void loadingChanged();

public slots:
    QString url();
    QString file();
    double progress();
    bool loading();

    void setUrl(QString);
    void setFile(QString);
    void setProgress(double);
    void setLoading(bool);
};

#endif // DOWNLOADPROGRESS_H
