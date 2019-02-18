#ifndef OCCMAINWINDOW_H
#define OCCMAINWINDOW_H

#include <QWidget>

#include "commondef.h"

class QMenu;
class QAction;
class ParameterSetModel;
class CommandExecuteDialog;

namespace Ui
{
    class OccMainWindow;
}

class OccMainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit OccMainWindow(QWidget *parent = nullptr);
    ~OccMainWindow();

private slots:
    void on_btnAutoDetect_clicked();

    void on_btnBrowser_clicked();

    void on_btnReset_clicked();

    void on_btnAnalysis_clicked();

private:
    Ui::OccMainWindow *ui;
    QMenu *m_addMenu;
    QMenu *m_lvParamMenu;
    QAction *m_actions[ActionTypeCount];
    QAction *m_lastAction;
    ParameterSetModel *m_model;
    CommandExecuteDialog *m_dialog;

private:
    void InitMenus();

    void InitViews();

    void InitConnections();

    void SetLastAction(QAction *act);

    void OnAddPath();

    void OnAddExport();

    void OnAddExecutable();

    void OnAddModule();

    void OnAddSourceDir();

    void OnAddExcludedSourceDir();

    void ShowLvParameterMenu(const QPoint &pos);

    void OnRemoveParameter(const QModelIndex &index);

signals:

};

#endif // OCCMAINWINDOW_H
