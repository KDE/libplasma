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

#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <plasma/plasma_export.h>

#include <QtCore/QObject>
#include <QtCore/QDateTime>

#include <kurl.h>

#include <plasma.h>

class QStringList;
class QString;

/**
  * Global namespace for Signing library
  */
namespace Plasma
{

class SigningPrivate;

/**
  * A class to manage plasmoids authentication
  *
  * The Signing class is used to get the trust level of a scripted plasmoid by performing a check
  * between the signature shipped with it, and the PGP keys found in the user keystore. If the keystore
  * is empty, this class will take charge of populating it with the pre-shipped keys. To accomplish
  * that, it relies upon the Gpgme++ libray; however the developer won't notice this, since the keys
  * are accessed by their QString ID.
  * It also performs handy operations, such as populating the local keystore with the keys shipped
  * by default with KDE/linux distro, and notify the application which is using it about changes on the
  * local keystore or in the keys folder.
  *
  * Example of usage:
  * @code
  *
  * Plasma::Signing *m_auth = new Plasma::Signing(); // the constructor takes care of loading and spltting the keys
  *                                                                // loaded by the default keystore.
  *
  * // If you want to specify a different keystore, simply put its relative path as argument:
  * Plasma::Signing *m_auth = new Plasma::Signing(keystoreFullPath);
  *
  * // Now, supposed to previously saved in plasmoidPath and signaturePath the paths of the plasmoid and signature to check; then
  * QString signer = m_auth->signerOf(plasmoidPath, signaturePath); // Returns the the hash of the signer key, if the file and
  *                                                                 // the signature have been successfully verified
  *
  * // If the signature is in the same folder of the file, you can also omit its path; the library will automatically load the
  * // signature named plasmoidPath plus the ".asc" file extension.
  * QString signer = m_auth->signerOf(plasmoidPath);
  *
  * // If you need to know the authentication level associated with a specific signer, simply call:
  * Plasma::Types::TrustLevel level = m_auth->trustLevelOf(signer)
  *
  * // If you need more details about a key with a given keyID, you have to call:
  * QString info = m_auth->descriptiveString(keyID);
  *
  * @endcode
  *
  * @since 4.6
  * @author Diego '[Po]lentino' Casella <polentino911@gmail.com>
  */
class PLASMA_EXPORT Signing : public QObject
{

    Q_OBJECT

public:
    /**
      * Default constructor.
      * It takes charge of loading the KDE keys, splitting the keys found in the default keystore (or in the one
      * specified by @param keystorePath).
      *
      * @param keystorePath a default QString that denotes the full path of the keystore being used for the authentication
      * process. Note that once selected, you can't change it.
      *
      */
    Signing(const QString &keystorePath = 0);
    ~Signing();

    /**
      * Returns a QStringList object containing all the key ID that matches the TrustLevel passed
      * as parameter to the function
      *
      * @param trustLevel the TrustLevel we want to
      *
      * @return the QStringList with all the ID of the matching keys, or an empty one otherwise
      */
    QStringList keysByTrustLevel(TrustLevel trustLevel) const;

    /**
      * Return the TrustLevel associated with the key passed as parameter to the function
      *
      * @param keyID the key we want to retrieve its trust level
      *
      * @return the TrustLevel value
      */
    TrustLevel trustLevelOf(const QString &keyID) const;

    /**
      * @return a QStringList object containing the ID of all the private keys found in the keyring.
      */
    QStringList privateKeys() const;

    /**
      * @return a QStringList object containing the ID of all the public keys found in the keyring.
      * @note Private/Public key pairs are not returned: @see privateKeys() for that.
      */
    QStringList publicKeys() const;

    /**
      * Tests for a successful match between the plasmoid and signature files, referenced by their
      * absolute path. The signature path is optional and, if not specified, the function will
      * automatically try to retrieve it by appending the string ".asc" to the plasmoid path.
      *
      * @arg plasmoidPath the full path of the plasmoid package we want to test
      * @arg plasmoidSignaturePath optional path for the signature file
      *
      * @return a Qstring with the signer's unique key id, or an empty one if a signer was not found.
      */
    QString signerOf(const KUrl &plasmoidPath, const KUrl &plasmoidSignaturePath = KUrl()) const ;


    /**
      * @return the QString path of the keystore being used.
      */
    QString keyStorePath() const;

    /**
      * Method used to retrieve additional informations about the key passed as parameter to the function.
      *
      * @arg keyID the string containing the hash of the key we want to retrive the informations.
      *
      * @return a QString object containing the name, the email and the comment (if any) referenced by the
      * id passed as argument to the function.
      */
    QString descriptiveString(const QString &keyID) const;

Q_SIGNALS:

    /**
      * Emitted when a new key is added from the trusted keys folder. Applications should connect
      * to this signal in order to update the trust level of the associated plasmoids.
      *
      * @param keyId the key added
      */
    void keyAdded(const QString &keyId);

    /*
     * Emitted when a new key is removed from the trusted keys folder. Applications should connect
     * to this signal in order to update the trust level of the associated plasmoids.
     *
     * @param keyID the key removed
     */
    void keyRemoved(const QString &keyId);

private:
    Q_PRIVATE_SLOT(d, void slotProcessKeystore());
    Q_PRIVATE_SLOT(d, void slotKDEKeyAdded(QString path));
    Q_PRIVATE_SLOT(d, void slotKDEKeyRemoved(QString path));
    SigningPrivate * const d;
    friend class SigningPrivate;
};

} // end Plasma namespace

#endif // AUTHENTICATION_H
