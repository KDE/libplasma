/*
 * SPDX-FileCopyrightText: 2019 Aleix Pol <aleixpol@kde.org>
 * SPDX-FileCopyrightText: 2020 Noah Davis <noahadvs@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */
import QtQuick
import QtQuick.Layouts
import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami
// Run with qmlscene to use qqc2-desktop-style

ComponentBase {
    id: root
    title: "Plasma Components 3 Slider"
    contentItem: GridLayout {
        columnSpacing: Kirigami.Units.gridUnit
        rowSpacing: Kirigami.Units.gridUnit
        columns: 2

        PlasmaComponents.Label {
            text: "Horizontal slider"
        }
        PlasmaComponents.Slider {
            id: horizontalSlider
            from: minSpinBox.value
            to: maxSpinBox.value
            stepSize: stepSizeSpinBox.value
        }

        PlasmaComponents.Label {
            text: "Vertical slider"
        }
        PlasmaComponents.Slider {
            id: verticalSlider
            from: minSpinBox.value
            to: maxSpinBox.value
            stepSize: stepSizeSpinBox.value
            orientation: Qt.Vertical
        }

        PlasmaComponents.Label {
            text: "Horizontal slider, disabled"
        }
        PlasmaComponents.Slider {
            enabled: false
            from: minSpinBox.value
            to: maxSpinBox.value
            stepSize: stepSizeSpinBox.value
        }

        PlasmaComponents.Label {
            text: "from: "
        }
        PlasmaComponents.SpinBox {
            id: minSpinBox
            value: 0
            from: -999
            to: 999
            editable: true
        }

        PlasmaComponents.Label {
            text: "to: "
        }
        PlasmaComponents.SpinBox {
            id: maxSpinBox
            value: 100
            from: -999
            to: 999
            editable: true
        }

        PlasmaComponents.Label {
            text: "stepSize: "
        }
        PlasmaComponents.SpinBox {
            id: stepSizeSpinBox
            value: 1
            to: 999
            editable: true
        }

        PlasmaComponents.Label {
            text: "disabled: "
        }
        PlasmaComponents.SpinBox {
            enabled: false
            value: 1
            to: 999
            editable: true
        }
    }
}
