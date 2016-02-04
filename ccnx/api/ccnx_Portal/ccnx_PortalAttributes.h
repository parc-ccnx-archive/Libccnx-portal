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
 * @file ccnx_PortalAttributes.h
 * @brief Attributes related to the `CCNxPortal` instance.
 *
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#ifndef __CCNx_Portal_API__ccnx_PortalAttributes__
#define __CCNx_Portal_API__ccnx_PortalAttributes__

#include <stdbool.h>

struct ccnx_portal_attributes;
/**
 * @brief The attributes for the instance of {@link CCNxPortal}
 */
typedef struct ccnx_portal_attributes CCNxPortalAttributes;

/**
 * Non-blocking (reads)
 */
extern const CCNxPortalAttributes ccnxPortalAttributes_NonBlocking;

/**
 * Return `true` if the given attributes indicate Portal Logging is enabled.
 *
 * @param [in] attributes A pointer to a valid {@link CCNxPortalAttributes} instance.
 *
 * @return `true` If the given attributes indicate Portal Logging is enabled.
 * @return `false` If the given attributes do not indicate Portal Logging is enabled.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxPortalAttributes_IsLogging(const CCNxPortalAttributes *attributes);

#endif /* defined(__CCNx_Portal_API__ccnx_PortalAttributes__) */
