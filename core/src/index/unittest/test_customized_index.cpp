// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include <gtest/gtest.h>

#include "unittest/Helper.h"
#include "unittest/utils.h"

class SingleIndexTest : public DataGen, public TestGpuIndexBase {
 protected:
    void
    SetUp() override {
        TestGpuIndexBase::SetUp();
        Generate(DIM, NB, NQ);
        k = K;
    }

    void
    TearDown() override {
        TestGpuIndexBase::TearDown();
    }

 protected:
    std::string index_type;
    knowhere::IVFIndexPtr index_ = nullptr;
};

#ifdef CUSTOMIZATION
TEST_F(SingleIndexTest, IVFSQHybrid) {
    assert(!xb.empty());

    index_type = "IVFSQHybrid";
    index_ = IndexFactory(index_type);
    auto conf = ParamGenerator::GetInstance().Gen(ParameterType::ivfsq);
    auto preprocessor = index_->BuildPreprocessor(base_dataset, conf);
    index_->set_preprocessor(preprocessor);

    auto model = index_->Train(base_dataset, conf);
    index_->set_index_model(model);
    index_->Add(base_dataset, conf);
    EXPECT_EQ(index_->Count(), nb);
    EXPECT_EQ(index_->Dimension(), dim);

    auto binaryset = index_->Serialize();
    {
        // copy cpu to gpu
        auto cpu_idx = std::make_shared<knowhere::IVFSQHybrid>(DEVICEID);
        cpu_idx->Load(binaryset);

        {
            for (int i = 0; i < 3; ++i) {
                auto gpu_idx = cpu_idx->CopyCpuToGpu(DEVICEID, conf);
                auto result = gpu_idx->Search(query_dataset, conf);
                AssertAnns(result, nq, conf->k);
                // PrintResult(result, nq, k);
            }
        }
    }

    {
        // quantization already in gpu, only copy data
        auto cpu_idx = std::make_shared<knowhere::IVFSQHybrid>(DEVICEID);
        cpu_idx->Load(binaryset);

        auto pair = cpu_idx->CopyCpuToGpuWithQuantizer(DEVICEID, conf);
        auto gpu_idx = pair.first;
        auto quantization = pair.second;

        auto result = gpu_idx->Search(query_dataset, conf);
        AssertAnns(result, nq, conf->k);
        //        PrintResult(result, nq, k);

        auto quantizer_conf = std::make_shared<knowhere::QuantizerCfg>();
        quantizer_conf->mode = 2;  // only copy data
        quantizer_conf->gpu_id = DEVICEID;
        for (int i = 0; i < 2; ++i) {
            auto hybrid_idx = std::make_shared<knowhere::IVFSQHybrid>(DEVICEID);
            hybrid_idx->Load(binaryset);

            auto new_idx = hybrid_idx->LoadData(quantization, quantizer_conf);
            auto result = new_idx->Search(query_dataset, conf);
            AssertAnns(result, nq, conf->k);
            //            PrintResult(result, nq, k);
        }
    }

    {
        // quantization already in gpu, only set quantization
        auto cpu_idx = std::make_shared<knowhere::IVFSQHybrid>(DEVICEID);
        cpu_idx->Load(binaryset);

        auto pair = cpu_idx->CopyCpuToGpuWithQuantizer(DEVICEID, conf);
        auto quantization = pair.second;

        for (int i = 0; i < 2; ++i) {
            auto hybrid_idx = std::make_shared<knowhere::IVFSQHybrid>(DEVICEID);
            hybrid_idx->Load(binaryset);

            hybrid_idx->SetQuantizer(quantization);
            auto result = hybrid_idx->Search(query_dataset, conf);
            AssertAnns(result, nq, conf->k);
            //            PrintResult(result, nq, k);
            hybrid_idx->UnsetQuantizer();
        }
    }
}

#endif
