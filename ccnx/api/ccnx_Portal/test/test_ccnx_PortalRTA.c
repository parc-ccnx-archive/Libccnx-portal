/*
 * Copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Runner.
#include "../ccnx_PortalRTA.c"

#include <LongBow/unit-test.h>

LONGBOW_TEST_RUNNER(ccnx_PortalRTA)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified here, but every test must be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Static);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_PortalRTA)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_PortalRTA)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxPortalRTA_Chunked);
    LONGBOW_RUN_TEST_CASE(Global, ccnxPortalRTA_LoopBack);
    LONGBOW_RUN_TEST_CASE(Global, ccnxPortalRTA_Message);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxPortalRTA_Chunked)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Global, ccnxPortalRTA_LoopBack)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Global, ccnxPortalRTA_Message)
{
    testUnimplemented("");
}

LONGBOW_TEST_FIXTURE(Static)
{
    LONGBOW_RUN_TEST_CASE(Static, _autowrap_destroy__CCNxPortalRTAContext);
    LONGBOW_RUN_TEST_CASE(Static, _blockingPortal);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalProtocol_RTALoopback);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalProtocol_RTAMetis);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTAContext_Create);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTAContext_Destroy);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTAContext_Release);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTA_CreatePortal);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTA_GetAttributes);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTA_GetFileId);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTA_Ignore);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTA_IsConnected);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTA_Listen);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTA_Receive);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTA_Send);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTA_SetAttributes);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTA_Start);
    LONGBOW_RUN_TEST_CASE(Static, _ccnxPortalRTA_Stop);
    LONGBOW_RUN_TEST_CASE(Static, _createTransportConfig);
    LONGBOW_RUN_TEST_CASE(Static, _nonBlockingPortal);
}

LONGBOW_TEST_FIXTURE_SETUP(Static)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Static)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Static, _autowrap_destroy__CCNxPortalRTAContext)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _blockingPortal)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalProtocol_RTALoopback)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalProtocol_RTAMetis)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTAContext_Create)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTAContext_Destroy)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTAContext_Release)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTA_CreatePortal)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTA_GetAttributes)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTA_GetFileId)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTA_Ignore)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTA_IsConnected)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTA_Listen)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTA_Receive)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTA_Send)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTA_SetAttributes)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTA_Start)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _ccnxPortalRTA_Stop)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _createTransportConfig)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Static, _nonBlockingPortal)
{
    testUnimplemented("");
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_PortalRTA);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
