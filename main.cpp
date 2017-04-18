#include <QCoreApplication>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

bool convertM2TS (QFile& inFile, const QString& outFileName) {
    if (!inFile.open(QIODevice::ReadOnly))
        return false;

    QFile outFile(outFileName);
    qDebug() << "      - Creating a TS file" << outFileName;

    if (!outFile.open(QIODevice::WriteOnly))
        return false;

    char* buffer = (char*) malloc(192); // = Q_NULLPTR;

    qint64 length = inFile.size();
    qint64 bytesProcessed = 0;

    QDataStream in(&inFile);    // read the data serialized from the file
    in.setByteOrder(QDataStream::BigEndian);

    QDataStream out(&outFile);

    while (bytesProcessed < length) {
        int numBytes = in.readRawData(buffer, 192);

        if (buffer[4] != 0x47) {
            qDebug() << "Out of sync!";
            return false;
        }

        out.writeRawData((buffer+4), 188);

        bytesProcessed += 192;
    }

    outFile.close();

    free(buffer);

    return true;
}

int main (int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("M2TS to TS Converter");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    parser.process(app);

    foreach (const QString &inParam, parser.positionalArguments()) {
        qDebug() << "Input parameter" << inParam;
        QFileInfo fileInfo(inParam);
        if (fileInfo.exists()) {
            QString fileName = fileInfo.canonicalFilePath();
            qDebug() << "   - Processing file" << fileName;
            bool res = convertM2TS(QFile(fileName), fileInfo.baseName() + ".ts");
            qDebug() << "      - result:" << res;
        }
    }

    return 0;
//    connect(app, &QCoreApplication::aboutToQuit, this, )
//    return app.exec();
}

