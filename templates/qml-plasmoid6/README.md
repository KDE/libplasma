# Plasma Applet Template

## Namespace adaption

Each Plasma plugin has a unique identifier, which is also used to find related
resources (like the translation catalogs).
To avoid naming collisions, Plasma plugins use a reverse domain name notation
for that identifier:

* org.kde.plasma.* - plugins coming from Plasma modules
* org.kde.*        - plugins coming from other software from KDE
* $(my.domain).*   - plugins of your 3rd-party

The generated code uses the "org.kde.plasma" namespace for the plugin identifier.
As this namespace is reserved for use by plugins part of Plasma modules, you will
need to adapt this namespace if you are writing a plugin which is not intended to
end up in the Plasma modules.

## About paths

Plasmoids that consist only of QML files are installed directly to `/usr/share/plasma/plasmoids/` or `~/.local/share/plasma/plasmoids/` and are found by default when run with `plasmawindowed` or `plasmoidviewer`.

To test the project locally in userspace, install it with `kpackagetool6`.

When a plasmoid consists only of QML files, the file structure should be:

```
your.plasmoid.id/
├── contents/
│   ├── config/
│   │   ├── config.qml # optional
│   │   └── main.xml # optional
│   └── ui/
│       ├── configGeneral.qml # optional
│       └── main.qml
└── metadata.json
```

Since Plasma 6, plasmoids compiled from QML to C++ are installed as libraries in `/usr/lib64/plugins/plasma/applets/` or `~/.local/lib64/plugins/plasma/applets/`.

When testing a project compiled to C++ locally in userspace (`~/.local`), you will need to override `QT_PLUGIN_PATH`. See [Running instructions](#running-instructions) below.

## Running instructions

To install locally for testing:

```bash
kpackagetool6 --type "Plasma/Applet" --install "your.plasmoid.id.here"
plasmawindowed your.plasmoid.id.here
# or
plasmoidviewer --applet your.plasmoid.id.here
```

The command `plasmawindowed` comes by default with Plasma and provides a simple way to visualize plasmoids.
The `plasmoidviewer` command comes from [plasma-sdk](https://invent.kde.org/plasma/plasma-sdk) and provides different views for plasmoids.

Instead of installing the plasmoid using the project directory, you may also convert it to a `.plasmoid` archive so your users may install it with `kpackagetool6` as well:

```bash
ark your.plasmoid.folder/ --add-to plasmoidname.plasmoid
kpackagetool6 --type "Plasma/Applet" --install "plasmoidname.plasmoid"
```

---

Optionally, you may install and run it after compiling it to C++:

```bash
cd /where/your/applet/is/generated
cmake -B build/ --install-prefix ~/.local
cmake --build build/
cmake --install build/
QT_PLUGIN_PATH=~/.local/lib64/plugins/ plasmawindowed your.plasmoid.id.here
## or
QT_PLUGIN_PATH=~/.local/lib64/plugins/ plasmoidviewer --applet your.plasmoid.id.here
```

## Tutorials and resources

You can learn more about how to make plasmoids with the [Plasma widgets tutorial](https://develop.kde.org/docs/plasma/widget/).

See also:

* [Explanation of the template](https://techbase.kde.org/Development/Tutorials/Plasma5/QML2/GettingStarted) (possibly outdated)
* [Plasma QML API explained](https://techbase.kde.org/Development/Tutorials/Plasma2/QML2/API) (possibly outdated)
