override MY_TEST_NAME=sdk_release_clouddoc

# SDK RELEASE TESTS NOTE
# This test can run on Linux or Windows (on Cygwin), either remote or local
# - To run it locally:
#   make TARGET_USER=<local Linux/Windows username>
# - To run it remotely:
#   make TARGET_USER=<remote Linux/Windows username> TARGET_MACHINE=<remote machine name>
#
# By default, it downloads the dxshell package from the archive machine. Then it replaces the ccd and dxshell
# with the local copy built by the developer. When it runs on buildbot, it always gets the latest copy
# from the archive machine.
#
# To setup your machine for this test to run locally, you need to install cygwin and setup ssh
# Please follow the instruction here to set it up
# http://intwww.routefree.com/wiki/index.php/PersonalCloud_How_To_Setup_Windows_Test_Machine

# makefile included by all test makefiles
include ../make/test_env.mk

# Common definitions for tests.
include ../make/test_defs.mk

# Common test target rules
include ../make/test_rules.mk

# Specialization rules for this test suite.

.PHONY: sdk_release_clouddoc
sdk_release_clouddoc:
ifdef RUN_FULL_TEST
	$(REMOTE_RUN) 'cd $(TARGET_TESTROOT); ./dxshell$(EXE) AutoTest SdkCloudDocRelease $(LAB_DOMAIN_NAME) $(CCD_TEST_ACCOUNT) password -f'
else
	$(REMOTE_RUN) 'cd $(TARGET_TESTROOT); ./dxshell$(EXE) AutoTest SdkCloudDocRelease $(LAB_DOMAIN_NAME) $(CCD_TEST_ACCOUNT) password'
endif

.PHONY: extra_cloudpc_config
 extra_cloudpc_config:
	$(REMOTE_RUN) 'cd $(TARGET_TESTROOT); ./dxshell$(EXE) -m $(CLOUDPC_ALIAS) CCDConfig Set syncUpRetryInterval 30'
	$(REMOTE_RUN) 'cd $(TARGET_TESTROOT); ./dxshell$(EXE) -m $(CLOUDPC_ALIAS) CCDConfig Set syncDownRetryInterval 30'

.PHONY: extra_md_config
extra_md_config:
	$(REMOTE_RUN) 'cd $(TARGET_TESTROOT); ./dxshell$(EXE) -m $(MD_ALIAS) CCDConfig Set syncUpRetryInterval 30'
	$(REMOTE_RUN) 'cd $(TARGET_TESTROOT); ./dxshell$(EXE) -m $(MD_ALIAS) CCDConfig Set syncDownRetryInterval 30'

.PHONY: extra_client_config
extra_client_config:
	$(REMOTE_RUN) 'cd $(TARGET_TESTROOT); ./dxshell$(EXE) -m $(CLIENT_ALIAS) CCDConfig Set syncUpRetryInterval 30'
	$(REMOTE_RUN) 'cd $(TARGET_TESTROOT); ./dxshell$(EXE) -m $(CLIENT_ALIAS) CCDConfig Set syncDownRetryInterval 30'

.PHONY: prepare_test_data
prepare_test_data: prepare_test_doc_data

.PHONY: clean_test_data
clean_test_data:

