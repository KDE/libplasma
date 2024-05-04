// SPDX-FileCopyrightText: 2024 Fushan Wen <qydwhotmail@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtTest

TestCase {
    id: root
    when: windowShown

    function test_components_data() {
        return [
                    {text: "import org.kde.plasma.components as PC; PC.BusyIndicator { running: true; }"},
                    {text: "import org.kde.plasma.components as PC; PC.Button { icon.name: 'start-here-kde-plasma'; text: 'test'; }"},
                    {text: "import org.kde.plasma.components as PC; PC.CheckBox { icon.name: 'start-here-kde-plasma'; text: 'test'; }"},
                    {text: "import org.kde.plasma.components as PC; PC.ComboBox { model: 10; }"},
                    {text: "import org.kde.plasma.components as PC; PC.ProgressBar { from: 0; to: 100; value: 50; }"},
                    {text: "import org.kde.plasma.components as PC; PC.RadioButton { icon.name: 'start-here-kde-plasma'; text: 'test'; }"},
                    {text: "import org.kde.plasma.components as PC; PC.RoundButton { icon.name: 'start-here-kde-plasma'; text: 'test'; }"},
                    {text: "import org.kde.plasma.components as PC; PC.Slider { from: 0; to: 100; }"},
                    {text: "import org.kde.plasma.components as PC; PC.SpinBox { implicitWidth: 2; implicitHeight: 2; from: 0; to: 100; }"},
                    {text: "import org.kde.plasma.components as PC; PC.Switch { icon.name: 'start-here-kde-plasma'; text: 'test'; }"},
                    {text: "import org.kde.plasma.components as PC; PC.TabBar { PC.TabButton { text: 'test'; } }"},
                    {text: "import org.kde.plasma.components as PC; PC.TextArea { placeholderText: 'test'; }"},
                    {text: "import org.kde.plasma.components as PC; PC.TextField { placeholderText: 'test'; }"},
                    {text: "import org.kde.plasma.components as PC; PC.ToolTip { visible: true; text: 'test'; }"},
                ];
    }

    function test_components(data) {
        const object = createTemporaryQmlObject(data.text, root);
        verify(object);
        verify(object.implicitWidth > 1)
        verify(object.implicitHeight > 1)
    }
}
