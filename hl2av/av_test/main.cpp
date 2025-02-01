
#include <windows.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Input.h>

#include "encoder.h"
#include "decoder.h"
#include "sample.h"
#include "core.h"
#include "log.h"

using namespace winrt;

using namespace Windows;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Composition;

constexpr LONGLONG HNS_BASE = 10 * 1000 * 1000;

void* CreateTestImage(DWORD size, LONGLONG time, LONGLONG duration)
{
    void* base;
    void* sample = Sample_Create(base, size);
    Sample_SetInfo(sample, time, duration);
    memset(base, 0, size);
    return sample;
}

void TestVideo(int N)
{
    H26xFormat fe;

    fe.width = 640;
    fe.height = 360;
    fe.framerate = 30;
    fe.divisor = 1;
    fe.profile = H264Profile_Main;
    fe.level = H26xLevel_Default;
    fe.bitrate = (fe.width * fe.height * fe.framerate * 12) / 100;

    H26xFormat fd;

    fd.profile = fe.profile;

    void* encoder = Encoder_CreateForVideo(fe.width, fe.width, fe.height, fe.framerate, fe.divisor, VideoSubtype_NV12, fe.profile, fe.level, fe.bitrate);
    void* decoder = Decoder_CreateForVideo(fd.profile, VideoSubtype_NV12);

    ShowMessage("Video Test BEGIN");

    for (int i = 0; i < N; ++i)
    {
        void* pTestImage = CreateTestImage((fe.width * fe.height * 3) / 2, i * (HNS_BASE / fe.framerate), HNS_BASE / fe.framerate);

        void* base1;
        uint32_t size1;
        Sample_GetBase(pTestImage, base1, size1);
        ShowMessage("TestImage size: %d", size1);

        Encoder_Push(encoder, pTestImage); // encoder->Push(pTestImage);

        Sample_Release(pTestImage); // pTestImage->Release();
    }

    for (int i = 0; i < N; ++i)
    {
        Encoder_Peek(encoder);
        void* pTestEncoded = Encoder_Pull(encoder); // encoder->Pull(&pTestEncoded);

        void* base2;
        uint32_t size2;
        int64_t timestamp2;
        int64_t duration2;
        uint32_t cleanpoint2;
        Sample_GetBase(pTestEncoded, base2, size2);
        Sample_GetInfo(pTestEncoded, timestamp2, duration2, cleanpoint2);
        ShowMessage("TestEncoded size: %d time: %lld duration: %lld cleanpoint: %d", size2, timestamp2, duration2, cleanpoint2);

        Decoder_Push(decoder, pTestEncoded); // decoder->Push(pTestEncoded);

        Sample_Release(pTestEncoded); // pTestEncoded->Release();

        Decoder_Peek(decoder);
        void* pTestDecoded = Decoder_Pull(decoder); // decoder->Pull(&pTestDecoded);
        
        void* base3;
        uint32_t size3;
        int64_t timestamp3;
        int64_t duration3;
        uint32_t cleanpoint3;
        uint32_t width, stride, height;
        Sample_GetBase(pTestDecoded, base3, size3);
        Sample_GetInfo(pTestDecoded, timestamp3, duration3, cleanpoint3);
        Sample_GetResolution(pTestDecoded, width, stride, height);
        ShowMessage("TestDecoded size: %d time: %lld duration: %lld cleanpoint: %d dims: (%d,%d,%d)", size3, timestamp3, duration3, cleanpoint3, width, stride, height);

        Sample_Release(pTestDecoded); // pTestDecoded->Release();
    }

    Encoder_Destroy(encoder);
    Decoder_Destroy(decoder);

    ShowMessage("Video Test END");
}

void TestAudio(int N)
{
    int const sample_group = 1024;

    AACFormat fe;

    fe.channels = 2;
    fe.samplerate = 48000;
    fe.profile = AACProfile_24000;
    fe.level = AACLevel_L4;

    AACFormat fd;

    fd.profile = AACProfile_24000;
    fd.level = AACLevel_NotSet;

    void* encoder = Encoder_CreateForAudio(fe.channels, fe.samplerate, AudioSubtype::AudioSubtype_S16, fe.profile, fe.level);
    void* decoder = nullptr;

    ShowMessage("Audio Test BEGIN");

    for (int i = 0; i < N; ++i)
    {
        void* pTestImage = CreateTestImage(fe.channels * sizeof(int16_t) * sample_group, (i + 1) * (sample_group * (HNS_BASE / fe.samplerate)), sample_group * (HNS_BASE / fe.samplerate));
        
        void* base1;
        uint32_t size1;
        Sample_GetBase(pTestImage, base1, size1);
        ShowMessage("TestImage size: %d", size1);

        Encoder_Push(encoder, pTestImage); // encoder->Push(pTestImage);

        Sample_Release(pTestImage); // pTestImage->Release();
    }

    for (int i = 0; i < (N - 2); ++i)
    {
        Encoder_Peek(encoder);
        void* pTestEncoded = Encoder_Pull(encoder); // encoder->Pull(&pTestEncoded);

        void* base2;
        uint32_t size2;
        int64_t timestamp2;
        int64_t duration2;
        uint32_t cleanpoint2;
        Sample_GetBase(pTestEncoded, base2, size2);
        Sample_GetInfo(pTestEncoded, timestamp2, duration2, cleanpoint2);
        ShowMessage("TestEncoded size: %d time: %lld duration: %lld cleanpoint: %d", size2, timestamp2, duration2, cleanpoint2);

        if (!decoder)
        {
            bool ok = TranslateADTSOptions((BYTE*)base2, fd.channels, fd.samplerate);
            ShowMessage("AAC Parameters: (%d, %d, %d)", ok, fd.channels, fd.samplerate);
            decoder = Decoder_CreateForAudio(fd.channels, fd.samplerate, fd.profile, AudioSubtype::AudioSubtype_S16);
        }        
        
        Decoder_Push(decoder, pTestEncoded); // decoder->Push(pTestEncoded);

        Sample_Release(pTestEncoded); // pTestEncoded->Release();

        Decoder_Peek(decoder);
        void* pTestDecoded = Decoder_Pull(decoder); // decoder->Pull(&pTestDecoded);

        void* base3;
        uint32_t size3;
        int64_t timestamp;
        int64_t duration;
        uint32_t cleanpoint;
        Sample_GetBase(pTestDecoded, base3, size3);
        Sample_GetInfo(pTestDecoded, timestamp, duration, cleanpoint);
        ShowMessage("TestDecoded size: %d time: %lld duration: %lld cleanpoint: %d", size3, timestamp, duration, cleanpoint);

        Sample_Release(pTestDecoded); // pTestDecoded->Release();
    }

    Encoder_Destroy(encoder);
    Decoder_Destroy(decoder);

    ShowMessage("Audio Test END");
}

struct App : implements<App, IFrameworkViewSource, IFrameworkView>
{
    bool m_windowClosed = false;

    IFrameworkView CreateView()
    {
        return *this;
    }

    void App::OnWindowClosed(CoreWindow const& sender, CoreWindowEventArgs const& args)
    {
        (void)sender;
        (void)args;

        m_windowClosed = true;
    }

    void Initialize(CoreApplicationView const &)
    {
    }


    void Load(hstring const&)
    {
    }

    void Uninitialize()
    {
    }

    void Run()
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();
        window.Activate();

        Core_Startup();

        TestVideo(5);
        //TestAudio(5);
        
        while (!m_windowClosed)
        {
            window.Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
        }
    }

    void SetWindow(CoreWindow const & window)
    {
        window.Closed({ this, &App::OnWindowClosed });
    }
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    CoreApplication::Run(make<App>());
    return 0;
}
