#include "containmentactions.h"
#include <KPluginFactory>

class DummyContainmentAction : public Plasma::ContainmentActions
{
    Q_OBJECT

public:
    explicit DummyContainmentAction(QObject *parent, const KPluginMetaData &metadata, const QVariantList &args)
        : Plasma::ContainmentActions(parent, metadata, args)
    {
    }
};

K_PLUGIN_CLASS_WITH_JSON(DummyContainmentAction, "dummycontainmentaction.json")

#include "dummycontainmentaction.moc"
