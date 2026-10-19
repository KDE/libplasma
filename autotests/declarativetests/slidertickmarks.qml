// SPDX-FileCopyrightText: 2026 Yuma Kakei <yumasansansan@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtTest

import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PC3

// Check when PC3.Slider shows tick marks, and how many
TestCase {
    id: root
    width: 640
    height: 480
    when: windowShown
    // The tick marks are only loaded for visible sliders
    visible: true

    PC3.Slider {
        id: oneMarkPerStep
        to: 100
        stepSize: 10
    }
    PC3.Slider {
        id: tooManySteps
        to: 100
        stepSize: 1
    }
    PC3.Slider {
        id: customSpacing
        to: 100
        stepSize: 1
        Kirigami.StyleHints.tickMarkStepSize: 10
    }
    PC3.Slider {
        id: hiddenMarks
        to: 100
        stepSize: 10
        Kirigami.StyleHints.tickMarkStepSize: 0
    }
    PC3.Slider {
        id: fractionalSteps
        to: 10
        // As an integer hint, this would put tick marks every 2, off the steps
        stepSize: 2.5
    }

    // The tick marks are laid out by a Loader in the background of the slider
    function tickMarks(slider) {
        for (const child of slider.background.children) {
            if (child instanceof Loader) {
                return child;
            }
        }
        return null;
    }

    function test_tickMarks_data() {
        return [
            { tag: "one mark per step", slider: oneMarkPerStep, count: 11 },
            { tag: "too many steps", slider: tooManySteps, count: 0 },
            { tag: "custom spacing", slider: customSpacing, count: 11 },
            { tag: "hidden marks", slider: hiddenMarks, count: 0 },
            { tag: "fractional steps", slider: fractionalSteps, count: 0 },
        ];
    }

    function test_tickMarks(data) {
        const loader = tickMarks(data.slider);
        verify(loader);
        if (data.count === 0) {
            verify(!loader.visible);
            return;
        }
        verify(loader.visible);
        const repeater = loader.item.children.find(child => child instanceof Repeater);
        verify(repeater);
        compare(repeater.count, data.count);
    }
}
