#ifndef PARAMETERSETMODEL_H
#define PARAMETERSETMODEL_H

#include "commondef.h"

#include <QAbstractListModel>
#include <QList>

class ParameterSetModel: public QAbstractListModel
{
    Q_OBJECT

    QList<ParameterItem> m_model_data;
public:
    ParameterSetModel(QObject *parent = nullptr);

    void AddParameter(int type, const QString &arg);

    void Reset();

    void BuildArguments(QStringList &container) const;

    void RemoveAt(int index);

    QStringList GetProgramEnv() const;
    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
};

#endif // PARAMETERSETMODEL_H
