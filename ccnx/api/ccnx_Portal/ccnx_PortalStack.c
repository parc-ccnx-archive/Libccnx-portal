/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#include <config.h>
#include <sys/errno.h>

#include <LongBow/runtime.h>

#include <ccnx/api/ccnx_Portal/ccnx_PortalStack.h>

#include <parc/algol/parc_Object.h>

struct CCNxPortalStack {
    CCNxPortalFactory *factory;

    const CCNxPortalAttributes *attributes;

    void *privateData;

    void (*start)(void *privateData);

    void (*stop)(void *privateData);

    CCNxMetaMessage *(*read)(void *privateData, const uint64_t *microSeconds);

    bool (*write)(void *privateData, const CCNxMetaMessage *portalMessage, const uint64_t *microSeconds);

    bool (*listen)(void *privateData, const CCNxName *restrict name, const uint64_t *microSeconds);

    bool (*ignore)(void *privateData, const CCNxName *restrict name, const uint64_t *microSeconds);

    int (*getFileId)(void *privateData);

    bool (*setAttributes)(void *privateData, const CCNxPortalAttributes *attributes);

    CCNxPortalAttributes * (*getAttributes)(void *privateData);

    void (*releasePrivateData)(void **privateData);
};

static void
_destroy(CCNxPortalStack **instancePtr)
{
    CCNxPortalStack *instance = *instancePtr;

    if (instance->privateData != NULL) {
        instance->releasePrivateData(&instance->privateData);
    }

    ccnxPortalFactory_Release(&instance->factory);
}

parcObject_ExtendPARCObject(CCNxPortalStack, _destroy, NULL, NULL, NULL, NULL, NULL, NULL);

parcObject_ImplementAcquire(ccnxPortalStack, CCNxPortalStack);

parcObject_ImplementRelease(ccnxPortalStack, CCNxPortalStack);

CCNxPortalStack *
ccnxPortalStack_Create(const CCNxPortalFactory *factory,
                       const CCNxPortalAttributes *attributes,
                       void (*start)(void *privateData),
                       void (*stop)(void *privateData),
                       CCNxMetaMessage *(*receive)(void *privateData, const uint64_t *microSeconds),
                       bool (*send)(void *privateData, const CCNxMetaMessage *message, const uint64_t *microSeconds),
                       bool (*listen)(void *privateData, const CCNxName *name, const uint64_t *microSeconds),
                       bool (*ignore)(void *privateData, const CCNxName *name, const uint64_t *microSeconds),
                       int (*getFileId)(void *privateData),
                       bool (*setAttributes)(void *privateData, const CCNxPortalAttributes *attributes),
                       CCNxPortalAttributes * (*getAttributes)(void *privateData),
                       void *privateData,
                       void (*releasePrivateData)(void **privateData))
{
    CCNxPortalStack *result = parcObject_CreateInstance(CCNxPortalStack);

    if (result != NULL) {
        result->factory = ccnxPortalFactory_Acquire(factory);
        result->attributes = attributes;
        result->start = start;
        result->stop = stop;
        result->read = receive;
        result->write = send;
        result->getFileId = getFileId;
        result->listen = listen;
        result->ignore = ignore;
        result->setAttributes = setAttributes;
        result->getAttributes = getAttributes;
        result->privateData = privateData;
        result->releasePrivateData = releasePrivateData;
    }
    return result;
}

bool
ccnxPortalStack_Start(const CCNxPortalStack *portalStack)
{
    portalStack->start(portalStack->privateData);
    return true;
}

bool
ccnxPortalStack_Stop(const CCNxPortalStack *portalStack)
{
    portalStack->stop(portalStack->privateData);
    return true;
}

CCNxMetaMessage *
ccnxPortalStack_Receive(const CCNxPortalStack *restrict portalStack, const uint64_t *microSeconds)
{
    CCNxMetaMessage *result = portalStack->read(portalStack->privateData, microSeconds);

    return result;
}

bool
ccnxPortalStack_Send(const CCNxPortalStack *portalStack, const CCNxMetaMessage *portalMessage, const uint64_t *microSeconds)
{
    return portalStack->write(portalStack->privateData, portalMessage, microSeconds);
}

bool
ccnxPortalStack_SetAttributes(const CCNxPortalStack *portalStack, const CCNxPortalAttributes *attributes)
{
    return portalStack->setAttributes(portalStack->privateData, attributes);
}

bool
ccnxPortalStack_Listen(const CCNxPortalStack *portalStack, const CCNxName *name, const uint64_t *microSeconds)
{
    return portalStack->listen(portalStack->privateData, name, microSeconds);
}

bool
ccnxPortalStack_Ignore(const CCNxPortalStack *portalStack, const CCNxName *name, const uint64_t *microSeconds)
{
    return portalStack->ignore(portalStack->privateData, name, microSeconds);
}

int
ccnxPortalStack_GetErrorCode(const CCNxPortalStack *portalStack)
{
    extern int errno;
    return errno;
}

const CCNxPortalAttributes *
ccnxPortalStack_GetAttributes(const CCNxPortalStack *portalStack)
{
    return portalStack->attributes;
}

int
ccnxPortalStack_GetFileId(const CCNxPortalStack *portalStack)
{
    return portalStack->getFileId(portalStack->privateData);
}

const PARCKeyId *
ccnxPortalStack_GetKeyId(const CCNxPortalStack *portalStack)
{
    return ccnxPortalFactory_GetKeyId(portalStack->factory);
}

PARCProperties *
ccnxPortalStack_GetProperties(const CCNxPortalStack *portalStack)
{
    return ccnxPortalFactory_GetProperties(portalStack->factory);
}

const char *
ccnxPortalStack_GetProperty(const CCNxPortalStack *portalStack, const char *restrict name, const char *restrict defaultValue)
{
    return ccnxPortalFactory_GetProperty(portalStack->factory, name, defaultValue);
}
