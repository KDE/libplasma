# SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>
# SPDX-License-Identifier: BSD-2-Clause

#! /usr/bin/env bash
$XGETTEXT `find . -name \*.qml` -o $podir/plasma_applet_org.kde.plasma.%{APPNAMELC}.pot
