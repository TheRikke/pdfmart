#ifndef LOGGER_H
#define LOGGER_H

class QProcess;
class QString;
class QByteArray;

namespace Logger {
   void SetLogLevel(int);
   void Log(int, const QString &logMessage);
   void Log(QProcess &proccessToLog);
   void Log(QProcess &proccessToLog, const QByteArray& output);

   const int ERROR = 0;
   const int WARNING = 1;
   const int INFO = 2;
}

#endif // LOGGER_H
