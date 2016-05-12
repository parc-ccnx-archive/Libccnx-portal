/*
 * Copyright (c) 2014, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ################################################################################
 * #
 * # PATENT NOTICE
 * #
 * # This software is distributed under the BSD 2-clause License (see LICENSE
 * # file).  This BSD License does not make any patent claims and as such, does
 * # not act as a patent grant.  The purpose of this section is for each contributor
 * # to define their intentions with respect to intellectual property.
 * #
 * # Each contributor to this source code is encouraged to state their patent
 * # claims and licensing mechanisms for any contributions made. At the end of
 * # this section contributors may each make their own statements.  Contributor's
 * # claims and grants only apply to the pieces (source code, programs, text,
 * # media, etc) that they have contributed directly to this software.
 * #
 * # There is no guarantee that this section is complete, up to date or accurate. It
 * # is up to the contributors to maintain their portion of this section and up to
 * # the user of the software to verify any claims herein.
 * #
 * # Do not remove this header notification.  The contents of this section must be
 * # present in all distributions of the software.  You may only modify your own
 * # intellectual property statements.  Please provide contact information.
 * 
 * - Palo Alto Research Center, Inc
 * This software distribution does not grant any rights to patents owned by Palo
 * Alto Research Center, Inc (PARC). Rights to these patents are available via
 * various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
 * intellectual property used by its contributions to this software. You may
 * contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
 */
#include <stdio.h>

#include <LongBow/runtime.h>

#include <ccnx/api/ccnx_Portal/ccnx_Portal.h>

#include <parc/security/parc_Security.h>
#include <parc/security/parc_PublicKeySignerPkcs12Store.h>
#include <parc/security/parc_IdentityFile.h>

#include <ccnx/common/ccnx_Name.h>
#include <ccnx/common/ccnx_SignedInfo.h>
#include <ccnx/common/ccnx_ContentObject.h>
#include <ccnx/common/ccnx_TimeStamp.h>

PARCIdentity *
getIdentity_FromFile(const char *keystoreFileName, const char *password)
{
    PARCIdentity *result = NULL;

    PARCIdentityFile *identityFile = parcIdentityFile_Create(keystoreFileName, password);
    if (identityFile != NULL) {
        result = parcIdentity_Create(identityFile, PARCIdentityFileAsPARCIdentity);
        parcIdentityFile_Release(&identityFile);
    }

    return result;
}

int
reader_writer(CCNxPortalFactory *factory, const char *uri)
{

    CCNxPortal *portal = ccnxPortalFactory_GetInstance(factory, ccnxPortalTypeDatagram, ccnxPortalProtocol_TLV, &ccnxPortalAttributes_Blocking);

    CCNxName *prefix = ccnxName_CreateFromURI(uri);
    CCNxName *bye = ccnxName_CreateFromURI("lci:/Hello/Goodbye%21");
    CCNxName *contentname = ccnxName_CreateFromURI("lci:/Hello/World");

    if (ccnxPortal_Listen(portal, prefix, 365 * 86400, CCNxStackTimeout_Never)) {
        CCNxMetaMessage *msg;

        while ((msg = ccnxPortal_Receive(portal, CCNxStackTimeout_Never)) != NULL) {

            if (ccnxMetaMessage_IsInterest(msg)) {
                CCNxInterest *interest = ccnxMetaMessage_GetInterest(msg);

                CCNxName *interestName = ccnxInterest_GetName(interest);

                if (ccnxName_Equals(interestName, contentname)) {
                    const PARCKeyId *publisherKeyId = ccnxPortal_GetKeyId(portal);

                    char buffer[128];
                    time_t theTime = time(0);
                    sprintf(buffer, "Hello World. The time is %s", ctime(&theTime));

                    PARCBuffer *payload = parcBuffer_CreateFromArray(buffer, 128);
                    parcBuffer_Flip(payload);

                    PARCBuffer *b = parcBuffer_Acquire(payload);
                    CCNxContentObject *uob = ccnxContentObject_CreateWithNameAndPayload(contentname, b);

                    // missing NULL check, case 1024

                    CCNxMetaMessage *message = ccnxMetaMessage_CreateFromContentObject(uob);
                    if (ccnxPortal_Send(portal, message, CCNxTransportStackTimeCCNxStackTimeout_Neverout_Never) == false) {
                        fprintf(stderr, "ccnx_write failed\n");
                    }
                    // ccnxMessage_Release(message);
                } else if (ccnxName_Equals(interestName, bye)) {
                    break;
                }
            } else {
                ccnxMetaMessage_Display(msg, 0);
            }
            ccnxMetaMessage_Release(&msg);
        }
    }

    ccnxName_Release(&prefix);
    ccnxName_Release(&bye);
    ccnxName_Release(&contentname);

    ccnxPortal_Release(&portal);

    return 0;
}

int
ccnx_Portal_Reader(char *keystoreFileName, const char *password, const char *uri)
{
    parcSecurity_Init();

    PARCIdentity *identity = getIdentity_FromFile(keystoreFileName, password);

    if (identity != NULL) {
        CCNxPortalFactory *factory = ccnxPortalFactory_Create(identity);

        reader_writer(factory, uri);

        ccnxPortalFactory_Release(&factory);
        parcIdentity_Release(&identity);
    }

    parcSecurity_Fini();

    return 0;
}

int
main(int argc, char *argv[argc])
{
    char *keystoreFileName = argv[1];
    char *password = argv[2];
    char *uri = argv[3];

    keystoreFileName = "/tmp/keystore";
    password = "password";
    uri = "lci:/Hello";

    // read fileName password  lci://my/name  lci
    return ccnx_Portal_Reader(keystoreFileName, password, uri);
}
