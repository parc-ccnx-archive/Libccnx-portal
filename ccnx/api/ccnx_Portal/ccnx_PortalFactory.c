/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
/**
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>

#include <LongBow/runtime.h>

#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include <ccnx/api/ccnx_Portal/ccnx_PortalFactory.h>

#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_List.h>
#include <parc/algol/parc_ArrayList.h>
#include <parc/algol/parc_DisplayIndented.h>

#include <parc/security/parc_Signer.h>
#include <parc/security/parc_Security.h>

const char *CCNxPortalFactory_LocalRouterName = "/localstack/portalFactory/LocalRouterName";
const char *CCNxPortalFactory_LocalForwarder = "/localstack/portalFactory/LocalForwarder";
const char *CCNxPortalFactory_LocalRouterTimeout = "/localstack/portalFactory/LocalRouterTimeout";

struct CCNxPortalFactory {
    const PARCIdentity *identity;
    const PARCSigner *signer;
    const PARCKeyId *keyId;
    const CCNxPortalAttributes *attributeTemplate;
    PARCProperties *properties;
};

static void
_ccnxPortalFactory_Destroy(CCNxPortalFactory **factoryPtr)
{
    CCNxPortalFactory *factory = *factoryPtr;

    parcIdentity_Release((PARCIdentity **) &factory->identity);

    parcSigner_Release((PARCSigner **) &factory->signer);

    parcKeyId_Release((PARCKeyId **) &factory->keyId);

    parcProperties_Release(&factory->properties);

    parcSecurity_Fini();
}

parcObject_ExtendPARCObject(CCNxPortalFactory, _ccnxPortalFactory_Destroy, NULL, NULL, NULL, NULL, NULL, NULL);

parcObject_ImplementAcquire(ccnxPortalFactory, CCNxPortalFactory);

parcObject_ImplementRelease(ccnxPortalFactory, CCNxPortalFactory);

CCNxPortalFactory *
ccnxPortalFactory_Create(const PARCIdentity *identity)
{
    parcIdentity_OptionalAssertValid(identity);

    parcSecurity_Init();
    CCNxPortalFactory *result = parcObject_CreateInstance(CCNxPortalFactory);
    if (result != NULL) {
        result->identity = parcIdentity_Acquire(identity);
        result->signer = parcIdentity_CreateSigner(identity);
        result->keyId = parcSigner_CreateKeyId(result->signer);
        result->properties = parcProperties_Create();

        ccnxPortalFactory_SetProperty(result, CCNxPortalFactory_LocalRouterName, "lci:/local/dcr");
        ccnxPortalFactory_SetProperty(result, CCNxPortalFactory_LocalForwarder, "tcp://127.0.0.1:9695");
        ccnxPortalFactory_SetProperty(result, CCNxPortalFactory_LocalRouterTimeout, "1000000");
    }
    return result;
}

const PARCIdentity *
ccnxPortalFactory_GetIdentity(const CCNxPortalFactory *factory)
{
    return factory->identity;
}

const PARCKeyId *
ccnxPortalFactory_GetKeyId(const CCNxPortalFactory *factory)
{
    return factory->keyId;
}

void
ccnxPortalFactory_Display(const CCNxPortalFactory *factory, int indentation)
{
    parcDisplayIndented_PrintLine(indentation, "CCNxPortalFactory@%p {", factory);

    parcIdentity_Display(factory->identity, indentation + 1);
    parcProperties_Display(factory->properties, indentation + 1);

    parcDisplayIndented_PrintLine(indentation, "}");
}

CCNxPortal *
ccnxPortalFactory_CreatePortal(const CCNxPortalFactory *factory, CCNxStackImpl *stackImplementation)
{
    return stackImplementation(factory, &ccnxPortalAttributes_NonBlocking);
}

PARCProperties *
ccnxPortalFactory_GetProperties(const CCNxPortalFactory *factory)
{
    return factory->properties;
}

const char *
ccnxPortalFactory_GetProperty(const CCNxPortalFactory *factory, const char *restrict name, const char *restrict defaultValue)
{
    return parcProperties_GetPropertyDefault(factory->properties, name, defaultValue);
}

void
ccnxPortalFactory_SetProperty(const CCNxPortalFactory *factory, const char *restrict name, const char *restrict value)
{
    parcProperties_SetProperty(factory->properties, name, value);
}
