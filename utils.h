#ifndef UTILS_H
#define UTILS_H

#include <QString>

namespace Utils
{

    bool GetSystemExecutablePath(const QString &cmd, QString &out_path);

}

#endif  // UTILS_H
