
using System;
using System.Runtime.InteropServices;

public static class hl2av
{
    public const byte  RAWProfile        = 0xFF;
    public const sbyte H26xLevel_Default = -1;

    public enum AACProfile : byte
    {
        AACProfile_12000,
        AACProfile_16000,
        AACProfile_20000,
        AACProfile_24000,
        AACProfile_None = RAWProfile
    };

    public enum AACLevel : byte
    {
        AACLevel_L2      = 0x29,
        AACLevel_L4      = 0x2A,
        AACLevel_L5      = 0x2B,
        AACLevel_HEv1L2  = 0x2C,
        AACLevel_HEv1L4  = 0x2E,
        AACLevel_HEv1L5  = 0x2F,
        AACLevel_HEv2L2  = 0x30,
        AACLevel_HEv2L3  = 0x31,
        AACLevel_HEv2L4  = 0x32,
        AACLevel_HEv2L5  = 0x33,
        AACLevel_NotSet  = 0xFE,
        AACLevel_Default = AACLevel_L2
    };

    public enum AudioSubtype : byte
    {
        AudioSubtype_F32,
        AudioSubtype_S16
    };

    public enum H26xProfile : byte
    {
        H264Profile_Base,
        H264Profile_Main,
        H264Profile_High,
        H265Profile_Main,
        H26xProfile_None = RAWProfile
    };

    public enum VideoSubtype : byte
    {
        VideoSubtype_NV12,
        VideoSubtype_YUY2,
        VideoSubtype_IYUV,
        VideoSubtype_YV12
    };

    //-----------------------------------------------------------------------------
    // Core
    //-----------------------------------------------------------------------------

    [DllImport("hl2av")]
    private static extern void core_startup();

    [DllImport("hl2av")]
    private static extern void core_shutdown();

    public static void Startup()
    {
        core_startup();
    }

    public static void Shutdown()
    {
        core_shutdown();
    }

    //-----------------------------------------------------------------------------
    // Sample
    //-----------------------------------------------------------------------------

    [DllImport("hl2av")]
    private static extern IntPtr sample_create(out IntPtr address, uint size);
    
    [DllImport("hl2av")]
    private static extern void sample_setinfo(IntPtr self, long time, long duration);
    
    [DllImport("hl2av")]
    private static extern void sample_getbase(IntPtr self, out IntPtr address, out uint size);
    
    [DllImport("hl2av")]
    private static extern void sample_getinfo(IntPtr self, out long time, out long duration, out uint cleanpoint);
    
    [DllImport("hl2av")]
    private static extern void sample_getresolution(IntPtr self, out uint width, out uint stride, out uint height);
    
    [DllImport("hl2av")]
    private static extern void sample_release(IntPtr self);
    
    [DllImport("hl2av")]
    private static extern byte buffer_getaudioparameters(IntPtr address, out byte channels, out ushort samplerate);

    public class Sample : IDisposable
    {
        private IntPtr m_handle;
        private IntPtr m_address;
        private uint   m_size;

        public IntPtr Handle { get { return m_handle; } }

        public IntPtr Data { get { return m_address; } }

        public uint Size { get { return m_size; } }

        private Sample(uint size)
        {
            m_handle = sample_create(out m_address, size);
            m_size = size;
        }

        private Sample(IntPtr handle)
        {
            m_handle = handle;
            sample_getbase(m_handle, out m_address, out m_size);
        }

        private void Check()
        {
            if (m_handle == IntPtr.Zero) { throw new NullReferenceException("Invalid Sample Handle"); }
        }

        public void SetInfo(long time, long duration)
        {
            Check();
            sample_setinfo(m_handle, time, duration);
        }

        public void GetInfo(out long time, out long duration, out bool keyframe)
        {
            Check();
            sample_getinfo(m_handle, out long t, out long d, out uint c);
            time     = t;
            duration = d;
            keyframe = c != 0;
        }

        public void GetVideoResolution(out ushort width, out ushort stride, out ushort height)
        {
            Check();
            sample_getresolution(m_handle, out uint w, out uint s, out uint h);
            width  = (ushort)w;
            stride = (ushort)s;
            height = (ushort)h;
        }

        public bool GetAudioParameters(out byte channels, out ushort samplerate)
        {
            Check();
            channels   = 0;
            samplerate = 0;
            if (m_size < 7) { return false; }
            byte ok = buffer_getaudioparameters(m_address, out byte c, out ushort r);
            channels   = c;
            samplerate = r;
            return ok != 0;
        }

        public static Sample Create(uint size)
        {
            if (size <= 0) { return null; }
            return new Sample(size);
        }

        public static Sample Create(IntPtr m_handle)
        {
            if (m_handle == IntPtr.Zero) { return null; }
            return new Sample(m_handle);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (m_handle == IntPtr.Zero) { return; }
            sample_release(m_handle);
            m_handle = IntPtr.Zero;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        ~Sample()
        {
            Dispose(false);
        }
    }

    //-----------------------------------------------------------------------------
    // Encoder
    //-----------------------------------------------------------------------------

    [DllImport("hl2av")]
    private static extern IntPtr encoder_createforvideo(ushort width, ushort stride, ushort height, byte framerate, byte divisor, byte subtype, byte profile, sbyte level, uint bitrate);
    
    [DllImport("hl2av")]
    private static extern IntPtr encoder_createforaudio(byte channels, ushort samplerate, byte subtype, byte profile, byte level);
    
    [DllImport("hl2av")]
    private static extern void encoder_push(IntPtr self, IntPtr sample);
    
    [DllImport("hl2av")]
    private static extern byte encoder_peek(IntPtr self);
    
    [DllImport("hl2av")]
    private static extern IntPtr encoder_pull(IntPtr self);
    
    [DllImport("hl2av")]
    private static extern void encoder_destroy(IntPtr self);

    public class Encoder : IDisposable
    {
        private IntPtr m_handle;

        public Encoder(ushort width, ushort stride, ushort height, byte framerate, byte divisor, VideoSubtype subtype, H26xProfile profile, sbyte level, uint bitrate)
        {
            m_handle = encoder_createforvideo(width, stride, height, framerate, divisor, (byte)subtype, (byte)profile, level, bitrate);
        }

        public Encoder(byte channels, ushort samplerate, AudioSubtype subtype, AACProfile profile, AACLevel level)
        {
            m_handle = encoder_createforaudio(channels, samplerate, (byte)subtype, (byte)profile, (byte)level);
        }

        private void Check()
        {
            if (m_handle == IntPtr.Zero) { throw new NullReferenceException("Invalid Encoder Handle"); }
        }

        public void Push(Sample sample)
        {
            Check();
            encoder_push(m_handle, sample.Handle);
        }

        public bool Peek()
        {
            Check();
            return encoder_peek(m_handle) != 0;
        }

        public Sample Pull()
        {
            Check();
            return Sample.Create(encoder_pull(m_handle));
        }

        protected virtual void Dispose(bool disposing)
        {
            if (m_handle == IntPtr.Zero) { return; }
            encoder_destroy(m_handle);
            m_handle = IntPtr.Zero;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        ~Encoder()
        {
            Dispose(false);
        }
    }

    //-----------------------------------------------------------------------------
    // Decoder
    //-----------------------------------------------------------------------------

    [DllImport("hl2av")]
    private static extern IntPtr decoder_createforvideo(byte profile, byte subtype);
    
    [DllImport("hl2av")]
    private static extern IntPtr decoder_createforaudio(byte channels, ushort samplerate, byte profile, byte subtype);

    [DllImport("hl2av")]
    private static extern void decoder_push(IntPtr self, IntPtr sample);

    [DllImport("hl2av")]
    private static extern byte decoder_peek(IntPtr self);

    [DllImport("hl2av")]
    private static extern IntPtr decoder_pull(IntPtr self);

    [DllImport("hl2av")]
    private static extern void decoder_destroy(IntPtr self);

    public class Decoder : IDisposable
    {
        private IntPtr m_handle;

        public Decoder(H26xProfile profile, VideoSubtype subtype)
        {
            m_handle = decoder_createforvideo((byte)profile, (byte)subtype);
        }

        public Decoder(byte channels, ushort samplerate, AudioSubtype subtype, AACProfile profile, AACLevel level)
        {
            m_handle = decoder_createforaudio(channels, samplerate, (byte)profile, (byte)subtype);
        }

        private void Check()
        {
            if (m_handle == IntPtr.Zero) { throw new NullReferenceException("Invalid Decoder Handle"); }
        }

        public void Push(Sample sample)
        {
            Check();
            decoder_push(m_handle, sample.Handle);
        }

        public bool Peek()
        {
            Check();
            return decoder_peek(m_handle) != 0;
        }

        public Sample Pull()
        {
            Check();
            return Sample.Create(decoder_pull(m_handle));
        }

        protected virtual void Dispose(bool disposing)
        {
            if (m_handle == IntPtr.Zero) { return; }
            decoder_destroy(m_handle);
            m_handle = IntPtr.Zero;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        ~Decoder()
        {
            Dispose(false);
        }
    }
}
