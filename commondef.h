#ifndef COMMONDEF_H
#define COMMONDEF_H

#include <QString>
#include <QIcon>

enum ActionType
{
    ActionTypeAddPath = 0,
    ActionTypeAddExecutable,
    ActionTypeAddSource,
    ActionTypeAddExcludeSource,
    ActionTypeAddModule,
    ActionTypeAddExport,
    ActionTypeAddWorkDirectory,

    ActionTypeCount,
};

struct ParameterItem
{
    int type;
    QString arg;
};

class AppIconSet
{
private:
    AppIconSet();
public:
    const QIcon ic_export;
    const QIcon ic_ignore;
    const QIcon ic_module;
    const QIcon ic_program;
    const QIcon ic_select;
    const QIcon ic_source;
    const QIcon ic_variable;
    const QIcon ic_edit;
    const QIcon ic_delete;
    const QIcon ic_workdir;

    static AppIconSet *GetInstance();
    const QIcon GetIconAt(int type) const;
};

#define IC_SET (AppIconSet::GetInstance())

const QString &GetInformation(int type);

#endif // COMMONDEF_H
