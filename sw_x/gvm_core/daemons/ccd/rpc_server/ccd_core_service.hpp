//
//  Copyright 2010 iGware Inc.
//  All Rights Reserved.
//
//  THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND
//  TRADE SECRETS OF IGWARE INC.
//  USE, DISCLOSURE OR REPRODUCTION IS PROHIBITED WITHOUT
//  THE PRIOR EXPRESS WRITTEN PERMISSION OF IGWARE INC.
//
#ifndef __CCD_CORE_SERVICE_HPP__
#define __CCD_CORE_SERVICE_HPP__

//============================================================================
/// @file
/// Concrete implementation of the services defined in ccdi_rpc.proto.
/// Allows you to use #ccd::CCDIService::handleRpc() 
/// to connect a #ProtoChannel to the actual CCDI API.
//============================================================================

#include "ccdi_rpc-server.pb.h"

#include "ccdi.hpp"
#include "vplu_common.h"
#include "log.h"

namespace ccd {

class CCDIServiceImpl : public CCDIService
{
public:
    virtual google::protobuf::int32 AddDataset(const ccd::AddDatasetInput& request, ccd::AddDatasetOutput& response)
    {
        return CCDIAddDataset(request, response);
    }
    virtual google::protobuf::int32 AddSyncSubscription(const ccd::AddSyncSubscriptionInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIAddSyncSubscription(request);
    }
    virtual google::protobuf::int32 DeleteDataset(const ccd::DeleteDatasetInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIDeleteDataset(request);
    }
    virtual google::protobuf::int32 DeleteSyncSubscriptions(const ccd::DeleteSyncSubscriptionsInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIDeleteSyncSubscriptions(request);
    }
    virtual google::protobuf::int32 EventsCreateQueue(const ccd::EventsCreateQueueInput& request, ccd::EventsCreateQueueOutput& response)
    {
        return CCDIEventsCreateQueue(request, response);
    }
    virtual google::protobuf::int32 EventsDestroyQueue(const ccd::EventsDestroyQueueInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIEventsDestroyQueue(request);
    }
    virtual google::protobuf::int32 EventsDequeue(const ccd::EventsDequeueInput& request, ccd::EventsDequeueOutput& response)
    {
        return CCDIEventsDequeue(request, response);
    }
    virtual google::protobuf::int32 GetDatasetDirectoryEntries(const ccd::GetDatasetDirectoryEntriesInput& request, ccd::GetDatasetDirectoryEntriesOutput& response)
    {
        return CCDIGetDatasetDirectoryEntries(request, response);
    }
    virtual google::protobuf::int32 GetInfraHttpInfo(const ccd::GetInfraHttpInfoInput& request, ccd::GetInfraHttpInfoOutput& response)
    {
        return CCDIGetInfraHttpInfo(request, response);
    }
    virtual google::protobuf::int32 GetLocalHttpInfo(const ccd::GetLocalHttpInfoInput& request, ccd::GetLocalHttpInfoOutput& response)
    {
        return CCDIGetLocalHttpInfo(request, response);
    }
    virtual google::protobuf::int32 MSABeginCatalog(const ccd::BeginCatalogInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIMSABeginCatalog(request);
    }
    virtual google::protobuf::int32 MSACommitCatalog(const ccd::CommitCatalogInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIMSACommitCatalog(request);
    }
    virtual google::protobuf::int32 MSAEndCatalog(const ccd::EndCatalogInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIMSAEndCatalog(request);
    }
    virtual google::protobuf::int32 MSABeginMetadataTransaction(const ccd::BeginMetadataTransactionInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIMSABeginMetadataTransaction(request);
    }
    virtual google::protobuf::int32 MSAUpdateMetadata(const ccd::UpdateMetadataInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIMSAUpdateMetadata(request);
    }
    virtual google::protobuf::int32 MSADeleteMetadata(const ccd::DeleteMetadataInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIMSADeleteMetadata(request);
    }
    virtual google::protobuf::int32 MSACommitMetadataTransaction(const ccd::NoParamRequest& request, ccd::NoParamResponse& response)
    {
        UNUSED(request);
        UNUSED(response);
        return CCDIMSACommitMetadataTransaction();
    }
    virtual google::protobuf::int32 MSAGetMetadataSyncState (const ccd::NoParamRequest& request, media_metadata::GetMetadataSyncStateOutput& response)
    {
        UNUSED(request);
        return CCDIMSAGetMetadataSyncState(response);
    }
    virtual google::protobuf::int32 MSADeleteCollection(const ccd::DeleteCollectionInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIMSADeleteCollection(request);
    }
    virtual google::protobuf::int32 MSADeleteCatalog(const ccd::DeleteCatalogInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIMSADeleteCatalog(request);
    }
    virtual google::protobuf::int32 MSAListCollections(const ccd::NoParamRequest& request, media_metadata::ListCollectionsOutput& response)
    {
        UNUSED(request);
        return CCDIMSAListCollections(response);
    }
    virtual google::protobuf::int32 MSAGetCollectionDetails(const ccd::GetCollectionDetailsInput& request, ccd::GetCollectionDetailsOutput& response)
    {
        return CCDIMSAGetCollectionDetails(request, response);
    }
    virtual google::protobuf::int32 MSAGetContentURL(const ccd::MSAGetContentURLInput& request, ccd::MSAGetContentURLOutput& response)
    {
        return CCDIMSAGetContentURL(request, response);
    }
    virtual google::protobuf::int32 MCAQueryMetadataObjects(const ccd::MCAQueryMetadataObjectsInput& request,
                                                            ccd::MCAQueryMetadataObjectsOutput& response)
    {
        return CCDIMCAQueryMetadataObjects(request, response);
    }
    virtual google::protobuf::int32 GetPersonalCloudState(const ccd::GetPersonalCloudStateInput& request, ccd::GetPersonalCloudStateOutput& response)
    {
        return CCDIGetPersonalCloudState(request, response);
    }
    virtual google::protobuf::int32 GetSyncState(const ccd::GetSyncStateInput& request, ccd::GetSyncStateOutput& response)
    {
        return CCDIGetSyncState(request, response);
    }
    virtual google::protobuf::int32 GetSyncStateNotifications(const ccd::GetSyncStateNotificationsInput& request, ccd::GetSyncStateNotificationsOutput& response)
    {
        return CCDIGetSyncStateNotifications(request, response);
    }
    virtual google::protobuf::int32 GetSystemState(const ccd::GetSystemStateInput& request, ccd::GetSystemStateOutput& response)
    {
        return CCDIGetSystemState(request, response);
    }
    virtual google::protobuf::int32 InfraHttpRequest(const ccd::InfraHttpRequestInput& request, ccd::InfraHttpRequestOutput& response)
    {
        return CCDIInfraHttpRequest(request, response);
    }
    virtual google::protobuf::int32 LinkDevice(const ccd::LinkDeviceInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDILinkDevice(request);
    }
    virtual google::protobuf::int32 ListLinkedDevices(const ccd::ListLinkedDevicesInput& request, ccd::ListLinkedDevicesOutput& response)
    {
        return CCDIListLinkedDevices(request, response);
    }
    virtual google::protobuf::int32 ListOwnedDatasets(const ccd::ListOwnedDatasetsInput& request, ccd::ListOwnedDatasetsOutput& response)
    {
        return CCDIListOwnedDatasets(request, response);
    }
    virtual google::protobuf::int32 ListSyncSubscriptions(const ccd::ListSyncSubscriptionsInput& request, ccd::ListSyncSubscriptionsOutput& response)
    {
        return CCDIListSyncSubscriptions(request, response);
    }
    virtual google::protobuf::int32 Login(const ccd::LoginInput& request, ccd::LoginOutput& response)
    {
        return CCDILogin(request, response);
    }
    virtual google::protobuf::int32 Logout(const ccd::LogoutInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDILogout(request);
    }
    virtual google::protobuf::int32 OwnershipSync(const ccd::NoParamRequest& request, ccd::NoParamResponse& response)
    {
        UNUSED(request);
        UNUSED(response);
        return CCDIOwnershipSync();
    }
    virtual google::protobuf::int32 PrivateMsaDataCommit(const ccd::PrivateMsaDataCommitInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIPrivateMsaDataCommit(request);
    }
    virtual google::protobuf::int32 SyncOnce(const ccd::SyncOnceInput& request, ccd::SyncOnceOutput& response)
    {
        return CCDISyncOnce(request, response);
    }
    virtual google::protobuf::int32 RegisterStorageNode(const ccd::RegisterStorageNodeInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIRegisterStorageNode(request);
    }
    virtual google::protobuf::int32 ReportLanDevices(const ccd::ReportLanDevicesInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIReportLanDevices(request);
    }
    virtual google::protobuf::int32 ListLanDevices(const ccd::ListLanDevicesInput& request, ccd::ListLanDevicesOutput& response)
    {
        return CCDIListLanDevices(request, response);
    }
    virtual google::protobuf::int32 ProbeLanDevices(const ccd::NoParamRequest& request, ccd::NoParamResponse& response)
    {
        return CCDIProbeLanDevices();
    }
    virtual google::protobuf::int32 ListStorageNodeDatasets(const ccd::NoParamRequest& request, ccd::ListStorageNodeDatasetsOutput& response)
    {
        return CCDIListStorageNodeDatasets(response);
    }
    virtual google::protobuf::int32 RemoteWakeup(const ccd::RemoteWakeupInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIRemoteWakeup(request);
    }
    virtual google::protobuf::int32 RenameDataset(const ccd::RenameDatasetInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIRenameDataset(request);
    }
    virtual google::protobuf::int32 UnlinkDevice(const ccd::UnlinkDeviceInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIUnlinkDevice(request);
    }
    virtual google::protobuf::int32 UnregisterStorageNode(const ccd::UnregisterStorageNodeInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIUnregisterStorageNode(request);
    }
    virtual google::protobuf::int32 UpdateAppState(const ccd::UpdateAppStateInput& request, ccd::UpdateAppStateOutput& response)
    {
        return CCDIUpdateAppState(request, response);
    }
    virtual google::protobuf::int32 UpdateStorageNode(const ccd::UpdateStorageNodeInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIUpdateStorageNode(request);
    }
    virtual google::protobuf::int32 ListUserStorage(const ccd::ListUserStorageInput& request, ccd::ListUserStorageOutput& response)
    {
        return CCDIListUserStorage(request, response);
    }
    virtual google::protobuf::int32 SWUpdateCheck(const ccd::SWUpdateCheckInput& request, ccd::SWUpdateCheckOutput& response)
    {
        return CCDISWUpdateCheck(request, response);
    }
    virtual google::protobuf::int32 SWUpdateBeginDownload(const ccd::SWUpdateBeginDownloadInput& request, ccd::SWUpdateBeginDownloadOutput& response)
    {
        return CCDISWUpdateBeginDownload(request, response);
    }
    virtual google::protobuf::int32 SWUpdateGetDownloadProgress(const ccd::SWUpdateGetDownloadProgressInput& request, ccd::SWUpdateGetDownloadProgressOutput& response)
    {
        return CCDISWUpdateGetDownloadProgress(request, response);
    }
    virtual google::protobuf::int32 SWUpdateEndDownload(const ccd::SWUpdateEndDownloadInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDISWUpdateEndDownload(request);
    }
    virtual google::protobuf::int32 SWUpdateCancelDownload(const ccd::SWUpdateCancelDownloadInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDISWUpdateCancelDownload(request);
    }
    virtual google::protobuf::int32 SWUpdateSetCcdVersion(const ccd::SWUpdateSetCcdVersionInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDISWUpdateSetCcdVersion(request);
    }
    virtual google::protobuf::int32 UpdateSyncSettings(const ccd::UpdateSyncSettingsInput& request, ccd::UpdateSyncSettingsOutput& response)
    {
        return CCDIUpdateSyncSettings(request, response);
    }
    virtual google::protobuf::int32 UpdateSyncSubscription(const ccd::UpdateSyncSubscriptionInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIUpdateSyncSubscription(request);
    }
    virtual google::protobuf::int32 UpdateSystemState(const ccd::UpdateSystemStateInput& request, ccd::UpdateSystemStateOutput& response)
    {
        return CCDIUpdateSystemState(request, response);
    }
    virtual google::protobuf::int32 RemoteSwUpdateMessage(const ccd::RemoteSwUpdateMessageInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIRemoteSwUpdateMessage(request);
    }
	#if (defined LINUX || defined __CLOUDNODE__) && !defined LINUX_EMB
    virtual google::protobuf::int32 EnableInMemoryLogging(const ccd::NoParamRequest& request, ccd::NoParamResponse& response)
    {
        UNUSED(request);
        UNUSED(response);
        return CCDIEnableInMemoryLogging();
    }
    virtual google::protobuf::int32 DisableInMemoryLogging(const ccd::NoParamRequest& request, ccd::NoParamResponse& response)
    {
        UNUSED(request);
        UNUSED(response);
        return CCDIDisableInMemoryLogging();
    }
	#endif
    virtual google::protobuf::int32 FlushInMemoryLogs(const ccd::NoParamRequest& request, ccd::NoParamResponse& response)
    {
        UNUSED(request);
        UNUSED(response);
        return CCDIFlushInMemoryLogs();
    }
    virtual google::protobuf::int32 RespondToPairingRequest(const ccd::RespondToPairingRequestInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIRespondToPairingRequest(request);
    }
    virtual google::protobuf::int32 RequestPairing(const ccd::RequestPairingInput& request, ccd::RequestPairingOutput& response)
    {
        return CCDIRequestPairing(request, response);
    }
    virtual google::protobuf::int32 RequestPairingPin(const ccd::RequestPairingPinInput& request, ccd::RequestPairingPinOutput& response)
    {
        return CCDIRequestPairingPin(request, response);
    }
    virtual google::protobuf::int32 GetPairingStatus(const ccd::GetPairingStatusInput& request, ccd::GetPairingStatusOutput& response)
    {
        return CCDIGetPairingStatus(request, response);
    }

    virtual google::protobuf::int32 QueryPicStreamObjects(const ccd::CCDIQueryPicStreamObjectsInput& request, ccd::CCDIQueryPicStreamObjectsOutput& response)
    {
        return CCDIQueryPicStreamObjects(request, response);
    }
    virtual google::protobuf::int32 SharedFilesStoreFile(const ccd::SharedFilesStoreFileInput& request, ccd::SharedFilesStoreFileOutput& response)
    {
        return CCDISharedFilesStoreFile(request, response);
    }
    virtual google::protobuf::int32 SharedFilesShareFile(const ccd::SharedFilesShareFileInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDISharedFilesShareFile(request);
    }
    virtual google::protobuf::int32 SharedFilesUnshareFile(const ccd::SharedFilesUnshareFileInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDISharedFilesUnshareFile(request);
    }
    virtual google::protobuf::int32 SharedFilesDeleteSharedFile(const ccd::SharedFilesDeleteSharedFileInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDISharedFilesDeleteSharedFile(request);
    }
    virtual google::protobuf::int32 SharedFilesQuery(const ccd::SharedFilesQueryInput& request, ccd::SharedFilesQueryOutput& response)
    {
        return CCDISharedFilesQuery(request, response);
    }
    virtual google::protobuf::int32 RegisterRemoteExecutable(const ccd::RegisterRemoteExecutableInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIRegisterRemoteExecutable(request);
    }
    virtual google::protobuf::int32 UnregisterRemoteExecutable(const ccd::UnregisterRemoteExecutableInput& request, ccd::NoParamResponse& response)
    {
        UNUSED(response);
        return CCDIUnregisterRemoteExecutable(request);
    }
    virtual google::protobuf::int32 ListRegisteredRemoteExecutables(const ccd::ListRegisteredRemoteExecutablesInput& request, ccd::ListRegisteredRemoteExecutablesOutput& response)
    {
        return CCDIListRegisteredRemoteExecutables(request, response);
    }
};

/// For logging Protobuf RPCs without exposing passwords in the logs.
void CCDProtobufRpcDebugGenericCallback(const char* msg);
void CCDProtobufRpcDebugRequestCallback(const std::string& methodName,
                                        bool isValid,
                                        const ProtoMessage& reqMsg);
void CCDProtobufRpcDebugResponseCallback(const std::string& methodName,
                                         const RpcStatus& status,
                                         const ProtoMessage* respMsg);
} // end namespace ccd

#endif // include guard
