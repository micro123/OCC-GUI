#ifndef COMMANDEXECUTEDIALOG_H
#define COMMANDEXECUTEDIALOG_H

#include <QDialog>


class QProcess;

namespace Ui
{
    class CommandExecuteDialog;
}

class CommandExecuteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommandExecuteDialog(QWidget *parent = nullptr);
    ~CommandExecuteDialog();

    void OpenDialog(const QString &cmd, const QStringList &evn = QStringList(), const QStringList &args = QStringList());
private slots:
    void on_btnStop_clicked();

    void on_btnRerun_clicked();

    void on_btnClose_clicked();

private:
    Ui::CommandExecuteDialog *ui;
    QProcess *m_proc;
};

#endif // COMMANDEXECUTEDIALOG_H
