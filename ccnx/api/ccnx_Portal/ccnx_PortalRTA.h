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
 * @file ccnx_PortalRTA.h
 * @brief The CCNxPortalStack representation of an "RTA" Transport Stack
 *
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#ifndef __CCNx_Portal_API__ccnx_PortalRTA__
#define __CCNx_Portal_API__ccnx_PortalRTA__

#include <ccnx/api/ccnx_Portal/ccnx_PortalAttributes.h>
#include <ccnx/api/ccnx_Portal/ccnx_Portal.h>

/**
 * Specification for an "RTA" Tranport Stack configured for Message-by-Message interaction.
 *
 * @param [in] factory A pointer to a valid {@link CCNxPortalFactory} instance.
 * @param [in] attributes A pointer to a valid {@link CCNxPortalAttributes} instance.
 *
 * @return non-NULL A pointer to a valid {@link CCNxPortal} instance bound to the specified Transport Stack.
 *
 * Example:
 * @code
 * {
 *     CCNxPortal *portal = ccnxPortalFactory_CreatePortal(factory, ccnxPortalRTA_Message);
 * }
 * @endcode
 */
CCNxPortal *ccnxPortalRTA_Message(const CCNxPortalFactory *factory, const CCNxPortalAttributes *attributes);

/**
 * Specification for an "RTA" Tranport Stack configured for Chunked interaction.
 *
 * The flow of inbound Content Objects is initiated by the first `CCNxInterest` specifying a valid Chunked protocol.
 *
 * @param [in] factory A pointer to a valid {@link CCNxPortalFactory} instance.
 * @param [in] attributes A pointer to a valid {@link CCNxPortalAttributes} instance.
 *
 * @return non-NULL A pointer to a valid {@link CCNxPortal} instance bound to the specified Transport Stack.
 *
 * Example:
 * @code
 * {
 *     CCNxPortal *portal = ccnxPortalFactory_CreatePortal(factory, ccnxPortalRTA_Chunked);
 * }
 * @endcode
 */
CCNxPortal *ccnxPortalRTA_Chunked(const CCNxPortalFactory *factory, const CCNxPortalAttributes *attributes);

/**
 * Specification for an "RTA" Tranport Stack configured for a loopback, Message-by-Message interaction.
 *
 * The loopback causes all messages sent to be reflected back to be received.
 *
 * @param [in] factory A pointer to a valid {@link CCNxPortalFactory} instance.
 * @param [in] attributes A pointer to a valid {@link CCNxPortalAttributes} instance.
 *
 * @return non-NULL A pointer to a valid {@link CCNxPortal} instance bound to the specified Transport Stack.
 *
 * Example:
 * @code
 * {
 *     CCNxPortal *portal = ccnxPortalFactory_CreatePortal(factory, ccnxPortalRTA_LoopBack);
 * }
 * @endcode
 */
CCNxPortal *ccnxPortalRTA_LoopBack(const CCNxPortalFactory *factory, const CCNxPortalAttributes *attributes);

#endif /* defined(__CCNx_Portal_API__ccnx_PortalRTA__) */
