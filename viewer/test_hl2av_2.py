# Pipeline Test
# Receive encoded frame from hl2ss
# Send encoded frame to hl2av
# hl2av decode
# hl2av encode
# Receive encoded frame from hl2av
# Decode frame with pyav

import fractions
import multiprocessing as mp
import cv2
import hl2ss_imshow
import av
import hl2ss
import hl2ss_lnm
import hl2ss_mp

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

    producer = hl2ss_mp.producer()
    producer.configure(hl2ss.StreamPort.PERSONAL_VIDEO, hl2ss_lnm.rx_pv(host, hl2ss.StreamPort.PERSONAL_VIDEO, width=width, height=height, framerate=framerate, profile=hl2ss.VideoProfile.H264_MAIN, decoded_format='bgr24'))
    producer.initialize(hl2ss.StreamPort.PERSONAL_VIDEO, buffer_length * framerate)
    producer.start(hl2ss.StreamPort.PERSONAL_VIDEO)

    consumer = hl2ss_mp.consumer()
    sink_pv = consumer.create_sink(producer, hl2ss.StreamPort.PERSONAL_VIDEO, mp.Manager(), None)
    sink_pv.get_attach_response()

    cv2.namedWindow('Video')

    encoder = av.CodecContext.create('h264', 'w')
    encoder.pix_fmt = 'nv12'
    encoder.framerate = framerate
    encoder.width = width
    encoder.height = height
    encoder.bit_rate_tolerance = 0
    encoder.bit_rate = 2*1024*1024
    encoder.profile = 66
    #encoder.ticks_per_frame = 1 NO
    encoder.time_base = fractions.Fraction(1, 10000000)
    encoder.gop_size = framerate
    #encoder.max_bit_rate = 2*1024*1024 # NO WR
    #encoder.has_b_frames = False # NO WR
    #encoder.time_base = fractions.Fraction(1, hl2ss.TimeBase.HUNDREDS_OF_NANOSECONDS)
    index = 0
    state = 0
    while (True):
        if ((cv2.waitKey(1) & 0xFF) == 27):
            break

        _, data = sink_pv.get_most_recent_frame()
        if (data is None):
            continue

        frame = av.VideoFrame.from_ndarray(data.payload.image, format='bgr24')
        frame.pts = index*333333
        frame.dts = index*333333
        #frame.time = index # no WR
        frame.time_base = fractions.Fraction(1, 10000000)

        index += 1

        encoded_frame = bytearray()
        packets = encoder.encode(frame)
        print(f'PACKETS:  {len(packets)}')
        for packet in packets:
            encoded_frame.extend(bytes(packet))

        encoded_size = len(encoded_frame)
        print(f'encoded to {encoded_size} bytes')

        if (encoded_size <= 0):
            continue

        if (state == 0):
            coalesced_frame = bytearray()
            coalesced_frame.extend(encoded_frame)
            state = 1
            continue
        elif (state == 1):
            coalesced_frame.extend(encoded_frame)
            state = 2
            continue
        elif (state == 2):
            coalesced_frame.extend(encoded_frame)
            state = 3
        else:
            coalesced_frame = encoded_frame

        buffer = command_buffer()
        buffer.push_encoded_frame(bytes(coalesced_frame))
        client_mq.push(buffer)
        client_mq.pull(buffer)

        print(f'server response')
        
        while (True):
            msg = client_mqx.pull()
            if (msg is None):
                break
            client_mqx.push(1)
            cmd, param = msg
            if (cmd == 0xFFFFFFFD):
                print(f'pulled {len(param)} bytes')
                image = decoder_pv.decode(param, 'bgr24')
                if (image is not None):
                    cv2.imshow('Video', image)
                    cv2.waitKey(1)




        #image = decoder_pv.decode(coalesced_frame, 'bgr24')
        #if (image is not None):
        #    cv2.imshow('Video', image)
        #    cv2.waitKey(1)
        #else:
        #    print('No image')

        



        #image = encoded_frame #decoder_pv.decode(encoded_frame, 'bgr24')
        #if (image is not None):
        #    cv2.imshow('Video', image)
        #    cv2.waitKey(1)

        '''
        

        
        
        

        

        

        

        
        '''
    
    sink_pv.detach()
    producer.stop(hl2ss.StreamPort.PERSONAL_VIDEO)

    client_mq.close()
    client_mqx.close()

    hl2ss_lnm.stop_subsystem_pv(host, hl2ss.StreamPort.PERSONAL_VIDEO)

