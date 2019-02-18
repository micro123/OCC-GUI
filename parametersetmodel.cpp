#include "parametersetmodel.h"
#include "commondef.h"

#include <QIcon>
#include <QProcess>

ParameterSetModel::ParameterSetModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

void ParameterSetModel::AddParameter(int type, const QString &arg)
{
    beginResetModel ();
    // replace all '/' to '\'
    QString t = arg.trimmed ();
    t.replace ("/", "\\");
    m_model_data.append (ParameterItem{type, t});
    endResetModel ();
}

void ParameterSetModel::Reset()
{
    beginResetModel ();
    m_model_data.clear ();
    endResetModel ();
}

void ParameterSetModel::BuildArguments(QStringList &container) const
{
    container.clear ();

    for (const auto &x : m_model_data)
    {
        switch (x.type)
        {
            case ActionTypeAddExecutable:
                container << x.arg;
                break;

            case ActionTypeAddSource:
                container << "--sources" << x.arg;
                break;

            case ActionTypeAddExcludeSource:
                container << "--excluded_sources" << x.arg;
                break;

            case ActionTypeAddModule:
                container << "--modules" << x.arg;
                break;

            case ActionTypeAddExport:
                container << "--export_type" << QString("html:%1").arg (x.arg);
                break;
        }
    }
}

void ParameterSetModel::RemoveAt(int index)
{
    beginResetModel ();
    m_model_data.removeAt (index);
    endResetModel ();
}

const QString &ParameterSetModel::GetWorkDirectory() const
{
    static const QString empty_path{""};

    for (auto const &x : m_model_data)
    {
        if (x.type == ActionTypeAddWorkDirectory)
        {
            return x.arg;
        }
    }

    return empty_path;
}

QStringList ParameterSetModel::GetProgramEnv() const
{
    QStringList sys_env = QProcess::systemEnvironment ();

    QString append_str = ";";

    for(const auto &x : m_model_data)
    {
        if (x.type == ActionTypeAddPath)
        {
            append_str.append (x.arg);
            append_str.append (';');
        }
    }

    for (auto &x : sys_env)
    {
        if (x.startsWith ("PATH", Qt::CaseInsensitive))
        {
            x.append (append_str);
        }
    }

    return sys_env;
}


int ParameterSetModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED (parent)

    return m_model_data.size ();
}

QVariant ParameterSetModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid ())
    {
        return QVariant();
    }

    int idx = index.row ();

    if (role == Qt::DisplayRole)
    {
        return QVariant(m_model_data[idx].arg);
    }

    if (role == Qt::DecorationRole)
    {
        return QVariant(IC_SET->GetIconAt (m_model_data[idx].type));
    }

    if (role == Qt::TextAlignmentRole)
    {
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }

    if (role == Qt::ToolTipRole)
    {
        return QVariant(GetInformation(m_model_data[idx].type) + QString(": %1").arg(m_model_data[idx].arg));
    }

    if (role == Qt::UserRole)
    {
        return QVariant(m_model_data[idx].type);
    }

    return QVariant();
}
