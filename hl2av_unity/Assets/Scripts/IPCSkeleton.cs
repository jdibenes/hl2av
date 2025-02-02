
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;

public class IPCSkeleton : MonoBehaviour
{
    private hl2av.Decoder m_decoder;
    private hl2av.Encoder m_encoder;
    private byte m_framerate;
    private long m_duration;
    private long m_decode_index;

    void Start()
    {
        hl2av.Startup();

        m_decoder = null;
        m_encoder = null;
        m_framerate = 30;
        m_duration = (10 * 1000 * 1000) / m_framerate;
        m_decode_index = 0;
    }

    void Update()
    {
        GetMessage();
        GetResult();
        GetDecodedFrame();
        GetEncodedFrame();
    }

    void GetDecodedFrame()
    {
        if (m_decoder == null) { return; }
        if (!m_decoder.Peek()) { return; }
        using (hl2av.Sample sample = m_decoder.Pull())
        {
            if (m_encoder == null)
            {
                sample.GetVideoResolution(out ushort width, out ushort stride, out ushort height);
                m_encoder = new hl2av.Encoder(width, stride, height, m_framerate, 1, hl2av.VideoSubtype.VideoSubtype_NV12, hl2av.H26xProfile.H264Profile_Main, hl2av.H26xLevel_Default, (uint)((width * height * m_framerate * 12) / 75));
            }
            m_encoder.Push(sample);
        }
    }

    void GetEncodedFrame()
    {
        if (m_encoder == null) { return; }
        if (!m_encoder.Peek()) { return; }
        using (hl2av.Sample sample = m_encoder.Pull())
        {
            hl2ss.PushMessage(0xFFFFFFFD, sample.Size, sample.Data);
        }
    }

    bool GetMessage()
    {
        uint command;
        byte[] data;
        if (!hl2ss.PullMessage(out command, out data)) { return false; }
        hl2ss.PushResult(ProcessMessage(command, data));
        hl2ss.AcknowledgeMessage(command);
        return true;
    }

    bool GetResult()
    {
        uint result;
        if (!hl2ss.PullResult(out result)) { return false; }
        hl2ss.AcknowledgeResult(result);
        return true;
    }

    uint ProcessMessage(uint command, byte[] data)
    {
        uint ret = ~0U;

        switch (command)
        {
        case 0xFFFFFFFC: ret = MSG_ResetPipeline(data);    break;
        case 0xFFFFFFFD: ret = MSG_PushEncodedFrame(data); break;
        case 0xFFFFFFFE: ret = MSG_DebugMessage(data);     break;
        case 0xFFFFFFFF: ret = MSG_Disconnect(data);       break;
        }

        return ret;
    }

    uint MSG_Disconnect(byte[] data)
    {
        return ~0U;
    }

    uint MSG_DebugMessage(byte[] data)
    {
        string str;
        try { str = Encoding.UTF8.GetString(data); } catch { return 0; }
        hl2ss.Print(str);
        return 1;
    }

    uint MSG_PushEncodedFrame(byte[] data)
    {
        using (hl2av.Sample sample = hl2av.Sample.Create((uint)data.Length))
        {
            Marshal.Copy(data, 0, sample.Data, data.Length);
            sample.SetInfo(m_decode_index++ * m_duration, m_duration);
            m_decoder.Push(sample);
        }
        return 1;
    }

    uint MSG_ResetPipeline(byte[] data)
    {
        m_decoder = new hl2av.Decoder(hl2av.H26xProfile.H264Profile_Main, hl2av.VideoSubtype.VideoSubtype_NV12);
        m_encoder = null;
        m_decode_index = 0;
        return 1;
    }
}
