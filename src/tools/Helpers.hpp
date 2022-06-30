#pragma once

#include <QDebug>
#include <QString>
#include <QThread>

typedef void (*function_type)(const char * message);

namespace Helpers {

   enum DebugMessageType
   {
      DBG_INFO    = 45,
      DBG_WARNING = 220,
      DBG_ERROR   = 160
   };

   template<class T>
   inline void debugMessage(
     T message, bool printThreadId = true, DebugMessageType type = DebugMessageType::DBG_INFO)
   {
      if (printThreadId) {
         qDebug()
           << "\u001b[41m\u001b[37;1m" << QThread::currentThreadId() << "\u001b[0m"
           << QString("\u001b[38;5;%1m%2\u001b[0m").arg(type).arg(message).toStdString().c_str();
      } else {
         qDebug()
           << QString("\u001b[38;5;%1m%2\u001b[0m").arg(type).arg(message).toStdString().c_str();
      }
   }

   template<class T>
   inline void debugMessage(T message, DebugMessageType type, bool printThreadId = true)
   {
      Helpers::debugMessage(message, printThreadId, type);
   }

   template<class T1, class T2>
   inline void debugMessage(
     T1 moduleName,
     T2 message,
     bool printThreadId    = true,
     DebugMessageType type = DebugMessageType::DBG_INFO)
   {
      if (printThreadId) {
         qDebug() << "\u001b[41m\u001b[37;1m" << QThread::currentThreadId() << "\u001b[0m"
                  << QString("[\u001b[38;5;215m%1\u001b[0m] \u001b[38;5;%2m%3\u001b[0m")
                       .arg(moduleName)
                       .arg(type)
                       .arg(message)
                       .toStdString()
                       .c_str();
      } else {
         qDebug() << QString("[\u001b[38;5;215m%1\u001b[0m] \u001b[38;5;%2m%3\u001b[0m")
                       .arg(moduleName)
                       .arg(type)
                       .arg(message)
                       .toStdString()
                       .c_str();
      }
   }

   template<class T1, class T2>
   inline void debugMessage(
     T1 moduleName, T2 message, DebugMessageType type, bool printThreadId = true)
   {
      Helpers::debugMessage(moduleName, message, printThreadId, type);
   }

}    // namespace Helpers
