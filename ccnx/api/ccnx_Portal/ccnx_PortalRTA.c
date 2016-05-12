/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>

#include <LongBow/runtime.h>

#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include <stdio.h>

#include <ccnx/api/ccnx_Portal/ccnx_PortalRTA.h>
#include <ccnx/api/ccnx_Portal/ccnx_PortalFactory.h>
#include <ccnx/api/ccnx_Portal/ccnx_PortalStack.h>

#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_List.h>
#include <parc/algol/parc_ArrayList.h>
#include <parc/algol/parc_DisplayIndented.h>

#include <parc/logging/parc_Log.h>
#include <parc/logging/parc_LogReporterTextStdout.h>

#include <parc/security/parc_Signer.h>

#include <ccnx/api/control/cpi_Forwarding.h>
#include <ccnx/api/control/cpi_ControlMessage.h>
#include <ccnx/api/control/cpi_ControlFacade.h>
#include <ccnx/transport/common/transport_MetaMessage.h>
#include <ccnx/transport/common/ccnx_StackConfig.h>
#include <ccnx/transport/common/ccnx_ConnectionConfig.h>

#include <ccnx/common/internal/ccnx_TlvDictionary.h>

#include <ccnx/api/notify/notify_Status.h>

#include <ccnx/transport/transport_rta/rta_Transport.h>

#include <ccnx/transport/transport_rta/config/config_ProtocolStack.h>
#include <ccnx/transport/transport_rta/config/config_ApiConnector.h>
#include <ccnx/transport/transport_rta/config/config_FlowControl_Vegas.h>
#include <ccnx/transport/transport_rta/config/config_Codec_Tlv.h>
#include <ccnx/transport/transport_rta/config/config_Forwarder_Local.h>
#include <ccnx/transport/transport_rta/config/config_Forwarder_Metis.h>
#include <ccnx/transport/transport_rta/config/config_PublicKeySigner.h>

static const uint16_t ccnxPortal_MetisPort = 9695;

typedef enum {
    ccnxPortalTypeChunked,
    ccnxPortalTypeMessage
} _CCNxPortalType;

typedef enum {
    CCNxPortalProtocol_RTALoopback,
    ccnxPortalProtocol_RTA,
    CCNxPortalProtocol_APILoopback
} _CCNxPortalProtocol;

typedef struct _CCNxPortalRTAContext {
    RTATransport *rtaTransport;
    const CCNxTransportConfig *(*createTransportConfig)(const CCNxPortalFactory *, _CCNxPortalType, _CCNxPortalProtocol);
    const CCNxTransportConfig *configuration;
    int fileId;
    PARCLog *logger;
} _CCNxPortalRTAContext;

static void
_ccnxPortalRTAContext_Destroy(_CCNxPortalRTAContext **instancePtr)
{
    _CCNxPortalRTAContext *instance = *instancePtr;

    rtaTransport_Close(instance->rtaTransport, instance->fileId);
    rtaTransport_Destroy(&instance->rtaTransport);

    ccnxTransportConfig_Destroy((CCNxTransportConfig **) &instance->configuration);
    parcLog_Release(&instance->logger);
}

parcObject_ExtendPARCObject(_CCNxPortalRTAContext, _ccnxPortalRTAContext_Destroy,
                            NULL, NULL, NULL, NULL, NULL, NULL);

static parcObject_ImplementRelease(_ccnxPortalRTAContext, _CCNxPortalRTAContext);

static _CCNxPortalRTAContext *
_ccnxPortalRTAContext_Create(RTATransport *rtaTransport, const CCNxTransportConfig *configuration, int fileId)
{
    _CCNxPortalRTAContext *result = parcObject_CreateInstance(_CCNxPortalRTAContext);
    if (result != NULL) {
        result->rtaTransport = rtaTransport;
        result->configuration = configuration;
        result->fileId = fileId;

        PARCLogReporter *reporter = parcLogReporterTextStdout_Create();
        result->logger = parcLog_Create(NULL, "ccnxPortalRTA", NULL, reporter);
        parcLogReporter_Release(&reporter);
        parcLog_SetLevel(result->logger, PARCLogLevel_Debug);
    }

    return result;
}

static void
_ccnxPortalProtocol_RTALoopback(CCNxConnectionConfig *connConfig, CCNxStackConfig *stackConfig, PARCArrayList *listOfComponentNames)
{
    char *bentPipeNameEnv = getenv("BENT_PIPE_NAME");
    if (bentPipeNameEnv != NULL) {
    } else {
        printf("The BENT_PIPE_NAME environment variable needs to the name of a 'fifo' file.  Try /tmp/test_ccnx_Portal\n");
    }

    parcArrayList_Add(listOfComponentNames, (char *) tlvCodec_GetName());
    tlvCodec_ProtocolStackConfig(stackConfig);
    tlvCodec_ConnectionConfig(connConfig);

    parcArrayList_Add(listOfComponentNames, (char *) localForwarder_GetName());
    localForwarder_ProtocolStackConfig(stackConfig);
    localForwarder_ConnectionConfig(connConfig, bentPipeNameEnv);
}

static void
_ccnxPortalProtocol_RTAMetis(CCNxConnectionConfig *connConfig, CCNxStackConfig *stackConfig, PARCArrayList *listOfComponentNames)
{
    uint16_t metisPort = ccnxPortal_MetisPort;
    char *metisPortEnv = getenv("METIS_PORT");
    if (metisPortEnv != NULL) {
        metisPort = (uint16_t) atoi(metisPortEnv);
    }
    parcArrayList_Add(listOfComponentNames, (char *) tlvCodec_GetName());
    tlvCodec_ProtocolStackConfig(stackConfig);
    tlvCodec_ConnectionConfig(connConfig);

    parcArrayList_Add(listOfComponentNames, (char *) metisForwarder_GetName());
    metisForwarder_ProtocolStackConfig(stackConfig);
    metisForwarder_ConnectionConfig(connConfig, metisPort);
}

/**
 * This composes a CCNxTransportConfig instance that describes a complete transport stack assembly.
 */
static const CCNxTransportConfig *
_createTransportConfig(const CCNxPortalFactory *factory, _CCNxPortalType type, _CCNxPortalProtocol protocol)
{
    if (type == ccnxPortalTypeChunked) {
        // Good.
    } else if (type == ccnxPortalTypeMessage) {
        // Good.
    } else {
        return NULL;
    }

    // TODO: This is in need of some narrative of what's going on here.

    CCNxConnectionConfig *connConfig = ccnxConnectionConfig_Create();
    CCNxStackConfig *stackConfig = ccnxStackConfig_Create();

    PARCArrayList *listOfComponentNames = parcArrayList_Create_Capacity(NULL, NULL, 8);

    parcArrayList_Add(listOfComponentNames, (char *) apiConnector_GetName());

    apiConnector_ProtocolStackConfig(stackConfig);
    apiConnector_ConnectionConfig(connConfig);

    if (type == ccnxPortalTypeChunked) {
        parcArrayList_Add(listOfComponentNames, (char *) vegasFlowController_GetName());
        vegasFlowController_ProtocolStackConfig(stackConfig);
        vegasFlowController_ConnectionConfig(connConfig);
    }

    switch (protocol) {
        case CCNxPortalProtocol_RTALoopback:
            _ccnxPortalProtocol_RTALoopback(connConfig, stackConfig, listOfComponentNames);
            break;

        case ccnxPortalProtocol_RTA:
            _ccnxPortalProtocol_RTAMetis(connConfig, stackConfig, listOfComponentNames);
            break;

        default:
            errno = EPROTOTYPE;
            assertTrue(0, "Unknown protocol type: %d", protocol);
    }


    protocolStack_ComponentsConfigArrayList(stackConfig, listOfComponentNames);
    parcArrayList_Destroy(&listOfComponentNames);

    const PARCIdentity *identity = ccnxPortalFactory_GetIdentity(factory);

    configPublicKeySigner_SetIdentity(connConfig, identity);

    CCNxTransportConfig *result = ccnxTransportConfig_Create(stackConfig, connConfig);

    ccnxStackConfig_Release(&stackConfig);

    return result;
}

static void
_ccnxPortalRTA_Start(void *privateData)
{
}

static void
_ccnxPortalRTA_Stop(void *privateData)
{
}

static bool
_ccnxPortalRTA_Send(void *privateData, const CCNxMetaMessage *portalMessage, const CCNxStackTimeout *microSeconds)
{
    const _CCNxPortalRTAContext *transportContext = (_CCNxPortalRTAContext *) privateData;

    bool result = rtaTransport_Send(transportContext->rtaTransport, transportContext->fileId, portalMessage, microSeconds);

    return result;
}

static CCNxMetaMessage *
_ccnxPortalRTA_Receive(void *privateData, const CCNxStackTimeout *microSeconds)
{
    const _CCNxPortalRTAContext *transportContext = (_CCNxPortalRTAContext *) privateData;

    CCNxMetaMessage *result = NULL;

    TransportIOStatus status = rtaTransport_Recv(transportContext->rtaTransport, transportContext->fileId, &result, microSeconds);

    if (status != TransportIOStatus_Success) {
        return NULL;
    }

    return result;
}

static int
_ccnxPortalRTA_GetFileId(void *privateData)
{
    const _CCNxPortalRTAContext *transportContext = (_CCNxPortalRTAContext *) privateData;

    return transportContext->fileId;
}

static CCNxPortalAttributes *
_ccnxPortalRTA_GetAttributes(void *privateData)
{
    return NULL;
}

static bool
_nonBlockingPortal(const _CCNxPortalRTAContext *transportContext)
{
    int fd = transportContext->fileId;
    int flags;

    if ((flags = fcntl(fd, F_GETFL, NULL)) != -1) {
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1) {
            return true;
        }
    }
    return false;
}

static bool
_ccnxPortalRTA_SetAttributes(void *privateData, const CCNxPortalAttributes *attributes)
{
    const _CCNxPortalRTAContext *transportContext = (_CCNxPortalRTAContext *) privateData;

    bool result = _nonBlockingPortal(transportContext);

    return result;
}

static bool
_ccnxPortalRTA_Listen(void *privateData, const CCNxName *name, const CCNxStackTimeout *microSeconds)
{
    CCNxControl *control = ccnxControl_CreateAddRouteToSelfRequest(name);

    CCNxMetaMessage *message = ccnxMetaMessage_CreateFromControl(control);

    ccnxControl_Release(&control);

    bool result = _ccnxPortalRTA_Send(privateData, message, CCNxStackTimeout_Never);

    // There is a problem here if the client invokes this function on a portal that is already receiving messages.
    // This simply absorbs messages until the receipt of the acknowledgement of this listen.
    // Perhaps what should happen is not read any messages and let the client sort it out in its read loop.
    if (result == true) {
        CCNxMetaMessage *response = _ccnxPortalRTA_Receive(privateData, microSeconds);

        if (ccnxMetaMessage_IsControl(response)) {
            // TODO: Check that the response was success.
            result = true;
        }

        ccnxMetaMessage_Release(&response);
    }

    ccnxMetaMessage_Release(&message);

    return result;
}

static bool
_ccnxPortalRTA_Ignore(void *privateData, const CCNxName *name, const CCNxStackTimeout *microSeconds)
{
    CCNxControl *control = ccnxControl_CreateRemoveRouteToSelfRequest(name);

    CCNxMetaMessage *message = ccnxMetaMessage_CreateFromControl(control);
    ccnxControl_Release(&control);

    bool result = _ccnxPortalRTA_Send(privateData, message, CCNxStackTimeout_Never);

    // There is a problem here if the client invokes this function on a portal that is already receiving messages.
    // This simply absorbs messages until the receipt of the acknowledgement of this listen.
    // Perhaps what should happen is not read any messages and let the client sort it out in its read loop.
    if (result == true) {
        CCNxMetaMessage *response = _ccnxPortalRTA_Receive(privateData, microSeconds);

        if (ccnxMetaMessage_IsControl(response)) {
            // TODO: Check that the response was success.
            result = true;
        }

        ccnxMetaMessage_Release(&response);
    }

    ccnxMetaMessage_Release(&message);

    return result;
}

static bool
_ccnxPortalRTA_IsConnected(CCNxPortal *portal)
{
    bool result = false;

    CCNxMetaMessage *response;

    if ((response = ccnxPortal_Receive(portal, CCNxStackTimeout_Never)) != NULL) {
        if (ccnxMetaMessage_IsControl(response)) {
            CCNxControl *control = ccnxMetaMessage_GetControl(response);

            if (ccnxControl_IsNotification(control)) {
                NotifyStatus *status = ccnxControl_GetNotifyStatus(control);

                if (notifyStatus_IsConnectionOpen(status) == true) {
                    result = true;
                }
                notifyStatus_Release(&status);
            }
        }
        ccnxMetaMessage_Release(&response);
    }

    return result;
}

static CCNxPortal *
_ccnxPortalRTA_CreatePortal(const CCNxPortalFactory *factory,
                            _CCNxPortalType type,
                            _CCNxPortalProtocol protocol,
                            const CCNxPortalAttributes *attributes)
{
    CCNxPortal *result = NULL;

    const CCNxTransportConfig *configuration = _createTransportConfig(factory, type, protocol);

    if (ccnxTransportConfig_IsValid(configuration) == false) {
        return NULL;
    }

    RTATransport *rtaTransport = rtaTransport_Create();
    if (rtaTransport != NULL) {
        int fileDescriptor = rtaTransport_Open(rtaTransport, (CCNxTransportConfig *) configuration);

        _CCNxPortalRTAContext *transportContext = _ccnxPortalRTAContext_Create(rtaTransport, configuration, fileDescriptor);

        if (transportContext != NULL) {
            CCNxPortalStack *implementation =
                ccnxPortalStack_Create(factory,
                                       attributes,
                                       _ccnxPortalRTA_Start,
                                       _ccnxPortalRTA_Stop,
                                       _ccnxPortalRTA_Receive,
                                       _ccnxPortalRTA_Send,
                                       _ccnxPortalRTA_Listen,
                                       _ccnxPortalRTA_Ignore,
                                       _ccnxPortalRTA_GetFileId,
                                       _ccnxPortalRTA_SetAttributes,
                                       _ccnxPortalRTA_GetAttributes,
                                       transportContext,
                                       (void (*)(void **))_ccnxPortalRTAContext_Release);

            result = ccnxPortal_Create(attributes, implementation);

            if (result != NULL) {
                if (_ccnxPortalRTA_IsConnected(result) == true) {
                    _nonBlockingPortal(transportContext);
                } else {
                    ccnxPortal_Release(&result);
                }
            }
        }
    }

    return result;
}

CCNxPortal *
ccnxPortalRTA_Message(const CCNxPortalFactory *factory, const CCNxPortalAttributes *attributes)
{
    return _ccnxPortalRTA_CreatePortal(factory, ccnxPortalTypeMessage, ccnxPortalProtocol_RTA, attributes);
}

CCNxPortal *
ccnxPortalRTA_Chunked(const CCNxPortalFactory *factory, const CCNxPortalAttributes *attributes)
{
    return _ccnxPortalRTA_CreatePortal(factory, ccnxPortalTypeChunked, ccnxPortalProtocol_RTA, attributes);
}

CCNxPortal *
ccnxPortalRTA_LoopBack(const CCNxPortalFactory *factory, const CCNxPortalAttributes *attributes)
{
    return _ccnxPortalRTA_CreatePortal(factory, ccnxPortalTypeMessage, CCNxPortalProtocol_RTALoopback, attributes);
}
