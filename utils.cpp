#include "utils.h"
#include "commondef.h"

#include <QProcess>
#include <QIcon>
#include <QMap>

bool Utils::GetSystemExecutablePath(const QString &cmd, QString &out_path)
{
    QProcess proc;
    proc.setProgram("WHERE");
    proc.setArguments(QStringList() << cmd);

    proc.start();
    proc.waitForFinished();

    if (proc.exitCode() != 0) //
    {
        out_path = QString();
        return false;
    }

    out_path = QString(proc.readAllStandardOutput()).trimmed();
    return true;
}

AppIconSet::AppIconSet():
    ic_export{":/resources/icons/export.png"},
    ic_ignore{":/resources/icons/ignore.png"},
    ic_module{":/resources/icons/module.png"},
    ic_program{":/resources/icons/programe.png"},
    ic_select{":/resources/icons/select.png"},
    ic_source{":/resources/icons/source.png"},
    ic_variable{":/resources/icons/variable.png"},
    ic_edit{":/resources/icons/edit.png"},
    ic_delete{":/resources/icons/delete.png"},
    ic_workdir{":/resources/icons/work_dir.png"}
{

}

AppIconSet *AppIconSet::GetInstance()
{
    static AppIconSet set;
    return &set;
}

const QIcon AppIconSet::GetIconAt(int type) const
{
    static QMap<int, QIcon> my_icons
    {
        {ActionTypeAddPath, ic_variable},
        {ActionTypeAddExecutable, ic_program},
        {ActionTypeAddSource, ic_source},
        {ActionTypeAddExcludeSource, ic_ignore},
        {ActionTypeAddModule, ic_module},
        {ActionTypeAddExport, ic_export},
        {ActionTypeAddWorkDirectory, ic_workdir},
    };

    if (my_icons.contains (type))
    {
        return my_icons[type];
    }

    return QIcon();
}

const QString &GetInformation(int type)
{
    static const QString infos[] =
    {
        "额外的Path路径",
        "可执行文件路径",
        "源代码路径",
        "排除源代码路径",
        "要分析的模块名称",
        "分析结果导出位置",
        "程序工作目录",
        "",
    };

    if (0 <= type && type < ActionTypeCount)
    {
        return infos[type];
    }

    return infos[ActionTypeCount];
}
