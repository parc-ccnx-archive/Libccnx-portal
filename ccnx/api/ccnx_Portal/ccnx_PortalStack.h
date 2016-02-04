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
 * @file ccnx_PortalStack.h
 * @brief A polymorphic interface to different Portal Stack implementations.
 *
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#ifndef CCNx_Portal_API_ccnx_PortalImplementation_h
#define CCNx_Portal_API_ccnx_PortalImplementation_h

struct CCNxPortalStack;
/**
 * @typedef CCNxPortalStack
 * @brief A set of functions and state for the Portal instance.
 */
typedef struct CCNxPortalStack CCNxPortalStack;

#include <parc/algol/parc_Properties.h>
#include <ccnx/api/ccnx_Portal/ccnx_PortalFactory.h>

/**
 * Create function for a `CCNxPortalStack`
 *
 * @param [in] factory An instance of a {@link CCNxPortalFactory}.
 * @param [in] start A pointer to a function that takes `*privateData` as a parameter.
 * @param [in] stop A pointer to a function that takes `*privateData` as a parameter.
 * @param [in] receiveFunction A pointer to a function that takes `*privateData` as a parameter and returns a pointer to a {@link CCNxMetaMessage}.
 * @param [in] send A pointer to a function that takes `*privateData` and `*portalMessage` as parameters.
 * @param [in] listen A pointer to a function that takes `*privateData` and a {@link CCNxName} as parameters.
 * @param [in] ignore A pointer to a function that takes `*privateData` and a {@link CCNxName} as parameters.
 * @param [in] getFileId A pointer to a function that takes `*privateData` as a parameter and returns an `int`.
 * @param [in] setAttributes A pointer to a function that takes `*privateData` and a {@link CCNxPortalAttributes} as parameters.
 * @param [in] getAttributes A pointer to a function that takes `*privateData` as a parameter and returns an instance of `CCNxPortalAttributes`.
 * @param [in] privateData A pointer to some data.
 * @param [in] releasePrivateData A pointer to a function that takes `**privateData` as a parameter.
 *
 * @return  A pointer to a new `CCNxPortalStack`.
 *
 */
CCNxPortalStack *
ccnxPortalStack_Create(const CCNxPortalFactory *factory,
                       const CCNxPortalAttributes *attributes,
                       void (*start)(void *privateData),
                       void (*stop)(void *privateData),
                       CCNxMetaMessage *(*receive)(void *privateData, const CCNxStackTimeout *microSeconds),
                       bool (*send)(void *privateData, const CCNxMetaMessage *message, const CCNxStackTimeout *microSeconds),
                       bool (*listen)(void *privateData, const CCNxName *name, const CCNxStackTimeout *microSeconds),
                       bool (*ignore)(void *privateData, const CCNxName *name, const CCNxStackTimeout *microSeconds),
                       int (*getFileId)(void *privateData),
                       bool (*setAttributes)(void *privateData, const CCNxPortalAttributes *attributes),
                       CCNxPortalAttributes * (*getAttributes)(void *privateData),
                       void *privateData,
                       void (*releasePrivateData)(void **privateData));

/**
 * Increase the number of references to a `PARCBuffer`.
 *
 * Note that new `PARCBuffer` is not created,
 * only that the given `PARCBuffer` reference count is incremented.
 * Discard the reference by invoking `parcBuffer_Release`.
 *
 * @param [in] portal A pointer to a valid CCNxPortalStack instance.
 *
 * @return The input `CCNxPortalStack` pointer.
 *
 * Example:
 * @code
 * {
 *     PARCBuffer *x = parcBuffer_Allocate(10);
 *
 *     PARCBuffer *x_2 = parcBuffer_Acquire(x);
 *
 *     parcBuffer_Release(&x);
 *     parcBuffer_Release(&x_2);
 * }
 * @endcode
 */
CCNxPortalStack *ccnxPortalStack_Acquire(const CCNxPortalStack *portal);

/**
 * Release a previously acquired reference to the specified `CCNxPortalStack` instance,
 * decrementing the reference count for the instance.
 *
 * The pointer to the instance is set to NULL as a side-effect of this function.
 *
 * If the invocation causes the last reference to the instance to be released,
 * the instance is deallocated and the instance's implementation will perform
 * additional cleanup and release other privately held references.
 *
 * @param [in,out] portalPtr A pointer to a pointer to a `CCNxPortalStack` instance to be released.
 *
 * Example:
 * @code
 * {
 *     CCNxPortalStack *stack = ccnxPortalStack_Create(...);
 *
 *     ccnxPortalStack_Release(&stack);
 * }
 * @endcode
 *
 * @see {@link ccnxPortalStack_Create}
 * @see {@link ccnxPortalStack_Acquire}
 */
void ccnxPortalStack_Release(CCNxPortalStack **portalPtr);

/**
 * Release a `CCNxPortalStack` reference.
 *
 * Only the last invocation where the reference count is decremented to zero,
 * will actually destroy the `CCNxPortalStack`.
 *
 * @param [in,out] instancePtr is a pointer to the `CCNxPortalStack` reference to be released.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void ccnxPortalStack_Release(CCNxPortalStack **instancePtr);

/**
 * Start a `CCNxPortalStack`
 *
 * @param [in] implementation A pointer to an instance of `CCNxPortalStack`
 *
 * @return `true` if stack started successfully, else `false`.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxPortalStack_Start(const CCNxPortalStack *implementation);

/**
 * Stop a `CCNxPortalStack`
 *
 * @param [in] implementation A pointer to an instance of `CCNxPortalStack`
 *
 * @return `true` if stack started successfully, else `false`.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxPortalStack_Stop(const CCNxPortalStack *implementation);

/**
 * Receive a message from a `CCNxPortalStack`
 *
 * @param [in] portalStack A pointer to an instance of `CCNxPortalStack`
 * @param [in] microSeconds A pointer to a `struct timeval` or NULL indicating no timeout.
 *
 * @return An instance of {@link CCNxMetaMessage}.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxMetaMessage *ccnxPortalStack_Receive(const CCNxPortalStack *portalStack, const CCNxStackTimeout *microSeconds);

/**
 * Send a message through a `CCNxPortalStack`
 *
 * @param [in] implementation A pointer to an instance of `CCNxPortalStack`.
 * @param [in] portalMessage A pointer to an instance of `CCNxMetaMessage` to send.
 *
 * @return `true` if message sent successfully, else `false`.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxPortalStack_Send(const CCNxPortalStack *implementation, const CCNxMetaMessage *portalMessage, const CCNxStackTimeout *microSeconds);

/**
 * Set the attributes on a `CCNxPortalStack`.
 *
 * @param [in] implementation A pointer to an instance of `CCNxPortalStack`.
 * @param [in] attributes A pointer to an instance of `CCNxPortalAttributes`.
 * @return `true` if attributes set successfully, else `false`.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */

bool ccnxPortalStack_SetAttributes(const CCNxPortalStack *implementation, const CCNxPortalAttributes *attributes);

/**
 * Get the attributes from a `CCNxPortalStack`.
 *
 * @param [in] implementation A pointer to an instance of `CCNxPortalStack`.
 * @return A pointer to an instance of `CCNxPortalAttributes` associated with the @p implementation.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const CCNxPortalAttributes *ccnxPortalStack_GetAttributes(const CCNxPortalStack *implementation);

/**
 * Listen for @p name on the @p implementation.
 *
 * @param [in] implementation A pointer to an instance of `CCNxPortalStack`.
 * @param [in] name A pointer to an instance of {@link CCNxName} to listen for.
 *
 * @return `true` if listen started successfully, else `false`.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxPortalStack_Listen(const CCNxPortalStack *implementation, const CCNxName *name, const CCNxStackTimeout *microSeconds);
/**
 * Ignore (stop listening for) @p name on the @p implementation.
 *
 * @param [in] implementation A pointer to an instance of `CCNxPortalStack`.
 * @param [in] name A pointer to an instance of {@link CCNxName} to listen for.
 *
 * @return `true` if ignore was successful, else `false`.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */

bool ccnxPortalStack_Ignore(const CCNxPortalStack *implementation, const CCNxName *name, const CCNxStackTimeout *microSeconds);
/**
 * Get the file ID for @p implementation.
 *
 * @param [in] implementation A pointer to an instance of `CCNxPortalStack`.
 *
 * @return `int` The file ID.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
int ccnxPortalStack_GetFileId(const CCNxPortalStack *implementation);

/**
 * Get the {@link PARCKeyId} associated with the @p implementation.
 *
 * @param [in] implementation A pointer to an instance of `CCNxPortalStack`.
 *
 * @return A pointer to the `PARCKeyId`.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const PARCKeyId *ccnxPortalStack_GetKeyId(const CCNxPortalStack *implementation);

/**
 * Get the error code for the most recent operation.
 *
 * @param [in] implementation A pointer to an instance of `CCNxPortalStack`.
 *
 * @return A value corresponding to the UNIX `errno`  see `sys/errno.h`
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 */
int ccnxPortalStack_GetErrorCode(const CCNxPortalStack *implementation);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in#> | <#out#> | <#in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 */
PARCProperties *ccnxPortalStack_GetProperties(const CCNxPortalStack *portalStack);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] portalStack <#description#>
 * @param [in] name <#description#>
 * @param [in] defaultValue <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 */
const char *ccnxPortalStack_GetProperty(const CCNxPortalStack *portalStack, const char *restrict name, const char *restrict defaultValue);
#endif
