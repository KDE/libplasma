/******************************************************************************
*   Copyright 2007-2009 by Aaron Seigo <aseigo@kde.org>                       *
*   Copyright 2013 by Sebastian Kügler <sebas@kde.org>                        *
*                                                                             *
*   This library is free software; you can redistribute it and/or             *
*   modify it under the terms of the GNU Library General Public               *
*   License as published by the Free Software Foundation; either              *
*   version 2 of the License, or (at your option) any later version.          *
*                                                                             *
*   This library is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU          *
*   Library General Public License for more details.                          *
*                                                                             *
*   You should have received a copy of the GNU Library General Public License *
*   along with this library; see the file COPYING.LIB.  If not, write to      *
*   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
*   Boston, MA 02110-1301, USA.                                               *
*******************************************************************************/

#include "packages.h"

#include <QDebug>

#include <klocalizedstring.h>
#include <kdeclarative/kdeclarative.h>

#include <Plasma/Package>


void LookAndFeelPackage::initPackage(Plasma::Package *package)
{
    // http://community.kde.org/Plasma/lookAndFeelPackage#

    package->setDefaultPackageRoot("plasma/look-and-feel/");

    //Defaults
    package->addFileDefinition("defaults", "defaults", i18n("Default settings for theme, etc."));

    //Directories
    package->addDirectoryDefinition("previews", "previews", i18n("Preview Images"));
    package->addFileDefinition("loginmanagerpreview", "previews/loginmanager.png", i18n("Preview for the Login Manager"));
    package->addFileDefinition("lockscreenpreview", "previews/lockscreen.png", i18n("Preview for the Lock Screen"));
    package->addFileDefinition("userswitcherpreview", "previews/userswitcher.png", i18n("Preview for the Userswitcher"));
    package->addFileDefinition("desktopswitcherpreview", "previews/desktopswitcher.png", i18n("Preview for the Virtual Desktop Switcher"));
    package->addFileDefinition("splashpreview", "previews/splash.png", i18n("Preview for Splash Screen"));
    package->addFileDefinition("runcommandpreview", "previews/runcommand.png", i18n("Preview for KRunner"));
    package->addFileDefinition("windowdecorationpreview", "previews/windowdecoration.png", i18n("Preview for the Window Decorations"));
    package->addFileDefinition("windowswitcherpreview", "previews/windowswitcher.png", i18n("Preview for Window Switcher"));

    package->addDirectoryDefinition("loginmanager", "loginmanager", i18n("Login Manager"));
    package->addFileDefinition("loginmanagermainscript", "loginmanager/LoginManager.qml", i18n("Main Script for Login Manager"));

    package->addDirectoryDefinition("logout", "logout", i18n("Logout Dialog"));
    package->addFileDefinition("logoutmainscript", "logout/Logout.qml", i18n("Main Script for Logout Dialog"));

    package->addDirectoryDefinition("lockscreen", "lockscreen", i18n("Screenlocker"));
    package->addFileDefinition("lockscreenmainscript", "lockscreen/LockScreen.qml", i18n("Main Script for Lock Screen"));

    package->addDirectoryDefinition("userswitcher", "userswitcher", i18n("UI for fast user switching"));
    package->addFileDefinition("userswitchermainscript", "userswitcher/UserSwitcher.qml", i18n("Main Script for User Switcher"));

    package->addDirectoryDefinition("desktopswitcher", "desktopswitcher", i18n("Virtual Desktop Switcher"));
    package->addFileDefinition("desktopswitchermainscript", "desktopswitcher/DesktopSwitcher.qml", i18n("Main Script for Virtual Desktop Switcher"));

    package->addDirectoryDefinition("osd", "osd", i18n("On-Screen Display Notifications"));
    package->addFileDefinition("osdmainscript", "osd/Osd.qml", i18n("Main Script for On-Screen Display Notifications"));

    package->addDirectoryDefinition("splash", "splash", i18n("Splash Screen"));
    package->addFileDefinition("splashmainscript", "splash/Splash.qml", i18n("Main Script for Splash Screen"));

    package->addDirectoryDefinition("runcommand", "runcommand", i18n("KRunner UI"));
    package->addFileDefinition("runcommandmainscript", "runcommand/RunCommand.qml", i18n("Main Script KRunner"));

    package->addDirectoryDefinition("windowdecoration", "windowdecoration", i18n("Window Decoration"));
    package->addFileDefinition("windowdecorationmainscript", "windowdecoration/WindowDecoration.qml", i18n("Main Script for Window Decoration"));

    package->addDirectoryDefinition("windowswitcher", "windowswitcher", i18n("Window Switcher"));
    package->addFileDefinition("windowswitchermainscript", "windowswitcher/WindowSwitcher.qml", i18n("Main Script for Window Switcher"));

}

void QmlWallpaperPackage::initPackage(Plasma::Package *package)
{
    package->addFileDefinition("mainscript", "ui/main.qml", i18n("Main Script File"));
    //FIXME: why setting it required doesn't work?
    //package->setRequired("mainscript", true);


    QStringList platform = KDeclarative::KDeclarative::runtimePlatform();
    if (!platform.isEmpty()) {
        QMutableStringListIterator it(platform);
        while (it.hasNext()) {
            it.next();
            it.setValue("platformcontents/" + it.value());
        }

        platform.append("contents");
        package->setContentsPrefixPaths(platform);
    }

    package->setDefaultPackageRoot("plasma/wallpapers/");

    package->addDirectoryDefinition("images", "images", i18n("Images"));
    package->addDirectoryDefinition("theme", "theme", i18n("Themed Images"));
    QStringList mimetypes;
    mimetypes << "image/svg+xml" << "image/png" << "image/jpeg";
    package->setMimeTypes("images", mimetypes);
    package->setMimeTypes("theme", mimetypes);

    package->addDirectoryDefinition("config", "config", i18n("Configuration Definitions"));
    mimetypes.clear();
    mimetypes << "text/xml";
    package->setMimeTypes("config", mimetypes);

    package->addDirectoryDefinition("ui", "ui", i18n("User Interface"));

    package->addDirectoryDefinition("data", "data", i18n("Data Files"));

    package->addDirectoryDefinition("scripts", "code", i18n("Executable Scripts"));
    mimetypes.clear();
    mimetypes << "text/plain";
    package->setMimeTypes("scripts", mimetypes);

    package->addDirectoryDefinition("translations", "locale", i18n("Translations"));
}
