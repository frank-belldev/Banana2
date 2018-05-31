#include "downloadprogress.h"

DownloadProgress::DownloadProgress(QObject *parent) : QObject(parent)
{
    m_rProgressValue = 0;

    m_bLoading = false;
    transfer = new CurlEasy(this); // Parent it so it will be destroyed automatically

    connect(transfer, &CurlEasy::done, this, &DownloadProgress::onTransferDone);
    connect(transfer, &CurlEasy::aborted, this, &DownloadProgress::onTransferAborted);
    connect(transfer, &CurlEasy::progress, this, &DownloadProgress::onTransferProgress);
}

QString DownloadProgress::url()
{
    return m_szUrl;
}

QString DownloadProgress::file()
{
    return m_szFile;
}

double DownloadProgress::progress()
{
    return m_rProgressValue;
}

bool DownloadProgress::loading()
{
    return m_bLoading;
}

void DownloadProgress::setUrl(QString newUrl)
{
    m_szUrl= newUrl;
    emit urlChanged();
}

void DownloadProgress::setFile(QString newFile)
{
    m_szFile = newFile;
    emit fileChanged();
}

void DownloadProgress::setProgress(double newProg)
{
    m_rProgressValue = newProg;
    emit progressChanged();
}

void DownloadProgress::setLoading(bool newLoadingState)
{
    m_bLoading = newLoadingState;
    emit loadingChanged();
}

void DownloadProgress::startDownload()
{
    emit clearLog();
    setProgress(0);

    // Prepare target file
    downloadFile = new QFile(m_szFile);
    if (!downloadFile->open(QIODevice::WriteOnly)) {
        emit appendLog("Failed to open file for writing.");
        delete downloadFile;
        downloadFile = nullptr;
        return;
    }

    // Set a simple file writing function
    transfer->setWriteFunction([this](char *data, size_t size)->size_t {
        qint64 bytesWritten = downloadFile->write(data, static_cast<qint64>(size));
        return static_cast<size_t>(bytesWritten);
    });

    // Print headers to the transfer log box
    transfer->setHeaderFunction([this](char *data, size_t size)->size_t {
        emit appendLog(QString::fromUtf8(data, static_cast<int>(size)));
        return size;
    });

    transfer->set(CURLOPT_URL, QUrl(m_szUrl));
    transfer->set(CURLOPT_FOLLOWLOCATION, long(1)); // Follow redirects
    transfer->set(CURLOPT_FAILONERROR, long(1)); // Do not return CURL_OK in case valid server responses reporting errors.

    setLoading(true);
    emit appendLog("Transfer started.");

    transfer->perform();
}

void DownloadProgress::cancelDownload()
{
    if (transfer->isRunning()) {
        transfer->abort();
        setLoading(false);
        return;
    }
}

void DownloadProgress::onTransferProgress(qint64 downloadTotal, qint64 downloadNow, qint64 uploadTotal, qint64 uploadNow)
{
    Q_UNUSED(uploadTotal);
    Q_UNUSED(uploadNow);

    if (downloadTotal > 0) {
        if (downloadNow > downloadTotal) downloadNow = downloadTotal;
        setProgress(downloadNow * 1.0/downloadTotal);
    } else {
        setProgress(0);
    }
}

void DownloadProgress::onTransferDone()
{
    if (transfer->result() != CURLE_OK) {
        emit appendLog(QString("Transfer failed with curl error '%1'")
                    .arg(curl_easy_strerror(transfer->result())));
        downloadFile->remove();
    } else {
        emit appendLog(QString("Transfer complete. %1 bytes downloaded.")
            .arg(downloadFile->size()));
        setProgress(1);
    }

    delete downloadFile;
    downloadFile = nullptr;
    setLoading(false);
}

void DownloadProgress::onTransferAborted()
{
    emit appendLog(QString("Transfer aborted. %1 bytes downloaded.")
        .arg(downloadFile->size()));

    downloadFile->remove();
    delete downloadFile;
    downloadFile = nullptr;
    setLoading(false);
}

void DownloadProgress::safeRemove()
{
    cancelDownload();
    if (downloadFile)
    {
        downloadFile->close();
        delete downloadFile;
        downloadFile = nullptr;
    }
}
