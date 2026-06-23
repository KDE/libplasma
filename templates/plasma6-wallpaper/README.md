<!--
    SPDX-License-Identifier: CC0-1.0
    SPDX-FileCopyrightText: none
-->

# Plasma Wallpaper

## Namespace adaption

Each Plasma plugin has a unique identifier, which is also used to find related
resources (like the translation catalogs).
To avoid naming collisions, Plasma plugins use a reverse domain name notation
for that identifier:

* org.kde.plasma.* - plugins coming from Plasma modules
* org.kde.*        - plugins coming from other software from KDE
* $(my.domain).*   - plugins of your 3rd-party

The generated code uses the `org.kde.plasma` namespace for the plugin identifier.
As this namespace is reserved for use by plugins part of Plasma modules, you will
need to adapt this namespace if you are writing a plugin which is not intended to
end up in the Plasma modules.


## Build instructions ##

```
cd /where/your/applet/is/generated
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=MYPREFIX ..
make
make install
```

(`MYPREFIX` is where you install your Plasma setup, replace it accordingly)

Restart plasma to load the applet
`plasmashell --replace`

or view it with
`plasmoidviewer -a YourAppletName`

## Tutorials and resources

For more information and tutorials on developing plasma widgets, visit [develop.kde.org](https://develop.kde.org/docs/plasma/widget/)
