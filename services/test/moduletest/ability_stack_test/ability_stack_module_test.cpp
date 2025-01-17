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

#include <gtest/gtest.h>

#define private public
#define protected public
#include "ability_stack_manager.h"
#include "ability_manager_errors.h"
#include "ability_event_handler.h"
#include "mock_ability_scheduler.h"
#include "mock_app_mgr_client.h"
#include "mock_app_scheduler.h"
#include "ability_manager_service.h"
#undef private
#undef protected

#include "mock_bundle_mgr.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace testing;

namespace OHOS {
namespace AAFwk {
class AbilityStackModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    Want CreateWant(const std::string &entity);
    AbilityInfo CreateAbilityInfo(const std::string &name, const std::string &appName, const std::string &bundleName);
    ApplicationInfo CreateAppInfo(const std::string &appName, const std::string &bundleName);
    AbilityRequest GenerateAbilityRequest(const std::string &deviceName, const std::string &abilityName,
        const std::string &appName, const std::string &bundleName);
    void makeScene(const std::string &abilityName, const std::string &bundleName, AbilityInfo &abilityInfo, Want &want);

public:
    std::shared_ptr<AbilityStackManager> stackManager_;
    BundleMgrService *bundleObject_ = nullptr;
};

void AbilityStackModuleTest::SetUpTestCase(void)
{}

void AbilityStackModuleTest::TearDownTestCase(void)
{}

void AbilityStackModuleTest::SetUp(void)
{
    bundleObject_ = new BundleMgrService();
    OHOS::DelayedSingleton<SaMgrClient>::GetInstance()->RegisterSystemAbility(
        OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject_);

    auto ams = DelayedSingleton<AbilityManagerService>::GetInstance();
    // stackManager_ = ams->GetStackManager();
    stackManager_ = std::make_shared<AbilityStackManager>(10);
    auto bms = ams->GetBundleManager();
    ams->OnStart();
    EXPECT_NE(bms, nullptr);
}

void AbilityStackModuleTest::TearDown(void)
{
    auto ams = DelayedSingleton<AbilityManagerService>::GetInstance();
    OHOS::DelayedSingleton<AbilityManagerService>::DestroyInstance();
}

AbilityRequest AbilityStackModuleTest::GenerateAbilityRequest(const std::string &deviceName,
    const std::string &abilityName, const std::string &appName, const std::string &bundleName)
{
    ElementName element(deviceName, abilityName, bundleName);
    Want want;
    want.SetElement(element);

    AbilityInfo abilityInfo;
    ApplicationInfo appinfo;

    abilityInfo.name = abilityName;
    abilityInfo.bundleName = bundleName;
    abilityInfo.applicationName = appName;
    abilityInfo.applicationInfo.bundleName = bundleName;
    abilityInfo.applicationInfo.name = appName;

    makeScene(abilityName, bundleName, abilityInfo, want);

    appinfo = abilityInfo.applicationInfo;
    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = abilityInfo;
    abilityRequest.appInfo = appinfo;

    return abilityRequest;
}

void AbilityStackModuleTest::makeScene(
    const std::string &abilityName, const std::string &bundleName, AbilityInfo &abilityInfo, Want &want)
{
    if (bundleName == "com.ix.hiworld") {
        std::string entity = Want::ENTITY_HOME;
        want.AddEntity(entity);
        abilityInfo.type = AbilityType::PAGE;
        abilityInfo.applicationInfo.isLauncherApp = true;
        abilityInfo.process = "p";
        abilityInfo.launchMode = LaunchMode::SINGLETON;
    }

    if (bundleName == "com.ix.hiMusic") {
        abilityInfo.type = AbilityType::PAGE;
        abilityInfo.applicationInfo.isLauncherApp = false;

        if (abilityName == "MusicAbility" || abilityName == "MusicAbility2th") {
            abilityInfo.process = "p1";
            abilityInfo.launchMode = LaunchMode::STANDARD;
        }
        if (abilityName == "MusicTopAbility") {
            abilityInfo.process = "p1";
            abilityInfo.launchMode = LaunchMode::SINGLETOP;
        }
        if (abilityName == "MusicSAbility") {
            abilityInfo.process = "p2";
            abilityInfo.launchMode = LaunchMode::SINGLETON;
        }
    }

    if (bundleName == "com.ix.hiRadio") {
        abilityInfo.type = AbilityType::PAGE;
        abilityInfo.process = "p3";
        if (abilityName == "RadioAbility") {
            abilityInfo.launchMode = LaunchMode::STANDARD;
        }
        if (abilityName == "RadioTopAbility") {
            abilityInfo.launchMode = LaunchMode::SINGLETON;
        }
    }
}

Want AbilityStackModuleTest::CreateWant(const std::string &entity)
{
    Want want;
    if (!entity.empty()) {
        want.AddEntity(entity);
    }
    return want;
}

AbilityInfo AbilityStackModuleTest::CreateAbilityInfo(
    const std::string &name, const std::string &appName, const std::string &bundleName)
{
    AbilityInfo abilityInfo;
    abilityInfo.name = name;
    abilityInfo.applicationName = appName;
    abilityInfo.bundleName = bundleName;
    return abilityInfo;
}

ApplicationInfo AbilityStackModuleTest::CreateAppInfo(const std::string &appName, const std::string &bundleName)
{
    ApplicationInfo appInfo;
    appInfo.name = appName;
    appInfo.bundleName = bundleName;

    return appInfo;
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: start launcher ability
 * FunctionPoints: launcher mission stack
 * EnvConditions: NA
 * CaseDescription: start launcher ability when current launcher mission stack empty.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_001 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityInfo abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    ApplicationInfo appInfo = CreateAppInfo(bundleName, bundleName);
    Want want = CreateWant(Want::ENTITY_HOME);

    AbilityRequest abilityRequest;
    abilityRequest.want = want;
    abilityRequest.abilityInfo = abilityInfo;
    abilityRequest.appInfo = appInfo;
    abilityRequest.appInfo.isLauncherApp = true;  // launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    ASSERT_TRUE(curMissionStack);
    stackManager_->StartAbility(abilityRequest);
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    EXPECT_TRUE(curMissionStack->GetTopMissionRecord() != nullptr);
    EXPECT_EQ(1, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    EXPECT_STREQ(abilityName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().name.c_str());
    EXPECT_STREQ(bundleName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().bundleName.c_str());

    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_001 end";
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: start launcher ability
 * FunctionPoints: launcher mission stack
 * EnvConditions: NA
 * CaseDescription: not add new ability to current mission record when start the same launcher ability twice.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_002 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant(Want::ENTITY_HOME);
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);
    abilityRequest.appInfo.isLauncherApp = true;  // launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    stackManager_->missionStackList_.clear();
    EXPECT_EQ(true, stackManager_->waittingAbilityQueue_.empty());
    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();

    int result = stackManager_->StartAbility(abilityRequest);
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    EXPECT_EQ(1, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    usleep(1000);
    result = stackManager_->StartAbility(abilityRequest);  // same launcher ability
    // not change current mission stack
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    // not add new mission to current mission stack
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    // not add new ability to current mission record
    EXPECT_EQ(1, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    EXPECT_STREQ(abilityName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().name.c_str());
    EXPECT_STREQ(bundleName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().bundleName.c_str());
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_002 end";
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: start launcher ability
 * FunctionPoints: launcher mission stack
 * EnvConditions: NA
 * CaseDescription: add new mission to current mission stack whene start the different launcher ability.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_003 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";
    int index = 1;

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant(Want::ENTITY_HOME);
    std::string strInd = std::to_string(index);
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName + strInd, bundleName + strInd, bundleName + strInd);
    abilityRequest.appInfo = CreateAppInfo(bundleName + strInd, bundleName + strInd);
    abilityRequest.appInfo.isLauncherApp = true;  // launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    AbilityRequest abilityRequest2;
    abilityRequest2.want = CreateWant(Want::ENTITY_HOME);
    index++;
    strInd = std::to_string(index);
    abilityRequest2.abilityInfo = CreateAbilityInfo(abilityName + strInd, bundleName + strInd, bundleName + strInd);
    abilityRequest2.appInfo = CreateAppInfo(bundleName + strInd, bundleName + strInd);
    abilityRequest2.appInfo.isLauncherApp = true;  // another launcher ability
    abilityRequest2.abilityInfo.applicationInfo = abilityRequest2.appInfo;

    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    int result = stackManager_->StartAbility(abilityRequest);
    usleep(1000);
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    curMissionStack->GetTopAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    result = stackManager_->StartAbility(abilityRequest2);
    // not change current mission stack
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    // add new mission to current mission stack
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    EXPECT_EQ(2, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    EXPECT_STREQ((abilityName + strInd).c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().name.c_str());
    EXPECT_STREQ(
        (bundleName + strInd).c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().bundleName.c_str());
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_003 end";
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: start non-launcher ability
 * FunctionPoints: non-launcher mission stack
 * EnvConditions: NA
 * CaseDescription: change current mission stack when start non-launcher ability.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_004 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";
    int index = 1;

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant(Want::ENTITY_HOME);
    std::string strInd = std::to_string(index);
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName + strInd, bundleName + strInd, bundleName + strInd);
    abilityRequest.appInfo = CreateAppInfo(bundleName + strInd, bundleName + strInd);
    abilityRequest.appInfo.isLauncherApp = true;  // launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    AbilityRequest abilityRequest2;
    abilityRequest2.want = CreateWant("");
    strInd = std::to_string(++index);
    abilityRequest2.abilityInfo = CreateAbilityInfo(abilityName + strInd, bundleName + strInd, bundleName + strInd);
    abilityRequest2.appInfo = CreateAppInfo(bundleName + strInd, bundleName + strInd);
    abilityRequest2.appInfo.isLauncherApp = false;  // non-launcher ability
    abilityRequest2.abilityInfo.applicationInfo = abilityRequest2.appInfo;

    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    int result = stackManager_->StartAbility(abilityRequest);
    usleep(1000);
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    curMissionStack->GetTopAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    result = stackManager_->StartAbility(abilityRequest2);
    curMissionStack = stackManager_->GetCurrentMissionStack();
    // change current mission stack
    EXPECT_EQ(AbilityStackManager::DEFAULT_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    // add new mission to current mission stack
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    EXPECT_EQ(1, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    EXPECT_STREQ((abilityName + strInd).c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().name.c_str());
    EXPECT_STREQ(
        (bundleName + strInd).c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().bundleName.c_str());
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_004 end";
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: terminate launcher ability
 * FunctionPoints: launcher mission stack
 * EnvConditions: NA
 * CaseDescription: verify unique launcher ability can't be terminated.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_005 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant(Want::ENTITY_HOME);
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);
    abilityRequest.appInfo.isLauncherApp = true;  // launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    int result = stackManager_->StartAbility(abilityRequest);
    usleep(1000);
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    curMissionStack->GetTopAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    result = stackManager_->TerminateAbility(curMissionStack->GetTopAbilityRecord()->GetToken(), -1, nullptr);
    EXPECT_EQ(TERMINATE_LAUNCHER_DENIED, result);
    // not change current mission stack
    EXPECT_EQ(AbilityStackManager::LAUNCHER_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    EXPECT_EQ(1, curMissionStack->GetMissionRecordCount());
    // not change current ability state
    EXPECT_TRUE(curMissionStack->GetTopAbilityRecord() != nullptr);
    EXPECT_EQ(OHOS::AAFwk::AbilityState::ACTIVE, curMissionStack->GetTopAbilityRecord()->GetAbilityState());
    EXPECT_STREQ(abilityName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().name.c_str());
    EXPECT_STREQ(bundleName.c_str(), curMissionStack->GetTopAbilityRecord()->GetAbilityInfo().bundleName.c_str());

    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_005 end";
}

/*
 * Feature: AaFwk
 * Function: ability stack management
 * SubFunction: start and terminate non-launcher ability
 * FunctionPoints: single top
 * EnvConditions: NA
 * CaseDescription: start and terminate multiple non-launcher ability(single application).
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_006 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    std::string appSuffix = "1";
    std::vector<AbilityRequest> abilityRequests;
    for (int i = 0; i < 3; i++) {
        AbilityRequest abilityRequest;
        abilityRequest.want = CreateWant("");
        std::string abilitySuffix = appSuffix + std::to_string(i + 1);
        abilityRequest.abilityInfo =
            CreateAbilityInfo(abilityName + abilitySuffix, bundleName + appSuffix, bundleName + appSuffix);
        abilityRequest.abilityInfo.launchMode = LaunchMode::STANDARD;
        abilityRequest.appInfo = CreateAppInfo(bundleName + appSuffix, bundleName + appSuffix);
        abilityRequest.appInfo.isLauncherApp = false;  // non-launcher ability
        abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

        abilityRequests.push_back(abilityRequest);
    }
    stackManager_->Init();
    // start "ability_name11"
    int result = stackManager_->StartAbility(abilityRequests[0]);
    usleep(1000);
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    EXPECT_EQ(AbilityStackManager::DEFAULT_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    std::shared_ptr<AbilityRecord> currentTopAbilityRecord = stackManager_->GetCurrentTopAbility();
    EXPECT_STREQ("ability_name11", currentTopAbilityRecord->GetAbilityInfo().name.c_str());
    currentTopAbilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);

    // start "ability_name12"
    result = stackManager_->StartAbility(abilityRequests[1]);
    usleep(1000);
    EXPECT_EQ(AbilityStackManager::DEFAULT_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    currentTopAbilityRecord = stackManager_->GetCurrentTopAbility();
    EXPECT_STREQ("ability_name12", currentTopAbilityRecord->GetAbilityInfo().name.c_str());
    currentTopAbilityRecord->GetPreAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);
    currentTopAbilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(2, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());

    // start "ability_name12"
    result = stackManager_->StartAbility(abilityRequests[1]);
    usleep(1000);
    currentTopAbilityRecord = stackManager_->GetCurrentTopAbility();
    EXPECT_STREQ("ability_name12", currentTopAbilityRecord->GetAbilityInfo().name.c_str());
    // EXPECT_STREQ("ability_name11",
    currentTopAbilityRecord->GetPreAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);
    currentTopAbilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(3, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());

    // start "ability_name13"
    result = stackManager_->StartAbility(abilityRequests[2]);
    usleep(1000);
    currentTopAbilityRecord = stackManager_->GetCurrentTopAbility();
    EXPECT_STREQ("ability_name13", currentTopAbilityRecord->GetAbilityInfo().name.c_str());
    currentTopAbilityRecord->GetPreAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);
    currentTopAbilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(4, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());

    // start "ability_name12"
    result = stackManager_->StartAbility(abilityRequests[1]);
    usleep(1000);
    currentTopAbilityRecord = stackManager_->GetCurrentTopAbility();
    EXPECT_STREQ("ability_name12", currentTopAbilityRecord->GetAbilityInfo().name.c_str());
    EXPECT_STREQ("ability_name13", currentTopAbilityRecord->GetPreAbilityRecord()->GetAbilityInfo().name.c_str());
    currentTopAbilityRecord->GetPreAbilityRecord()->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);
    currentTopAbilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    EXPECT_EQ(5, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());

    // terminate stack top ability "ability_name12"
    currentTopAbilityRecord->lifecycleDeal_ = nullptr;
    Want want;
    result = stackManager_->TerminateAbility(currentTopAbilityRecord->GetToken(), -1, &want);
    EXPECT_EQ(AbilityStackManager::DEFAULT_MISSION_STACK_ID, curMissionStack->GetMissionStackId());
    EXPECT_EQ(4, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    currentTopAbilityRecord = curMissionStack->GetTopAbilityRecord();
    ASSERT_TRUE(currentTopAbilityRecord != nullptr);
    EXPECT_STREQ("ability_name13", currentTopAbilityRecord->GetAbilityInfo().name.c_str());

    // terminate stack bottom ability
    auto bottomAbility = curMissionStack->GetTopMissionRecord()->abilities_.back();
    result = stackManager_->TerminateAbility(bottomAbility->GetToken(), -1, &want);
    EXPECT_EQ(3, curMissionStack->GetTopMissionRecord()->GetAbilityRecordCount());
    currentTopAbilityRecord = curMissionStack->GetTopAbilityRecord();
    ASSERT_TRUE(currentTopAbilityRecord != nullptr);
    EXPECT_STREQ("ability_name13", currentTopAbilityRecord->GetAbilityInfo().name.c_str());

    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_006 end";
}

/*
 * Feature: AaFwk
 * Function: attach ability
 * SubFunction: attach ability thread
 * FunctionPoints: update ability state
 * EnvConditions: NA
 * CaseDescription: update ability state when attach ability.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_007 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant("");
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);
    abilityRequest.appInfo.isLauncherApp = false;  // non-launcher ability
    abilityRequest.abilityInfo.applicationInfo = abilityRequest.appInfo;

    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::INACTIVE);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord);

    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    MockAppMgrClient *mockAppMgrClient = new MockAppMgrClient();
    appScheduler->appMgrClient_.reset(mockAppMgrClient);

    OHOS::sptr<MockAbilityScheduler> abilityScheduler(new MockAbilityScheduler());
    // ams handler is non statrt so times is 0
    EXPECT_CALL(*mockAppMgrClient, UpdateAbilityState(testing::_, testing::_)).Times(1);

    EXPECT_TRUE(abilityRecord->GetToken());
    stackManager_->AttachAbilityThread(abilityScheduler, abilityRecord->GetToken());
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_007 end";
}

/*
 * Feature: AaFwk
 * Function: ability state transition
 * SubFunction: ability state transition done
 * FunctionPoints: ability state transition(TERMINATING->INITIAL)
 * EnvConditions: NA
 * CaseDescription: complete ability state transition(TERMINATING->INITIAL).
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_011 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant("");
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);

    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord->lifecycleDeal_ = nullptr;
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::TERMINATING);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord);
    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);

    std::shared_ptr<AbilityEventHandler> handler =
        std::make_shared<AbilityEventHandler>(nullptr, OHOS::DelayedSingleton<AbilityManagerService>::GetInstance());
    OHOS::DelayedSingleton<AbilityManagerService>::GetInstance()->handler_ = handler;

    int result = stackManager_->AbilityTransitionDone(abilityRecord->GetToken(), OHOS::AAFwk::AbilityState::INITIAL);
    EXPECT_EQ(OHOS::ERR_OK, result);

    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_011 end";
}

/*
 * Feature: AaFwk
 * Function: add Window
 * SubFunction: add window for ability
 * FunctionPoints: add window for ability
 * EnvConditions: NA
 * CaseDescription: add window for ability.
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_012 start";

    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant("");
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);

    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord);
    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);

    stackManager_->AddWindowInfo(abilityRecord->GetToken(), 1);
    EXPECT_TRUE(abilityRecord->GetWindowInfo() != nullptr);

    GTEST_LOG_(INFO) << "AbilityStackModuleTest ability_stack_test_012 end";
}

/*
 * Feature: AaFwk
 * Function: app state callback
 * SubFunction: OnAbilityRequestDone
 * FunctionPoints: OnAbilityRequestDone
 * EnvConditions: NA
 * CaseDescription: activate ability(ABILITY_STATE_FOREGROUND).
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_010, TestSize.Level1)
{
    std::string abilityName = "ability_name";
    std::string bundleName = "com.ix.aafwk.moduletest";

    AbilityRequest abilityRequest;
    abilityRequest.want = CreateWant("");
    abilityRequest.abilityInfo = CreateAbilityInfo(abilityName, bundleName, bundleName);
    abilityRequest.appInfo = CreateAppInfo(bundleName, bundleName);

    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);

    OHOS::sptr<MockAbilityScheduler> scheduler(new MockAbilityScheduler());
    EXPECT_CALL(*scheduler, AsObject()).Times(2);
    abilityRecord->SetScheduler(scheduler);

    abilityRecord->SetAbilityState(OHOS::AAFwk::AbilityState::ACTIVE);
    std::shared_ptr<MissionRecord> mission = std::make_shared<MissionRecord>(bundleName);
    mission->AddAbilityRecordToTop(abilityRecord);
    stackManager_->Init();
    std::shared_ptr<MissionStack> curMissionStack = stackManager_->GetCurrentMissionStack();
    curMissionStack->AddMissionRecordToTop(mission);

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(testing::_, testing::_)).Times(1);
    stackManager_->OnAbilityRequestDone(
        abilityRecord->GetToken(), static_cast<int32_t>(OHOS::AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND));
}

/*
 * Feature: AbilityStackManager
 * Function:  RemoveStack
 * SubFunction: NA
 * FunctionPoints: RemoveStack
 * EnvConditions: NA
 * CaseDescription: Succeeded to verify RemoveStack
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_011, TestSize.Level1)
{

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    MockAppMgrClient *mockAppMgrClient = new MockAppMgrClient();
    appScheduler->appMgrClient_.reset(mockAppMgrClient);
    EXPECT_CALL(*mockAppMgrClient, LoadAbility(_, _, _, _)).Times(1).WillOnce(Return(AppMgrResultCode::RESULT_OK));
    EXPECT_CALL(*mockAppMgrClient, UpdateAbilityState(testing::_, testing::_))
        .Times(1)
        .WillOnce(Return(AppMgrResultCode::RESULT_OK));

    AbilityRequest launcherAbilityRequest_ =
        GenerateAbilityRequest("device", "LauncherAbility", "launcher", "com.ix.hiworld");
    stackManager_->Init();
    stackManager_->StartAbility(launcherAbilityRequest_);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    EXPECT_TRUE(firstTopAbility);
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto transactionDoneCaller = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteInactive(firstTopAbility);
    };

    auto transactionDoneCaller2 = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteBackground(firstTopAbility);
    };

    OHOS::sptr<MockAbilityScheduler> scheduler(new MockAbilityScheduler());

    firstTopAbility->SetScheduler(scheduler);
    EXPECT_CALL(*scheduler, AsObject()).Times(testing::AtLeast(1)).WillOnce(Return(nullptr));

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(testing::_, testing::_))
        .Times(testing::AtLeast(2))
        .WillOnce(testing::Invoke(transactionDoneCaller))
        .WillOnce(testing::Invoke(transactionDoneCaller2));

    int result = stackManager_->PowerOff();
    EXPECT_EQ(ERR_OK, result);

    EXPECT_NE(stackManager_->powerStorage_, nullptr);
    EXPECT_EQ(OHOS::AAFwk::BACKGROUND, firstTopAbility->GetAbilityState());

    auto recordVector = stackManager_->powerStorage_->GetPowerOffRecord();
    EXPECT_TRUE(recordVector.empty());

    testing::Mock::AllowLeak(mockAppMgrClient);
    testing::Mock::AllowLeak(scheduler);
}

/*
 * Feature: AbilityStackManager
 * Function:  RemoveStack
 * SubFunction: NA
 * FunctionPoints: RemoveStack
 * EnvConditions: NA
 * CaseDescription: Succeeded to verify RemoveStack
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_012, TestSize.Level1)
{
    stackManager_->Init();

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    MockAppMgrClient *mockAppMgrClient = new MockAppMgrClient();
    appScheduler->appMgrClient_.reset(mockAppMgrClient);

    EXPECT_CALL(*mockAppMgrClient, LoadAbility(_, _, _, _))
        .Times(AtLeast(1))
        .WillOnce(Return(AppMgrResultCode::RESULT_OK));

    auto launcherAbilityRequest_ = GenerateAbilityRequest("device", "LauncherAbility", "launcher", "com.ix.hiworld");
    stackManager_->StartAbility(launcherAbilityRequest_);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto musicAbilityRequest_ = GenerateAbilityRequest("device", "MusicAbility", "music", "com.ix.hiMusic");
    stackManager_->StartAbility(musicAbilityRequest_);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    firstTopAbility->SetAbilityState(OHOS::AAFwk::INACTIVE);

    auto transactionDoneCaller = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteBackground(firstTopAbility);
    };

    auto transactionDoneCaller2 = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteActive(firstTopAbility);
    };

    auto transactionDoneCaller3 = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteInactive(firstTopAbility);
    };

    OHOS::sptr<MockAbilityScheduler> scheduler(new MockAbilityScheduler());
    firstTopAbility->SetScheduler(scheduler);
    EXPECT_CALL(*scheduler, AsObject())
        .Times(testing::AtLeast(1))
        .WillOnce(Return(nullptr))
        .WillOnce(Return(nullptr))
        .WillOnce(Return(nullptr));

    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(testing::_, testing::_))
        .Times(testing::AtLeast(3))
        .WillOnce(testing::Invoke(transactionDoneCaller))
        .WillOnce(testing::Invoke(transactionDoneCaller2))
        .WillOnce(testing::Invoke(transactionDoneCaller3))
        .WillOnce(testing::Invoke(transactionDoneCaller));

    auto transactionDoneCaller2_1 = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteInactive(secondTopAbility);
    };

    auto transactionDoneCaller2_2 = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteBackground(secondTopAbility);
    };

    auto transactionDoneCaller2_3 = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteActive(secondTopAbility);
    };

    OHOS::sptr<MockAbilityScheduler> scheduler2(new MockAbilityScheduler());
    secondTopAbility->SetScheduler(scheduler2);
    EXPECT_CALL(*scheduler2, AsObject())
        .Times(testing::AtLeast(1))
        .WillOnce(Return(nullptr))
        .WillOnce(Return(nullptr))
        .WillOnce(Return(nullptr));

    EXPECT_CALL(*scheduler2, ScheduleAbilityTransaction(testing::_, testing::_))
        .Times(testing::AtLeast(3))
        .WillOnce(testing::Invoke(transactionDoneCaller2_1))
        .WillOnce(testing::Invoke(transactionDoneCaller2_2))
        .WillOnce(testing::Invoke(transactionDoneCaller2_3));

    int result = stackManager_->PowerOff();
    EXPECT_EQ(ERR_OK, result);

    EXPECT_EQ(OHOS::AAFwk::BACKGROUND, firstTopAbility->GetAbilityState());
    EXPECT_EQ(OHOS::AAFwk::BACKGROUND, secondTopAbility->GetAbilityState());

    auto recordVector = stackManager_->powerStorage_->GetPowerOffRecord();
    int size = recordVector.size();
    EXPECT_EQ(size, 1);

    for (const auto &it : recordVector) {
        EXPECT_EQ(it.ability.lock()->GetRecordId(), firstTopAbility->GetRecordId());
        EXPECT_EQ(it.ability.lock()->GetAbilityState(), OHOS::AAFwk::BACKGROUND);
    }

    auto requestDone = [&](const sptr<IRemoteObject> &token, const AppExecFwk::AbilityState state) -> AppMgrResultCode {
        firstTopAbility->Activate();
        return AppMgrResultCode::RESULT_OK;
    };

    auto requestDone2 = [&](const sptr<IRemoteObject> &token,
                            const AppExecFwk::AbilityState state) -> AppMgrResultCode {
        secondTopAbility->Activate();
        return AppMgrResultCode::RESULT_OK;
    };

    auto requestDone3 = [&](const sptr<IRemoteObject> &token,
                            const AppExecFwk::AbilityState state) -> AppMgrResultCode {
        return AppMgrResultCode::RESULT_OK;
    };

    EXPECT_CALL(*mockAppMgrClient, UpdateAbilityState(testing::_, testing::_))
        .Times(3)
        .WillOnce(Invoke(requestDone))
        .WillOnce(Invoke(requestDone2))
        .WillOnce(Invoke(requestDone3));

    result = stackManager_->PowerOn();
    EXPECT_EQ(ERR_OK, result);

    EXPECT_EQ(OHOS::AAFwk::BACKGROUND, firstTopAbility->GetAbilityState());  // end last move to background
    EXPECT_EQ(OHOS::AAFwk::ACTIVE, secondTopAbility->GetAbilityState());

    testing::Mock::AllowLeak(mockAppMgrClient);
}

/*
 * Feature: AaFwk
 * Function: StartLockMission
 * SubFunction: NA
 * FunctionPoints: lock a mission , It's locked mission top ability will be active
 * EnvConditions: NA
 * CaseDescription: lock a mission
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_013, TestSize.Level1)
{
    stackManager_->Init();
    int sysUid = 1000;
    EXPECT_CALL(*bundleObject_, GetUidByBundleName(_, _))
        .Times(AtLeast(2))
        .WillOnce(Return(sysUid))
        .WillOnce(Return(sysUid));

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    MockAppMgrClient *mockAppMgrClient = new MockAppMgrClient();
    appScheduler->appMgrClient_.reset(mockAppMgrClient);

    EXPECT_CALL(*mockAppMgrClient, LoadAbility(_, _, _, _))
        .Times(AtLeast(1))
        .WillOnce(Return(AppMgrResultCode::RESULT_OK));

    auto launcherAbilityRequest_ = GenerateAbilityRequest("device", "LauncherAbility", "launcher", "com.ix.hiworld");
    stackManager_->StartAbility(launcherAbilityRequest_);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto musicAbilityRequest_ = GenerateAbilityRequest("device", "MusicAbility", "music", "com.ix.hiMusic");
    stackManager_->StartAbility(musicAbilityRequest_);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // lock LauncherAbility
    auto launcherMisionRecord = firstTopAbility->GetMissionRecord();
    auto launcherMisionRecordId = launcherMisionRecord->GetMissionRecordId();
    auto result = stackManager_->StartLockMission(sysUid, launcherMisionRecordId, true, true);
    EXPECT_EQ(ERR_OK, result);

    auto topRecord = stackManager_->GetCurrentTopAbility();
    auto abilityInfo = topRecord->GetAbilityInfo();
    EXPECT_EQ(abilityInfo.name, "LauncherAbility");
    topRecord->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // unlock
    result = stackManager_->StartLockMission(sysUid, launcherMisionRecordId, true, false);
    EXPECT_EQ(ERR_OK, result);

    testing::Mock::AllowLeak(mockAppMgrClient);
    testing::Mock::AllowLeak(bundleObject_);
}

/*
 * Feature: AaFwk
 * Function: StartLockMissions
 * SubFunction: NA
 * FunctionPoints: lock a mission , It's locked mission top ability will be active
 * EnvConditions: NA
 * CaseDescription: multiple mission lock someone
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_014, TestSize.Level1)
{
    stackManager_->Init();
    int sysUid = 1000;
    EXPECT_CALL(*bundleObject_, GetUidByBundleName(_, _))
        .Times(AtLeast(2))
        .WillOnce(Return(sysUid))
        .WillOnce(Return(sysUid));

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    MockAppMgrClient *mockAppMgrClient = new MockAppMgrClient();
    appScheduler->appMgrClient_.reset(mockAppMgrClient);

    EXPECT_CALL(*mockAppMgrClient, LoadAbility(_, _, _, _))
        .Times(AtLeast(1))
        .WillOnce(Return(AppMgrResultCode::RESULT_OK));

    auto launcherAbilityRequest_ = GenerateAbilityRequest("device", "MusicAbility", "launcher", "com.ix.hiworld");
    auto ref = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, ref);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto musicAbilityRequest_ = GenerateAbilityRequest("device", "MusicAbility2th", "launcher", "com.ix.hiMusic");
    ref = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, ref);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto redioAbilityRequest_ = GenerateAbilityRequest("device", "RadioTopAbility", "Radio", "com.ix.hiRadio");
    ref = stackManager_->StartAbility(redioAbilityRequest_);
    EXPECT_EQ(ERR_OK, ref);
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    thirdTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto transactionDoneCaller = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteActive(secondTopAbility);
    };
    auto transactionDoneCaller2 = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteInactive(secondTopAbility);
    };

    OHOS::sptr<MockAbilityScheduler> scheduler(new MockAbilityScheduler());
    secondTopAbility->SetScheduler(scheduler);
    EXPECT_CALL(*scheduler, ScheduleAbilityTransaction(testing::_, testing::_))
        .Times(testing::AtLeast(2))
        .WillOnce(testing::Invoke(transactionDoneCaller))
        .WillOnce(testing::Invoke(transactionDoneCaller2));

    EXPECT_CALL(*scheduler, AsObject()).Times(testing::AtLeast(1)).WillOnce(Return(nullptr));
    auto transactionDoneCaller3 = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteInactive(thirdTopAbility);
    };
    auto transactionDoneCaller4 = [&](const Want &want, const LifeCycleStateInfo &targetState) {
        stackManager_->CompleteActive(thirdTopAbility);
    };

    OHOS::sptr<MockAbilityScheduler> scheduler2(new MockAbilityScheduler());
    thirdTopAbility->SetScheduler(scheduler2);
    EXPECT_CALL(*scheduler2, ScheduleAbilityTransaction(testing::_, testing::_))
        .Times(testing::AtLeast(2))
        .WillOnce(testing::Invoke(transactionDoneCaller3))
        .WillOnce(testing::Invoke(transactionDoneCaller4));

    EXPECT_CALL(*scheduler2, AsObject()).Times(testing::AtLeast(1)).WillOnce(Return(nullptr));

    secondTopAbility->SetAbilityState(OHOS::AAFwk::INACTIVE);
    auto musicMisionRecord = secondTopAbility->GetMissionRecord();
    auto musicMisionRecordId = musicMisionRecord->GetMissionRecordId();

    auto result = stackManager_->StartLockMission(sysUid, musicMisionRecordId, false, true);
    EXPECT_EQ(ERR_OK, result);

    auto topRecord = stackManager_->GetCurrentTopAbility();
    auto abilityInfo = topRecord->GetAbilityInfo();
    EXPECT_EQ(abilityInfo.name, "MusicAbility2th");

    // unlock
    result = stackManager_->StartLockMission(sysUid, musicMisionRecordId, false, false);
    EXPECT_EQ(ERR_OK, result);

    testing::Mock::AllowLeak(mockAppMgrClient);
    testing::Mock::AllowLeak(bundleObject_);
}

/*
 * Feature: AaFwk
 * Function: StartLockMission
 * SubFunction: NA
 * FunctionPoints: lock a mission , It's locked mission top ability will be active
 * EnvConditions: NA
 * CaseDescription: when a misson locked other mission do not to statr ability
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_015, TestSize.Level1)
{
    stackManager_->Init();
    int userUid = 10;
    EXPECT_CALL(*bundleObject_, GetUidByBundleName(_, _))
        .Times(AtLeast(2))
        .WillOnce(Return(userUid))
        .WillOnce(Return(userUid));

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    MockAppMgrClient *mockAppMgrClient = new MockAppMgrClient();
    appScheduler->appMgrClient_.reset(mockAppMgrClient);

    EXPECT_CALL(*mockAppMgrClient, LoadAbility(_, _, _, _))
        .Times(AtLeast(1))
        .WillOnce(Return(AppMgrResultCode::RESULT_OK));

    // start a SINGLETON ability
    auto musicAbilityRequest_ = GenerateAbilityRequest("device", "MusicSAbility", "music", "com.ix.hiMusic");
    stackManager_->StartAbility(musicAbilityRequest_);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // lock
    auto musicMisionRecord = stackManager_->GetTopMissionRecord();
    auto musicMisionRecordId = musicMisionRecord->GetMissionRecordId();
    auto result = stackManager_->StartLockMission(userUid, musicMisionRecordId, false, true);
    EXPECT_EQ(ERR_OK, result);

    // not can start a ability
    auto redioAbilityRequest_ = GenerateAbilityRequest("device", "RedioAbility", "music", "com.ix.hiRadio");
    auto ref = stackManager_->StartAbility(redioAbilityRequest_);
    EXPECT_EQ(LOCK_MISSION_STATE_DENY_REQUEST, ref);

    auto topAbility = stackManager_->GetCurrentTopAbility();
    topAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);
    // unlock
    result = stackManager_->StartLockMission(userUid, musicMisionRecordId, false, false);
    EXPECT_EQ(ERR_OK, result);

    // can start a ability
    ref = stackManager_->StartAbility(redioAbilityRequest_);
    EXPECT_EQ(ERR_OK, ref);
    testing::Mock::AllowLeak(mockAppMgrClient);
    testing::Mock::AllowLeak(bundleObject_);
}

/*
 * Feature: AaFwk
 * Function: StartLockMission
 * SubFunction: NA
 * FunctionPoints: lock a mission , It's locked mission top ability will be active
 * EnvConditions: NA
 * CaseDescription: when a mission locked, you cannot delete the last one
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_016, TestSize.Level1)
{
    stackManager_->Init();
    int userUid = 10;
    EXPECT_CALL(*bundleObject_, GetUidByBundleName(_, _))
        .Times(AtLeast(2))
        .WillOnce(Return(userUid))
        .WillOnce(Return(userUid));

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    MockAppMgrClient *mockAppMgrClient = new MockAppMgrClient();
    appScheduler->appMgrClient_.reset(mockAppMgrClient);

    EXPECT_CALL(*mockAppMgrClient, LoadAbility(_, _, _, _))
        .Times(AtLeast(1))
        .WillOnce(Return(AppMgrResultCode::RESULT_OK));

    // start a SINGLETON ability
    auto musicAbilityRequest_ = GenerateAbilityRequest("device", "MusicSAbility", "music", "com.ix.hiMusic");
    stackManager_->StartAbility(musicAbilityRequest_);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // lock
    auto musicMisionRecord = stackManager_->GetTopMissionRecord();
    auto musicMisionRecordId = musicMisionRecord->GetMissionRecordId();
    auto result = stackManager_->StartLockMission(userUid, musicMisionRecordId, false, true);
    EXPECT_EQ(ERR_OK, result);

    result = stackManager_->TerminateAbility(secondTopAbility->GetToken(), -1, nullptr);
    EXPECT_EQ(LOCK_MISSION_STATE_DENY_REQUEST, result);

    // unlock
    result = stackManager_->StartLockMission(userUid, musicMisionRecordId, false, false);
    EXPECT_EQ(ERR_OK, result);

    result = stackManager_->TerminateAbility(secondTopAbility->GetToken(), -1, nullptr);
    EXPECT_EQ(ERR_OK, result);

    testing::Mock::AllowLeak(mockAppMgrClient);
    testing::Mock::AllowLeak(bundleObject_);
}

/*
 * Feature: AaFwk
 * Function: StartLockMissions
 * SubFunction: NA
 * FunctionPoints: lock a mission , It's locked mission top ability will be active
 * EnvConditions: NA
 * CaseDescription: lock a  multi ability mission
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_017, TestSize.Level1)
{
    stackManager_->Init();
    int sysUid = 1000;
    EXPECT_CALL(*bundleObject_, GetUidByBundleName(_, _))
        .Times(AtLeast(2))
        .WillOnce(Return(sysUid))
        .WillOnce(Return(sysUid));

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    MockAppMgrClient *mockAppMgrClient = new MockAppMgrClient();
    appScheduler->appMgrClient_.reset(mockAppMgrClient);

    EXPECT_CALL(*mockAppMgrClient, LoadAbility(_, _, _, _))
        .Times(AtLeast(1))
        .WillOnce(Return(AppMgrResultCode::RESULT_OK));

    auto launcherAbilityRequest_ = GenerateAbilityRequest("device", "MusicAbility", "launcher", "com.ix.hiMusic");
    auto ref = stackManager_->StartAbility(launcherAbilityRequest_);
    EXPECT_EQ(ERR_OK, ref);
    auto firstTopAbility = stackManager_->GetCurrentTopAbility();
    firstTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto musicAbilityRequest_ = GenerateAbilityRequest("device", "MusicAbility2th", "launcher", "com.ix.hiMusic");
    ref = stackManager_->StartAbility(musicAbilityRequest_);
    EXPECT_EQ(ERR_OK, ref);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    auto redioAbilityRequest_ = GenerateAbilityRequest("device", "RadioTopAbility", "Radio", "com.ix.hiRadio");
    ref = stackManager_->StartAbility(redioAbilityRequest_);
    EXPECT_EQ(ERR_OK, ref);
    auto thirdTopAbility = stackManager_->GetCurrentTopAbility();
    thirdTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    firstTopAbility->SetAbilityState(OHOS::AAFwk::BACKGROUND);
    secondTopAbility->SetAbilityState(OHOS::AAFwk::BACKGROUND);

    auto musicMisionRecord = secondTopAbility->GetMissionRecord();
    auto musicMisionRecordId = musicMisionRecord->GetMissionRecordId();

    auto result = stackManager_->StartLockMission(sysUid, musicMisionRecordId, true, true);
    EXPECT_EQ(ERR_OK, result);

    auto topRecord = stackManager_->GetCurrentTopAbility();
    auto abilityInfo = topRecord->GetAbilityInfo();
    EXPECT_EQ(abilityInfo.name, "MusicAbility2th");

    // unlock
    result = stackManager_->StartLockMission(sysUid, musicMisionRecordId, true, false);
    EXPECT_EQ(ERR_OK, result);

    testing::Mock::AllowLeak(mockAppMgrClient);
    testing::Mock::AllowLeak(bundleObject_);
}

/*
 * Feature: AaFwk
 * Function: StartLockMission
 * SubFunction: NA
 * FunctionPoints: lock a mission , It's locked mission top ability will be active
 * EnvConditions: NA
 * CaseDescription: when locked mission died
 */
HWTEST_F(AbilityStackModuleTest, ability_stack_test_018, TestSize.Level1)
{
    stackManager_->Init();
    int userUid = 10;
    EXPECT_CALL(*bundleObject_, GetUidByBundleName(_, _))
        .Times(AtLeast(2))
        .WillOnce(Return(userUid))
        .WillOnce(Return(userUid));

    auto appScheduler = OHOS::DelayedSingleton<AppScheduler>::GetInstance();
    MockAppMgrClient *mockAppMgrClient = new MockAppMgrClient();
    appScheduler->appMgrClient_.reset(mockAppMgrClient);

    EXPECT_CALL(*mockAppMgrClient, LoadAbility(_, _, _, _))
        .Times(AtLeast(1))
        .WillOnce(Return(AppMgrResultCode::RESULT_OK));

    // start a SINGLETON ability
    auto musicAbilityRequest_ = GenerateAbilityRequest("device", "MusicSAbility", "music", "com.ix.hiMusic");
    stackManager_->StartAbility(musicAbilityRequest_);
    auto secondTopAbility = stackManager_->GetCurrentTopAbility();
    secondTopAbility->SetAbilityState(OHOS::AAFwk::ACTIVE);

    // lock
    auto musicMisionRecord = stackManager_->GetTopMissionRecord();
    auto musicMisionRecordId = musicMisionRecord->GetMissionRecordId();
    auto result = stackManager_->StartLockMission(userUid, musicMisionRecordId, false, true);
    EXPECT_EQ(ERR_OK, result);

    // let ability die
    stackManager_->OnAbilityDied(secondTopAbility);
    auto state = stackManager_->lockMissionContainer_->IsLockedMissionState();
    EXPECT_FALSE(state);

    testing::Mock::AllowLeak(mockAppMgrClient);
    testing::Mock::AllowLeak(bundleObject_);
}

}  // namespace AAFwk
}  // namespace OHOS
