//
// (C) CharLS Team 2014, all rights reserved. See the accompanying "License.txt" for licensed use.
//

#include "stdafx.h"

#include "../src/decoderstrategy.h"
#include "encoderstrategytester.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


class DecoderStrategyTester : public DecoderStrategy
{
public:
    DecoderStrategyTester(const JlsParameters& params, uint8_t* pOutBuf, size_t nOutBufLen) : DecoderStrategy(params)
    {
        ByteStreamInfo stream;
        stream.rawStream = nullptr;
        stream.rawData = pOutBuf;
        stream.count = nOutBufLen;
        Init(stream);
    }

    void SetPresets(const JpegLSPresetCodingParameters& /*presets*/) override {}

    std::unique_ptr<ProcessLine> CreateProcess(ByteStreamInfo /*rawStreamInfo*/) override
    {
        return nullptr;
    }

    int32_t Read(int32_t length) { return ReadLongValue(length); }
    void Finish() { EndScan(); }
};


namespace CharLSUnitTest
{
    TEST_CLASS(DecoderStrategyTest)
    {
    public:
        TEST_METHOD(DecodeEncodedFFPattern)
        {
            const struct
            {
                int32_t val;
                int bits;
            } inData[5] = { { 0x00, 24 },{ 0xFF, 8 },{ 0xFFFF, 16 },{ 0xFFFF, 16 },{ 0x12345678, 31 } };

            uint8_t encBuf[100];
            const JlsParameters params = { 0 };

            EncoderStrategyTester encoder(params);

            ByteStreamInfo stream;
            stream.rawStream = nullptr;
            stream.rawData = encBuf;
            stream.count = sizeof(encBuf);
            encoder.InitForward(stream);

            for (int i = 0; i < sizeof(inData) / sizeof(inData[0]); i++)
            {
                encoder.AppendToBitStreamForward(inData[i].val, inData[i].bits);
            }
            encoder.EndScanForward();
            // Note: Correct encoding is tested in EncoderStrategyTest::AppendToBitStreamFFPattern.

            auto length = encoder.GetLengthForward();
            DecoderStrategyTester dec(params, encBuf, length);
            for (auto i = 0; i < sizeof(inData) / sizeof(inData[0]); i++)
            {
                auto actual = dec.Read(inData[i].bits);
                Assert::AreEqual(inData[i].val, actual);
            }
        }
    };
}
