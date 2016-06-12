#include "Logger.h"

#include <QString>
#include <QProcess>
#include <iostream>

static int LogLevel = Logger::WARNING;

void Logger::SetLogLevel(int logLevel)
{
   LogLevel = logLevel;
}

void Logger::Log(const int logLevel, const QString &logMessage)
{
   if(logLevel <= LogLevel) {
      std::cout << logMessage.toStdString() << std::endl;
   }
}

void Logger::Log(QProcess &proccessToLog)
{
   Log(proccessToLog, proccessToLog.readAll());
}

void Logger::Log(QProcess &proccessToLog, const QByteArray& output)
{
   int logLevel = (proccessToLog.exitStatus() == QProcess::NormalExit) ? INFO : ERROR;
   Log(logLevel, QString() + "Run tool " + proccessToLog.program() + " '" + proccessToLog.arguments().join("' '") + "'");
   Log(logLevel, QString() + "   Exit code " + QString::number(proccessToLog.state()) + ", Output: '" + QString(output).replace('\r',"") + "'");
}
