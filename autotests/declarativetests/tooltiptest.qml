// SPDX-FileCopyrightText: 2026 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtTest

import org.kde.plasma.components as PC3

TestCase {
    id: root
    name: "ToolTipTest"
    width: 640
    height: 480
    when: windowShown

    PC3.ToolTip {
        id: toolTip
    }

    function init() {
        toolTip.text = ""
    }

    function test_ampersand_data() {
        return [
            { tag: "no_ampersand", input: "Hello World", expected: "Hello World" },
            { tag: "accelerator_start", input: "&File", expected: "File" },
            { tag: "accelerator_middle", input: "Save &As...", expected: "Save As..." },
            { tag: "escaped_ampersand_start", input: "&&File", expected: "&File" },
            { tag: "escaped_ampersand_middle", input: "AT&&T", expected: "AT&T" },
            { tag: "ampersand_before_whitespace", input: "A & B", expected: "A & B" },
            { tag: "trailing_ampersand", input: "A &", expected: "A &" },
            { tag: "triple_ampersand", input: "&&&File", expected: "&&File" },
            { tag: "mixed", input: "A & B && C &D &&E", expected: "A & B && C D &E" },
        ]
    }

    function test_ampersand(data) {
        toolTip.text = data.input;
        const label = toolTip.contentItem.children[0];
        compare(label.text, data.expected);
    }
}
