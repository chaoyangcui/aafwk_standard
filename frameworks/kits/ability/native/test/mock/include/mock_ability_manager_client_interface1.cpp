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

#include <singleton.h>
#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include "string_ex.h"

#include "ability_manager_interface.h"
#include "ability_context.h"

#include "mock_ability_manager_client_interface1.h"

namespace OHOS {
namespace AAFwk {
MockAbilityManagerClient::MockAbilityManagerClient()
{
    startAbility_ = ERR_INVALID_OPERATION;
    terminateAbility_ = ERR_INVALID_OPERATION;
    terminateAbilityResult_ = ERR_INVALID_OPERATION;
    isFirstInMission_ = ERR_INVALID_OPERATION;
    removeMissionsE_ = ERR_INVALID_OPERATION;
    terminateAbilityValue_ = 0;
    removeMissions_ = 0;
    missionId_ = -1;
    moveMissionToEnd_ = false;
}
MockAbilityManagerClient::~MockAbilityManagerClient()
{}

std::shared_ptr<MockAbilityManagerClient> MockAbilityManagerClient::mock_instance_ = nullptr;
bool MockAbilityManagerClient::mock_intanceIsNull_ = true;

std::shared_ptr<MockAbilityManagerClient> MockAbilityManagerClient::GetInstance()
{
    if (mock_instance_ == nullptr) {
        mock_instance_ = std::make_shared<MockAbilityManagerClient>();
    }

    return mock_instance_;
}

void MockAbilityManagerClient::SetInstanceNull(bool flag)
{
    mock_intanceIsNull_ = flag;
}

std::shared_ptr<AbilityManagerClient> AbilityManagerClient::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = MockAbilityManagerClient::GetInstance();
        }
    }
    if (MockAbilityManagerClient::mock_intanceIsNull_)
        return instance_;
    else
        return nullptr;
}

AbilityManagerClient::~AbilityManagerClient()
{}

ErrCode AbilityManagerClient::AttachAbilityThread(
    const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::AbilityTransitionDone(const sptr<IRemoteObject> &token, int state)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::ScheduleConnectAbilityDone(
    const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::ScheduleDisconnectAbilityDone(const sptr<IRemoteObject> &token)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::ScheduleCommandAbilityDone(const sptr<IRemoteObject> &token)
{
    return ERR_OK;
}

void AbilityManagerClient::AddWindowInfo(const sptr<IRemoteObject> &token, int32_t windowToken)
{
    return;
}

ErrCode AbilityManagerClient::TerminateAbility(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::ConnectAbility(
    const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::DisconnectAbility(const sptr<IAbilityConnection> &connect)
{
    return ERR_OK;
}

sptr<IAbilityScheduler> AbilityManagerClient::AcquireDataAbility(
    const Uri &uri, bool tryBind, const sptr<IRemoteObject> &callerToken)
{
    return nullptr;
}

ErrCode AbilityManagerClient::ReleaseDataAbility(
    sptr<IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject> &callerToken)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::DumpState(const std::string &args, std::vector<std::string> &state)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::Connect()
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::GetAllStackInfo(StackInfo &stackInfo)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::StopServiceAbility(const Want &want)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::GetRecentMissions(
    const int32_t numMax, const int32_t flags, std::vector<AbilityMissionInfo> &recentList)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::GetMissionSnapshot(const int32_t missionId, MissionSnapshotInfo &snapshot)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::MoveMissionToTop(int32_t missionId)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::RemoveMissions(std::vector<int> missionId)
{
    ErrCode ret = MockAbilityManagerClient::GetInstance()->GetRemoveMissions();
    if (ret == ERR_OK) {
        MockAbilityManagerClient::GetInstance()->ChangeRemoveMissionsValue();
    }
    return ret;
}

ErrCode AbilityManagerClient::RemoveStack(int id)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::KillProcess(const std::string &bundleName)
{
    return ERR_OK;
}

ErrCode AbilityManagerClient::StartAbility(const Want &want, int requestCode)
{
    return MockAbilityManagerClient::GetInstance()->GetStartAbility();
}

ErrCode AbilityManagerClient::StartAbility(const Want &want, const sptr<IRemoteObject> &callerToken, int requestCode)
{
    return MockAbilityManagerClient::GetInstance()->GetStartAbility();
}

ErrCode AbilityManagerClient::TerminateAbility(const sptr<IRemoteObject> &callerToken, int requestCode)
{
    MockAbilityManagerClient::GetInstance()->SetTerminateAbilityValue(requestCode);
    return MockAbilityManagerClient::GetInstance()->GetTerminateAbility();
}

ErrCode AbilityManagerClient::TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId)
{
    return MockAbilityManagerClient::GetInstance()->GetTerminateAbilityResult();
}

ErrCode AbilityManagerClient::IsFirstInMission(const sptr<IRemoteObject> &token)
{
    return MockAbilityManagerClient::GetInstance()->GetIsFirstInMission();
}

ErrCode AbilityManagerClient::MoveMissionToEnd(const sptr<IRemoteObject> &token, const bool nonFirst)
{
    MockAbilityManagerClient::GetInstance()->SetMockMoveMissionToEnd(
        !MockAbilityManagerClient::GetInstance()->GetMockMoveMissionToEnd());
    return ERR_OK;
}

ErrCode AbilityManagerClient::LockMission(int missionId)
{
    MockAbilityManagerClient::GetInstance()->SetMockMissionId(missionId);
    return ERR_OK;
}

ErrCode AbilityManagerClient::UnlockMission(int missionId)
{
    MockAbilityManagerClient::GetInstance()->SetMockMissionId(missionId);
    return ERR_OK;
}

ErrCode AbilityManagerClient::SetMissionDescriptionInfo(
        const sptr<IRemoteObject> &token, const MissionDescriptionInfo &missionDescriptionInfo)
{
    return ERR_OK;
}

ErrCode MockAbilityManagerClient::GetStartAbility()
{
    return startAbility_;
}
ErrCode MockAbilityManagerClient::GetTerminateAbility()
{
    return terminateAbility_;
}
ErrCode MockAbilityManagerClient::GetTerminateAbilityResult()
{
    return terminateAbilityResult_;
}
ErrCode MockAbilityManagerClient::GetIsFirstInMission()
{
    return isFirstInMission_;
}
ErrCode MockAbilityManagerClient::GetRemoveMissions()
{
    return removeMissionsE_;
}

void MockAbilityManagerClient::SetStartAbility(ErrCode tValue)
{
    startAbility_ = tValue;
}
void MockAbilityManagerClient::SetTerminateAbility(ErrCode tValue)
{
    terminateAbility_ = tValue;
}
void MockAbilityManagerClient::SetTerminateAbilityResult(ErrCode tValue)
{
    terminateAbilityResult_ = tValue;
}
void MockAbilityManagerClient::SetIsFirstInMission(ErrCode tValue)
{
    isFirstInMission_ = tValue;
}
void MockAbilityManagerClient::SetRemoveMissions(ErrCode tValue)
{
    removeMissionsE_ = tValue;
}

int MockAbilityManagerClient::GetTerminateAbilityValue()
{
    return terminateAbilityValue_;
}
void MockAbilityManagerClient::SetTerminateAbilityValue(int nValue)
{
    terminateAbilityValue_ = nValue;
}
int MockAbilityManagerClient::GetRemoveMissionsValue()
{
    return removeMissions_;
}
void MockAbilityManagerClient::ChangeRemoveMissionsValue()
{
    ++removeMissions_;
}

int MockAbilityManagerClient::GetMockMissionId()
{
    return missionId_;
}
void MockAbilityManagerClient::SetMockMissionId(int missionId)
{
    missionId_ = missionId;
}

bool MockAbilityManagerClient::GetMockMoveMissionToEnd()
{
    return moveMissionToEnd_;
}
void MockAbilityManagerClient::SetMockMoveMissionToEnd(bool flag)
{
    moveMissionToEnd_ = flag;
}

}  // namespace AAFwk
}  // namespace OHOS

namespace OHOS {
namespace AppExecFwk {

MockAbilityContextDeal::MockAbilityContextDeal()
{
    hapModInfo_ = nullptr;
}
MockAbilityContextDeal::~MockAbilityContextDeal()
{}

std::shared_ptr<AppExecFwk::HapModuleInfo> MockAbilityContextDeal::GetHapModuleInfo()
{
    if (hapModInfo_ == nullptr) {
        hapModInfo_ = std::make_shared<AppExecFwk::HapModuleInfo>();
    }
    return hapModInfo_;
}

MockAbilityContextTest::MockAbilityContextTest()
{
    startAbilityRunCount_ = 0;
}
MockAbilityContextTest::~MockAbilityContextTest()
{}

void MockAbilityContextTest::StartAbility(const AAFwk::Want &want, int requestCode)
{
    ++startAbilityRunCount_;
}

size_t MockAbilityContextTest::GetStartAbilityRunCount()
{
    return startAbilityRunCount_;
}
void MockAbilityContextTest::SetStartAbilityRunCount(size_t nCount)
{
    startAbilityRunCount_ = nCount;
}

void MockAbilityContextTest::SetToken(const sptr<IRemoteObject> token)
{
    token_ = token;
}

}  // namespace AppExecFwk
}  // namespace OHOS
