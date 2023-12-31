/********************************************************************************
*   Copyright 2011 by Aaron Seigo <aseigo@kde.org>                              *
*                                                                               *
*   This library is free software; you can redistribute it and/or               *
*   modify it under the terms of the GNU Library General Public                 *
*   License as published by the Free Software Foundation; either                *
*   version 2 of the License, or (at your option) any later version.            *
*                                                                               *
*   This library is distributed in the hope that it will be useful,             *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU            *
*   Library General Public License for more details.                            *
*                                                                               *
*   You should have received a copy of the GNU Library General Public License   *
*   along with this library; see the file COPYING.LIB.  If not, write to        *
*   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,        *
*   Boston, MA 02110-1301, USA.                                                 *
*********************************************************************************/

#include "signingtest.h"

#include <QTime>

#include <kdebug.h>

#include "applet.h"
#include "signing.h"
#include "pluginloader.h"
#include <QStandardPaths>

static const QString fingerprint("8B8B22090C6F7C47B1EAEE75D6B72EB1A7F1DB43");
static const QString shortFingerprint("D6B72EB1A7F1DB43");

SigningTest::SigningTest(QObject *parent)
    : QObject(parent),
      m_signing(0),
      m_package(Plasma::PluginLoader::self()->loadPackage("Plasma/Applet"))
{
    const QString prefix = QString::fromLatin1(KDESRCDIR) + "data/";
    m_package.setPath(prefix + "signedPackage");
    m_path = prefix + "signed.plasmoid";
    m_sig = prefix + "signed.plasmoid.sig";
    m_invalidSig = prefix + "signed.plasmoid.invalid.sig";
}

void SigningTest::initTestCase()
{
    QStandardPaths::enableTestMode(true);
}

void SigningTest::confirmCtorPerformance()
{
    QTime t;
    t.start();
    m_signing = new Plasma::Signing;
    QVERIFY(t.elapsed() < 50);
}

void SigningTest::missingFiles()
{
    QVERIFY(m_signing->signerOf(KUrl("/nonexistantpackage"), KUrl("/noneexistantsignature")).isEmpty());
    QVERIFY(m_signing->signerOf(KUrl(m_path), KUrl("/noneexistantsignature")).isEmpty());
    QVERIFY(m_signing->signerOf(KUrl("/nonexistantpackage"), KUrl(m_sig)).isEmpty());
}

void SigningTest::invalidSignature()
{
    QCOMPARE(m_signing->signerOf(m_path, m_invalidSig), QString());
}

void SigningTest::validSignature()
{
    QCOMPARE(m_signing->signerOf(m_path, m_sig), fingerprint);
}

void SigningTest::validSignatureWithoutDefinedSigFile()
{
    QCOMPARE(m_signing->signerOf(m_path), fingerprint);
}

void SigningTest::validPackage()
{
    const QString sig = m_signing->signerOf(m_package);
    QVERIFY(sig == fingerprint || sig == shortFingerprint);
}

void SigningTest::trustLevel()
{
    QCOMPARE(m_signing->trustLevelOf(QString()), Plasma::UnverifiableTrust);
    QCOMPARE(m_signing->trustLevelOf(fingerprint), Plasma::SelfTrusted);
    //FIXME: need to test other TrustLevel values
}

void SigningTest::confirmDtorPerformance()
{
    QTime t;
    t.start();
    delete m_signing;
    m_signing = 0;
    QVERIFY(t.elapsed() < 50);
}

QTEST_MAIN(SigningTest)

