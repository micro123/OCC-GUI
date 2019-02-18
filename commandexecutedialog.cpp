#include "commandexecutedialog.h"
#include "ui_commandexecutedialog.h"

#include <QProcess>
#include <QDebug>

CommandExecuteDialog::CommandExecuteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommandExecuteDialog),
    m_proc(nullptr)
{
    ui->setupUi(this);
}

CommandExecuteDialog::~CommandExecuteDialog()
{
    if (m_proc)
    {
        m_proc->terminate ();
        delete m_proc;
    }

    delete ui;
}

void CommandExecuteDialog::OpenDialog(const QString &cmd, const QStringList &env, const QStringList &args)
{
    if (m_proc != nullptr)
    {
        return;
    }

    m_proc = new QProcess(this);
    m_proc->setProgram (cmd);
    m_proc->setEnvironment (env);
    m_proc->setArguments (args);

    connect (m_proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [&](int code, QProcess::ExitStatus st)
    {
        ui->etStdout->append (m_proc->readAllStandardOutput ());
        ui->etStderr->append (m_proc->readAllStandardError ());
    });

    connect (m_proc, &QProcess::stateChanged, [&](QProcess::ProcessState st)
    {
        if (st == QProcess::Running)
        {
            ui->lbStateText->setText ("正在运行");
            ui->pgStatus->setRange (0, 0);
        }
        else
        {
            ui->lbStateText->setText ("未在运行");
            ui->pgStatus->setRange (0, 1);
            ui->pgStatus->setValue (1);
        }
    });

    m_proc->start ();
    QDialog::open ();
}

void CommandExecuteDialog::on_btnStop_clicked()
{
    if (m_proc && m_proc->state () == QProcess::Running)
    {
        m_proc->terminate ();
    }
}

void CommandExecuteDialog::on_btnRerun_clicked()
{
    if (!m_proc || m_proc->state () != QProcess::NotRunning)
    {
        return;
    }

    ui->etStdout->clear ();
    ui->etStderr->clear ();

    m_proc->start ();
}

void CommandExecuteDialog::on_btnClose_clicked()
{
    done(0);

    if (m_proc && m_proc->state () != QProcess::NotRunning)
    {
        m_proc->terminate ();
    }

    m_proc->deleteLater ();
    m_proc = nullptr;
}
