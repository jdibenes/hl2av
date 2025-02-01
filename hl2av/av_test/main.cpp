
#include <windows.h>
#include <mfapi.h>
#include <codecapi.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Input.h>

#include "custom_encoder.h"
#include "custom_decoder.h"
#include "log.h"

using namespace winrt;

using namespace Windows;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Composition;

constexpr LONGLONG HNS_BASE = 10 * 1000 * 1000;

IMFSample* CreateTestImage(DWORD size, LONGLONG sample_time, LONGLONG duration)
{
    IMFMediaBuffer* pBuffer; // Release
    BYTE* base;
    IMFSample* pSample;

    MFCreateMemoryBuffer(size, &pBuffer);

    pBuffer->Lock(&base, NULL, NULL);
    memset(base, 0, size);
    pBuffer->Unlock();
    pBuffer->SetCurrentLength(size);

    MFCreateSample(&pSample);

    pSample->SetSampleTime(sample_time);
    pSample->SetSampleDuration(duration);
    pSample->AddBuffer(pBuffer);

    pBuffer->Release();

    return pSample;
}

void TestVideo(int N)
{
    H26xFormat format;

    format.width = 640;
    format.height = 360;
    format.framerate = 30;
    format.divisor = 1;
    format.profile = H264Profile_Main;
    format.level = H26xLevel_Default;
    format.bitrate = (format.width * format.height * format.framerate * 12) / 100;

    H26xFormat format_decoder;

    format_decoder.profile = format.profile;

    auto encoder = CustomEncoder::CreateForVideo(VideoSubtype_NV12, format, format.width, {});
    auto decoder = CustomDecoder::CreateForVideo(format_decoder, VideoSubtype_NV12);

    ShowMessage("Video Test BEGIN");

    for (int i = 0; i < N; ++i)
    {
        IMFSample* pTestImage = CreateTestImage((format.width * format.height * 3) / 2, i * (HNS_BASE / format.framerate), HNS_BASE / format.framerate);
        encoder->Push(pTestImage);

        IMFMediaBuffer* pBuffer1;
        DWORD cur_size1;
        DWORD max_size1;
        pTestImage->ConvertToContiguousBuffer(&pBuffer1);
        pBuffer1->GetCurrentLength(&cur_size1);
        pBuffer1->GetMaxLength(&max_size1);
        ShowMessage("TestImage size: (%d, %d)", cur_size1, max_size1);
        pBuffer1->Release();

        pTestImage->Release();
    }

    for (int i = 0; i < N; ++i)
    {
        IMFSample* pTestEncoded;
        encoder->Pull(&pTestEncoded);
        decoder->Push(pTestEncoded);

        IMFMediaBuffer* pBuffer2;
        DWORD cur_size2;
        DWORD max_size2;
        pTestEncoded->ConvertToContiguousBuffer(&pBuffer2);
        pBuffer2->GetCurrentLength(&cur_size2);
        pBuffer2->GetMaxLength(&max_size2);
        ShowMessage("TestEncoded size: (%d, %d)", cur_size2, max_size2);
        pBuffer2->Release();

        pTestEncoded->Release();

        IMFSample* pTestDecoded;
        decoder->Pull(&pTestDecoded);

        IMFMediaBuffer* pBuffer3;
        DWORD cur_size3;
        DWORD max_size3;
        pTestDecoded->ConvertToContiguousBuffer(&pBuffer3);
        pBuffer3->GetCurrentLength(&cur_size3);
        pBuffer3->GetMaxLength(&max_size3);
        ShowMessage("TestDecoded size: (%d, %d)", cur_size3, max_size3);
        pBuffer3->Release();

        pTestDecoded->Release();
    }

    ShowMessage("Video Test END");
}

void TestAudio(int N)
{
    int const sample_group = 1024;

    AACFormat format;

    format.channels = 2;
    format.samplerate = 48000;
    format.profile = AACProfile_24000;
    format.level = AACLevel_L4;

    AACFormat format_decoder;

    //format_decoder.channels = 1; // from ADTS
    //format_decoder.samplerate = 44100; // from ADTS 
    format_decoder.profile = AACProfile_24000; // Anything but NONE
    format_decoder.level = AACLevel_NotSet;

    auto encoder = CustomEncoder::CreateForAudio(AudioSubtype::AudioSubtype_S16, format);
    std::unique_ptr<CustomDecoder> decoder;

    ShowMessage("Audio Test BEGIN");

    for (int i = 0; i < N; ++i)
    {
        IMFSample* pTestImage = CreateTestImage(format.channels * sizeof(int16_t) * sample_group, (i + 1) * (sample_group * (HNS_BASE / format.samplerate)), sample_group * (HNS_BASE / format.samplerate));
        encoder->Push(pTestImage);

        IMFMediaBuffer* pBuffer1;
        DWORD cur_size1;
        DWORD max_size1;
        pTestImage->ConvertToContiguousBuffer(&pBuffer1);
        pBuffer1->GetCurrentLength(&cur_size1);
        pBuffer1->GetMaxLength(&max_size1);
        ShowMessage("TestImage size: (%d, %d)", cur_size1, max_size1);
        pBuffer1->Release();

        pTestImage->Release();
    }

    for (int i = 0; i < (N - 2); ++i)
    {
        IMFSample* pTestEncoded;
        encoder->Pull(&pTestEncoded);
        
        IMFMediaBuffer* pBuffer2;
        DWORD cur_size2;
        DWORD max_size2;
        BYTE* data;
        pTestEncoded->ConvertToContiguousBuffer(&pBuffer2);
        pBuffer2->GetCurrentLength(&cur_size2);
        pBuffer2->GetMaxLength(&max_size2);
        pBuffer2->Lock(&data, NULL, NULL);
        pBuffer2->Unlock();
        ShowMessage("TestEncoded size: (%d, %d)", cur_size2, max_size2);
        ShowMessage("ADTS: %x %x %x %x %x %x %x", data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
        pBuffer2->Release();

        if (!decoder)
        {
            bool ok = TranslateADTSOptions(data, format_decoder.channels, format_decoder.samplerate);
            ShowMessage("AAC Parameters: (%d, %d, %d)", ok, format_decoder.channels, format_decoder.samplerate);
            decoder = CustomDecoder::CreateForAudio(format_decoder, AudioSubtype::AudioSubtype_S16);            
        }        
        
        decoder->Push(pTestEncoded);

        pTestEncoded->Release();

        IMFSample* pTestDecoded;
        decoder->Pull(&pTestDecoded);

        IMFMediaBuffer* pBuffer3;
        DWORD cur_size3;
        DWORD max_size3;
        pTestDecoded->ConvertToContiguousBuffer(&pBuffer3);
        pBuffer3->GetCurrentLength(&cur_size3);
        pBuffer3->GetMaxLength(&max_size3);
        ShowMessage("TestDecoded size: (%d, %d)", cur_size3, max_size3);
        pBuffer3->Release();

        pTestDecoded->Release();
    }

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

        MFStartup(MF_VERSION);

        TestVideo(5);
        TestAudio(5);
        
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
