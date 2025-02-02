#------------------------------------------------------------------------------
# Pipeline Test:
# Receive encoded frame from hl2ss (h264 encode mf)
# Send encoded frame to hl2av      (MQ)
# hl2av decode                     (h264 decode mf)
# hl2av encode                     (h264 encode mf)
# Receive encoded frame from hl2av (MQX)
# Decode frame with pyav           (h264 decode av)

# Note that MQ/MQX are used for test transfers which are not efficient for real
# time streams but simplify testing
# Press ESC to stop
#------------------------------------------------------------------------------

import cv2
import hl2ss_imshow
import hl2ss
import hl2ss_lnm

# Settings --------------------------------------------------------------------

host = '192.168.1.7'
width = 640
height = 360
framerate = 30
buffer_length = 10

#------------------------------------------------------------------------------

class command_buffer(hl2ss.umq_command_buffer):
    def reset_pipeline(self):
        self.add(0xFFFFFFFC, b'')

    def push_encoded_frame(self, frame):
        self.add(0xFFFFFFFD, frame)


if __name__ == '__main__':
    hl2ss_lnm.start_subsystem_pv(host, hl2ss.StreamPort.PERSONAL_VIDEO)

    client_pv = hl2ss_lnm.rx_pv(host, hl2ss.StreamPort.PERSONAL_VIDEO, width=width, height=height, framerate=framerate, profile=hl2ss.VideoProfile.H264_MAIN, decoded_format=None)
    client_mq = hl2ss_lnm.ipc_umq(host, hl2ss.IPCPort.UNITY_MESSAGE_QUEUE)
    client_mqx = hl2ss_lnm.ipc_gmq(host, hl2ss.IPCPort.GUEST_MESSAGE_QUEUE)

    client_mqx.open()
    client_mq.open()

    buffer = command_buffer()
    buffer.reset_pipeline()
    client_mq.push(buffer)
    client_mq.pull(buffer)

    decoder_pv = hl2ss.decode_pv(hl2ss.VideoProfile.H264_MAIN)
    decoder_pv.create(0, 0)

    cv2.namedWindow('Video')
    client_pv.open()

    while (True):
        data = client_pv.get_next_packet()

        encoded_frame = hl2ss.unpack_pv(data.payload).image

        buffer = command_buffer()
        buffer.push_encoded_frame(encoded_frame)
        client_mq.push(buffer)
        client_mq.pull(buffer)

        print(f'pushed {len(encoded_frame)} bytes')

        msg = client_mqx.pull()
        if (msg is not None):
            client_mqx.push(1)
            cmd, param = msg
            if (cmd == 0xFFFFFFFD):
                print(f'pulled {len(param)} bytes')
                image = decoder_pv.decode(param, 'bgr24')
                if (image is not None):
                    cv2.imshow('Video', image)

        if ((cv2.waitKey(1) & 0xFF) == 27):
            break


    client_pv.close()
    client_mq.close()
    client_mqx.close()

    hl2ss_lnm.stop_subsystem_pv(host, hl2ss.StreamPort.PERSONAL_VIDEO)
