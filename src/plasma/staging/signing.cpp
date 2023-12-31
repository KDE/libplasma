/*
 *  Copyright (C) 2010 by Diego '[Po]lentino' Casella <polentino911@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */


#include "signing.h"
#include "signing_p.h"

#define _FILE_OFFSET_BITS 64
#include <gpgme.h>
#include <gpgme++/gpgmefw.h>
#include <gpgme++/global.h>
#include <gpgme++/context.h>
#include <gpgme++/error.h>
#include <gpgme++/engineinfo.h>
#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>
#include <gpgme++/keygenerationresult.h>
#include <gpgme++/importresult.h>
#include <gpgme++/data.h>

#include <QtCore/QDir>
#include <QtCore/QStringList>

#include <kdirwatch.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kprocess.h>
#include <kuser.h>


#include <cstdio> // FILE

namespace Plasma
{

SigningPrivate::SigningPrivate(Signing *auth, const QString &keystorePath = 0)
        : q(auth)
{
    GpgME::initializeLibrary();
    GpgME::Error error = GpgME::checkEngine(GpgME::OpenPGP);
    if (error) {
        kDebug() << "OpenPGP engine not found: authentication will not work.";
        return;
    }

    m_gpgContext = GpgME::Context::createForProtocol(GpgME::OpenPGP);
    m_gpgContext->setKeyListMode(GPGME_KEYLIST_MODE_LOCAL | GPGME_KEYLIST_MODE_SIGS);

    m_keystorePath = keystorePath;

    if (m_keystorePath.isEmpty() || m_keystorePath.isNull()) {
        // From the gpgme doc: if the homeDirectory() is null, it means we are using the standard dir,
        // that is "/home/$USER/.gnupg/ ; so let's retrieve it.
        KUser user;
        m_keystorePath.append(user.homeDir()).append("/.gnupg/");
    } else {
        error = m_gpgContext->setEngineHomeDirectory(m_keystorePath.toAscii().data());
        if (error) {
            kDebug() << "Failed setting custom gpg keystore directory: using default.";
        }
    }

    m_keystoreDir = new KDirWatch();
    m_keystoreDir->addDir(m_keystorePath);
    m_keystoreDir->startScan(true);

    m_KdeKeysDir = new KDirWatch();
    m_KdeKeysDir->addDir(KStandardDirs::locate("appdata", "plasmakeys/"));
    m_KdeKeysDir->startScan(true);

    // Start watching the keystore and the dir with the kde keys, and notify for changed
    q->connect(m_keystoreDir, SIGNAL(created(const QString &)), q, SLOT(slotProcessKeystore()));
//    q->connect(m_keystoreDir, SIGNAL(dirty(const QString &)), q, SLOT(slotKDEKeyRemoved(const QString &)));
    q->connect(m_keystoreDir, SIGNAL(deleted(const QString &)), q, SLOT(slotKDEKeyRemoved(const QString &)));

    q->connect(m_KdeKeysDir, SIGNAL(created(const QString &)), q, SLOT(slotKDEKeyAdded(const QString &)));
    q->connect(m_KdeKeysDir, SIGNAL(dirty(const QString &)), q, SLOT(slotKDEKeyAdded(const QString &)));
    q->connect(m_KdeKeysDir, SIGNAL(deleted(const QString &)), q, SLOT(slotKDEKeyRemoved(const QString &)));
}


void SigningPrivate::importKdeKeysToKeystore()
{
    QList< QByteArray > tmp;
    if (!m_gpgContext) {
        kDebug() << "GPGME context not valid: please re-initialize the library.";
        keys.insert(UltimatelyTrusted, tmp);
        return;
    }
    QString path(KStandardDirs::locate("appdata", "plasmakeys/"));
    QDir dir(path);
    if (!dir.exists() || path.isEmpty() || path.isNull()) {
        kDebug() << "Directory with KDE keys not found: aborting";
        keys[UltimatelyTrusted] = tmp;
        return;
    }
    QStringList keyFiles = dir.entryList(QDir::Files); // QDir::Files is rendundant in this stage

    // Foreach file found, open it and import the key into the gpg keyring.
    // First avoid firing multiple entryWritten() signals
    m_keystoreDir->stopScan();
    m_KdeKeysDir->stopScan();

    foreach(QString keyFile, keyFiles) {
        FILE *fp;
        fp = fopen(keyFile.toAscii().data(), "r");
        GpgME::Data data(fp);
        GpgME::ImportResult iRes = m_gpgContext->importKeys(data);
        if (iRes.error()) {
            kDebug() << "Error while importing the key located at: " << keyFile;
            kDebug() << " The error is:" << iRes.error().asString() << "; Skipping.";
            continue;
        }

        // The first fingerprint listed is the one we need to save
        tmp.append(iRes.import(0).fingerprint());
    }
    keys[UltimatelyTrusted] = tmp;

    // Restore scanning folders
    m_keystoreDir->startScan(true, true);
    m_KdeKeysDir->startScan(true, true);

}

void SigningPrivate::splitKeysByTrustLevel()
{
    if (!m_gpgContext) {
        kDebug() << "GPGME context not valid: please re-initialize the library.";
        return;
    }

    QList<QByteArray> temp = keys[UltimatelyTrusted];
    keys.clear();
    keys[UltimatelyTrusted] = temp;

    // Splitting the keys by their trust level is a boring task, since we have to distinguish
    // `which key has been signed with an other given key` :P
    //
    // Loop 1: import and load the KDE keys, already done in importKdeKeysToKeystore()
    //
    // Loop 2: load the user keyring (private keys only), and loop for:
    //    - a: a key not yet expired;
    //    - b: a key not already present in the keys[UltimatelyTrusted];
    //
    // Loop 3: load the user keyring, and loop for:
    //    - a: a key not yet expired;
    //    - b: a key not already present in the keys[UltimatelyTrusted];
    //    - c: a key not yet in keys[SelfTrusted]
    //
    // After Loop 3, the tmp object contains the remaining keys not yet processed.
    //
    // Loop 4: foreach key not yet classified, inspect their signatures and:
    //    - a: if contains a key from keys[UltimatelyTrusted], save it in keys[FullyTrused];
    //    - b: if contains a key from keys[SelfTrusted], save it in keys[UserTrusted];
    //    - c: if the signature is unknown, let's save it in keys[UnknownTrusted].
    QList< QByteArray > tmp;

    GpgME::Error error = m_gpgContext->startKeyListing("", true);
    while (!error) { // Loop 2

        GpgME::Key key = m_gpgContext->nextKey(error);
        if (error)
            break;

        QByteArray data(key.subkey(0).fingerprint());

        // If the key is disabled, expired, invalid or revoked, put it in the untrusted list
        if (key.isDisabled() || key.isExpired() || key.isInvalid() || key.isRevoked()) {
            keys[CompletelyUntrusted].append(data);
            continue;
        }

        // Ensure we are not processing twice the trusted KDE keys
        if (keys[UltimatelyTrusted].contains(data)) {
            continue;
        }

        // The keys is new, valid and private: save it !
        keys[SelfTrusted].append(data);
    }
    GpgME::KeyListResult lRes = m_gpgContext->endKeyListing();

    error = m_gpgContext->startKeyListing("");
    while (!error) { // Loop 3

        GpgME::Key key = m_gpgContext->nextKey(error);
        if (error)
            break;

        QByteArray data(key.subkey(0).fingerprint());

        if (keys[UltimatelyTrusted].contains(data) ||
                keys[SelfTrusted].contains(data)) {
            continue;
        }

        // If the key is disabled, expired, invalid or revoked, put it in the untrusted list
        if (key.isDisabled() || key.isExpired() || key.isInvalid() || key.isRevoked()) {
            keys[CompletelyUntrusted].append(data);
            continue;
        }

        // The keys is new, valid and public: save it !
        tmp.append(data);
    }
    lRes = m_gpgContext->endKeyListing();

    if (lRes.error()) {
        kDebug() << "Error while ending the keyListing operation: " << lRes.error().asString();
    }

    //Loop 4 - looking for keys signed by kde or by the user, tmp contains the valid public keys remaining
    QString kdeKeys;
    foreach(QByteArray s, keys[UltimatelyTrusted]) {
        kdeKeys.append(s).append(' ');
    }
    QString selfKeys;
    foreach(QByteArray s, keys[SelfTrusted]) {
        selfKeys.append(s).append(' ');
    }

    foreach(QByteArray unknowTmpKey, tmp) {
        QStringList signers = signersOf(QString(unknowTmpKey));

        bool stored = false;

        foreach(QString signer, signers) {
            if (kdeKeys.contains(signer)) {
                // if the unknown key has a signer that is a kde key, let's trust it
                keys[FullyTrused].append(unknowTmpKey);
                stored = true;
                break;
            } else if (selfKeys.contains(unknowTmpKey)) {
                // if the unknown key has a signer that is a user key, let's trust it
                keys[UserTrusted].append(unknowTmpKey);
                stored = true;
                break;
            }
        }

        if (!stored) {
            // We didn't stored the unknown key in the previous loop, which means that we
            // don't know the hey al all.
            keys[UnknownTrusted].append(unknowTmpKey);
        }
    }


    // Lets print out all the keys found till now.
    temp = keys[UltimatelyTrusted];
    QStringList list;
    foreach(QByteArray ba, temp) {
        list.append(ba);
    }

    kDebug() << "UltimatelyTrusted = " << list;
    list.clear();

    temp = keys[FullyTrused];
    foreach(QByteArray ba, temp) {
        list.append(ba);
    }

    kDebug() << "FullyTrused = " << list;
    list.clear();

    temp = keys[SelfTrusted];
    foreach(QByteArray ba, temp) {
        list.append(ba);
    }

    kDebug() << "SelfTrusted = " << list;
    list.clear();

    temp = keys[UserTrusted];
    foreach(QByteArray ba, temp) {
        list.append(ba);
    }

    kDebug() << "UserTrusted = " << list;
    list.clear();

    temp = keys[UnknownTrusted];
    foreach(QByteArray ba, temp) {
        list.append(ba);
    }

    kDebug() << "UnknownTrusted = " << list;
    list.clear();

    temp = keys[CompletelyUntrusted];
    foreach(QByteArray ba, temp) {
        list.append(ba);
    }

    kDebug() << "CompletelyUntrusted = " << list;
    kDebug() << "ALL = " << keys;
}

QStringList SigningPrivate::keysID(const bool returnPrivate) const
{
    QStringList result;

    if (!m_gpgContext) {
        kDebug() << "GPGME context not valid: please re-initialize the library.";
        return result;
    }

    GpgME::Error error = m_gpgContext->startKeyListing("", returnPrivate);
    while (!error) {
        GpgME::Key k = m_gpgContext->nextKey(error);
        if (error)
            break;

        result.append(k.subkey(0).fingerprint());
    }
    GpgME::KeyListResult lRes = m_gpgContext->endKeyListing();
    if (lRes.error()) {
        kDebug() << "Error while ending the keyListing operation: " << lRes.error().asString();
    }

    return result;
}

QString SigningPrivate::signerOf(const QString &messagePath, const QString &signaturePath) const
{
    FILE *pFile;
    FILE *pSig;

    pFile = fopen(messagePath.toAscii().data(), "r");
    pSig = fopen(signaturePath.toAscii().data(), "r");

    GpgME::Data file(pFile);
    GpgME::Data sig(pSig);

    GpgME::VerificationResult vRes = m_gpgContext->verifyDetachedSignature(sig, file);
    if (!vRes.error()) {
        kDebug() << "message " << messagePath << " and signature " << signaturePath << "matched! The fingerprint of the signer is: " << vRes.signature(0).fingerprint();
        return QString(vRes.signature(0).fingerprint());
    }

    return QString();
}

QString SigningPrivate::descriptiveString(const QString &keyID) const
{
    QString result;

    if (!m_gpgContext) {
        kDebug() << "GPGME context not valid: please re-initialize the library.";
        return result;
    }

    GpgME::Error error = m_gpgContext->startKeyListing("");
    while (!error) {
        GpgME::Key k = m_gpgContext->nextKey(error);
        if (error)
            break;

        QString fullID(k.subkey(0).fingerprint());
        if (fullID.contains(keyID)) {
            result.append(k.userID(0).id());
            break;
        }
    }
    GpgME::KeyListResult lRes = m_gpgContext->endKeyListing();
    if (lRes.error()) {
        kDebug() << "Error while ending the keyListing operation: " << lRes.error().asString();
    }

    return result;
}

QString SigningPrivate::keystorePath() const
{
    return m_keystorePath;
}

void SigningPrivate::slotProcessKeystore()
{
    QMap<TrustLevel, QList<QByteArray> > tmpMap = keys;
    splitKeysByTrustLevel();

    QList<QByteArray> oldValues;
    oldValues << tmpMap[UnverifiableTrust]
    << tmpMap[CompletelyUntrusted]
    << tmpMap[UnknownTrusted]
    << tmpMap[SelfTrusted]
    << tmpMap[FullyTrused]
    << tmpMap[UltimatelyTrusted];

    QList<QByteArray> newValues;
    newValues << keys[UnverifiableTrust]
    << keys[CompletelyUntrusted]
    << keys[UnknownTrusted]
    << keys[SelfTrusted]
    << keys[FullyTrused]
    << keys[UltimatelyTrusted];

    QString result;
    bool keystoreIncreased = (newValues.size() >= oldValues.size());
    if (keystoreIncreased) {
        foreach(QByteArray value, newValues) {
            if (!oldValues.contains(value)) {
                // Found the key added
                result.append(value);
                break;
            }
        }
        emit(q->keyAdded(result));
    } else {
        foreach(QByteArray value, oldValues) {
            if (!newValues.contains(value)) {
                // Found the removed key
                result.append(value);
                break;
            }
        }
        emit(q->keyRemoved(result));
    }
}

void SigningPrivate::slotKDEKeyAdded(const QString path)
{
    // Avoid firing multiple signals by kdirwatch instances
    m_KdeKeysDir->stopScan();
    m_keystoreDir->stopScan();

    FILE *fp;
    fp = fopen(path.toAscii().data(), "r");
    GpgME::Data data(fp);
    GpgME::ImportResult iRes = m_gpgContext->importKeys(data);

    bool alreadyInMap = false;

    // Ensure we don't already have the key
    foreach(QByteArray sec, keys[UltimatelyTrusted]) {
        if (strcmp(sec.data(), iRes.import(0).fingerprint())) {
            alreadyInMap = true;
            break;
        }
    }

    if (!alreadyInMap) {
        keys[UltimatelyTrusted] << QByteArray(iRes.import(0).fingerprint());
        splitKeysByTrustLevel();
    }

    // Restore scanning folders
    m_KdeKeysDir->startScan(true, true);
    m_keystoreDir->startScan(true, true);

    QString result(iRes.import(0).fingerprint());
    emit(q->keyAdded(result));
}

void SigningPrivate::slotKDEKeyRemoved(const QString path)
{
    Q_UNUSED(path)

    // Avoid firing multiple signals by kdirwatch instances
    m_KdeKeysDir->stopScan();
    m_keystoreDir->stopScan();

    QList<QByteArray> oldKeys = keys[UltimatelyTrusted];
    importKdeKeysToKeystore();
    QList<QByteArray> newkeys = keys[UltimatelyTrusted];

    QString result;
    foreach(QByteArray key, oldKeys) {
        if (!newkeys.contains(key)) {
            // We found the missing key :)
            result.append(key);
            break;
        }
    }


    GpgME::Error error = m_gpgContext->startKeyListing("");
    while (!error) {
        GpgME::Key k = m_gpgContext->nextKey(error);
        if (error)
            break;

        if (result.contains(k.subkey(0).fingerprint())) {
            error = m_gpgContext->startKeyDeletion(k, true); // GG

            if (error) {
                kDebug() << "Can't delete key with fingerprint: " << result;
                m_gpgContext->endKeyListing();
                return;
            }

            break;
        }
    }
    GpgME::KeyListResult lRes = m_gpgContext->endKeyListing();
    if (lRes.error()) {
        kDebug() << "Error while ending the keyListing operation: " << lRes.error().asString();
    }

    splitKeysByTrustLevel();

    // Restore scanning folders
    m_KdeKeysDir->startScan(true, true);
    m_keystoreDir->startScan(true, true);

    emit(q->keyRemoved(result));

}

QStringList SigningPrivate::signersOf(const QString id) const
{
    QStringList result;
    GpgME::Error error = m_gpgContext->startKeyListing("");
    while (!error) {
        GpgME::Key k = m_gpgContext->nextKey(error);
        if (error)
            break;

        for (unsigned int i = 0; i < k.numUserIDs(); ++i) {
            for (unsigned int j = 0; j < k.userID(i).numSignatures(); ++j) {
                QString sig(k.userID(i).signature(j).signerKeyID());
                if (!result.contains(sig)) {
                    if(!id.contains(sig))
                        result.append(sig);
                }
            }
        }
    }
    GpgME::KeyListResult lRes = m_gpgContext->endKeyListing();
    if (lRes.error()) {
        kDebug() << "Error while ending the keyListing operation: " << lRes.error().asString();
    }

    kDebug() << "Hey, the key " << id << " has been signed with " << result;

    return result;
}

Signing::Signing(const QString &keystorePath)
        : QObject(),
        d(new SigningPrivate(this, keystorePath))
{
    d->importKdeKeysToKeystore();
    d->splitKeysByTrustLevel();
}

Signing::~Signing()
{
    delete d;
}

QStringList Signing::keysByTrustLevel(TrustLevel trustLevel) const
{
    if (trustLevel == UnverifiableTrust) {
        return QStringList();
    }

    QList< QByteArray > s = d->keys[trustLevel];
    QStringList tmp;
    foreach(QByteArray sa, s) {
        tmp.append(sa);
    }

    return tmp;
}

TrustLevel Signing::trustLevelOf(const QString &keyID) const
{

    if (keyID.isEmpty() || keyID.isNull())
        return Plasma::UnverifiableTrust;

    for (int i = (int)Plasma::UnverifiableTrust; i <= (int)Plasma::UltimatelyTrusted; ++i) {
        QList< QByteArray > tmp = d->keys[(Plasma::Types::TrustLevel)i];
        foreach(QByteArray key, tmp) {
            if (key.contains(keyID.toAscii().data()))
                return (Plasma::Types::TrustLevel)i;
        }

    }

    return Plasma::UnverifiableTrust;
}

QStringList Signing::privateKeys() const
{
    return d->keysID(true);
}

QStringList Signing::publicKeys() const
{
    return d->keysID(false);
}

QString Signing::signerOf(const KUrl &plasmoidPath, const KUrl &plasmoidSignaturePath) const
{
    kDebug() << "Checking existence of " << plasmoidPath.pathOrUrl();
    kDebug() << "Checking existence of " << plasmoidSignaturePath.pathOrUrl();
    // Original file and signature to verify against
    QFile fileHanlder(plasmoidPath.path());
    QFile signatureHandler(plasmoidSignaturePath.isEmpty() ?
                           plasmoidPath.path().append(".asc") :
                           plasmoidSignaturePath.path());


    fileHanlder.open(QIODevice::ReadOnly);
    signatureHandler.open(QIODevice::ReadOnly);

    if (!fileHanlder.exists()) {
        kDebug() << "Plasmoid package doesn't exists: signature verification aborted.";
        return QString();
    }
    if (!signatureHandler.exists()) {
        kDebug() << "Plasmoid signature doesn't exists: signature verification aborted.";
        return QString();
    }

    kDebug() << "Cheking if " << plasmoidPath.pathOrUrl() << " and " << plasmoidSignaturePath.pathOrUrl() << " matches";

    return d->signerOf(plasmoidPath.pathOrUrl(), plasmoidSignaturePath.pathOrUrl());
}

QString Signing::keyStorePath() const
{
    return d->keystorePath();
}

QString Signing::descriptiveString(const QString &keyID) const
{
    if (keyID.isNull() || keyID.isEmpty())
        return QString();

    return d->descriptiveString(keyID);
}

}

#include "moc_signing.cpp"
