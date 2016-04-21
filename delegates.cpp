#include "delegates.h"
#include <QSpinBox>
#include <cmath>

AccumNumberDelegate::AccumNumberDelegate(QObject* parent)
    :
      QItemDelegate(parent)
{

}

QWidget *AccumNumberDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem &/* option */,
     const QModelIndex &/* index */) const
{
    QSpinBox *editor = new QSpinBox(parent);

    editor->setSingleStep(1000);
    editor->setMinimum(1000);
    editor->setMaximum(1000000);

    return editor;
}

void AccumNumberDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();

    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void AccumNumberDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}

void AccumNumberDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

ActiveTimeDelegate::ActiveTimeDelegate(QObject* parent)
    :
      QItemDelegate(parent)
{

}

QWidget *ActiveTimeDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem &/* option */,
     const QModelIndex &/* index */) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);

    editor->setSingleStep(2.56);
    editor->setMinimum(2.52);
    editor->setMaximum(static_cast<float>(0x1FF)*2.56+2.52);

    return editor;
}

void ActiveTimeDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    QString data = index.model()->data(index, Qt::EditRole).toString();
    double value = data.split(" ")[0].toDouble();

    int bins = int( ::round((value - 2.52) / 2.56));

    value = bins*2.56 + 2.52;

    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void ActiveTimeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();

    QString data(QObject::tr("%0 мкс").arg(value));

    model->setData(index, data, Qt::EditRole);
}

void ActiveTimeDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

SmoothingDelegate::SmoothingDelegate(QObject* parent)
    :
      QItemDelegate(parent)
{

}

QWidget *SmoothingDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem &/* option */,
     const QModelIndex &/* index */) const
{
    QSpinBox *editor = new QSpinBox(parent);

    editor->setSingleStep(1000L);
    editor->setMinimum(1L);
    editor->setMaximum(0x7FFFFFFFL);

    return editor;
}

void SmoothingDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    QString data = index.model()->data(index, Qt::EditRole).toString();
    int value = data.toInt();

    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void SmoothingDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    QString data(QObject::tr("%0").arg(value));

    model->setData(index, data, Qt::EditRole);
}

void SmoothingDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
