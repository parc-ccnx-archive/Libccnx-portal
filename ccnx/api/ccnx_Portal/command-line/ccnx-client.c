/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright 2014-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#include <config.h>

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include <LongBow/runtime.h>

#include "ccnxPortalClient_About.h"

#include <ccnx/api/ccnx_Portal/ccnx_Portal.h>
#include <ccnx/api/ccnx_Portal/ccnx_PortalRTA.h>

#include <parc/security/parc_Security.h>
#include <parc/security/parc_IdentityFile.h>
#include <parc/security/parc_PublicKeySignerPkcs12Store.h>

#include <parc/algol/parc_Memory.h>

#include <parc/algol/parc_InputStream.h>
#include <parc/algol/parc_OutputStream.h>

int
ccnGet(PARCIdentity *identity, CCNxName *name)
{
    CCNxPortalFactory *factory = ccnxPortalFactory_Create(identity);

    CCNxPortal *portal = ccnxPortalFactory_CreatePortal(factory, ccnxPortalRTA_Message);

    assertNotNull(portal, "Expected a non-null CCNxPortal pointer.");

    CCNxInterest *interest = ccnxInterest_CreateSimple(name);
    ccnxName_Release(&name);

    CCNxMetaMessage *message = ccnxMetaMessage_CreateFromInterest(interest);

    if (ccnxPortal_Send(portal, message, CCNxStackTimeout_Never)) {
        while (ccnxPortal_IsError(portal) == false) {
            CCNxMetaMessage *response = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
            if (response != NULL) {
                if (ccnxMetaMessage_IsContentObject(response)) {
                    CCNxContentObject *contentObject = ccnxMetaMessage_GetContentObject(response);

                    PARCBuffer *payload = ccnxContentObject_GetPayload(contentObject);

                    size_t length = parcBuffer_Remaining(payload);
                    ssize_t nwritten = write(1, parcBuffer_Overlay(payload, length), length);
                    assertTrue(nwritten == length, "Did not write whole buffer, got %zd expected %zu", nwritten, length);

                    break;
                }
                ccnxMetaMessage_Release(&response);
            }
        }
    }

    ccnxPortal_Release(&portal);

    ccnxPortalFactory_Release(&factory);

    return 0;
}

void
usage(void)
{
    printf("%s\n", ccnxPortalClientAbout_About());
    printf("ccn-client --identity <file> --password <password> <objectName>\n");
    printf("ccn-client [-h | --help]\n");
    printf("ccn-client [-v | --version]\n");
    printf("\n");
    printf("    --identity  The file name containing a PKCS12 keystore\n");
    printf("    --password  The password to unlock the keystore\n");
    printf("    <objectName> The LCI name of the object to fetch\n");
}

int
main(int argc, char *argv[argc])
{
    char *keystoreFile = NULL;
    char *keystorePassword = NULL;

    /* options descriptor */
    static struct option longopts[] = {
        { "identity", required_argument, NULL, 'f' },
        { "password", required_argument, NULL, 'p' },
        { "version",  no_argument,       NULL, 'v' },
        { "help",     no_argument,       NULL, 'h' },
        { NULL,       0,                 NULL, 0   }
    };

    int ch;
    while ((ch = getopt_long(argc, argv, "fphv", longopts, NULL)) != -1) {
        switch (ch) {
            case 'f':
                keystoreFile = optarg;
                break;

            case 'p':
                keystorePassword = optarg;
                break;

            case 'v':
                printf("%s\n", ccnxPortalClientAbout_Version());
                return 0;

            case 'h':
                usage();
                return 0;

            default:
                usage();
                return -1;
        }
    }

    argc -= optind;
    argv += optind;
    if (argv[0] == NULL || keystoreFile == NULL || keystorePassword == NULL) {
        usage();
        return -1;
    }

    char *objectName = argv[0];

    PARCIdentityFile *identityFile = parcIdentityFile_Create(keystoreFile, keystorePassword);
    if (parcIdentityFile_Exists(identityFile) == false) {
        printf("Inaccessible keystore file '%s'.\n", keystoreFile);
        exit(1);
    }
    PARCIdentity *identity = parcIdentity_Create(identityFile, PARCIdentityFileAsPARCIdentity);
    parcIdentityFile_Release(&identityFile);

    CCNxName *name = ccnxName_CreateFromURI(objectName);

    int result = ccnGet(identity, name);

    parcIdentity_Release(&identity);
    ccnxName_Release(&name);

    return result;
}
