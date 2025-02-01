
#include <mfapi.h>
#include <codecapi.h>
#include "custom_media_types.h"

struct AVOption
{
    GUID guid;
    uint32_t vt;
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static AVOption const g_AVLUT[] =
{
    {CODECAPI_AVEncCommonRateControlMode, VT_UI4},
    {CODECAPI_AVEncCommonQuality, VT_UI4},
    {CODECAPI_AVEncAdaptiveMode, VT_UI4},
    {CODECAPI_AVEncCommonBufferSize, VT_UI4},
    {CODECAPI_AVEncCommonMaxBitRate, VT_UI4},
    {CODECAPI_AVEncCommonMeanBitRate, VT_UI4},
    {CODECAPI_AVEncCommonQualityVsSpeed, VT_UI4},
    {CODECAPI_AVEncH264CABACEnable, VT_BOOL},
    {CODECAPI_AVEncH264SPSID, VT_UI4},
    {CODECAPI_AVEncMPVDefaultBPictureCount, VT_UI4},
    {CODECAPI_AVEncMPVGOPSize, VT_UI4},
    {CODECAPI_AVEncNumWorkerThreads, VT_UI4},
    {CODECAPI_AVEncVideoContentType, VT_UI4},
    {CODECAPI_AVEncVideoEncodeQP, VT_UI8},
    {CODECAPI_AVEncVideoForceKeyFrame, VT_UI4},
    {CODECAPI_AVEncVideoMinQP, VT_UI4},
    {CODECAPI_AVLowLatencyMode, VT_BOOL},
    {CODECAPI_AVEncVideoMaxQP, VT_UI4},
    {CODECAPI_VideoEncoderDisplayContentType, VT_UI4},
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// https://learn.microsoft.com/en-us/windows/win32/medfound/audio-subtype-guids
// OK
static HRESULT CreateTypePCMF32(IMFMediaType** ppType, uint32_t channels, uint32_t samplerate)
{
    uint32_t const bitspersample = 32;

    uint32_t blockalign = channels * (bitspersample / 8);
    uint32_t bytespersecond = blockalign * samplerate;
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
    pType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channels);
    pType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, samplerate);
    pType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, blockalign);
    pType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, bytespersecond);
    pType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, bitspersample);
    pType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);

    *ppType = pType;

    return S_OK;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/audio-subtype-guids
// OK
static HRESULT CreateTypePCMS16(IMFMediaType** ppType, uint32_t channels, uint32_t samplerate)
{
    uint32_t const bitspersample = 16;

    uint32_t blockalign = channels * (bitspersample / 8);
    uint32_t bytespersecond = blockalign * samplerate;
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    pType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channels);
    pType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, samplerate);
    pType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, blockalign);
    pType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, bytespersecond);
    pType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, bitspersample);
    pType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);

    *ppType = pType;

    return S_OK;
}

// https://docs.microsoft.com/en-us/windows/win32/medfound/aac-encoder
// OK
static HRESULT CreateTypeAAC(IMFMediaType** ppType, uint32_t channels, uint32_t samplerate, uint32_t bytespersecond, uint32_t level)
{
    IMFMediaType* pType;
    
    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
    pType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
    pType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channels);
    pType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, samplerate);
    pType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, bytespersecond * ((channels == 6) ? 6 : 1));
    pType->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, 1);
    pType->SetUINT32(MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, level);

    *ppType = pType;

    return S_OK;
}

// OK
HRESULT CreateTypeAudio(IMFMediaType** ppType, uint32_t channels, uint32_t samplerate, AudioSubtype subtype, AACProfile profile, AACLevel level)
{
    switch (profile)
    {
    case AACProfile::AACProfile_12000: return CreateTypeAAC(ppType, channels, samplerate, 12000, level);
    case AACProfile::AACProfile_16000: return CreateTypeAAC(ppType, channels, samplerate, 16000, level);
    case AACProfile::AACProfile_20000: return CreateTypeAAC(ppType, channels, samplerate, 20000, level);
    case AACProfile::AACProfile_24000: return CreateTypeAAC(ppType, channels, samplerate, 24000, level);
    }

    switch (subtype)
    {
    case AudioSubtype::AudioSubtype_F32: return CreateTypePCMF32(ppType, channels, samplerate);
    case AudioSubtype::AudioSubtype_S16: return CreateTypePCMS16(ppType, channels, samplerate);
    }

    *ppType = NULL;

    return E_INVALIDARG;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/video-subtype-guids
// OK
static HRESULT CreateTypeNV12(IMFMediaType **ppType, uint32_t width, uint32_t height, uint32_t stride, uint32_t fps_num, uint32_t fps_den)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
    pType->SetUINT32(MF_MT_DEFAULT_STRIDE, stride);
    MFSetAttributeRatio(pType, MF_MT_FRAME_RATE, fps_num, fps_den);
    MFSetAttributeSize(pType, MF_MT_FRAME_SIZE, width, height);
    pType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlaceMode::MFVideoInterlace_Progressive);
    pType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
    MFSetAttributeRatio(pType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    pType->SetUINT32(MF_MT_SAMPLE_SIZE, (3UL * width * height) / 2UL);
    pType->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, TRUE);

    *ppType = pType;

    return S_OK;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/video-subtype-guids
// OK
static HRESULT CreateTypeYUY2(IMFMediaType** ppType, uint32_t width, uint32_t height, uint32_t stride, uint32_t fps_num, uint32_t fps_den)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_YUY2);
    pType->SetUINT32(MF_MT_DEFAULT_STRIDE, stride);
    MFSetAttributeRatio(pType, MF_MT_FRAME_RATE, fps_num, fps_den);
    MFSetAttributeSize(pType, MF_MT_FRAME_SIZE, width, height);
    pType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlaceMode::MFVideoInterlace_Progressive);
    pType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
    MFSetAttributeRatio(pType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    pType->SetUINT32(MF_MT_SAMPLE_SIZE, 2UL * width * height);
    pType->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, TRUE);

    *ppType = pType;

    return S_OK;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/video-subtype-guids
// OK
static HRESULT CreateTypeIYUV(IMFMediaType** ppType, uint32_t width, uint32_t height, uint32_t stride, uint32_t fps_num, uint32_t fps_den)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV);
    pType->SetUINT32(MF_MT_DEFAULT_STRIDE, stride);
    MFSetAttributeRatio(pType, MF_MT_FRAME_RATE, fps_num, fps_den);
    MFSetAttributeSize(pType, MF_MT_FRAME_SIZE, width, height);
    pType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlaceMode::MFVideoInterlace_Progressive);
    pType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
    MFSetAttributeRatio(pType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    pType->SetUINT32(MF_MT_SAMPLE_SIZE, (3UL * width * height) / 2UL);
    pType->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, TRUE);

    *ppType = pType;

    return S_OK;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/video-subtype-guids
// OK
static HRESULT CreateTypeYV12(IMFMediaType** ppType, uint32_t width, uint32_t height, uint32_t stride, uint32_t fps_num, uint32_t fps_den)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_YV12);
    pType->SetUINT32(MF_MT_DEFAULT_STRIDE, stride);
    MFSetAttributeRatio(pType, MF_MT_FRAME_RATE, fps_num, fps_den);
    MFSetAttributeSize(pType, MF_MT_FRAME_SIZE, width, height);
    pType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlaceMode::MFVideoInterlace_Progressive);
    pType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
    MFSetAttributeRatio(pType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    pType->SetUINT32(MF_MT_SAMPLE_SIZE, (3UL * width * height) / 2UL);
    pType->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, TRUE);

    *ppType = pType;

    return S_OK;
}

// https://docs.microsoft.com/en-us/windows/win32/medfound/h-264-video-encoder
// OK
static HRESULT CreateTypeH264(IMFMediaType** ppType, uint32_t width, uint32_t height, uint32_t fps_num, uint32_t fps_den, eAVEncH264VProfile profile, int32_t level, uint32_t bitrate)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
    pType->SetUINT32(MF_MT_AVG_BITRATE, bitrate);
    MFSetAttributeRatio(pType, MF_MT_FRAME_RATE, fps_num, fps_den);
    MFSetAttributeSize(pType, MF_MT_FRAME_SIZE, width, height);
    pType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlaceMode::MFVideoInterlace_Progressive);
    pType->SetUINT32(MF_MT_MPEG2_PROFILE, profile);
    if (level != H26xLevel_Default) { pType->SetUINT32(MF_MT_MPEG2_LEVEL, level); }

    *ppType = pType;

    return S_OK;
}

// https://docs.microsoft.com/en-us/windows/win32/medfound/h-265---hevc-video-encoder
// OK
static HRESULT CreateTypeHEVC(IMFMediaType** ppType, uint32_t width, uint32_t height, uint32_t fps_num, uint32_t fps_den, eAVEncH265VProfile profile, int32_t level, uint32_t bitrate)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_HEVC);
    pType->SetUINT32(MF_MT_AVG_BITRATE, bitrate);
    MFSetAttributeRatio(pType, MF_MT_FRAME_RATE, fps_num, fps_den);
    MFSetAttributeSize(pType, MF_MT_FRAME_SIZE, width, height);
    pType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlaceMode::MFVideoInterlace_Progressive);
    pType->SetUINT32(MF_MT_MPEG2_PROFILE, profile);
    if (level != H26xLevel_Default) { pType->SetUINT32(MF_MT_MPEG2_LEVEL, level); }
    
    *ppType = pType;

    return S_OK;
}

// OK
HRESULT CreateTypeVideo(IMFMediaType** ppType, uint32_t width, uint32_t height, uint32_t stride, uint32_t fps_num, uint32_t fps_den, VideoSubtype subtype, H26xProfile profile, int32_t level, uint32_t bitrate)
{
    switch (profile)
    {
    case H26xProfile::H264Profile_Base: return CreateTypeH264(ppType, width, height, fps_num, fps_den, eAVEncH264VProfile::eAVEncH264VProfile_Base,       level, bitrate);
    case H26xProfile::H264Profile_Main: return CreateTypeH264(ppType, width, height, fps_num, fps_den, eAVEncH264VProfile::eAVEncH264VProfile_Main,       level, bitrate);
    case H26xProfile::H264Profile_High: return CreateTypeH264(ppType, width, height, fps_num, fps_den, eAVEncH264VProfile::eAVEncH264VProfile_High,       level, bitrate);
    case H26xProfile::H265Profile_Main: return CreateTypeHEVC(ppType, width, height, fps_num, fps_den, eAVEncH265VProfile::eAVEncH265VProfile_Main_420_8, level, bitrate);
    }

    switch (subtype)
    {
    case VideoSubtype::VideoSubtype_NV12: return CreateTypeNV12(ppType, width, height, stride, fps_num, fps_den);
    case VideoSubtype::VideoSubtype_YUY2: return CreateTypeYUY2(ppType, width, height, stride, fps_num, fps_den);
    case VideoSubtype::VideoSubtype_IYUV: return CreateTypeIYUV(ppType, width, height, stride, fps_num, fps_den);
    case VideoSubtype::VideoSubtype_YV12: return CreateTypeYV12(ppType, width, height, stride, fps_num, fps_den);
    }

    *ppType = NULL;

    return E_INVALIDARG;
}

// OK
void TranslateEncoderOptions(std::vector<uint64_t> const& options, IMFAttributes **pEncoderAttr)
{
	size_t size = options.size() & ~1ULL;

	MFCreateAttributes(pEncoderAttr, static_cast<UINT32>(size / 2));

	for (int i = 0; i < static_cast<int>(size); i += 2)
	{
	uint64_t option = options[i];
	uint64_t value  = options[i + 1];

	if (option >= (sizeof(g_AVLUT) / sizeof(AVOption))) { continue; }

	AVOption entry = g_AVLUT[option];

	switch (entry.vt)
	{
	case VT_UI4:  (*pEncoderAttr)->SetUINT32(entry.guid, static_cast<UINT32>(value));                  break;
	case VT_UI8:  (*pEncoderAttr)->SetUINT64(entry.guid, value);                                       break;
	case VT_BOOL: (*pEncoderAttr)->SetUINT32(entry.guid, (value == 0) ? VARIANT_FALSE : VARIANT_TRUE); break;
	}
	}
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/audio-subtype-guids
// OK
static HRESULT CreatePlaceholderPCMF32(IMFMediaType** ppType, uint32_t channels, uint32_t samplerate)
{
    uint32_t const bitspersample = 32;

    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
    pType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, bitspersample);
    pType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, samplerate);
    pType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channels);

    *ppType = pType;

    return S_OK;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/audio-subtype-guids
// OK
static HRESULT CreatePlaceholderPCMS16(IMFMediaType** ppType, uint32_t channels, uint32_t samplerate)
{
    uint32_t const bitspersample = 16;

    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    pType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, bitspersample);
    pType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, samplerate);
    pType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channels);

    *ppType = pType;

    return S_OK;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/aac-decoder
// OK
static HRESULT CreatePlaceholderAAC(IMFMediaType** ppType, uint32_t channels, uint32_t samplerate, uint32_t level)
{
    uint8_t adts = 1;
    uint8_t aot = 2;
    uint8_t sf;
    uint8_t cc = (uint8_t)channels;
    uint8_t apli = (uint8_t)level;

    switch (samplerate)
    {
    case 96000: sf =  0; break;
    case 88200: sf =  1; break;
    case 64000: sf =  2; break;
    case 48000: sf =  3; break;
    case 44100: sf =  4; break;
    case 32000: sf =  5; break;
    case 24000: sf =  6; break;
    case 22050: sf =  7; break;
    case 16000: sf =  8; break;
    case 12000: sf =  9; break;
    case 11025: sf = 10; break;
    case 8000:  sf = 11; break;
    case 7350:  sf = 12; break;
    default:    sf = 15; break;
    }

    uint8_t asch = (sf  << 7) | (cc << 3);
    uint8_t ascl = (aot << 3) | (sf >> 1);

    UINT8 heaac[14] =
    {
    adts, 0x00,             /* wPayloadType */ 
    apli, 0x00,             /* wAudioProfileLevelIndication */ 
    0x00, 0x00,             /* wStructType */ 
    0x00, 0x00,             /* wReserved1 */ 
    0x00, 0x00, 0x00, 0x00, /* dwReserved2 */
    ascl, asch              /* AudioSpecificConfig */
    };

    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_AAC);
    pType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, samplerate);
    pType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channels);
    pType->SetUINT32(MF_MT_AAC_PAYLOAD_TYPE, adts);
    pType->SetUINT32(MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION, level);
    pType->SetBlob(MF_MT_USER_DATA, heaac, sizeof(heaac));

    *ppType = pType;

    return S_OK;
}

// OK
HRESULT CreatePlaceholderAudio(IMFMediaType** ppType, uint32_t channels, uint32_t samplerate, AudioSubtype subtype, AACProfile profile, AACLevel level)
{
    switch (profile)
    {
    case AACProfile::AACProfile_12000: return CreatePlaceholderAAC(ppType, channels, samplerate, level);
    case AACProfile::AACProfile_16000: return CreatePlaceholderAAC(ppType, channels, samplerate, level);
    case AACProfile::AACProfile_20000: return CreatePlaceholderAAC(ppType, channels, samplerate, level);
    case AACProfile::AACProfile_24000: return CreatePlaceholderAAC(ppType, channels, samplerate, level);
    }

    switch (subtype)
    {
    case AudioSubtype::AudioSubtype_F32: return CreatePlaceholderPCMF32(ppType, channels, samplerate);
    case AudioSubtype::AudioSubtype_S16: return CreatePlaceholderPCMS16(ppType, channels, samplerate);
    }

    *ppType = NULL;

    return E_INVALIDARG;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/video-subtype-guids
// OK
static HRESULT CreatePlaceholderNV12(IMFMediaType** ppType)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);

    *ppType = pType;

    return S_OK;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/video-subtype-guids
// OK
static HRESULT CreatePlaceholderYUY2(IMFMediaType** ppType)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_YUY2);

    *ppType = pType;

    return S_OK;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/video-subtype-guids
// OK
static HRESULT CreatePlaceholderIYUV(IMFMediaType** ppType)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV);

    *ppType = pType;

    return S_OK;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/video-subtype-guids
// OK
static HRESULT CreatePlaceholderYV12(IMFMediaType** ppType)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_YV12);

    *ppType = pType;

    return S_OK;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/h-264-video-decoder
// OK
static HRESULT CreatePlaceholderH264(IMFMediaType** ppType)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);

    *ppType = pType;

    return S_OK;
}

// https://learn.microsoft.com/en-us/windows/win32/medfound/h-265---hevc-video-decoder
// OK
static HRESULT CreatePlaceholderHEVC(IMFMediaType** ppType)
{
    IMFMediaType* pType;

    MFCreateMediaType(&pType);

    pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_HEVC);

    *ppType = pType;

    return S_OK;
}

// OK
HRESULT CreatePlaceholderVideo(IMFMediaType** ppType, VideoSubtype subtype, H26xProfile profile)
{
    switch (profile)
    {
    case H26xProfile::H264Profile_Base: return CreatePlaceholderH264(ppType);
    case H26xProfile::H264Profile_Main: return CreatePlaceholderH264(ppType);
    case H26xProfile::H264Profile_High: return CreatePlaceholderH264(ppType);
    case H26xProfile::H265Profile_Main: return CreatePlaceholderHEVC(ppType);
    }

    switch (subtype)
    {
    case VideoSubtype::VideoSubtype_NV12: return CreatePlaceholderNV12(ppType);
    case VideoSubtype::VideoSubtype_YUY2: return CreatePlaceholderYUY2(ppType);
    case VideoSubtype::VideoSubtype_IYUV: return CreatePlaceholderIYUV(ppType);
    case VideoSubtype::VideoSubtype_YV12: return CreatePlaceholderYV12(ppType);
    }

    *ppType = NULL;

    return E_INVALIDARG;
}

// https://wiki.multimedia.cx/index.php/ADTS
// OK
bool TranslateADTSOptions(BYTE const* data, uint8_t& channels, uint16_t& samplerate)
{
    uint8_t sl  = data[0];
    uint8_t sh  = data[1] >> 4;
    uint8_t aot = ((data[2] >> 6) & 3) + 1;
    uint8_t sf  = ((data[2] >> 2) & 15);
    uint8_t cc  = ((data[2] << 2) & 4) | ((data[3] >> 6) & 3);
    uint8_t fc  = (data[6] & 3) + 1;

    switch (cc)
    {
    case 1:  channels = 1; break;
    case 2:  channels = 2; break;
    case 3:  channels = 3; break;
    case 4:  channels = 4; break;
    case 5:  channels = 5; break;
    case 6:  channels = 6; break;
    default: channels = 0; break;
    }

    switch (sf)
    {
    case 0x03: samplerate = 48000; break;
    case 0x04: samplerate = 44100; break;
    case 0x05: samplerate = 32000; break;
    case 0x06: samplerate = 24000; break;
    case 0x07: samplerate = 22050; break;
    case 0x08: samplerate = 16000; break;
    case 0x09: samplerate = 12000; break;
    case 0x0A: samplerate = 11025; break;
    case 0x0B: samplerate =  8000; break;
    default:   samplerate =     0; break;
    }

    return (sl == 0xFF) && (sh == 0x0F) && (aot == 0x02) && (sf >= 0x03) && (sf <= 0x0B) && (cc >= 0x01) && (cc <= 0x06) && (fc == 0x01);
}
