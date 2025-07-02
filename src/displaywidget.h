#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QSlider>
#include <QFrame>
#include <QPainter>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QStyle>
#include <QStyleOption>
#include <QToolTip>
#include <QMenu>
#include <QAction>

#include "displaymanager.h"

class DisplayWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected)
    Q_PROPERTY(bool primary READ isPrimary WRITE setPrimary)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)

public:
    explicit DisplayWidget(const DisplayInfo &display, QWidget *parent = nullptr);
    ~DisplayWidget();

    DisplayInfo getDisplayInfo() const;
    void setDisplayInfo(const DisplayInfo &display);
    
    bool isSelected() const;
    void setSelected(bool selected);
    
    bool isPrimary() const;
    void setPrimary(bool primary);
    
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    void updateFromDisplayInfo();
    void applyChanges();
    void resetChanges();
    bool hasChanges() const;

public slots:
    void onDisplayChanged();
    void onSettingsChanged();
    void onPrimaryToggled(bool primary);
    void onEnabledToggled(bool enabled);
    void onResolutionChanged(const QString &resolution);
    void onRefreshRateChanged(int refreshRate);
    void onScaleChanged(double scale);
    void onTransformChanged(const QString &transform);
    void onPositionChanged();
    void onWorkspaceChanged(const QString &workspace);
    void onMirrorChanged(const QString &mirrorOf);

signals:
    void displayChanged(const DisplayInfo &display);
    void primaryChanged(const QString &name, bool primary);
    void enabledChanged(const QString &name, bool enabled);
    void positionChanged(const QString &name, int x, int y);
    void settingsChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onPreviewClicked();
    void onAdvancedClicked();
    void onResetClicked();
    void onApplyClicked();
    void onDeleteClicked();
    void onDuplicateClicked();
    void onMirrorClicked();
    void onUnmirrorClicked();
    void onSetPrimaryClicked();
    void onSetSecondaryClicked();
    void onRotate90Clicked();
    void onRotate180Clicked();
    void onRotate270Clicked();
    void onFlipHorizontalClicked();
    void onFlipVerticalClicked();
    void onScale100Clicked();
    void onScale125Clicked();
    void onScale150Clicked();
    void onScale200Clicked();
    void onCustomScaleClicked();

private:
    void setupUI();
    void setupConnections();
    void updatePreview();
    void updatePosition();
    void updateResolution();
    void updateRefreshRate();
    void updateScale();
    void updateTransform();
    void updateWorkspace();
    void updateMirror();
    void updateEnabled();
    void updatePrimary();
    
    void createContextMenu();
    void createAdvancedDialog();
    void createScaleDialog();
    
    void animateSelection();
    void animatePosition();
    void animateScale();
    
    bool validateSettings();
    void showValidationError(const QString &message);
    
    // UI Components
    QVBoxLayout *m_mainLayout;
    QGroupBox *m_groupBox;
    QVBoxLayout *m_groupLayout;
    
    // Preview area
    QFrame *m_previewFrame;
    QLabel *m_previewLabel;
    QLabel *m_nameLabel;
    QLabel *m_resolutionLabel;
    QLabel *m_refreshLabel;
    QLabel *m_scaleLabel;
    QLabel *m_positionLabel;
    QLabel *m_workspaceLabel;
    
    // Basic settings
    QGridLayout *m_basicLayout;
    QCheckBox *m_enabledCheckBox;
    QCheckBox *m_primaryCheckBox;
    QComboBox *m_resolutionComboBox;
    QSpinBox *m_refreshRateSpinBox;
    QDoubleSpinBox *m_scaleSpinBox;
    QComboBox *m_transformComboBox;
    QComboBox *m_workspaceComboBox;
    QComboBox *m_mirrorComboBox;
    
    // Position controls
    QHBoxLayout *m_positionLayout;
    QSpinBox *m_xSpinBox;
    QSpinBox *m_ySpinBox;
    QPushButton *m_positionButton;
    
    // Buttons
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_previewButton;
    QPushButton *m_advancedButton;
    QPushButton *m_resetButton;
    QPushButton *m_applyButton;
    QPushButton *m_deleteButton;
    
    // Context menu
    QMenu *m_contextMenu;
    QAction *m_duplicateAction;
    QAction *m_mirrorAction;
    QAction *m_unmirrorAction;
    QAction *m_setPrimaryAction;
    QAction *m_setSecondaryAction;
    QAction *m_rotate90Action;
    QAction *m_rotate180Action;
    QAction *m_rotate270Action;
    QAction *m_flipHorizontalAction;
    QAction *m_flipVerticalAction;
    QAction *m_scale100Action;
    QAction *m_scale125Action;
    QAction *m_scale150Action;
    QAction *m_scale200Action;
    QAction *m_customScaleAction;
    
    // Data
    DisplayInfo m_displayInfo;
    DisplayInfo m_originalDisplayInfo;
    bool m_hasChanges;
    bool m_isSelected;
    bool m_isPrimary;
    bool m_isEnabled;
    
    // Animation
    QPropertyAnimation *m_selectionAnimation;
    QPropertyAnimation *m_positionAnimation;
    QPropertyAnimation *m_scaleAnimation;
    QGraphicsOpacityEffect *m_opacityEffect;
    
    // State
    bool m_isDragging;
    QPoint m_dragStartPos;
    QPoint m_originalPos;
    bool m_isHovered;
    bool m_isPressed;
    
    // Timers
    QTimer *m_updateTimer;
    QTimer *m_validationTimer;
    
    // Advanced dialog
    QDialog *m_advancedDialog;
    QVBoxLayout *m_advancedLayout;
    QGridLayout *m_advancedSettingsLayout;
    QSpinBox *m_customWidthSpinBox;
    QSpinBox *m_customHeightSpinBox;
    QSpinBox *m_customRefreshRateSpinBox;
    QDoubleSpinBox *m_customScaleSpinBox;
    QComboBox *m_customTransformComboBox;
    QLineEdit *m_customWorkspaceLineEdit;
    QPushButton *m_advancedApplyButton;
    QPushButton *m_advancedCancelButton;
    
    // Scale dialog
    QDialog *m_scaleDialog;
    QVBoxLayout *m_scaleLayout;
    QSlider *m_scaleSlider;
    QLabel *m_scaleValueLabel;
    QPushButton *m_scaleApplyButton;
    QPushButton *m_scaleCancelButton;
};

#endif // DISPLAYWIDGET_H 