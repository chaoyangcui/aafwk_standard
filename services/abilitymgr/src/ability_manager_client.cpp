/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ability_manager_client.h"

#include "string_ex.h"

#include "ability_manager_interface.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AAFwk {
std::shared_ptr<AbilityManagerClient> AbilityManagerClient::instance_ = nullptr;
std::mutex AbilityManagerClient::mutex_;

std::shared_ptr<AbilityManagerClient> AbilityManagerClient::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<AbilityManagerClient>();
        }
    }
    return instance_;
}

AbilityManagerClient::AbilityManagerClient()
{}

AbilityManagerClient::~AbilityManagerClient()
{}

ErrCode AbilityManagerClient::AttachAbilityThread(
    const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            return ABILITY_SERVICE_NOT_CONNECTED;
        }
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->AttachAbilityThread(scheduler, token);
}

ErrCode AbilityManagerClient::AbilityTransitionDone(const sptr<IRemoteObject> &token, int state)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->AbilityTransitionDone(token, state);
}

ErrCode AbilityManagerClient::ScheduleConnectAbilityDone(
    const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->ScheduleConnectAbilityDone(token, remoteObject);
}

ErrCode AbilityManagerClient::ScheduleDisconnectAbilityDone(const sptr<IRemoteObject> &token)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->ScheduleDisconnectAbilityDone(token);
}

ErrCode AbilityManagerClient::ScheduleCommandAbilityDone(const sptr<IRemoteObject> &token)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not command", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->ScheduleCommandAbilityDone(token);
}

void AbilityManagerClient::AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    abms->AddWindowInfo(token, windowToken);
}

ErrCode AbilityManagerClient::StartAbility(const Want &want, int requestCode)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->StartAbility(want, requestCode);
}

ErrCode AbilityManagerClient::StartAbility(const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->StartAbility(want, callerToken, requestCode);
}

ErrCode AbilityManagerClient::TerminateAbility(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->TerminateAbility(token, resultCode, resultWant);
}

ErrCode AbilityManagerClient::TerminateAbility(const sptr<IRemoteObject> &callerToken, int requestCode)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->TerminateAbility(callerToken, requestCode);
}

ErrCode AbilityManagerClient::TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->TerminateAbilityResult(token, startId);
}

ErrCode AbilityManagerClient::ConnectAbility(
    const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->ConnectAbility(want, connect, callerToken);
}

ErrCode AbilityManagerClient::DisconnectAbility(const sptr<IAbilityConnection> &connect)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->DisconnectAbility(connect);
}

sptr<IAbilityScheduler> AbilityManagerClient::AcquireDataAbility(
    const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return nullptr;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->AcquireDataAbility(uri, tryBind, callerToken);
}

ErrCode AbilityManagerClient::ReleaseDataAbility(
    sptr<IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject> &callerToken)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->ReleaseDataAbility(dataAbilityScheduler, callerToken);
}

ErrCode AbilityManagerClient::DumpState(const std::string &args, std::vector<std::string> &state)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    abms->DumpState(args, state);
    return ERR_OK;
}

ErrCode AbilityManagerClient::Connect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (remoteObject_ != nullptr) {
        return ERR_OK;
    }
    sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemManager == nullptr) {
        HILOG_ERROR("%{private}s:fail to get Registry", __func__);
        return GET_ABILITY_SERVICE_FAILED;
    }
    remoteObject_ = systemManager->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:fail to connect AbilityManagerService", __func__);
        return GET_ABILITY_SERVICE_FAILED;
    }
    HILOG_DEBUG("connect AbilityManagerService success");
    return ERR_OK;
}

ErrCode AbilityManagerClient::GetAllStackInfo(StackInfo &stackInfo)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }

    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->GetAllStackInfo(stackInfo);
}

ErrCode AbilityManagerClient::StopServiceAbility(const Want &want)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->StopServiceAbility(want);
}

ErrCode AbilityManagerClient::GetRecentMissions(
    const int32_t numMax, const int32_t flags, std::vector<AbilityMissionInfo> &recentList)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->GetRecentMissions(numMax, flags, recentList);
}

ErrCode AbilityManagerClient::GetMissionSnapshot(const int32_t missionId, MissionSnapshotInfo &snapshot)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->GetMissionSnapshot(missionId, snapshot);
}

ErrCode AbilityManagerClient::MoveMissionToTop(int32_t missionId)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->MoveMissionToTop(missionId);
}

ErrCode AbilityManagerClient::MoveMissionToEnd(const sptr<IRemoteObject> &token, const bool nonFirst)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->MoveMissionToEnd(token, nonFirst);
}

ErrCode AbilityManagerClient::RemoveMissions(std::vector<int> missionId)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }

    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    int error = ERR_OK;
    for (auto it : missionId) {
        error = abms->RemoveMission(it);
        if (error != ERR_OK) {
            HILOG_ERROR("%{private}s failed, error:%{private}d", __func__, error);
            break;
        }
    }

    return error;
}

ErrCode AbilityManagerClient::RemoveStack(int id)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->RemoveStack(id);
}

ErrCode AbilityManagerClient::KillProcess(const std::string &bundleName)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->KillProcess(bundleName);
}

ErrCode AbilityManagerClient::IsFirstInMission(const sptr<IRemoteObject> &token)
{
    if (token == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ERR_NULL_OBJECT;
    }
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    if (!(abms->IsFirstInMission(token))) {
        return NO_FIRST_IN_MISSION;
    }
    return ERR_OK;
}

ErrCode AbilityManagerClient::CompelVerifyPermission(
    const std::string &permission, int pid, int uid, std::string &message)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->CompelVerifyPermission(permission, pid, uid, message);
}

ErrCode AbilityManagerClient::PowerOff()
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->PowerOff();
}

ErrCode AbilityManagerClient::PowerOn()
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->PowerOn();
}

ErrCode AbilityManagerClient::LockMission(int missionId)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->LockMission(missionId);
}

ErrCode AbilityManagerClient::UnlockMission(int missionId)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->UnlockMission(missionId);
}

ErrCode AbilityManagerClient::SetMissionDescriptionInfo(
    const sptr<IRemoteObject> &token, const MissionDescriptionInfo &missionDescriptionInfo)
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->SetMissionDescriptionInfo(token, missionDescriptionInfo);
}

int AbilityManagerClient::GetMissionLockModeState()
{
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->GetMissionLockModeState();
}

sptr<IWantSender> AbilityManagerClient::GetWantSender(
    const WantSenderInfo &wantSenderInfo, const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s:begin.", __func__);

    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return nullptr;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->GetWantSender(wantSenderInfo, callerToken);
}

ErrCode AbilityManagerClient::SendWantSender(const sptr<IWantSender> &target, const SenderInfo &senderInfo)
{
    HILOG_INFO("%{public}s:begin.", __func__);

    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->SendWantSender(target, senderInfo);
}

void AbilityManagerClient::CancelWantSender(const sptr<IWantSender> &sender)
{
    HILOG_INFO("%{public}s:begin.", __func__);

    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    abms->CancelWantSender(sender);
}

ErrCode AbilityManagerClient::GetPendingWantUid(const sptr<IWantSender> &target, int32_t &uid)
{
    HILOG_INFO("%{public}s:begin.", __func__);

    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    if (target == nullptr) {
        HILOG_ERROR("%{private}s:target is nullptr", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    uid = abms->GetPendingWantUid(target);
    return ERR_OK;
}

ErrCode AbilityManagerClient::GetPendingWantUserId(const sptr<IWantSender> &target, int32_t &userId)
{
    HILOG_INFO("%{public}s:begin.", __func__);

    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    if (target == nullptr) {
        HILOG_ERROR("%{private}s:target is nullptr", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    userId = abms->GetPendingWantUserId(target);
    return ERR_OK;
}

ErrCode AbilityManagerClient::GetPendingWantBundleName(const sptr<IWantSender> &target, std::string &bundleName)
{
    HILOG_INFO("%{public}s:begin.", __func__);

    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    if (target == nullptr) {
        HILOG_ERROR("%{private}s:target is nullptr", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    bundleName = abms->GetPendingWantBundleName(target);
    return ERR_OK;
}

ErrCode AbilityManagerClient::GetPendingWantCode(const sptr<IWantSender> &target, int32_t &code)
{
    HILOG_INFO("%{public}s:begin.", __func__);

    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    if (target == nullptr) {
        HILOG_ERROR("%{private}s:target is nullptr", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    code = abms->GetPendingWantCode(target);
    return ERR_OK;
}

ErrCode AbilityManagerClient::GetPendingWantType(const sptr<IWantSender> &target, int32_t &type)
{
    HILOG_INFO("%{public}s:begin.", __func__);

    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    if (target == nullptr) {
        HILOG_ERROR("%{private}s:target is nullptr", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    type = abms->GetPendingWantType(target);
    return ERR_OK;
}

void AbilityManagerClient::RegisterCancelListener(const sptr<IWantSender> &sender, const sptr<IWantReceiver> &recevier)
{
    HILOG_INFO("%{public}s:begin.", __func__);

    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return;
    }
    if (sender == nullptr) {
        HILOG_ERROR("%{private}s:sender is nullptr", __func__);
        return;
    }
    if (recevier == nullptr) {
        HILOG_ERROR("%{private}s:recevier is nullptr", __func__);
        return;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    abms->RegisterCancelListener(sender, recevier);
}

void AbilityManagerClient::UnregisterCancelListener(
    const sptr<IWantSender> &sender, const sptr<IWantReceiver> &recevier)
{
    HILOG_INFO("%{public}s:begin.", __func__);

    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return;
    }
    if (sender == nullptr) {
        HILOG_ERROR("%{private}s:sender is nullptr", __func__);
        return;
    }
    if (recevier == nullptr) {
        HILOG_ERROR("%{private}s:recevier is nullptr", __func__);
        return;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    abms->UnregisterCancelListener(sender, recevier);
}

ErrCode AbilityManagerClient::GetPendingRequestWant(const sptr<IWantSender> &target, std::shared_ptr<Want> &want)
{
    HILOG_INFO("%{public}s:begin.", __func__);

    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:ability service not connect", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    if (target == nullptr) {
        HILOG_ERROR("%{private}s:target is nullptr", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    if (want == nullptr) {
        HILOG_ERROR("%{private}s:want is nullptr", __func__);
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    sptr<IAbilityManager> abms = iface_cast<IAbilityManager>(remoteObject_);
    return abms->GetPendingRequestWant(target, want);
}

}  // namespace AAFwk
}  // namespace OHOS