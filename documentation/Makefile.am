#
# Copyright (c) 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Patent rights are not granted under this agreement. Patent rights are
#       available under FRAND terms.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# @author Glenn Scott, Palo Alto Research Center (PARC)
# @copyright 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
#
include ../config.mk

docs: doxygen

doxygen: Makefile stage1 stage2

stage1: stage1-message doxygen-libccnx-stage1 doxygen-libccnx_api_control-stage1 doxygen-libccnx_api_notify-stage1 doxygen-libccnx_api_portal-stage1  doxygen-librta-stage1
stage2: stage2-message doxygen-libccnx-stage2 doxygen-libccnx_api_control-stage2 doxygen-libccnx_api_notify-stage2 doxygen-libccnx_api_portal-stage2  doxygen-librta-stage2

# Stage 1

stage1-message:
	@echo
	@echo !!  Running doxygen to create *only* TAG files.
	@echo

doxygen-libccnx-stage1: libccnx-stage1.doxygen
	${DOXYGEN_BIN} -s libccnx-stage1.doxygen >& doxygen-libccnx-stage1.log

doxygen-libccnx_api_control-stage1: libccnx_api_control-stage1.doxygen
	${DOXYGEN_BIN} -s libccnx_api_control-stage1.doxygen >& doxygen-libccnx_api_control-stage1.log

doxygen-libccnx_api_notify-stage1: libccnx_api_notify-stage1.doxygen
	${DOXYGEN_BIN} -s libccnx_api_notify-stage1.doxygen >& doxygen-libccnx_api_notify-stage1.log

doxygen-libccnx_api_portal-stage1: libccnx_api_portal-stage1.doxygen
	${DOXYGEN_BIN} -s libccnx_api_portal-stage1.doxygen >& doxygen-libccnx_api_portal-stage1.log

doxygen-librta-stage1: librta-stage1.doxygen
	${DOXYGEN_BIN} -s librta-stage1.doxygen >& doxygen-librta-stage1.log

# Stage 2

stage2-message:
	@echo
	@echo !!  Now running doxygen to create documentation, referencing previously generated TAG files.
	@echo

doxygen-libccnx-stage2: libccnx-stage2.doxygen
	${DOXYGEN_BIN} -s libccnx-stage2.doxygen >& doxygen-libccnx-stage2.log
	${LONGBOW_DOXYGEN_BIN_REPORT} --doxygenlog doxygen-libccnx-stage2.log

doxygen-libccnx_api_control-stage2: libccnx_api_control-stage2.doxygen
	${DOXYGEN_BIN} -s libccnx_api_control-stage2.doxygen >& doxygen-libccnx_api_control-stage2.log
	${LONGBOW_DOXYGEN_BIN_REPORT} --doxygenlog doxygen-libccnx_api_control-stage2.log

doxygen-libccnx_api_notify-stage2: libccnx_api_notify-stage2.doxygen
	${DOXYGEN_BIN} -s libccnx_api_notify-stage2.doxygen >& doxygen-libccnx_api_notify-stage2.log
	${LONGBOW_DOXYGEN_BIN_REPORT} --doxygenlog doxygen-libccnx_api_notify-stage2.log

doxygen-libccnx_api_portal-stage2: libccnx_api_portal-stage2.doxygen
	${DOXYGEN_BIN} -s libccnx_api_portal-stage2.doxygen >& doxygen-libccnx_api_portal-stage2.log
	${LONGBOW_DOXYGEN_BIN_REPORT} --doxygenlog doxygen-libccnx_api_portal-stage2.log

doxygen-librta-stage2: librta-stage2.doxygen
	${DOXYGEN_BIN} -s librta-stage2.doxygen >& doxygen-librta-stage2.log
	${LONGBOW_DOXYGEN_BIN_REPORT} --doxygenlog doxygen-librta-stage2.log

website: doxygen
	$(MAKE) -C Jekyll

# The Doxygen output directory removed here is set in libparc.conf
clobber: clean
	${RM} -r libccnx-documentation
	${RM} -r libccnx_api_control-documentation
	${RM} -r libccnx_api_keyvalue-documentation
	${RM} -r libccnx_api_mqueue-documentation
	${RM} -r libccnx_api_notify-documentation
	${RM} -r libccnx_api_portal-documentation
	${RM} -r librta-documentation

CLEANFILES=doxygen.log doxygen-libccnx.log doxygen-libccnx_api_control.log \
	doxygen-libccnx_api_keyvalue.log doxygen-libccnx_api_mqueue.log \
	doxygen-libccnx_api_notify.log doxygen-libccnx_api_notify.log \
	doxygen-libccnx_api_portal.log doxygen-librta.log *.doctags
