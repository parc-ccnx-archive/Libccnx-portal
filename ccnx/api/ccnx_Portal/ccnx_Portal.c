/*
 * Copyright (c) 2014-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @copyright (c) 2014-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>

#include <pthread.h>

#include <LongBow/runtime.h>

#include <ccnx/api/ccnx_Portal/ccnx_Portal.h>
#include <ccnx/api/ccnx_Portal/ccnx_PortalAnchor.h>

#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_DisplayIndented.h>
#include <ccnx/api/control/controlPlaneInterface.h>

#include <ccnx/transport/common/transport.h>

struct ccnx_portal_status {
    int error;
    bool eof;
};

struct ccnx_portal {
    CCNxPortalStatus status;

    const CCNxPortalStack *stack;
};

static CCNxMetaMessage *
_ccnxPortal_ComposeAnchorMessage(const CCNxName *routerName, const CCNxName *name, int secondsToLive)
{
    time_t expireTime = time(0) + secondsToLive;
    CCNxPortalAnchor *namePrefix = ccnxPortalAnchor_Create(name, expireTime);

    PARCBufferComposer *composer = parcBufferComposer_Create();
    ccnxPortalAnchor_Serialize(namePrefix, composer);
    PARCBuffer *payload = parcBufferComposer_ProduceBuffer(composer);

    CCNxInterest *interest = ccnxInterest_CreateSimple(routerName);
    ccnxInterest_SetPayload(interest, payload);

    CCNxMetaMessage *message = ccnxMetaMessage_CreateFromInterest(interest);

    parcBuffer_Release(&payload);
    ccnxInterest_Release(&interest);
    parcBufferComposer_Release(&composer);
    ccnxPortalAnchor_Release(&namePrefix);

    return message;
}

static int
_ccnxPortal_SetAnchor(CCNxPortal *portal, const CCNxName *name, time_t secondsToLive)
{
    int64_t timeOutMicroSeconds = parcProperties_GetAsInteger(ccnxPortalStack_GetProperties(portal->stack), CCNxPortalFactory_LocalRouterTimeout, 1000000);

    CCNxName *routerName = ccnxName_CreateFromCString(ccnxPortalStack_GetProperty(portal->stack, CCNxPortalFactory_LocalRouterName, "lci:/local/dcr"));
    CCNxName *fullName = ccnxName_ComposeNAME(routerName, "anchor");

    CCNxMetaMessage *message = _ccnxPortal_ComposeAnchorMessage(fullName, name, secondsToLive);

    ccnxPortal_Send(portal, message, CCNxStackTimeout_MicroSeconds(timeOutMicroSeconds));
    ccnxMetaMessage_Release(&message);

    CCNxMetaMessage *response = ccnxPortal_Receive(portal, CCNxStackTimeout_MicroSeconds(timeOutMicroSeconds));
    if (response != NULL) {
        ccnxMetaMessage_Release(&response);
    }

    ccnxName_Release(&fullName);
    ccnxName_Release(&routerName);

    return 0;
}

bool
ccnxPortal_Flush(CCNxPortal *portal, const CCNxStackTimeout *timeout)
{
    CCNxControl *control = ccnxControl_CreateFlushRequest();

    // this needs to be better wrapped in ccnxControl
    PARCJSON *json = ccnxControl_GetJson(control);
    uint64_t expectedSequenceNumber = controlPlaneInterface_GetSequenceNumber(json);

    CCNxMetaMessage *message = ccnxMetaMessage_CreateFromControl(control);
    bool result = ccnxPortal_Send(portal, message, CCNxStackTimeout_Never);

    ccnxMetaMessage_Release(&message);
    ccnxControl_Release(&control);

    if (result == true) {
        bool matchedSequenceNumber = false;
        while (!matchedSequenceNumber) {
            message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
            if (message) {
                if (ccnxMetaMessage_IsControl(message)) {
                    control = ccnxMetaMessage_GetControl(message);
                    if (ccnxControl_IsCPI(control) && ccnxControl_IsACK(control)) {
                        uint64_t sequenceNumber = ccnxControl_GetAckOriginalSequenceNumber(control);
                        if (sequenceNumber == expectedSequenceNumber) {
                            matchedSequenceNumber = true;
                        }
                    }
                }
                ccnxMetaMessage_Release(&message);
            } else {
                // this is likely some sort of error with the connection.
                // should report this somehow.  This case shows up from test_ccnx_PortalAPI.
                result = false;
                break;
            }
        }
    }

    return result;
}

static void
_ccnxPortal_Destroy(CCNxPortal **portalPtr)
{
    CCNxPortal *portal = *portalPtr;

    ccnxPortal_Flush(portal, CCNxStackTimeout_Never);

    ccnxPortalStack_Stop(portal->stack);
    ccnxPortalStack_Release((CCNxPortalStack **) &portal->stack);
}

parcObject_ExtendPARCObject(CCNxPortal, _ccnxPortal_Destroy, NULL, NULL, NULL, NULL, NULL, NULL);

parcObject_ImplementAcquire(ccnxPortal, CCNxPortal);

parcObject_ImplementRelease(ccnxPortal, CCNxPortal);

CCNxPortal *
ccnxPortal_Create(const CCNxPortalAttributes *attributes, const CCNxPortalStack *portalStack)
{
    CCNxPortal *result = parcObject_CreateInstance(CCNxPortal);

    if (result != NULL) {
        result->stack = portalStack;
        result->status.eof = false;
        result->status.error = 0;
    }

    if (ccnxPortalStack_Start(portalStack) == false) {
        parcObject_Release((void **) &result);
    }

    return result;
}

const CCNxPortalStatus *
ccnxPortal_GetStatus(const CCNxPortal *portal)
{
    return &portal->status;
}

bool
ccnxPortal_SetAttributes(CCNxPortal *portal, const CCNxPortalAttributes *attributes)
{
    return ccnxPortalStack_SetAttributes(portal->stack, attributes);
}

int
ccnxPortal_GetFileId(const CCNxPortal *portal)
{
    return ccnxPortalStack_GetFileId(portal->stack);
}

bool
ccnxPortal_Listen(CCNxPortal *restrict portal, const CCNxName *restrict name, const time_t secondsToLive, const CCNxStackTimeout *microSeconds)
{
    bool result = ccnxPortalStack_Listen(portal->stack, name, microSeconds);

    if (result == true) {
        _ccnxPortal_SetAnchor(portal, name, secondsToLive);
    }

    portal->status.error = (result == true) ? 0 : ccnxPortalStack_GetErrorCode(portal->stack);

    return result;
}

bool
ccnxPortal_Ignore(CCNxPortal *portal, const CCNxName *name, const CCNxStackTimeout *microSeconds)
{
    bool result = ccnxPortalStack_Ignore(portal->stack, name, microSeconds);

    portal->status.error = (result == true) ? 0 : ccnxPortalStack_GetErrorCode(portal->stack);

    return result;
}

bool
ccnxPortal_Send(CCNxPortal *restrict portal, const CCNxMetaMessage *restrict message, const CCNxStackTimeout *timeout)
{
    bool result = ccnxPortalStack_Send(portal->stack, message, timeout);

    portal->status.error = result ? 0 : ccnxPortalStack_GetErrorCode(portal->stack);
    return result;
}

CCNxMetaMessage *
ccnxPortal_Receive(CCNxPortal *portal, const CCNxStackTimeout *timeout)
{
    CCNxMetaMessage *result = ccnxPortalStack_Receive(portal->stack, timeout);

    // This modal operation of Portal is awkward.
    // Messages are interest = content-object, while Chunked is interest = {content-object_1, content-object_2, ...}
    // If chunked.
    //   If Content Object
    //      If this content object is the final chunk
    //          Set EOF

    portal->status.error = (result != NULL) ? 0 : ccnxPortalStack_GetErrorCode(portal->stack);
    return result;
}

const PARCKeyId *
ccnxPortal_GetKeyId(const CCNxPortal *portal)
{
    return ccnxPortalStack_GetKeyId(portal->stack);
}

bool
ccnxPortal_IsEOF(const CCNxPortal *portal)
{
    return portal->status.eof;
}

bool
ccnxPortal_IsError(const CCNxPortal *portal)
{
    return portal->status.error != 0;
}

int
ccnxPortal_GetError(const CCNxPortal *portal)
{
    return portal->status.error;
}
